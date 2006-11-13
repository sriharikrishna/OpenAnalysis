# -*-Mode: makefile;-*-
# $Header: /Volumes/cvsrep/developer/OpenAnalysis/src/OpenAnalysis/Makefile.am,v 1.9 2005/10/17 21:21:40 garvin Exp $

## * BeginRiceCopyright *****************************************************
## 
## Copyright ((c)) 2002, Rice University 
## All rights reserved.
## 
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
## 
## * Redistributions of source code must retain the above copyright
##   notice, this list of conditions and the following disclaimer.
## 
## * Redistributions in binary form must reproduce the above copyright
##   notice, this list of conditions and the following disclaimer in the
##   documentation and/or other materials provided with the distribution.
## 
## * Neither the name of Rice University (RICE) nor the names of its
##   contributors may be used to endorse or promote products derived from
##   this software without specific prior written permission.
## 
## This software is provided by RICE and contributors "as is" and any
## express or implied warranties, including, but not limited to, the
## implied warranties of merchantability and fitness for a particular
## purpose are disclaimed. In no event shall RICE or contributors be
## liable for any direct, indirect, incidental, special, exemplary, or
## consequential damages (including, but not limited to, procurement of
## substitute goods or services; loss of use, data, or profits; or
## business interruption) however caused and on any theory of liability,
## whether in contract, strict liability, or tort (including negligence
## or otherwise) arising in any way out of the use of this software, even
## if advised of the possibility of such damage. 
## 
## ******************************************************* EndRiceCopyright *

#############################################################################
#
# File:
#   $Source: /Volumes/cvsrep/developer/OpenAnalysis/src/OpenAnalysis/Makefile.am,v $
#
# Description:
#   *Process with automake to produce Makefile.in*
#
#   Note: All local variables are prefixed with MY to prevent name
#   clashes with automatic automake variables.
#
#############################################################################

# We do not want the standard GNU files (NEWS README AUTHORS ChangeLog...)
AUTOMAKE_OPTIONS = foreign

#############################################################################
# Common settings
#############################################################################

include $(top_srcdir)/src/Makeinclude.config

#############################################################################
# Local settings
#############################################################################

# Order: Interface; engines in alphabetical order; utils
MYINSTALLEDHEADERS = \
  IRInterface/IRHandles.hpp \
  IRInterface/AccessLocIRInterface.hpp \
  IRInterface/AliasIRInterface.hpp \
  IRInterface/AliasIRInterfaceDefault.hpp \
  IRInterface/CallGraphIRInterface.hpp \
  IRInterface/CallGraphDFProblemIRInterface.hpp \
  IRInterface/ConstValBasicInterface.hpp \
  IRInterface/ConstValIntInterface.hpp \
  IRInterface/CFGIRInterface.hpp \
  IRInterface/CFGIRInterfaceDefault.hpp \
  IRInterface/ICFGIRInterface.hpp \
  IRInterface/DUGIRInterface.hpp \
  IRInterface/ActivityIRInterface.hpp \
  IRInterface/EvalToConstVisitorIRInterface.hpp \
  IRInterface/ExprStmtPairIterator.hpp \
  IRInterface/MREBasicIRShortCircuit.hpp \
  IRInterface/MREBasicIRShortCircuitDefault.hpp \
  IRInterface/ParamBindingsIRInterface.hpp \
  IRInterface/ReachConstsIRInterface.hpp \
  IRInterface/ReachDefsIRInterface.hpp \
  IRInterface/SideEffectIRInterface.hpp \
  IRInterface/InterSideEffectIRInterface.hpp \
  IRInterface/InterSideEffectIRInterfaceDefault.hpp \
  IRInterface/SSAIRInterface.hpp \
  IRInterface/UDDUChainsIRInterface.hpp \
  IRInterface/XAIFIRInterface.hpp \
  \
  OABase/Annotation.hpp \
  \
  AccessLoc/ManagerAccessLocStandard.hpp \
  AccessLoc/Interface.hpp \
  AccessLoc/AccessLocStandard.hpp \
  \
  Activity/DepDFSet.hpp \
  Activity/ManagerDepStandard.hpp \
  Activity/DepStandard.hpp \
  Activity/ManagerInterDep.hpp \
  Activity/InterDep.hpp \
  Activity/ManagerICFGDep.hpp \
  Activity/ManagerProcIterDep.hpp \
  Activity/ICFGDep.hpp \
  Activity/ManagerVaryStandard.hpp \
  Activity/VaryStandard.hpp \
  Activity/ManagerICFGVary.hpp \
  Activity/ManagerICFGVaryInActive.hpp \
  Activity/ManagerICFGUseful.hpp \
  Activity/InterVary.hpp \
  Activity/ManagerUsefulStandard.hpp \
  Activity/UsefulStandard.hpp \
  Activity/InterUseful.hpp \
  Activity/ManagerActiveStandard.hpp \
  Activity/ActiveStandard.hpp \
  Activity/InActivePerStmt.hpp \
  Activity/ManagerEachActive.hpp \
  Activity/ManagerInterActive.hpp \
  Activity/ManagerICFGActive.hpp \
  Activity/InterActive.hpp \
  \
  Alias/Interface.hpp \
  Alias/EquivSets.hpp \
  Alias/ManagerNoAddressOf.hpp \
  Alias/AliasMap.hpp \
  Alias/ManagerAliasMapBasic.hpp \
  Alias/InterAliasInterface.hpp \
  Alias/InterAliasMap.hpp \
  Alias/ManagerInterAliasMapBasic.hpp \
  Alias/SymAliasSets.hpp \
  Alias/ManagerSymAliasSetsTop.hpp \
  Alias/ManagerSymAliasSetsBottom.hpp \
  Alias/ManagerInsNoPtrInterAliasMap.hpp \
  \
  CallGraph/ManagerCallGraphStandard.hpp \
  CallGraph/CallGraphStandard.hpp \
  CallGraph/Interface.hpp \
  \
  CFG/ManagerCFGStandard.hpp \
  CFG/CFGStandard.hpp \
  CFG/Interface.hpp \
  CFG/EachCFGInterface.hpp \
  CFG/EachCFGStandard.hpp \
  \
  DataFlow/DataFlowSet.hpp \
  DataFlow/DGraphIterativeDFP.hpp \
  DataFlow/CFGDFProblem.hpp \
  DataFlow/CallGraphDFProblem.hpp \
  DataFlow/CallGraphDFProblemNew.hpp \
  DataFlow/IRHandleDataFlowSet.hpp \
  DataFlow/LocDFSet.hpp \
  DataFlow/CountDFSet.hpp \
  DataFlow/ManagerParamBindings.hpp \
  DataFlow/ParamBindings.hpp \
  DataFlow/CalleeToCallerVisitor.hpp \
  DataFlow/ICFGDFSolver.hpp \
  DataFlow/ICFGDFProblem.hpp \
  \
  ExprTree/ExprTree.hpp \
  ExprTree/ExprTreeVisitor.hpp \
  ExprTree/DifferentiableLocsVisitor.hpp \
  ExprTree/EvalToConstVisitor.hpp \
  ExprTree/EvalToMemRefVisitor.hpp \
  \
  ICFG/ICFGStandard.hpp \
  ICFG/ManagerICFGStandard.hpp \
  \
  DUG/DUGStandard.hpp \
  DUG/ManagerDUGStandard.hpp \
  \
  duaa/ManagerDUActive.hpp \
  \
  Location/Location.hpp \
  Location/LocationVisitor.hpp \
  \
  MemRefExpr/MemRefExprBasic.hpp \
  MemRefExpr/LocBlock.hpp \
  MemRefExpr/MemRefExpr.hpp \
  MemRefExpr/MemRefExprVisitor.hpp \
  \
  ReachConsts/ReachConstsStandard.hpp \
  ReachConsts/ManagerReachConstsStandard.hpp \
  ReachConsts/Interface.hpp\
  ReachConsts/InterReachConsts.hpp \
  ReachConsts/ManagerInterReachConsts.hpp \
  \
  ReachDefs/Interface.hpp \
  ReachDefs/ReachDefsStandard.hpp \
  ReachDefs/ManagerReachDefsStandard.hpp \
  \
  SideEffect/SideEffectInterface.hpp \
  SideEffect/SideEffectStandard.hpp \
  SideEffect/ManagerSideEffectStandard.hpp \
  SideEffect/ManagerInterSideEffectStandard.hpp \
  SideEffect/InterSideEffectInterface.hpp \
  SideEffect/InterSideEffectStandard.hpp \
  \
  SSA/SSAStandard.hpp \
  SSA/ManagerSSAStandard.hpp \
  SSA/Phi.hpp \
  \
  UDDUChains/Interface.hpp \
  UDDUChains/UDDUChainsStandard.hpp \
  UDDUChains/ManagerUDDUChainsStandard.hpp \
  \
  XAIF/ManagerUDDUChainsXAIF.hpp \
  XAIF/UDDUChainsXAIF.hpp \
  XAIF/ManagerAliasMapXAIF.hpp \
  XAIF/AliasMapXAIF.hpp \
  \
  Utils/BaseGraph.hpp \
  Utils/DomTree.hpp \
  Utils/Exception.hpp \
  Utils/Iterator.hpp \
  Utils/NestedSCR.hpp \
  Utils/OA_ptr.hpp \
  Utils/OutputBuilder.hpp \
  Utils/OutputBuilderText.hpp \
  Utils/RIFG.hpp \
  Utils/SetIterator.hpp \
  Utils/SCC.hpp \
  Utils/Tree.hpp \
  Utils/UnionFindUniverse.hpp \
  Utils/Util.hpp \
  Utils/DGraph/Interface.hpp \
  Utils/DGraph/DGraphInterfaceIterators.hpp \
  Utils/DGraph/DGraphStandard.hpp


# Order: Interface; engines in alphabetical order; utils
MYSOURCES = \
  \
  OABase/Annotation.cpp \
  \
  Utils/OutputBuilder.cpp \
  Utils/OutputBuilderText.cpp \
  \
  AccessLoc/ManagerAccessLocStandard.cpp \
  AccessLoc/AccessLocStandard.cpp \
  \
  Activity/DepDFSet.cpp \
  Activity/ManagerDepStandard.cpp \
  Activity/DepStandard.cpp \
  Activity/InterDep.cpp \
  Activity/ManagerInterDep.cpp \
  Activity/ManagerICFGDep.cpp \
  Activity/ManagerProcIterDep.cpp \
  Activity/ICFGDep.cpp \
  Activity/ManagerVaryStandard.cpp \
  Activity/VaryStandard.cpp \
  Activity/ManagerICFGVary.cpp \
  Activity/ManagerICFGVaryInActive.cpp \
  Activity/ManagerUsefulStandard.cpp \
  Activity/UsefulStandard.cpp \
  Activity/ManagerICFGUseful.cpp \
  Activity/ManagerActiveStandard.cpp \
  Activity/ActiveStandard.cpp \
  Activity/InActivePerStmt.cpp \
  Activity/ManagerEachActive.cpp \
  Activity/ManagerICFGActive.cpp \
  Activity/InterActive.cpp \
  \
  Alias/EquivSets.cpp \
  Alias/ManagerNoAddressOf.cpp \
  Alias/AliasMap.cpp \
  Alias/ManagerAliasMapBasic.cpp \
  Alias/ManagerInterAliasMapBasic.cpp \
  Alias/SymAliasSets.cpp \
  Alias/ManagerSymAliasSetsTop.cpp \
  Alias/ManagerSymAliasSetsBottom.cpp \
  Alias/ManagerInsNoPtrInterAliasMap.cpp \
  \
  CallGraph/CallGraphStandard.cpp \
  CallGraph/ManagerCallGraphStandard.cpp \
  \
  CFG/CFGStandard.cpp \
  CFG/ManagerCFGStandard.cpp \
  \
  DataFlow/CFGDFProblem.cpp \
  DataFlow/CallGraphDFProblem.cpp \
  DataFlow/CallGraphDFProblemNew.cpp \
  DataFlow/DGraphIterativeDFP.cpp \
  DataFlow/LocDFSet.cpp \
  DataFlow/CountDFSet.cpp \
  DataFlow/ManagerParamBindings.cpp \
  DataFlow/ParamBindings.cpp \
  DataFlow/CalleeToCallerVisitor.cpp \
  DataFlow/ICFGDFSolver.cpp \
  \
  ExprTree/ExprTree.cpp \
  ExprTree/DifferentiableLocsVisitor.cpp \
  ExprTree/EvalToConstVisitor.cpp \
  \
  ICFG/ICFGStandard.cpp \
  ICFG/ManagerICFGStandard.cpp \
  \
  DUG/DUGStandard.cpp \
  DUG/ManagerDUGStandard.cpp \
  \
  duaa/ManagerDUActive.cpp \
  \
  Location/Location.cpp \
  \
  MemRefExpr/MemRefExpr.cpp \
  MemRefExpr/MemRefExprBasic.cpp \
  MemRefExpr/LocBlock.cpp \
  \
  ReachConsts/ManagerReachConstsStandard.cpp \
  ReachConsts/ReachConstsStandard.cpp \
  ReachConsts/InterReachConsts.cpp \
  ReachConsts/ManagerInterReachConsts.cpp \
  \
  ReachDefs/ReachDefsStandard.cpp \
  ReachDefs/ManagerReachDefsStandard.cpp \
  \
  SideEffect/SideEffectStandard.cpp \
  SideEffect/ManagerSideEffectStandard.cpp \
  SideEffect/InterSideEffectStandard.cpp \
  SideEffect/ManagerInterSideEffectStandard.cpp \
  \
  SSA/SSAStandard.cpp \
  SSA/ManagerSSAStandard.cpp \
  SSA/Phi.cpp \
  \
  UDDUChains/UDDUChainsStandard.cpp \
  UDDUChains/ManagerUDDUChainsStandard.cpp \
  \
  Utils/BaseGraph.cpp \
  Utils/DomTree.cpp \
  Utils/Graph.cpp \
  Utils/NestedSCR.cpp \
  Utils/RIFG.cpp \
  Utils/SCC.cpp \
  Utils/Tree.cpp \
  Utils/UnionFindUniverse.cpp \
  Utils/DGraph/DGraphStandard.cpp \
  Utils/DGraph/DGraphInterfaceIterators.cpp \
  \
  XAIF/UDDUChainsXAIF.cpp \
  XAIF/ManagerUDDUChainsXAIF.cpp \
  XAIF/ManagerAliasMapXAIF.cpp \
  XAIF/AliasMapXAIF.cpp 

MYCXXFLAGS = @HOST_CXXFLAGS@ $(OA_IFLAGS)

MYLIBADD = @HOST_LIBADD@

if IS_HOST_AR
  MYAR = @HOST_AR@
else
  MYAR = $(AR) cru
endif


#############################################################################
# Automake rules
#############################################################################

#-------------------------------------------------
# Library
#-------------------------------------------------

lib_LIBRARIES =
if OPT_ENABLE_LIBNATIVE
  lib_LIBRARIES += libOAul.a
endif
if OPT_ENABLE_LIBCROSS
  lib_LIBRARIES += libOAsz64.a
endif


if OPT_ENABLE_LIBNATIVE
  libOAul_a_SOURCES = $(MYSOURCES)
  libOAul_a_CXXFLAGS = $(MYCXXFLAGS) -DOA_IRHANDLETYPE_UL 
  libOAul_a_AR = $(MYAR)
  libOAul_a_LIBADD = $(MYLIBADD)
endif

if OPT_ENABLE_LIBCROSS
  libOAsz64_a_SOURCES = $(MYSOURCES)
  libOAsz64_a_CXXFLAGS = $(MYCXXFLAGS) -DOA_IRHANDLETYPE_SZ64
  libOAsz64_a_AR = $(MYAR)
  libOAsz64_a_LIBADD = $(MYLIBADD)
endif

nobase_pkginclude_HEADERS = $(MYINSTALLEDHEADERS)

#-------------------------------------------------
# Test Programs
#-------------------------------------------------

check_PROGRAMS = grtest trtest

trtest_SOURCES = Utils/trtest.cpp 
trtest_CXXFLAGS = $(MYCXXFLAGS) -DOA_IRHANDLETYPE_UL
trtest_LDFLAGS = -L$(builddir) -lOAul

grtest_SOURCES = Utils/grtest.cpp
grtest_CXXFLAGS = $(MYCXXFLAGS) -DOA_IRHANDLETYPE_UL
grtest_LDFLAGS = -L$(builddir) -lOAul 


#############################################################################
# Common rules
#############################################################################

include $(top_srcdir)/src/Makeinclude.rules

