
// DFAGenLivenessStandard.cpp


#include "DFAGenLivenessStandard.hpp"

namespace OA {
  namespace DFAGenLiveness {

OA_ptr<LocIterator> DFAGenLivenessStandard::getDFAGenLivenessIterator(StmtHandle s) 
{ 
    OA_ptr<LocIterator> retval;

    if (mDFAGenLiveness[s].ptrEqual(0)) {
        OA_ptr<LocSet> emptySet; emptySet = new LocSet;
        retval = new LocSetIterator(emptySet);
    } else {
    retval = new LocSetIterator(mDFAGenLiveness[s]);
    }
    return retval;
}

OA_ptr<LocIterator> DFAGenLivenessStandard::getExitDFAGenLivenessIterator() 
{ 
    OA_ptr<LocIterator> retval;
     if (mExitDFAGenLiveness.ptrEqual(0)) {
        OA_ptr<LocSet> emptySet; emptySet = new LocSet;
        retval = new LocSetIterator(emptySet);
    }
    else {
    retval = new LocSetIterator(mExitDFAGenLiveness);
    }
    return retval;
}

void DFAGenLivenessStandard::output(IRHandlesIRInterface& ir)
{    
    sOutBuild->objStart("DFAGenLivenessStandard");

    sOutBuild->mapStart("mDFAGenLiveness", "StmtHandle", "OA_ptr<LocSet>");
    std::map<StmtHandle,OA_ptr<LocSet> >::iterator mapIter;
    for (mapIter = mDFAGenLiveness.begin(); mapIter != mDFAGenLiveness.end(); mapIter++) {
        StmtHandle s = mapIter->first;
        OA_ptr<LocSet> rdset = mapIter->second;
        if ( rdset.ptrEqual(0) ) continue;

        sOutBuild->mapEntryStart();
        sOutBuild->mapKeyStart();
        sOutBuild->outputIRHandle(s, ir);
        sOutBuild->mapKeyEnd();
        sOutBuild->mapValueStart();
       
       
        sOutBuild->listStart();
        OA_ptr<LocIterator> setIter;
        setIter = getDFAGenLivenessIterator(s);
        for ( ; setIter->isValid(); (*setIter)++ ) {
             sOutBuild->listItemStart(); {
             setIter->current()->output(ir);  
            } sOutBuild->listItemEnd();
        }
        sOutBuild->listEnd();  
        sOutBuild->mapValueEnd();
        sOutBuild->mapEntryEnd();
    }
    sOutBuild->mapEnd("mDFAGenLiveness");
    sOutBuild->objEnd("DFAGenLivenessStandard");
}



void DFAGenLivenessStandard::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    std::cout<< "======DFAGenLiveness===="<< std::endl;
    OA_ptr<LocIterator> locIterPtr;
    std::map<StmtHandle,OA_ptr<LocSet> > ::iterator mapIter;
    for (mapIter = mDFAGenLiveness.begin(); mapIter != mDFAGenLiveness.end(); mapIter++) {
        std::cout<< "\tstmt = " << ir->toString(mapIter->first) << std::endl; 
         locIterPtr = getDFAGenLivenessIterator(mapIter->first);
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            std::cout << "\t\t";
            locIterPtr->current()->dump(std::cout,ir);
            std::cout << std::endl;
        }

    }
}



  } // end of DFAGenLiveness namespace
} // end of OA namespace