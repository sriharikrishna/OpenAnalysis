#include "CallGraphDFProblemIRInterfaceTest.hpp"

void CallGraphDFProblemIRInterfaceTest::addSym(OA::ProcHandle proc, 
        OA::SymHandle sym, 
        OA::OA_ptr<OA::Location::Location> loc)
{
    mSymToLoc[proc][sym] = loc;
}


void CallGraphDFProblemIRInterfaceTest::addFormalParam(OA::SymHandle formal, 
        bool isRef)
{
    mFormalParamIsRef[formal] = isRef;
}

void CallGraphDFProblemIRInterfaceTest::addCallsiteParam(OA::ExprHandle call, 
        OA::ExprHandle param)
{
    mCallsiteParams[call].push_back(param);
}

void CallGraphDFProblemIRInterfaceTest::addCallsiteParamInfo(OA::ExprHandle param,
        OA::SymHandle formal, 
        OA::OA_ptr<OA::ExprTree::ExprTree> etree)
{
    mCallsiteParamFormal[param] = formal;
    mCallsiteParamExprTree[param] = etree;
}

void CallGraphDFProblemIRInterfaceTest::dump(std::ostream& os)
{
    std::cout << "CallGraphDFProblemIRInterfaceTest::dump not implemented yet!";
    std::cout << std::endl;
}


