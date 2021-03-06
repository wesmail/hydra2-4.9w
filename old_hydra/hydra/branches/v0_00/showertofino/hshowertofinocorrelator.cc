#include "hshowertofinocorrelator.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "htofinodetector.h"
#include "hcategory.h"
#include "hmatrixcatiter.h"
#include "hlocation.h"
#include "hshowerhit.h"
#include "htofinocal.h"
#include "hshowerhittof.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "showerdef.h"
#include "htofinocalpar.h"
#include "htofinoshowermap.h"
#include "hlinearcategory.h"
#include "tofinodef.h"
#include "showertofinodef.h"

ClassImp(HShowerTofinoCorrelator)

///////////////////////////////////////////////////////////////////////
//HShowerTofinoCorrelator
//
// HShowerTofinoCorrelator reconstructor searches data describing
// the same pad in HShowerHit and HTofinoCal categories
// and joins them into one object. 
// Results are stored in HShowerHitTof category.
//
//////////////////////////////////////////////////////////////////////

HShowerTofinoCorrelator::HShowerTofinoCorrelator(Text_t *name,Text_t *title) :
  HReconstructor(name,title)
{
   fHitIter=NULL;
   fTofinoIter=NULL;
   fHitTofinoIter=NULL;
   m_pTofinoCalPar = NULL;
   m_pTofShowerMap = NULL;
   m_zeroLoc.set(0);
}

HShowerTofinoCorrelator::HShowerTofinoCorrelator()
{
   fHitIter=NULL;
   fTofinoIter=NULL;
   fHitTofinoIter=NULL;
   m_pTofinoCalPar = NULL;
   m_pTofShowerMap = NULL;
   m_zeroLoc.set(0);
}


HShowerTofinoCorrelator::~HShowerTofinoCorrelator(void) {
     if (fHitIter) delete fHitIter;
     if (fTofinoIter) delete fTofinoIter;
     if (fHitTofinoIter) delete fHitTofinoIter;
}

Bool_t HShowerTofinoCorrelator::init() {
  // creates the ShowerHit, ShowerTof and HTofinoCal categories 
  // and adds them to the current event,
  // creates an iterator which loops over all local maxima
  // and an iterator which loops over Tofino information.

    printf("initialization of Shower/Tofino correlator\n");
    
    HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                                  ->getDetector("Shower");
    HTofinoDetector *pTofinoDet = (HTofinoDetector*)gHades->getSetup()
                                                  ->getDetector("Tofino");
    HRuntimeDb* rtdb=gHades->getRuntimeDb();

    m_pTofinoCat=gHades->getCurrentEvent()->getCategory(catTofinoCal);
    if (!m_pTofinoCat) {
      m_pTofinoCat=pTofinoDet->buildCategory(catTofinoCal);

      if (!m_pTofinoCat) return kFALSE;
      else gHades->getCurrentEvent()
                 ->addCategory(catTofinoCal, m_pTofinoCat, "Tofino");
    }

    m_pHitCat=gHades->getCurrentEvent()->getCategory(catShowerHit);
    if (!m_pHitCat) {
      m_pHitCat=pShowerDet->buildCategory(catShowerHit);

      if (!m_pHitCat) return kFALSE;
      else gHades->getCurrentEvent()
                 ->addCategory(catShowerHit, m_pHitCat, "Shower");
    }

    m_pHitTofCat=gHades->getCurrentEvent()->getCategory(catShowerHitTof);
    if (!m_pHitTofCat) {
      //m_pHitTofCat=pShowerDet->buildCategory(catShowerHitTof);
      m_pHitTofCat = new HLinearCategory("HShowerHitTof", 1000);
      if (!m_pHitTofCat) return kFALSE;
      else gHades->getCurrentEvent()
                 ->addCategory(catShowerHitTof, m_pHitTofCat, "Tofino");
    }

    m_pTofinoCalPar = rtdb->getContainer("TofinoCalPar");
    if (!m_pTofinoCalPar) return kFALSE;

    m_pTofShowerMap = rtdb->getContainer("TofinoShowerMap");
    if (!m_pTofShowerMap) return kFALSE;

    fHitIter=(HIterator*)m_pHitCat->MakeIterator();
    fTofinoIter=(HIterator*)m_pTofinoCat->MakeIterator();
    fHitTofinoIter=(HIterator*)m_pHitTofCat->MakeIterator();

    return kTRUE;
}

Bool_t HShowerTofinoCorrelator::finalize(void) {
   return kTRUE;
}


HShowerHitTof* HShowerTofinoCorrelator::addHitTof(HShowerHit* pHit) {
// create one object, which contains data
// from HShowerHit and connect with it data from Tofino
  
    HShowerHitTof *pHitTof;

    pHitTof=(HShowerHitTof *)m_pHitTofCat->getNewSlot(m_zeroLoc);
    if (pHitTof!=NULL) {
      pHitTof=new(pHitTof) HShowerHitTof;

      *pHitTof = *pHit;
      pHitTof->setTof(0.0);
      pHitTof->setTofinoCell(9);
    }
  
    return pHitTof;
}



Int_t HShowerTofinoCorrelator::execute()
{
  HShowerHit *pHit;
  HShowerHitTof *pHitTof;
  HTofinoCal *pTCal;
  HTofinoCal *pTofCalTab[24];
  Int_t nMult[24]; //multiplicity in shower detector covered by one Tofino cell
  Int_t nSector, nRow, nTofCell; 
  Int_t nTofIndex;
  Float_t fTime, fDriftTime;

  fHitIter->Reset();
  fTofinoIter->Reset();

  //clearing tables before analysis next event
  for (Int_t i = 0; i < 24; i++) {
    nMult[i] = 0;
    pTofCalTab[i] = NULL;
  }

  //mapping data to table indexed by sector,cell combination
  while((pTCal = (HTofinoCal*)fTofinoIter->Next())) {
    pTofCalTab[pTCal->getSector()*4+pTCal->getCell()] = pTCal;
  }

  while((pHit = (HShowerHit*)fHitIter->Next())) {
     if (pHit->getModule()!=0) continue; //correlation only for first module

     //looking for Tofino cell covering pad row, col
     nTofCell = ((HTofinoShowerMap*)m_pTofShowerMap)->
               getCellNumber(pHit->getRow(), pHit->getCol());
     if (nTofCell==9) continue; //empty entry in map

     pHitTof = addHitTof(pHit);
     if (pHitTof) {
       nSector = pHit->getSector();
       nRow = pHit->getRow();

       nTofIndex = nSector*4 + nTofCell; 

       //calculation calibrated time of flight
       fDriftTime = ((HTofinoCalPar*)m_pTofinoCalPar)->
                          calcDriftTime(nSector, nTofCell, (Float_t)nRow);

       //filling new information
       if (pTofCalTab[nTofIndex]) {
         fTime = pTofCalTab[nTofIndex]->getTime();

         pHitTof->setTof(fTime - fDriftTime);
         pHitTof->setDriftTime(fDriftTime);
         pHitTof->setADC(pTofCalTab[nTofIndex]->getCharge());
       }
       else fTime = -100.0;

       pHitTof->setTofinoCell(nTofCell);
       nMult[nTofIndex]++;
     }
  
  }

  //updating multiplicity information
  fHitTofinoIter->Reset();
  while((pHitTof = (HShowerHitTof *)fHitTofinoIter->Next())) {
     nTofIndex = pHitTof->getSector()*4 +  pHitTof->getTofinoCell();
     pHitTof->setTofinoMult(nMult[nTofIndex]);
  }

  return 0;
}






