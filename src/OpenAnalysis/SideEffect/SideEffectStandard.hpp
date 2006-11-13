/*! \file
  
  \brief Definition of SideEffectStandard 

  \authors Michelle Strout
  \version $Id: SideEffectStandard.hpp,v 1.9 2005/06/10 02:32:05 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef SideEffectStandard_hpp
#define SideEffectStandard_hpp

#include <set>
#include <map>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/SideEffect/SideEffectInterface.hpp>


namespace OA {
  namespace SideEffect {

/* 
   Maps procedures to USE, DEF, MOD, and REF sets and their local versions.
*/
class SideEffectStandard : public Interface {
  public:
    SideEffectStandard() 
        { mUnknownLocSet = new LocSet;
          OA_ptr<Location> uLoc; 
          //uLoc = dynamic_cast<Location*>(new UnknownLoc());
          //MMS, even the SGI compiler shouldn't need the above
          uLoc = new UnknownLoc();
          mUnknownLocSet->insert(uLoc);
        }
    ~SideEffectStandard() {}

    //*****************************************************************
    // Interface Implementation
    //*****************************************************************
    
    //! Return an iterator over all locations that are may be
    //! modified locally.
    //! Locations modified directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLMODIterator();

    //! Return an iterator over all locations that are may be
    //! modified within the procedure or within a procedure
    //! called by the procedure.  
    OA_ptr<LocIterator> getMODIterator();

    //! Return an iterator over all locations that are definitely
    //! modified locally within the procedure.  
    //! Locations modified directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLDEFIterator();

    //! Return an iterator over all locations that are definitely
    //! modified within the procedure or within a procedure
    //! called by the procedure.  
    OA_ptr<LocIterator> getDEFIterator();

    //! Return an iterator over all locations that are 
    //! used locally within the procedure before being
    //! definitely modified.  
    //! Locations used directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLUSEIterator();

    //! Return an iterator over all locations that may be 
    //! used before being defined within the procedure or 
    //! within a procedure called by the procedure.  
    OA_ptr<LocIterator> getUSEIterator();

    //! Return an iterator over all locations that are 
    //! used locally within the procedure.
    //! Locations used directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLREFIterator();

    //! Return an iterator over all locations that are 
    //! used within the procedure or within a procedure
    //! called by the procedure.  
    OA_ptr<LocIterator> getREFIterator();

    //*****************************************************************
    // Other informational methods
    //*****************************************************************
    //! Return true if the given location is in the LMOD set 
    bool inLMOD(OA_ptr<Location> loc);

    //! Return true if the given location is in the MOD set 
    bool inMOD(OA_ptr<Location> loc);

    //! Return true if the given location is in the LDEF set 
    bool inLDEF(OA_ptr<Location> loc);

    //! Return true if the given location is in the DEF set 
    bool inDEF(OA_ptr<Location> loc);

    //! Return true if the given location is in the LUSE set
    bool inLUSE(OA_ptr<Location> loc);

    //! Return true if the given location is in the USE set 
    bool inUSE(OA_ptr<Location> loc);

    //! Return true if the given location is in the LREF set
    bool inLREF(OA_ptr<Location> loc);

    //! Return true if the given location is in the REF set
    bool inREF(OA_ptr<Location> loc);

    //*****************************************************************
    // Construction methods
    //*****************************************************************
    //! Insert a location into the LMOD set
    void insertLMOD(OA_ptr<Location> loc);
    
    //! Insert a location into the MOD set
    void insertMOD(OA_ptr<Location> loc);
    
    //! Insert a location into the LDEF set
    void insertLDEF(OA_ptr<Location> loc);
    
    //! Insert a location into the DEF set
    void insertDEF(OA_ptr<Location> loc);
    
    //! Insert a location into the LUSE set
    void insertLUSE(OA_ptr<Location> loc);
    
    //! Insert a location into the USE set
    void insertUSE(OA_ptr<Location> loc);
    
    //! Insert a location into the LREF set
    void insertLREF(OA_ptr<Location> loc);
    
    //! Insert a location into the REF set
    void insertREF(OA_ptr<Location> loc);

    //*************************************************************
    //! Make the LMOD set empty, default is to have Unknown location
    void emptyLMOD();
    
    //! Make the MOD set empty, default is to have Unknown location
    void emptyMOD();
    
    //! Make the LDEF set empty, default is empty, but want same interface
    void emptyLDEF();
    
    //! Make the DEF set empty, default is empty, but want same interface
    void emptyDEF();
    
    //! Make the LUSE set empty, default is to have Unknown location
    void emptyLUSE();
    
    //! Make the USE set empty, default is to have Unknown location
    void emptyUSE();
    
    //! Make the LREF set empty, default is to have Unknown location
    void emptyLREF();
    
    //! Make the REF set empty, default is to have Unknown location
    void emptyREF();
    
    //*****************************************************************
    // Output
    //*****************************************************************

    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);

  private:
    // various sets
    OA_ptr<LocSet> mLMODSet;
    OA_ptr<LocSet> mMODSet;
    OA_ptr<LocSet> mLDEFSet;
    OA_ptr<LocSet> mDEFSet;
    OA_ptr<LocSet> mLUSESet;
    OA_ptr<LocSet> mUSESet;
    OA_ptr<LocSet> mLREFSet;
    OA_ptr<LocSet> mREFSet;

    // default set returned to be conservative
    OA_ptr<LocSet> mUnknownLocSet;

};


  } // end of SideEffect namespace
} // end of OA namespace

#endif

