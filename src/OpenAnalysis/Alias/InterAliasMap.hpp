/*! \file
  
  \brief Declarations for the AliasMap implementation of interprocedural 
         Alias results.

  \authors Michelle Strout
  \version $Id: InterAliasMap.hpp,v 1.2 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef InterAliasMap_H
#define InterAliasMap_H

#include <OpenAnalysis/Alias/ManagerAliasMapBasic.hpp>
#include <OpenAnalysis/Alias/InterAliasInterface.hpp>
#include <map>

namespace OA {
  namespace Alias {

class InterAliasMap : public virtual InterAliasInterface {
  public:
    InterAliasMap(OA_ptr<ManagerAliasMapBasic> aliasman) 
        : mAliasMapMan(aliasman) {}
    virtual ~InterAliasMap() {}

    //! Returns alias analysis results for the given procedure
    //! starting with an optimistic AliasMap that doesn't have
    //! any aliases due to reference parameters
    OA_ptr<Alias::Interface> getAliasResults(ProcHandle proc)
      { if (mProcToAliasMap[proc].ptrEqual(0)) {
          OA_ptr<AliasMap> empty;
          empty = new AliasMap(proc);
          mProcToAliasMap[proc] = mAliasMapMan->performAnalysis(proc, empty); 
        }
        return mProcToAliasMap[proc];
      }

    //*****************************************************************
    // Output
    //*****************************************************************
    //! debugging output
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
      {
        os << "====================== InterAliasMap" << std::endl;
        std::map<ProcHandle,OA_ptr<AliasMap> >::iterator mapIter;
        for (mapIter=mProcToAliasMap.begin();
             mapIter!=mProcToAliasMap.end();
             mapIter++) 
        {
            OA_ptr<AliasMap> alias = mapIter->second;
            alias->dump(os,ir);
            os << "-------------------------------" << std::endl;
        }

      }
    
    //*****************************************************************
    // Construction methods 
    //*****************************************************************

    //! attach AliasMap to procedure
    void mapProcToAliasMap(ProcHandle proc, OA_ptr<AliasMap> alias)
      { mProcToAliasMap[proc] = alias; }

  private:
    OA_ptr<ManagerAliasMapBasic> mAliasMapMan;
    std::map<ProcHandle,OA_ptr<AliasMap> > mProcToAliasMap;


};

  } // end of Alias namespace
} // end of OA namespace

#endif

