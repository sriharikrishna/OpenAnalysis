/*! \file
  
  \brief Definitions for OutputBuilder for text output
  
  \authors Michelle Strout
  \version $Id: OutputBuilderText.cpp,v 1.3 2005/01/18 21:13:16 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "OutputBuilderText.hpp"

namespace OA {

//*****************************************************************
// IRHandles
//*****************************************************************
void OutputBuilderText::outputIRHandle(ProcHandle h, OA_ptr<IRHandlesIRInterface> pIR)
{
    mStream << "ProcHandle(" << pIR->toString(h) << ")";
}

void OutputBuilderText::outputIRHandle(StmtHandle h, OA_ptr<IRHandlesIRInterface> pIR)
{
    mStream << "StmtHandle(" << pIR->toString(h) << ")";
}

void OutputBuilderText::outputIRHandle(ExprHandle h, OA_ptr<IRHandlesIRInterface> pIR)
{
    mStream << "ExprHandle(" << pIR->toString(h) << ")";
}

void OutputBuilderText::outputIRHandle(OpHandle h, OA_ptr<IRHandlesIRInterface> pIR)
{
    mStream << "OpHandle(" << pIR->toString(h) << ")";
}

void 
OutputBuilderText::outputIRHandle(MemRefHandle h, OA_ptr<IRHandlesIRInterface> pIR)
{
    mStream << "MemRefHandle(" << pIR->toString(h) << ")";
}

void OutputBuilderText::outputIRHandle(SymHandle h, OA_ptr<IRHandlesIRInterface> pIR)
{
    mStream << "SymHandle(" << pIR->toString(h) << ")";
}

void 
OutputBuilderText::outputIRHandle(ConstSymHandle h, OA_ptr<IRHandlesIRInterface> pIR)
{
    mStream << "ConstSymHandle(" << pIR->toString(h) << ")";
}

void 
OutputBuilderText::outputIRHandle(ConstValHandle h, OA_ptr<IRHandlesIRInterface> pIR)
{
    mStream << "ConstValHandle(" << pIR->toString(h) << ")";
}


//*****************************************************************
// Objects
//*****************************************************************
void OutputBuilderText::objStart(const std::string& objName)
/*!
 * \param objName   Name of object class
 *
 * The start of each object increased object depth.
 */
{ 
    newLine();
    for (int i=0; i<mObjDepth; i++) { mStream << '\t'; }
    mStream << objName << "("; 
    mObjDepth++;
}

void OutputBuilderText::objEnd(const std::string& objName)
/*!
 * The end of each object decreases object depth.
 */
{ 
    mObjDepth--;
    for (int i=0; i<mObjDepth; i++) { mStream << '\t'; }
    mStream << ")"; 
    if (mObjDepth == 0) {
        newLine();
    }
}

void 
OutputBuilderText::field(const std::string& fieldname, const std::string& value)
{
    newLine();
    for (int i=0; i<mObjDepth; i++) { mStream << '\t'; }
    mStream << fieldname << ": " << value; 
}

void OutputBuilderText::fieldStart(const std::string& fieldname) 
{
    newLine();
    for (int i=0; i<mObjDepth; i++) { mStream << '\t'; }
    mStream << fieldname + ": "; 
}

void OutputBuilderText::fieldEnd(const std::string& fieldname) 
{
}
    
/*
void OutputBuilderText::fieldValue(const std::string& fieldvalue) 
{ 
    mStream << fieldvalue; 
}

void OutputBuilderText::fieldValueBegin() 
{}
    
void OutputBuilderText::fieldValueEnd() 
{}

void OutputBuilderText::fieldDelimit() 
{ mStream << ", "; }
*/

void OutputBuilderText::listStart() 
{ mStream << "[ ";  mListItemCount = 0; }

void OutputBuilderText::listEnd()   
{ mStream << " ] "; }

void OutputBuilderText::listItem(const std::string& value) 
{ 
  if(mListItemCount>0) {mStream << "    "; }
  mStream << value; 
  mListItemCount++;
} 

void OutputBuilderText::listItemStart() 
{
  if(mListItemCount>0) {mStream << "    "; }
}

void OutputBuilderText::listItemEnd() 
{
  mListItemCount++;
}

//*****************************************************************
// Maps
//*****************************************************************
void OutputBuilderText::mapStart(const std::string& label,
                  const std::string& keyLabel,
                  const std::string& valueLabel)
{ 
  mObjDepth++;
  for (int i=0; i<mObjDepth; i++) { mStream << '\t'; }
  mStream << keyLabel << "\t => " << valueLabel;
  newLine();  
}

void OutputBuilderText::mapEnd(const std::string& label)   
{ 
  newLine();
  mObjDepth--;
}

void OutputBuilderText::mapEntry(const std::string& key, const std::string& value)
{ 
  mapEntryStart();
  mapKey(key);
  mapValue(value);
  mapEntryEnd();
} 

void OutputBuilderText::mapKey(const std::string& key)
{ 
  mapKeyStart();
  mStream << key;
  mapKeyEnd();
} 

void OutputBuilderText::mapValue(const std::string& value)
{ 
  mapValueStart();
  mStream << value;
  mapValueEnd();
} 


void OutputBuilderText::mapEntryStart() 
{
  for (int i=0; i<mObjDepth; i++) { mStream << '\t'; }
}

void OutputBuilderText::mapEntryEnd() 
{
  newLine();  
}

void OutputBuilderText::mapKeyStart() 
{
}

void OutputBuilderText::mapKeyEnd() 
{
  mStream << "\t =>";
}

void OutputBuilderText::mapValueStart() 
{
}

void OutputBuilderText::mapValueEnd() 
{
}

void OutputBuilderText::newLine() 
{ mStream << std::endl; }


} // end of OA namespace


