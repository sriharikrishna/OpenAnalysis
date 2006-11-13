/*! \file
  
  \brief The OutputBuilder abstract interface.
  
  \authors Michelle Strout
  \version $Id: OutputBuilder.hpp,v 1.3 2005/01/18 21:13:16 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  To generate output to different file types such as dot and xml, 
  implement the interface specified here.  All Annotation subclasses
  (analysis results) will make calls to this abstract interface in their
  output method.

*/

#ifndef OutputBuilder_H
#define OutputBuilder_H

#include <iostream>
#include <string>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>

namespace OA {

std::string int2string(const int& number);
std::string bool2string(const bool& val);

class OutputBuilder { 
  public:
    OutputBuilder() {}
    OutputBuilder(OutputBuilder& other) {}
    OutputBuilder(std::ostream&) {}

    //virtual void configStream(ostream&) = 0;

    //*****************************************************************
    // IRHandles
    //*****************************************************************
    virtual void outputIRHandle(ProcHandle h, OA_ptr<IRHandlesIRInterface> pIR) = 0;
    virtual void outputIRHandle(StmtHandle h, OA_ptr<IRHandlesIRInterface> pIR) = 0;
    virtual void outputIRHandle(ExprHandle h, OA_ptr<IRHandlesIRInterface> pIR) = 0;
    virtual void outputIRHandle(OpHandle h, OA_ptr<IRHandlesIRInterface> pIR) = 0;
    virtual void outputIRHandle(MemRefHandle h, OA_ptr<IRHandlesIRInterface> pIR) = 0;
    virtual void outputIRHandle(SymHandle h, OA_ptr<IRHandlesIRInterface> pIR) = 0;
    virtual void outputIRHandle(ConstSymHandle h, OA_ptr<IRHandlesIRInterface> pIR) =0;
    virtual void outputIRHandle(ConstValHandle h, OA_ptr<IRHandlesIRInterface> pIR) =0;

    //*****************************************************************
    // Object
    //*****************************************************************
    virtual void objStart(const std::string& objName) = 0;
    virtual void objEnd(const std::string& objName) = 0;

    //! short hand when specifying string for field value
    virtual void field(const std::string& fieldname, 
                       const std::string& value) = 0;
 
    //! for fields whose value requires further output calls
    virtual void fieldStart(const std::string& fieldname) = 0;
    virtual void fieldEnd(const std::string& fieldname) = 0;

    //*****************************************************************
    // Lists
    //*****************************************************************
    virtual void listStart() = 0;
    virtual void listEnd() = 0;

    //! short hand when specifying string for list item
    virtual void listItem(const std::string& value) = 0;

    //! for list items that require more output calls
    virtual void listItemStart() = 0;
    virtual void listItemEnd() = 0;

    // a separator between list items
    //FIXME: deprecated
    //virtual void listItemDelimit() = 0;

    //*****************************************************************
    // Maps
    //*****************************************************************
    virtual void mapStart(const std::string& label,
                          const std::string& keyLabel,
                          const std::string& valueLabel) = 0;
    virtual void mapEnd(const std::string& label) = 0;

    //! short hand when all parts of entry are string
    virtual void mapEntry(const std::string& key, const std::string& value)=0;
    virtual void mapKey(const std::string& key) = 0;
    virtual void mapValue(const std::string& value) = 0;

    //! for map entries that require more output calls
    virtual void mapEntryStart() = 0;
    virtual void mapEntryEnd() = 0;
    virtual void mapKeyStart() = 0;
    virtual void mapKeyEnd() = 0;
    virtual void mapValueStart() = 0;
    virtual void mapValueEnd() = 0;


    //*****************************************************************
    // Graphs
    //*****************************************************************
    //graphStart(label), graphEnd(label),
    //      graphNode(id,value) // shorthand
    //      graphNodeStart(id), graphNodeEnd(id) 


    //virtual void newLine() = 0;

};


} // end of OA namespace

#endif

