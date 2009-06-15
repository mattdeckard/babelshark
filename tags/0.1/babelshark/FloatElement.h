// $Id$

#ifndef FLOAT_ELEMENT_H
#define FLOAT_ELEMENT_H

#include <bitset>
#include "InstructionElement.h"

#define BIT_MASK_MAX_SIZE 64

/*
	Design Pattern Used: Composite
	Class: FloatElement
	Parent: InstructionElement
	One of the Leaf(s) in the Composite Structure.
	Processes Float type elements
	
	Notes: not completed in Phase I.
*/


namespace BabelShark
{
	class FloatElement: public InstructionElement
	{
		public:
			FloatElement(unsigned int size, char* name);
			~FloatElement();

			//TODO -- Implement this
			//Will be used to read in data from packet
			void Interpret(char* buffer);

			//TODO -- Implement this
			//will be used to Display data to the WireShark output
			char* Display();

			std::bitset<BIT_MASK_MAX_SIZE> GetBitMask(){ return _BitMask;}
		private:
			//takes our Input Size and forms the _BitMask based on it.
			void SetupBitMask(unsigned int val);
			std::bitset<BIT_MASK_MAX_SIZE> _BitMask;
	};
}


#endif //FLOAT_ELEMENT_H