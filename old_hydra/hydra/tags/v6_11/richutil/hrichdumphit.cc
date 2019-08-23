// $Id: hrichdumphit.cc,v 1.2 2002-09-25 16:19:53 eberl Exp $
// Last update by Thomas Eberl: 02/09/25 17:33:19
//
#include "hrichdumphit.h"
#include "hrichhit.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hrichdetector.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hmatrixcatiter.h"
#include "hlocation.h"
#include "hrichraw.h"
#include "hrichcalpar.h"
#include "hrichcal.h"
#include "hdebug.h"
#include "hades.h"
#include "richdef.h"
#include <iostream.h>

ClassImp(HRichDumpHit)

HRichDumpHit::HRichDumpHit(Text_t *name,Text_t *title) :
  HReconstructor(name,title)
{
    fIter = NULL;
    nCounter = 0;
    nEvtCounter = 0;
}

HRichDumpHit::HRichDumpHit()
{
    fIter = NULL;
    nCounter = 0;
    nEvtCounter = 0;
}


HRichDumpHit::~HRichDumpHit(void) {
   if (fIter) delete fIter;
}


Bool_t HRichDumpHit::init() {

    HRichDetector *pRichDet = (HRichDetector*)gHades->getSetup()
                                                  ->getDetector("Rich");

    fHitCat=gHades->getCurrentEvent()->getCategory(catRichHit);
    if (!fHitCat) {
      fHitCat=pRichDet->buildCategory(catRichHit);

      if (!fHitCat) return kFALSE;
      else gHades->getCurrentEvent()
                         ->addCategory(catRichHit, fHitCat, "Rich");
    }

    fIter = (HIterator*)getHitCat()->MakeIterator(); 
    return kTRUE;
}

Bool_t HRichDumpHit::finalize() {

    return kTRUE;
}


Int_t HRichDumpHit::execute()
{
  HRichHit *pHit;
  fIter->Reset(); 
  while((pHit = (HRichHit *)fIter->Next()))
  {
      nCounter++;
      cout<<"evt no:"<<nEvtCounter<<" theta:"<<pHit->getTheta()<<" phi:"<<pHit->getPhi()<<" sec:"<<pHit->getSector()<<" ring nb:"<<nCounter<<endl;
  }
  
  nEvtCounter++;
  return 0;
}