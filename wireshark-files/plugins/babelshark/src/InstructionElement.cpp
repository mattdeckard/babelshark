// $Id$

#include "InstructionElement.h"
#include "DataDictionary.h"
#include "NullIterator.h"
#include <sstream>

namespace BabelShark
{
    // OLD AND BUSTED
    InstructionElement::InstructionElement(unsigned int size, char* name)
        :Instruction(size, name)
    {

    }

    // OLD AND BUSTED
    InstructionElement::InstructionElement(unsigned int size, char* name, std::string variable = "")
		:Instruction(size, name)
	{
        if (variable != "")
        {
            printf("InstructionElement()\n");
            DataDictionary::Instance()->AddVariable(variable, this);
        }
    }

    // NEW HOTNESS
    InstructionElement::InstructionElement(std::string size, std::string name)
        :Instruction(size, name)
    {

    }

    // NEW HOTNESS
    InstructionElement::InstructionElement(std::string size, std::string name, std::string variable = "")
        :Instruction(size, name)
    {
        if (variable != "")
        {
            printf("InstructionElement()\n");
            DataDictionary::Instance()->AddVariable(variable, this);
        }
    }

	InstructionElement::~InstructionElement()
	{

	}

	//virtual function, base definition does nothing.
	//should be implemented by any class inheriting from it.
    unsigned int InstructionElement::Interpret(char* buffer)
	{
        Notify();
        return 0;
	}

	//virtual function, base definition does nothing.
	//will be used to Display data to the WireShark output
	char* InstructionElement::Display()
	{
		return 0;
	}

	//Recall we are using the composite pattern
	//Since this is the parent class for all leaf nodes it won't have a child in any case and can always return 0
	Instruction* InstructionElement::GetChild()
	{
		return 0;
	}


	//creates a NULL iterator from the object defined in Instruction
	Iterator* InstructionElement::CreateIterator()
	{
		return new NullIterator(this);
	}


    // takes an Input Size and forms a bitmask based on it.
    std::bitset<BIT_MASK_MAX_SIZE> InstructionElement::SetupBitMask(unsigned int val)
    {
        std::bitset<BIT_MASK_MAX_SIZE> result(0);
		for (size_t bitMaskIndex = 0; bitMaskIndex < val && bitMaskIndex < result.size(); bitMaskIndex++)
        {
			result.set(bitMaskIndex, 1);
        }
        return result;
    }

    // rounds up bit size to byte size
    unsigned int InstructionElement::DetermineSizeInBytes(int bits)
    {
        int sizeInBytes = 0;
        while(bits > 0)
        {
            sizeInBytes++;
            bits -=8;
        }
        return sizeInBytes;
    }


    // OVERRIDE
    unsigned int InstructionElement::NumSubtrees()
    {
        // This is a leaf, so it has no subtrees!
        return 0;
    }

    /* PHASE 2 UPDATES */

    std::string InstructionElement::SimpleDisplay()
    {
        return _InterpretedData;
    }
}
