/*! \file
  
  \brief Concrete Location that will use alias results and parameter
         bindings to convert a callee location to a set of caller locations.
  
  \authors Michelle Strout
  \version $Id: CalleeToCallerVisitor.hpp,v 1.2 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef CalleeToCallerVisitor_H
#define CalleeToCallerVisitor_H

#include <OpenAnalysis/Location/LocationVisitor.hpp>
#include <OpenAnalysis/DataFlow/ParamBindings.hpp>
#include <OpenAnalysis/Alias/InterAliasInterface.hpp>

namespace OA {
    namespace DataFlow {

//--------------------------------------------------------------------
/*! 
*/
class CalleeToCallerVisitor : public virtual LocationVisitor {
    public:
        //! get iterator over all caller locs that are associated
        //! with the callee loc that is being visited
        OA_ptr<LocIterator> getCallerLocIterator();

        CalleeToCallerVisitor(
                              ExprHandle call, ProcHandle caller,
                              OA_ptr<Alias::InterAliasInterface> interAlias,
                              OA_ptr<ParamBindings> paramBind);
        ~CalleeToCallerVisitor() {}
        
        void visitNamedLoc(NamedLoc& otherLoc); 

        void visitUnnamedLoc(UnnamedLoc& loc);

        void visitInvisibleLoc(InvisibleLoc& loc);

        void visitUnknownLoc(UnknownLoc& loc);

        void visitLocSubSet(LocSubSet& loc); 

      private:
        ExprHandle mCall;
        ProcHandle mCaller;
        OA_ptr<Alias::InterAliasInterface> mInterAlias;
        OA_ptr<DataFlow::ParamBindings> mParamBind;
        OA_ptr<Alias::Interface> mCallerAlias;
        OA_ptr<LocSet> mLocSet;
    };

    } // end of DataFlow namespace
} // end of OA namespace

#endif
