#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "top-parser.h"

#include <OpenAnalysis/Alias/ManagerAliasMapBasic.hpp>
#include <OpenAnalysis/Alias/ManagerInterAliasMapBasic.hpp>
#include <OpenAnalysis/AccessLoc/ManagerAccessLocStandard.hpp>
#include <OpenAnalysis/Activity/ManagerDepStandard.hpp>
#include <OpenAnalysis/CallGraph/ManagerCallGraphStandard.hpp>
#include <OpenAnalysis/SideEffect/ManagerSideEffectStandard.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectStandard.hpp>
#include <OpenAnalysis/SideEffect/ManagerInterSideEffectStandard.hpp>

#include "InterSideEffectIRInterfaceTest.hpp"

// lexing from a string
// global variables set up by this driver and then used by the parsers
// because of my_yyinput_getchar in top-parser.c
char * input_buffer;
char * input_ptr;
int input_lim;

int main(int argc, char **argv)
{
  FILE * pFile;
  long lSize;

  // open input file
  if (argc <= 1) { 
      printf( "usage: top-parser file.in\n");
      exit(0);
  }
  pFile = fopen ( argv[1] , "rb" );
  if (pFile==NULL) exit (1);

  // obtain file size
  int ret = fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file
  input_buffer = (char*) malloc (lSize);
  if (input_buffer == NULL) exit (2);

  // copy the whole file into the buffer
  size_t num_read = fread (input_buffer,1,lSize,pFile);
  input_ptr = input_buffer;
  input_lim = (int) (strlen(input_buffer) + input_ptr);

  // parse all possible IRInterface information out of the file
  top_parser();
  
  //----------------------------------------------------------------------
  // AccessLoc
  if (!gAccessLocIR.ptrEqual(0)) {

    OA::OA_ptr<OA::AccessLoc::ManagerAccessLocStandard> accessLocMan;
    accessLocMan = new OA::AccessLoc::ManagerAccessLocStandard(gAccessLocIR);

    // iterate over all the procedures in the file and perform analysis
    OA::OA_ptr<OA::IRProcIterator> procIterPtr = gAccessLocIR->getProcIterator();
    for (; procIterPtr->isValid(); (*procIterPtr)++ ) {
      OA::OA_ptr<OA::AccessLoc::AccessLocStandard> accessLoc
        = accessLocMan->performAnalysis(procIterPtr->current());

      accessLoc->dump(std::cout, gAccessLocIR);
    } 

  }

  //----------------------------------------------------------------------
  // AliasMap
  if (!gAliasIR.ptrEqual(0)) {

    OA::OA_ptr<OA::Alias::ManagerAliasMapBasic> aliasmapman;
    aliasmapman = new OA::Alias::ManagerAliasMapBasic(gAliasIR);

    // iterate over all the procedures in the file and perform analysis
    OA::OA_ptr<OA::IRProcIterator> procIterPtr = gAccessLocIR->getProcIterator();
    for (; procIterPtr->isValid(); (*procIterPtr)++ ) {
      OA::OA_ptr<OA::Alias::AliasMap> aliasMap
        = aliasmapman->performAnalysis(procIterPtr->current());
      aliasMap->dump(std::cout, gAliasIR);
    }
  }
   
  //----------------------------------------------------------------------
  // CallGraph
  if (!gCallGraphIR.ptrEqual(0)) {

    OA::OA_ptr<OA::CallGraph::ManagerStandard> cgraphman;
    cgraphman = new OA::CallGraph::ManagerStandard(gCallGraphIR);

    OA::OA_ptr<OA::IRProcIterator> procIterPtr = gCallGraphIR->getProcIterator();
    OA::OA_ptr<OA::CallGraph::CallGraphStandard> cgraph;
    cgraph = cgraphman->performAnalysis(procIterPtr);

    cgraph->dump(std::cout, gCallGraphIR);
  }
   
  //----------------------------------------------------------------------
  // Activity/Dep
  if (!gDepIR.ptrEqual(0) && !gAliasIR.ptrEqual(0)
      && !gAccessLocIR.ptrEqual(0) && !gCallGraphIR.ptrEqual(0)
      && !gCallGraphDFProblemIR.ptrEqual(0)) 
  {
    // Alias 
    OA::OA_ptr<OA::Alias::ManagerInterAliasMapBasic> interaliasmapman;
    interaliasmapman = new OA::Alias::ManagerInterAliasMapBasic(gAliasIR);
    OA::OA_ptr<OA::Alias::InterAliasMap> interAlias;
    interAlias = interaliasmapman->performAnalysis();

    // CallGraph
    OA::OA_ptr<OA::CallGraph::ManagerStandard> cgraphman;
    cgraphman = new OA::CallGraph::ManagerStandard(gCallGraphIR);

    OA::OA_ptr<OA::IRProcIterator> procIterPtr = gCallGraphIR->getProcIterator();
    OA::OA_ptr<OA::CallGraph::CallGraphStandard> cgraph;
    cgraph = cgraphman->performAnalysis(procIterPtr);

    // Intra Side-Effect
    OA::OA_ptr<OA::SideEffect::ManagerStandard> sideeffectman;
    sideeffectman = new OA::SideEffect::ManagerStandard(gSideEffectIR);

    // InterSideEffect
    // first create interSEIR
    OA::OA_ptr<InterSideEffectIRInterfaceTest> interSEIR;
    interSEIR = new InterSideEffectIRInterfaceTest(gSideEffectIR,
                                                   gCallGraphDFProblemIR);
    OA::OA_ptr<OA::SideEffect::ManagerInterSideEffectStandard> interSEman;
    interSEman 
        = new OA::SideEffect::ManagerInterSideEffectStandard(interSEIR);

    OA::OA_ptr<OA::SideEffect::InterSideEffectStandard> interSE;
    interSE = interSEman->performAnalysis(cgraph, interAlias, sideeffectman);

    // Activity
    OA::OA_ptr<OA::Activity::ManagerDepStandard> depman;
    depman = new OA::Activity::ManagerDepStandard(gDepIR);

    // iterate over all the procedures in the file and perform analysis
    procIterPtr = gDepIR->getProcIterator();
    for (; procIterPtr->isValid(); (*procIterPtr)++ ) {
      OA::OA_ptr<OA::Activity::DepStandard> dep
        = depman->performAnalysis(procIterPtr->current(), 
                interAlias->getAliasResults(procIterPtr->current()),
                interSE);
      dep->dump(std::cout, gAliasIR);
    }
  }
   
  //----------------------------------------------------------------------
  // InterSideEffect
  if (!gSideEffectIR.ptrEqual(0) && !gAliasIR.ptrEqual(0)
      && !gAccessLocIR.ptrEqual(0) && !gCallGraphIR.ptrEqual(0)
      && !gCallGraphDFProblemIR.ptrEqual(0)) 
  {
    // Alias 
    OA::OA_ptr<OA::Alias::ManagerInterAliasMapBasic> interaliasmapman;
    interaliasmapman = new OA::Alias::ManagerInterAliasMapBasic(gAliasIR);
    OA::OA_ptr<OA::Alias::InterAliasMap> interAlias;
    interAlias = interaliasmapman->performAnalysis();

    // CallGraph
    OA::OA_ptr<OA::CallGraph::ManagerStandard> cgraphman;
    cgraphman = new OA::CallGraph::ManagerStandard(gCallGraphIR);

    OA::OA_ptr<OA::IRProcIterator> procIterPtr = gCallGraphIR->getProcIterator();
    OA::OA_ptr<OA::CallGraph::CallGraphStandard> cgraph;
    cgraph = cgraphman->performAnalysis(procIterPtr);

    // Intra Side-Effect
    OA::OA_ptr<OA::SideEffect::ManagerStandard> sideeffectman;
    sideeffectman = new OA::SideEffect::ManagerStandard(gSideEffectIR);

    // InterSideEffect
    // first create interSEIR
    OA::OA_ptr<InterSideEffectIRInterfaceTest> interSEIR;
    interSEIR = new InterSideEffectIRInterfaceTest(gSideEffectIR,
                                                   gCallGraphDFProblemIR);
    OA::OA_ptr<OA::SideEffect::ManagerInterSideEffectStandard> interSEman;
    interSEman 
        = new OA::SideEffect::ManagerInterSideEffectStandard(interSEIR);

    OA::OA_ptr<OA::SideEffect::InterSideEffectStandard> interSE;
    interSE = interSEman->performAnalysis(cgraph, interAlias, sideeffectman);

    interSE->dump(std::cout, gSideEffectIR);
  }

 
  //----------------------------------------------------------------------
  // SideEffect
  if (!gSideEffectIR.ptrEqual(0) && !gAliasIR.ptrEqual(0)
      && !gAccessLocIR.ptrEqual(0)) 
  {

    OA::OA_ptr<OA::AccessLoc::ManagerAccessLocStandard> accessLocMan;
    accessLocMan = new OA::AccessLoc::ManagerAccessLocStandard(gAccessLocIR);

    OA::OA_ptr<OA::Alias::ManagerAliasMapBasic> aliasmapman;
    aliasmapman = new OA::Alias::ManagerAliasMapBasic(gAliasIR);

    OA::OA_ptr<OA::SideEffect::ManagerStandard> sideeffectman;
    sideeffectman = new OA::SideEffect::ManagerStandard(gSideEffectIR);

    // iterate over all the procedures in the file and perform analysis
    OA::OA_ptr<OA::IRProcIterator> procIterPtr 
        = gSideEffectIR->getProcIterator();
    for (; procIterPtr->isValid(); (*procIterPtr)++ ) {
      OA::OA_ptr<OA::SideEffect::InterSideEffectStandard> interSE;
      interSE = new OA::SideEffect::InterSideEffectStandard;

      OA::OA_ptr<OA::Alias::AliasMap> aliasMap
        = aliasmapman->performAnalysis(procIterPtr->current());

      OA::OA_ptr<OA::SideEffect::SideEffectStandard> sideEffect
        = sideeffectman->performAnalysis(procIterPtr->current(),
                                         aliasMap,interSE);
      sideEffect->dump(std::cout, gSideEffectIR);
    }
  }


  exit(0);

}

