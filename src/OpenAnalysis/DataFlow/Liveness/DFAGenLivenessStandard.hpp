
 // DFAGenLiveness.hpp

#ifndef DFAGenLivenessStandard_hpp
#define LivenessStandard_hpp

#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/OABase/Annotation.hpp>
#include <OpenAnalysis/Utils/GenOutputTool.hpp>
#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/DataFlow/LocDFSet.hpp>
#include <OpenAnalysis/Utils/Util.hpp>

namespace OA {
  namespace DFAGenLiveness {



class DFAGenLivenessStandard : public virtual Annotation 
{
  public:
    DFAGenLivenessStandard(ProcHandle p){} 
    ~DFAGenLivenessStandard() {}

    OA_ptr<LocIterator> getDFAGenLivenessIterator(StmtHandle s); 

    OA_ptr<LocIterator> getExitDFAGenLivenessIterator(); 
// TODO
 //    Live will be a set of locations
    void insertDFAGenLiveness(StmtHandle s, OA_ptr<Location> Live)
    { 
      if(mDFAGenLiveness[s].ptrEqual(0)) {
        mDFAGenLiveness[s] = new LocSet;
      }
      mDFAGenLiveness[s]->insert(Live);    
    }

    void insertExitDFAGenLiveness(OA_ptr<Location> Live)
    { 
      mExitDFAGenLiveness->insert(Live); 
    }


    void output(IRHandlesIRInterface& pIR);

    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);

  private:
    std::map<StmtHandle,OA_ptr<LocSet> > mDFAGenLiveness;
    OA_ptr<LocSet> mExitDFAGenLiveness;
}; 

  } // end of Liveness namespace
} // end of OA namespace

#endif
