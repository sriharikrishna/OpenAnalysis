/*! \file
  
  \brief Maps StmtHandles to set of locations that are in the 
         set InUseful based on a set of dependent location.

  \authors Michelle Strout
  \version $Id: UsefulStandard.hpp,v 1.7 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef UsefulStandard_hpp
#define UsefulStandard_hpp

#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/ReachDefs/Interface.hpp>
#include <OpenAnalysis/DataFlow/LocDFSet.hpp>

namespace OA {
  namespace Activity {



class UsefulStandard {
  public:
    UsefulStandard(ProcHandle p); 
    ~UsefulStandard() {}

    //! Return an iterator for set of dependent locations
    //! the analysis calculated InUseful from
    OA_ptr<LocIterator> getDepSetIterator(); 

    //! Return an iterator that will iterate over the set of locations
    //! that are in InUseful set for stmt
    OA_ptr<LocIterator> getInUsefulIterator(StmtHandle s);

    //! returns a location data flow set of locations that 
    //! are useful after the statement for the given call
    //FIXME: an iterator or make getInUsefulIterator return a set?
    OA_ptr<DataFlow::LocDFSet> getOutUsefulSet(ExprHandle call);

    OA_ptr<DataFlow::LocDFSet> getFinalUseful();

    //! access the number of iterations over all nodes for Useful
    //int getNumIter() { return mNumIter; }

    //*****************************************************************
    // Construction methods 
    //*****************************************************************

    //! insert an dependent location
    void insertDepLoc(OA_ptr<Location> loc) 
    { mDepLocSet->insert(loc); }

    //! copy into the InUseful set for a statement
    void copyIntoInUseful(StmtHandle s, 
                          OA_ptr<DataFlow::LocDFSet> dfset)
    {
      OA_ptr<DataFlow::DataFlowSet> cloneDFset = dfset->clone();
      mInUseful[s] = cloneDFset.convert<DataFlow::LocDFSet>();
    }

    //! copy into the OutUseful set for a call
    void copyIntoOutUseful(ExprHandle e, 
                           OA_ptr<DataFlow::LocDFSet> dfset)
    {
      OA_ptr<DataFlow::DataFlowSet> cloneDFset = dfset->clone();
      mOutUseful[e] = cloneDFset.convert<DataFlow::LocDFSet>();
    }

    //! set the final Useful or InUseful for the entry node of proc
    void mapFinalUseful(OA_ptr<DataFlow::LocDFSet> final);

    //!  set the number of iterations over all nodes for Useful
    //void setNumIter(int n) { mNumIter = n; }

    //*****************************************************************
    // Output
    //*****************************************************************

    //! incomplete output of info for debugging, just lists stmts
    //! and associated set of InUseful locations
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);

  private:
    // data members
    std::map<StmtHandle,OA_ptr<DataFlow::LocDFSet> >  mInUseful;
    std::map<ExprHandle,OA_ptr<DataFlow::LocDFSet> >  mOutUseful;
    OA_ptr<DataFlow::LocDFSet>  mDepLocSet;
    OA_ptr<DataFlow::LocDFSet>  mFinalInUseful;
  //int mNumIter;

};

  } // end of Activity namespace
} // end of OA namespace

#endif

