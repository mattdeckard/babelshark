// $Id$

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <vector>
#include "Iterator.h"
/*
	Design Pattern Used: Composite
	Class:Instruction
		used purely as a Composite portion of the Composite Design Pattern
		inherited by InstructionElement and InstructionSet
		Composite allows objects to be treated uniformly.
*/



namespace BabelShark
{
    class Iterator; // forward declaration
	class Instruction
	{
		public:
			Instruction(unsigned int size, char* name);
			virtual ~Instruction();

			//virtual function, base definition does nothing.
			//should be implemented by any class inheriting from it.
			virtual void Interpret(char* buffer);

			//virtual function, base definition does nothing.
			//will be used to Display data to the WireShark output
			virtual char* Display();

			//virtual function, no implementation in base class
			//used in the InstructionList class.
			virtual Instruction* GetChild();

			//virtual function, implementation does nothing.
			//used in the InstructionList class.
			virtual void Add(Instruction*);

			//virtual function, no implementation in base class
			//used in the InstructionList class.
			virtual void Remove(Instruction*);

			//virtual function, no implementation in base class
			//used in the InstructionList class.
            virtual Iterator* CreateIterator();

            //virtual function, no implementation in base class
            virtual unsigned int GetSizeInBytes(){return _SizeInBytes; }

			void SetSize(unsigned int size){ _Size = size;}
			void SetName(char* name){ _Name = name;}

			unsigned int GetSize() { return _Size;}
			char* GetName(){ return (char*)(_Name.c_str());}

            // For keeping track of subtrees
            virtual unsigned int NumSubtrees();

		protected:
			unsigned int _Size;
			unsigned int _SizeInBytes;
			std::string _Name;

		private:
	};

}

#endif //INSTRUCTION_H