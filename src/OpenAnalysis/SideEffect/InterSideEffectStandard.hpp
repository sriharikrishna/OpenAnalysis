/*! \file
  
  \brief Definition of SideEffectStandard 

  \authors Michelle Strout
  \version $Id: InterSideEffectStandard.hpp,v 1.7 2005/08/08 20:03:52 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef InterSideEffectStandard_hpp
#define InterSideEffectStandard_hpp

#include <set>
#include <map>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectInterface.hpp>
#include <OpenAnalysis/SideEffect/SideEffectStandard.hpp>


namespace OA {
  namespace SideEffect {

/* 
   
*/
class InterSideEffectStandard : public InterSideEffectInterface {
  public:
    InterSideEffectStandard();
    ~InterSideEffectStandard() {}

    //*****************************************************************
    // Interface Implementation
    //*****************************************************************
    
    //! Return an iterator over all procedures for which interprocedural
    //! side effect information is available 
    OA_ptr<ProcIterator> getKnownProcIterator();

    //*************************************************************************

    //! Return an iterator over all locations that may be
    //! modified locally within the called procedure.  
    //! Locations modified directly in the called procedure and not
    //! by a function call in the called procedure.
    OA_ptr<LocIterator> getLMODIterator(ExprHandle call);

    //! Return an iterator over all locations that are may be
    //! modified within the called procedure or within a procedure
    //! called by the called procedure.  
    OA_ptr<LocIterator> getMODIterator(ExprHandle call);

    //! Return an iterator over all locations that are definitely
    //! modified locally within the called procedure.  
    //! Locations modified directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLDEFIterator(ExprHandle call);

    //! Return an iterator over all locations that are definitely
    //! modified within the called procedure or within a procedure
    //! called by the called procedure.  
    OA_ptr<LocIterator> getDEFIterator(ExprHandle call);

    //! Return an iterator over all locations that are 
    //! used locally within the called procedure before being
    //! definitely modified.  
    //! Locations used directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLUSEIterator(ExprHandle call);

    //! Return an iterator over all locations that may be 
    //! used before being defined within the called procedure or 
    //! within a procedure called by the called procedure.  
    OA_ptr<LocIterator> getUSEIterator(ExprHandle call);

    //! Return an iterator over all locations that are 
    //! used locally within the called procedure.
    //! Locations used directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLREFIterator(ExprHandle call);

    //! Return an iterator over all locations that are 
    //! used within the called procedure or within a procedure
    //! called by the called procedure.  
    OA_ptr<LocIterator> getREFIterator(ExprHandle call);

    //*************************************************************************
    
    //! Return an iterator over all locations that are may be
    //! modified locally within the given procedure.  
    //! Locations modified directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLMODIterator(ProcHandle p);

    //! Return an iterator over all locations that are may be
    //! modified within the given procedure or within a procedure
    //! called by the given procedure.  
    OA_ptr<LocIterator> getMODIterator(ProcHandle p);

    //! Return an iterator over all locations that are definitely
    //! modified locally within the given procedure.  
    //! Locations modified directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLDEFIterator(ProcHandle p);

    //! Return an iterator over all locations that are definitely
    //! modified within the given procedure or within a procedure
    //! called by the given procedure.  
    OA_ptr<LocIterator> getDEFIterator(ProcHandle p);

    //! Return an iterator over all locations that are 
    //! used locally within the given procedure before being
    //! definitely modified.  
    //! Locations used directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLUSEIterator(ProcHandle p);

    //! Return an iterator over all locations that may be 
    //! used before being defined within the given procedure or 
    //! within a procedure called by the given procedure.  
    OA_ptr<LocIterator> getUSEIterator(ProcHandle p);

    //! Return an iterator over all locations that are 
    //! used locally within the given procedure.
    //! Locations used directly in the procedure and not
    //! by a function call in the procedure.
    OA_ptr<LocIterator> getLREFIterator(ProcHandle p);

    //! Return an iterator over all locations that are 
    //! used within the given procedure or within a procedure
    //! called by the given procedure.  
    OA_ptr<LocIterator> getREFIterator(ProcHandle p);

    //*****************************************************************
    // Other informational methods
    //*****************************************************************
    //! Return true if the given location is in the LMOD set for given proc
    //bool inLMOD(ProcHandle p, OA_ptr<Location> loc);

    //! Return true if the given location is in the MOD set for given proc
    //bool inMOD(ProcHandle p, OA_ptr<Location> loc);

    //! Return true if the given location is in the LDEF set for given proc
    //bool inLDEF(ProcHandle p, OA_ptr<Location> loc);

    //! Return true if the given location is in the DEF set for given proc
    //bool inDEF(ProcHandle p, OA_ptr<Location> loc);

    //! Return true if the given location is in the LUSE set for given proc
    //bool inLUSE(ProcHandle p, OA_ptr<Location> loc);

    //! Return true if the given location is in the USE set for given proc
    //bool inUSE(ProcHandle p, OA_ptr<Location> loc);

    //! Return true if the given location is in the LREF set for given proc
    //bool inLREF(ProcHandle p, OA_ptr<Location> loc);

    //! Return true if the given location is in the REF set for given proc
    //bool inREF(ProcHandle p, OA_ptr<Location> loc);

    //*****************************************************************
    // Construction methods
    //*****************************************************************
    
    //! Associate the given procedure with the given intraprocedural
    //! SideEffect information
    void mapProcToSideEffect(ProcHandle proc, 
                         OA_ptr<OA::SideEffect::SideEffectStandard> sideEffect);
    
    //! Get the SideEffect information associated with the given procedure
    OA_ptr<OA::SideEffect::SideEffectStandard> 
        getSideEffectResults(ProcHandle proc);

    //! Associate the given call with the given intraprocedural
    //! SideEffect information
    void mapCallToSideEffect(ExprHandle call, 
                         OA_ptr<OA::SideEffect::SideEffectStandard> sideEffect);
    
    
    //! Return count of locations in all sets for given procedure
    int getLocCount(ProcHandle proc);
    
    //! Insert a location into the LMOD set for the given call
    void insertLMOD(ExprHandle call, OA_ptr<Location> loc);
    
    //! Insert a location into the MOD set for the given call
    void insertMOD(ExprHandle call, OA_ptr<Location> loc);
    
    //! Insert a location into the LDEF set for the given call
    void insertLDEF(ExprHandle call, OA_ptr<Location> loc);
    
    //! Insert a location into the DEF set for the given call
    void insertDEF(ExprHandle call, OA_ptr<Location> loc);
    
    //! Insert a location into the LUSE set for the given call
    void insertLUSE(ExprHandle call, OA_ptr<Location> loc);
    
    //! Insert a location into the USE set for the given call
    void insertUSE(ExprHandle call, OA_ptr<Location> loc);
    
    //! Insert a location into the LREF set for the given call
    void insertLREF(ExprHandle call, OA_ptr<Location> loc);
    
    //! Insert a location into the REF set for the given call
    void insertREF(ExprHandle call, OA_ptr<Location> loc);

    //! Initialize the information for a particular call to have all
    //! empty sets
    void initCallSideEffect(ExprHandle call);

   
    //*****************************************************************
    // Output
    //*****************************************************************
  public:
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);

  private:

    typedef std::map<ProcHandle,OA_ptr<OA::SideEffect::SideEffectStandard> >  
            ProcToSideEffectMap;
    typedef std::map<ExprHandle,OA_ptr<OA::SideEffect::SideEffectStandard> >  
            CallToSideEffectMap;

    // mappings of procedures to various sets
    ProcToSideEffectMap mProcToSideEffectMap;

    // mapping function calls to various sets
    CallToSideEffectMap mCallToSideEffectMap;

    // default SideEffect results
    OA_ptr<OA::SideEffect::Interface> mDefaultSideEffect;

    //! Iterator over procedures in the mProcToSideEffectMap
    class InterSideEffectProcIter : public virtual ProcIterator,
                                    public IRHandleSetIterator<ProcHandle>
    {
      public:
        InterSideEffectProcIter(OA_ptr<std::set<ProcHandle> > pSet )
            : IRHandleSetIterator<ProcHandle>(pSet) {}
        ~InterSideEffectProcIter() { }

        ProcHandle current() const
          { return IRHandleSetIterator<ProcHandle>::current(); }
        bool isValid() const 
          { return IRHandleSetIterator<ProcHandle>::isValid(); }
        void operator++() { IRHandleSetIterator<ProcHandle>::operator++(); }
        void reset() { IRHandleSetIterator<ProcHandle>::reset(); }
    };


};


  } // end of SideEffect namespace
} // end of OA namespace

#endif

