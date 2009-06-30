// $Id$

// C++ headers
#include "Dissector.h"
#include "DataDictionary.h"
#include <sstream>

BabelShark::Instruction* Parse(std::string inFile); // delcaration; this is defined in PdiParser.cpp

namespace BabelShark
{

    Dissector::Dissector(const char* inFile, int* proto)
        : _proto(proto),
          _nameChanged(true)
    {

        _RootInstruction = Parse(inFile);
        if (_RootInstruction == NULL)
        {
            _RootInstruction = new BabelShark::Instruction(0, "ERR_BAD_PARSE");
        }
        _protoName       = _RootInstruction->GetName();

        /* Setup protocol subtree array */
        _ett = new gint*[1]; // TODO: size array to number of InstructionSets
        *_ett[0] = -1;

        *_proto = proto_register_protocol(_protoName.c_str(),  /* name       */
                                          "Babelshark",        /* short name */
                                          "babelshark");       /* abbrev     */

        proto_register_field_array(*_proto, NULL, 0);
        proto_register_subtree_array(_ett, array_length(_ett));

	}

    Dissector::~Dissector()
    {
        // free dynamically allocated memory
        delete [] *_ett;
	}

    void Dissector::ReparseTree(const char* inFile)
    {
        // NOTE: This will not change the registered name of the protocol
        // Reparse Instruction tree
        _RootInstruction = Parse(inFile);
        if (_RootInstruction == NULL)
        {
            _RootInstruction = new BabelShark::Instruction(0, "ERR_BAD_PARSE");
        }
        _protoName       = _RootInstruction->GetName();
        _nameChanged     = true;
        printf("name: %s\n\n", _RootInstruction->GetName());

        Test(); // TODO: REMOVE
    }

    void Dissector::Dissect(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
    {
        guint8 packet_type = tvb_get_guint8(tvb, 0);

        if (check_col(pinfo->cinfo, COL_PROTOCOL) || _nameChanged) {
           col_set_str(pinfo->cinfo, COL_PROTOCOL, _protoName.c_str());
           _nameChanged = false;
        }
        /* Clear out stuff in the info column */
        if (check_col(pinfo->cinfo,COL_INFO)) {
           col_clear(pinfo->cinfo,COL_INFO);
        }
        if (check_col(pinfo->cinfo, COL_INFO)) {
            col_add_fstr(pinfo->cinfo, COL_INFO, "decoded by Babelshark");
        }

        // create buffer
        int length = tvb->length;
        char* buffer = new char[length];
        const guint8* tvb_ptr = tvb_get_ptr(tvb, 0, length);
        memcpy(buffer, tvb_ptr, length);

        if (tree) { // we are being asked for details
           proto_item *ti = NULL;
           proto_tree *babelshark_tree = NULL;

           ti = proto_tree_add_item(tree, *_proto, tvb, 0, -1, FALSE);

           // proto-level text
           proto_item_append_text(ti, " (decoded by Babelshark)");

           // Init byte offset
           gint offset = 0;

           // Create root node
           babelshark_tree = proto_item_add_subtree(ti, *_ett[0]);

           // Parse it
           /*
           if (_RootInstruction->GetSize() > 0)
           {
              ParseInstructions(_RootInstruction, tvb, babelshark_tree, buffer, offset);
           }
           */

           // Parse it
           if (_TestInstruction->GetSize() > 0)
           {
              ParseInstructions(_TestInstruction, tvb, babelshark_tree, buffer, offset);
           }

       }

       // free dynamically allocated memory
       delete [] buffer;
    }

    void Dissector::Test()
    {
        //printf("Dissector::Test\n _ett: %i\n _proto: %u\n", _ett[0], *_proto);

        _TestInstruction = new InstructionSet(1, "TestProtocol");
        InstructionSet* tempTree;

        // make a dynamic type
        DataDictionary::Instance()->AddDynamic("&BODY", "1", "&ACK");
        DataDictionary::Instance()->AddDynamic("&BODY", "2", "&INIT");

        // make a static type
        tempTree = new InstructionSet(1, "TestHeader");
        tempTree->Add(new UintElement(8, "Message ID", "$MSG_ID"));
        tempTree->Add(new PadElement(8, "Padding"));
        tempTree->Add(new UintElement(16, "Event ID"));
        tempTree->Add(new AsciiElement(16, "Name"));
        tempTree->Add(new PadElement(32, "Padding"));
        DataDictionary::Instance()->AddStatic("&HEADER", tempTree);

        // make another static type
        tempTree = new InstructionSet(1, "TestAck");
        tempTree->Add(new UintElement(32, "Status"));
        DataDictionary::Instance()->AddStatic("&ACK", tempTree);

        // make another static type
        tempTree = new InstructionSet(1, "TestInit");
        tempTree->Add(new UintElement(7, "Age"));
        tempTree->Add(new BoolElement(1, "Male?"));
        tempTree->Add(new PadElement(56, "Pad"));
        DataDictionary::Instance()->AddStatic("&INIT", tempTree);

        // build tree to test new functionality
        _TestInstruction->Add(new AliasedInstruction(1, "Header", "&HEADER"));
        _TestInstruction->Add(new AliasedInstruction(1, "DynamicTest", "&BODY", "$MSG_ID"));

        //_TestAliased = new AliasedInstruction(1, "DynamicTest", "&BODY", "$MSG_ID");

    }

    char* Dissector::ShiftBits(char* buffer, unsigned int size, unsigned int offset)
    {
        // offset: 6
        // size:   3
        //
        // buffer:  ~~__ ____   --== ====   ^^%% %%%%    0000 0000
        // result:  ==== ==~~   %%%% %%--   0000 00^^

        char *result;
        result = new char[size];
        char mask = 0xFF << offset; // 1100 0000
        for (unsigned int i = 0; i < size; i++)
        {
            char temp;
            result[i] = buffer[i] & mask;     // ~~00 0000
            result[i] =result[i] >> offset;   // 0000 00~~
            // WARNING:
            if (i + 1 == size)
            {
                temp = 0;                     // 0000 0000
            }
            else
            {
                temp = buffer[i + 1] & ~mask; // 00== ====
            }
            temp = temp << (8 - offset);      // ==== ==00
            result[i] |= temp;                // ==== ==~~
        }

        return result;
    }

    void Dissector::ParseInstructions(Instruction* in, tvbuff_t *tvb, proto_tree *tree, char* buffer, gint &offset)
    {
        static unsigned int bitOffset = 0;

        // interpret children
        for (Iterator* it = in->CreateIterator(); ! it->IsDone(); it->Next())
        {
           Instruction* currentIns = it->CurrentItem();
           printf("bitOffset for %s is %u\n", currentIns->GetName(), bitOffset);
           if ( bitOffset > 0 )
           {
              offset -= 1;
              bitOffset = (bitOffset + currentIns->Interpret(ShiftBits(buffer + offset,
                                                                       currentIns->GetSizeInBytes(),
                                                                       bitOffset))) % 8;
           }
           else
           {
              bitOffset = (bitOffset + currentIns->Interpret(buffer + offset)) % 8;
           }

           // find out if this instruction is a leaf or a node
           if (currentIns->CreateIterator()->IsDone())
           {
               // BASE CASE

               // TODO: figure out if we need to play with the bits

               // currentIns is a leaf, move forward in the buffer
               proto_tree_add_text(tree,
                                   tvb,
                                   offset,
                                   currentIns->GetSizeInBytes(),
                                   currentIns->Display()
                                  );
               offset += currentIns->GetSizeInBytes();
           }
           else
           {
               // RESURSION
               // currentIns is a node with children
               proto_tree *sub_node = proto_tree_add_text(tree,
                                                          tvb,
                                                          offset,
                                                          currentIns->GetSizeInBytes() * currentIns->GetSize(),
                                                          currentIns->Display()
                                                         );

               // create a new subtree
               proto_tree *sub_tree = proto_item_add_subtree(sub_node, *_ett[0]);

               // parse this set
               ParseSet(currentIns, tvb, sub_tree, buffer, offset);
           }
        }
    }

    void Dissector::ParseSet(Instruction* in, tvbuff_t *tvb, proto_tree *tree, char* buffer, gint &offset)
    {
        // in is a subtree
        proto_tree *sub_tree = NULL;
        proto_tree *sub_node = NULL;
        unsigned int size = in->GetSize();

        if (size > 1)
        {
            // for each instance of this subtree
            for (unsigned int count = 0; count < size; count++)
            {
                // create another subtree
                std::stringstream treeDisplay;
                treeDisplay << "[" << count << "]";
                sub_node = proto_tree_add_text(tree,
                                               tvb,
                                               offset,
                                               in->GetSizeInBytes(),
                                               treeDisplay.str().c_str()
                                              );
                sub_tree = proto_item_add_subtree(sub_node, *_ett[0]);

                // parse the children
                ParseInstructions(in, tvb, sub_tree, buffer, offset);
            }
        }
        else
        {
            // only one instance, just put it directly under the subtree
            ParseInstructions(in, tvb, tree, buffer, offset);
        }
    }


} // namespace BabelShark

