
#include "AccessLocIRInterfaceTest.hpp"
#include "AliasIRInterfaceTest.hpp"
#include "CallGraphIRInterfaceTest.hpp"
#include "CallGraphDFProblemIRInterfaceTest.hpp"
#include "DepIRInterfaceTest.hpp"
#include "SideEffectIRInterfaceTest.hpp"

// global variables created in top-parser routine
extern OA::OA_ptr<AccessLocIRInterfaceTest> gAccessLocIR;
extern OA::OA_ptr<AliasIRInterfaceTest> gAliasIR;
extern OA::OA_ptr<CallGraphIRInterfaceTest> gCallGraphIR;
extern OA::OA_ptr<CallGraphDFProblemIRInterfaceTest> gCallGraphDFProblemIR;
extern OA::OA_ptr<DepIRInterfaceTest> gDepIR;
extern OA::OA_ptr<SideEffectIRInterfaceTest> gSideEffectIR;

void top_parser();
