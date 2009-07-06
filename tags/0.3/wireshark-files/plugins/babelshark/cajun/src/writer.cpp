/***********************************************

Author: Julie Betlach

Adapted from code written by: Terry Caton, tcaton(a)hotmail.com
Project Webpage: http://cajun-jsonapi.sourceforge.net/

Original code parsed files in json format.
Code now parses files in PDI format for a project for Washington University, 
course CSE 533S: Pattern Oriented Software Design and Development, Summer 2009.

*************************************************/

#include "writer.h"
#include <iostream>


/*  

TODO:
* better documentation
* unicode character encoding

***/

namespace PDI
{


void Writer::Write(const Element& elementRoot, std::ostream& ostr) { 
   Writer writer(ostr);
   elementRoot.Accept(writer);
   ostr.flush(); // all done
}

void Writer::Visit(const Array& array) {
   if (array.Empty())
      m_ostr << "{}";
   else
   {
      m_ostr << '{' << std::endl;
      ++m_nTabDepth;

      Array::const_iterator it(array.Begin()),
                             itEnd(array.End());
      while (it != itEnd) {
         m_ostr << std::string(m_nTabDepth, '\t') << '"' << it->Name() << "\" : ";
         it->Accept(*this); 

         if (++it != itEnd)
            m_ostr << ',';
         m_ostr << std::endl;
      }

      --m_nTabDepth;
      m_ostr << std::string(m_nTabDepth, '\t') << '}';
   }
}

/*
// This will be used for the ArraySize.
void Writer::Visit(const Number& number) {
   m_ostr << number;
}
***/
void Writer::Visit(const Null& nullElement) {
   m_ostr << "null";
}

void Writer::Visit(const DisplayElement& stringElement) {
   m_ostr << '"';

   const std::string& s = stringElement;
   
   std::string::const_iterator it(s.begin()),
                               itEnd(s.end());
   for (; it != itEnd; ++it)
   {
      switch (*it)
      {
         case '"':         m_ostr << "\\\"";    break;
         case '\\':        m_ostr << "\\\\";    break;
         case '\b':        m_ostr << "\\b";    break;
         case '\f':        m_ostr << "\\f";    break;
         case '\n':        m_ostr << "\\n";    break;
         case '\r':        m_ostr << "\\r";    break;
         case '\t':        m_ostr << "\\t";    break;
         //case '\u':        m_ostr << "";    break;  ??
         default:          m_ostr << *it;       break;
      }
   }

   m_ostr << '"';   
}


} // End namespace