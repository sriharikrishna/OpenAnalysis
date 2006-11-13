#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined in top.lex.c from top-parser.l
extern int toplex();
//extern char* retstr;

// aliasparse defined in Alias.tab.c from Alias.y
#include "AliasIRInterfaceTest.hpp"
OA::OA_ptr<AliasIRInterfaceTest> gAliasIR;
extern int aliasdebug;
extern int aliasparse();

// accesslocparse defined in AccessLoc.tab.c from AccessLoc.y
#include "AccessLocIRInterfaceTest.hpp"
OA::OA_ptr<AccessLocIRInterfaceTest> gAccessLocIR;
extern int accesslocdebug;
extern int accesslocparse();

// depparse defined in Dep.tab.c from Dep.y
#include "DepIRInterfaceTest.hpp"
OA::OA_ptr<DepIRInterfaceTest> gDepIR;
extern int depdebug;
extern int depparse();

// callgraphlocparse defined in CallGraph.tab.c 
// from CallGraph.y
#include "CallGraphIRInterfaceTest.hpp"
OA::OA_ptr<CallGraphIRInterfaceTest> gCallGraphIR;
extern int callgraphdebug;
extern int callgraphparse();

// callgraphdfproblemlocparse defined in CallGraphDFProblem.tab.c 
// from CallGraphDFProblem.y
#include "CallGraphDFProblemIRInterfaceTest.hpp"
OA::OA_ptr<CallGraphDFProblemIRInterfaceTest> gCallGraphDFProblemIR;
extern int callgraphdfproblemdebug;
extern int callgraphdfproblemparse();


// sideeffectparse defined in SideEffect.tab.c from SideEffect.y
#include "SideEffectIRInterfaceTest.hpp"
OA::OA_ptr<SideEffectIRInterfaceTest> gSideEffectIR;
extern int sideeffectdebug;
extern int sideeffectparse();


// getting defines of toplex retvals
#include "top-parser.token.h"

// lexing from a string
extern char * input_buffer;
extern char * input_ptr;
extern int input_lim;
#define min(a,b) ((a)<(b) ? (a) : (b))

int 
my_yyinput_getchar()
{
    int n = input_lim - (int)input_ptr;
    int retchar;

    if (n> 0) {
        retchar = input_ptr[0];
        input_ptr ++;
    } else {
        retchar = EOF;
    }
    return retchar;
}

void top_parser()
{
  //=================================================================
  // FSM for top-parser
  //
  // OutsideProc:
  //     T_PROCEDURE     --> StartProc
  //     others          --> Error
  // 
  // StartProc:
  //     T_ID            --> InsideProc
  //     others          --> Error
  // 
  // InsideProc:
  //     T_ALIASIRINTERFACE --> BeginMiniParser 
  //                            ( whichIRInterface = T_ALIASIRINTERFACE )
  //     T_END                --> EndProc
  //     others             --> Error
  //
  // EndProc:            
  //     T_PROCEDURE    --> EndProc
  //     T_ID           --> OutsideProc
  //     others         --> Error
  // 
  // BeginMiniParser:
  //     {               --> call specific parser and goto EndMiniParser
  //     others          --> Error
  //     
  // EndMiniParser:
  //     }               --> InsideProc
  //     others          --> Error
  //
  //=================================================================
  typedef enum { OutsideProc, StartProc, InsideProc, BeginMiniParser, 
                 EndMiniParser, EndProc, Error
               } FSMState;
  FSMState state = OutsideProc;
  int token, whichIRInterface;

  // loop on tokens
  while ( (token=toplex()) > 0 ) {
      switch (state) {

          case OutsideProc:
              switch (token) {
                  case T_PROCEDURE:
                      state = StartProc;
                      break;
                  default:
                      state = Error;
                      printf("Error: Expecting PROCEDURE keyword, got %d\n", token);
                      break;
              }
              break;

          case StartProc:
              switch (token) {
                  case T_ID:
                      state = InsideProc;
                      break;
                  default:
                      state = Error;
                      printf("Error: Expecting procedure name, got %d\n", token);
                      break;
              }
              break;

          case InsideProc:
              whichIRInterface = token;
              switch (token) {
                  case T_END:
                      state = EndProc;
                      break;
                  case T_ACCESSLOCIRINTERFACE:
                  case T_ALIASIRINTERFACE:
                  case T_CALLGRAPHIRINTERFACE:
                  case T_CALLGRAPHDFPROBLEMIRINTERFACE:
                  case T_DEPIRINTERFACE:
                  case T_SIDEEFFECTIRINTERFACE:
                      state = BeginMiniParser;
                      break;
                  default:
                      state = Error;
                      printf("Error: Expecting an IRInterface identifier, got %d\n", 
                             token);
                      break;
              }
              break;

          case EndProc:
              switch (token) {
                  case T_PROCEDURE:
                      state = EndProc;
                      break;
                  case T_ID:
                      state = OutsideProc;
                      break;
                  default:
                      state = Error;
                      printf("Error: Expecting PROCEDURE id, got %d\n", 
                             token);
                      break;
              }
              break;


          case BeginMiniParser:
              switch (token) {
                  case '{':
                      /*
                      int yychar;
                      while ((yychar=aliaslex()) > 0) {
                        printf("yychar = %d\n", yychar);
                      }
                      */
                      // call specific parser
                      switch (whichIRInterface) {
                          case T_ACCESSLOCIRINTERFACE:
                              // only need to create new one for first proc
                              if (gAccessLocIR.ptrEqual(0)) {
                                gAccessLocIR = new AccessLocIRInterfaceTest;
                              }
                              // accesslocparse sees gAccessLocIR as global
                              //accesslocdebug = 1;
                              accesslocparse();
                              break;
                          case T_ALIASIRINTERFACE:
                              // only need to create new one for first proc
                              if (gAliasIR.ptrEqual(0)) {
                                gAliasIR = new AliasIRInterfaceTest;
                              }
                              // aliasparse sees gAliasIR as global
                              //aliasdebug = 1;
                              aliasparse();
                              break;
                          case T_CALLGRAPHDFPROBLEMIRINTERFACE:
                              // only need to create new one for first proc
                              if (gCallGraphDFProblemIR.ptrEqual(0)) {
                                gCallGraphDFProblemIR = new CallGraphDFProblemIRInterfaceTest;
                              }
                              // callgraphdfproblemparse sees 
                              // gCallGraphDFProblemIR as global
                              //callgraphdfproblemdebug = 1;
                              callgraphdfproblemparse();
                              break;
                          case T_CALLGRAPHIRINTERFACE:
                              // only need to create new one for first proc
                              if (gCallGraphIR.ptrEqual(0)) {
                                gCallGraphIR = new CallGraphIRInterfaceTest;
                              }
                              // callgraphparse sees gCallGraphIR as global
                              //callgraphdebug = 1;
                              callgraphparse();
                              break;
                          case T_DEPIRINTERFACE:
                              // only need to create new one for first proc
                              if (gDepIR.ptrEqual(0)) {
                                gDepIR = new DepIRInterfaceTest;
                              }
                              // depparse sees gDepIR as global
                              //depdebug = 1;
                              depparse();
                              break;
                          case T_SIDEEFFECTIRINTERFACE:
                              // only need to create new one for first proc
                              if (gSideEffectIR.ptrEqual(0)) {
                                gSideEffectIR = new SideEffectIRInterfaceTest;
                              }
                              // sideeffectparse sees gSideEffectIR as global
                              //sideeffectdebug = 1;
                              sideeffectparse();
                              break;
                      }
                      state = EndMiniParser;
                      break;
                  default:
                      state = Error;
                      printf("Error: Expecting '{', got %d\n", token);
                      break;
              }
              break;

          case EndMiniParser:
              switch (token) {
                  case '}':
                      state = InsideProc;
                      break;
                  default:
                      state = Error;
                      printf("Error: Expecting a '}', got %d\n", token);
                      break;
              }
              break;

          case Error:
              printf("Exiting due to Error\n");
              exit(0);
      } // end switch over FSM states
  } // end while

}

/*
#include <OpenAnalysis/Alias/AliasMap.hpp>
#include <OpenAnalysis/CallGraph/CallGraphStandard.hpp>
#include <OpenAnalysis/CFG/CFGStandard.hpp>
#include <OpenAnalysis/ReachConsts/ReachConstsStandard.hpp>
#include <OpenAnalysis/ReachDefs/ReachDefsStandard.hpp>
#include <OpenAnalysis/SideEffect/SideEffectStandard.hpp>
#include <OpenAnalysis/UDDUChains/UDDUChainsStandard.hpp>
#include <OpenAnalysis/XAIF/UDDUChainsXAIF.hpp>

void linkTest()
{
    OA::OA_ptr<OA::Alias::Interface> aliasMap; 
    aliasMap = new OA::Alias::AliasMap(OA::ProcHandle(0));

    OA::OA_ptr<OA::CallGraph::Interface> callgraph; 
    callgraph = new OA::CallGraph::CallGraphStandard();

    OA::OA_ptr<OA::CFG::Interface> cfg; 
    cfg = new OA::CFG::CFGStandard(OA::SymHandle(0));

    OA::OA_ptr<OA::ReachConsts::Interface> reachConsts; 
    reachConsts = new OA::ReachConsts::ReachConstsStandard(OA::ProcHandle(0));

    OA::OA_ptr<OA::ReachDefs::Interface> reachDefs; 
    reachDefs = new OA::ReachDefs::ReachDefsStandard(OA::ProcHandle(0));

    OA::OA_ptr<OA::SideEffect::Interface> sideEffect; 
    sideEffect = new OA::SideEffect::SideEffectStandard();

    OA::OA_ptr<OA::UDDUChains::Interface> uddu; 
    uddu = new OA::UDDUChains::UDDUChainsStandard(OA::ProcHandle(0));

    OA::OA_ptr<OA::XAIF::UDDUChainsXAIF> udduXAIF; 
    udduXAIF = new OA::XAIF::UDDUChainsXAIF(OA::ProcHandle(0));
}
*/
