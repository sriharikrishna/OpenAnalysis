/*! \file
  
  \brief An OutputBuilder for text output
  
  \authors Michelle Strout
  \version $Id: OutputBuilderText.hpp,v 1.4 2005/01/18 21:13:16 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef OutputBuilderText_H
#define OutputBuilderText_H

#include "OutputBuilder.hpp"
#include <iostream>

namespace OA {

class OutputBuilderText : public OutputBuilder { 

  public:
    OutputBuilderText() : mObjDepth(0), mStream(std::cout) {}
    OutputBuilderText(std::ostream& s) : mObjDepth(0), mStream(s) {}

    //void configStream(std::ostream& s) : mStream(s) {}

    //*****************************************************************
    // IRHandles
    //*****************************************************************
    void outputIRHandle(ProcHandle h, OA_ptr<IRHandlesIRInterface> pIR);
    void outputIRHandle(StmtHandle h, OA_ptr<IRHandlesIRInterface> pIR);
    void outputIRHandle(ExprHandle h, OA_ptr<IRHandlesIRInterface> pIR);
    void outputIRHandle(OpHandle h, OA_ptr<IRHandlesIRInterface> pIR);
    void outputIRHandle(MemRefHandle h, OA_ptr<IRHandlesIRInterface> pIR);
    void outputIRHandle(SymHandle h, OA_ptr<IRHandlesIRInterface> pIR);
    void outputIRHandle(ConstSymHandle h, OA_ptr<IRHandlesIRInterface> pIR);
    void outputIRHandle(ConstValHandle h, OA_ptr<IRHandlesIRInterface> pIR);

    //*****************************************************************
    // Object
    //*****************************************************************
    void objStart(const std::string& objName); 
    void objEnd(const std::string& objName);

    //! short hand when specifying string for field value
    void field(const std::string& fieldname, const std::string& value);
 
    // for fields whose value requires further output calls
    void fieldStart(const std::string& fieldname);
    void fieldEnd(const std::string& fieldname);

    //*****************************************************************
    // Lists
    //*****************************************************************
    void listStart();
    void listEnd();

    //! short hand when specifying string for list item
    void listItem(const std::string& value);

    //! for list items that require more output calls
    void listItemStart();
    void listItemEnd();

    //*****************************************************************
    // Maps
    //*****************************************************************
    void mapStart(const std::string& label,
                  const std::string& keyLabel,
                  const std::string& valueLabel);
    void mapEnd(const std::string& label);

    //! short hand when all parts of entry are string
    void mapEntry(const std::string& key, const std::string& value);
    void mapKey(const std::string& key);
    void mapValue(const std::string& value);

    //! for map entries that require more output calls
    void mapEntryStart();
    void mapEntryEnd();
    void mapKeyStart();
    void mapKeyEnd();
    void mapValueStart();
    void mapValueEnd();




    void newLine();

  private:
    int mObjDepth;
    int mListItemCount;
    std::ostream& mStream;

};


} // end of OA namespace

#endif

