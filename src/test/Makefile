# NEEDS GENERALIZED!!
PLATFORM       = powerpc-Darwin


CC             = g++
OADIR          = ../../
CXXFLAGS       = -g -I$(OADIR)/$(PLATFORM)/include -DOA_IRHANDLETYPE_UL -Wall
#CXXFLAGS       = -DYYDEBUG=1 -g -I$(OADIR)/$(PLATFORM)/include -DOA_IRHANDLETYPE_UL -Wall
OALIBDIR       = $(OADIR)/$(PLATFORM)/lib

LIBS           = -L$(OALIBDIR) -lOAul -lfl

DRIVER         = driver
#DRIVEROBJS     = top.lex.o Alias.lex.o top-parser.o Alias.tab.o \
#                 AliasIRInterfaceTest.o IRHandlesIRInterfaceTest.o driver.o \
#                 AccessLocIRInterfaceTest.o AccessLoc.lex.o AccessLoc.tab.o
DRIVEROBJS     = top.lex.o AccessLoc.lex.o Alias.lex.o SideEffect.lex.o\
                 CallGraphDFProblem.lex.o \
                 CallGraph.lex.o \
                 Dep.lex.o \
                 top-parser.o \
                 AccessLoc.tab.o Alias.tab.o SideEffect.tab.o \
                 CallGraphDFProblem.tab.o \
                 CallGraph.tab.o \
                 Dep.tab.o \
                 AccessLocIRInterfaceTest.o AliasIRInterfaceTest.o \
                 IRHandlesIRInterfaceTest.o driver.o \
                 SideEffectIRInterfaceTest.o \
                 CallGraphDFProblemIRInterfaceTest.o \
                 CallGraphIRInterfaceTest.o \
                 DepIRInterfaceTest.o

DRIVERSRCS     = top.lex.c Alias.lex.c Alias.tab.c Alias.tab.h top-parser.c \
                 AliasIRInterfaceTest.cpp AliasIRInterfaceTest.hpp \
                 IRHandlesIRInterfaceTest.cpp IRHandlesIRInterfaceTest.hpp \
                 driver.cpp top-parser.h top-parser.token.h top-parser-lex.l \
                 AccessLocIRInterfaceTest.cpp AccessLocIRInterfaceTest.hpp \
                 AccessLoc.tab.c AccessLoc.lex.c AccessLoc.tab.h \
                 SideEffectIRInterfaceTest.hpp SideEffectIRInterfaceTest.cpp \
                 SideEffect.tab.c SideEffect.lex.c SideEffect.tab.h \
                 CallGraphDFProblemIRInterfaceTest.hpp \
                 CallGraphDFProblemIRInterfaceTest.cpp \
                 CallGraphDFProblem.tab.c CallGraphDFProblem.lex.c \
                 CallGraphDFProblem.tab.h \
                 CallGraphIRInterfaceTest.hpp \
                 CallGraphIRInterfaceTest.cpp \
                 CallGraph.tab.c CallGraph.lex.c \
                 CallGraph.tab.h \
                 DepIRInterfaceTest.hpp DepIRInterfaceTest.cpp \
                 Dep.tab.c Dep.lex.c Dep.tab.h

all: $(DRIVER)

# creates toplex routine for the top-parser routine
top.lex.c: top-parser-lex.l top-parser.token.h
	flex -Ptop -otop.lex.c top-parser-lex.l

################################### creates accessloclex routine
AccessLoc.lex.c: AccessLoc.l AccessLoc.tab.h
	flex -Paccessloc -oAccessLoc.lex.c AccessLoc.l

# creates accesslocparse routine
AccessLoc.tab.c: AccessLoc.y AccessLocIRInterfaceTest.hpp AccessLocIRInterfaceTest.cpp
	bison -d -p accessloc -o AccessLoc.tab.c AccessLoc.y

AccessLoc.tab.h: AccessLoc.y AccessLocIRInterfaceTest.hpp AccessLocIRInterfaceTest.cpp
	bison -d -p accessloc -o AccessLoc.tab.c AccessLoc.y


################################### creates aliaslex routine
Alias.lex.c: Alias.l Alias.tab.h
	flex -Palias -oAlias.lex.c Alias.l

# creates aliasparse routine
Alias.tab.c: Alias.y AliasIRInterfaceTest.cpp AliasIRInterfaceTest.hpp
	bison -d -p alias -o Alias.tab.c Alias.y

Alias.tab.h: Alias.y AliasIRInterfaceTest.cpp AliasIRInterfaceTest.hpp
	bison -d -p alias -o Alias.tab.c Alias.y

################################### creates callgraphlex routine
CallGraph.lex.c: CallGraph.l CallGraph.tab.h
	flex -Pcallgraph -oCallGraph.lex.c CallGraph.l

# creates callgraphparse routine
CallGraph.tab.c: CallGraph.y CallGraphIRInterfaceTest.cpp CallGraphIRInterfaceTest.hpp
	bison -v -d -p callgraph -o CallGraph.tab.c CallGraph.y

CallGraph.tab.h: CallGraph.y  CallGraphIRInterfaceTest.cpp CallGraphIRInterfaceTest.hpp
	bison -v -d -p callgraph -o CallGraph.tab.c CallGraph.y


################################### creates callgraphdfproblemlex routine
CallGraphDFProblem.lex.c: CallGraphDFProblem.l CallGraphDFProblem.tab.h
	flex -Pcallgraphdfproblem -oCallGraphDFProblem.lex.c CallGraphDFProblem.l

# creates callgraphdfproblemparse routine
CallGraphDFProblem.tab.c: CallGraphDFProblem.y CallGraphDFProblemIRInterfaceTest.cpp CallGraphDFProblemIRInterfaceTest.hpp
	bison -v -d -p callgraphdfproblem -o CallGraphDFProblem.tab.c CallGraphDFProblem.y

CallGraphDFProblem.tab.h: CallGraphDFProblem.y  CallGraphDFProblemIRInterfaceTest.cpp CallGraphDFProblemIRInterfaceTest.hpp
	bison -v -d -p callgraphdfproblem -o CallGraphDFProblem.tab.c CallGraphDFProblem.y

################################### creates deplex routine
Dep.lex.c: Dep.l Dep.tab.h
	flex -Pdep -oDep.lex.c Dep.l

# creates depparse routine
Dep.tab.c: Dep.y DepIRInterfaceTest.cpp DepIRInterfaceTest.hpp
	bison -v -d -p dep -o Dep.tab.c Dep.y

Dep.tab.h: Dep.y  DepIRInterfaceTest.cpp DepIRInterfaceTest.hpp
	bison -v -d -p dep -o Dep.tab.c Dep.y




################################### creates sideeffectlex routine
SideEffect.lex.c: SideEffect.l SideEffect.tab.h
	flex -Psideeffect -oSideEffect.lex.c SideEffect.l

# creates sideeffectparse routine
SideEffect.tab.c: SideEffect.y SideEffectIRInterfaceTest.hpp SideEffectIRInterfaceTest.cpp
	bison -d -p sideeffect -o SideEffect.tab.c SideEffect.y

SideEffect.tab.h: SideEffect.y SideEffectIRInterfaceTest.hpp SideEffectIRInterfaceTest.cpp
	bison -d -p sideeffect -o SideEffect.tab.c SideEffect.y


#####################################################################
$(DRIVER): $(DRIVEROBJS) $(OADIR)/$(PLATFORM)/lib/libOAul.a
	$(CC) $(CXXFLAGS) -o $(DRIVER) $(DRIVEROBJS) $(LIBS)

.cpp.o:
	$(CC) $(CXXFLAGS) -c $<

.c.o:
	$(CC) $(CXXFLAGS) -c $<

clean:
	/bin/rm -f $(DRIVEROBJS) $(DRIVER) *.tab.* *.lex.c

