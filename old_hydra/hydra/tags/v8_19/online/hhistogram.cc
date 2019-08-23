//*-- Author : Dmitry Vasiliev (vassili@lns.infn.it)
/* 
 * Copyright P.Finocchiaro and D.Vasiliev (INFN-LNS, Catania) HADES collaboration
*/

using namespace std;
#include "hhistogram.h"
#include "honlinecontrol.h"
#include "hlocalcont.h"
#include "hglobalcont.h"
#include "hlocalcondition.h"
#include "hparameter.h"
#include "hglobalcondition.h"
#include "hdetset.h"
#include "hdetsetcont.h"
#include "hcalclocation.h"
#include "TList.h"
#include "herror.h"
#include "hmemorycheck.h"

#include <iostream> 
#include <iomanip>

//-------------------------------------------------------------------------
//
// File name:       hhistogram.cc
// Subject:         Histogrammes.
// Author:          D.Vasiliev (INFN-LNS, Catania) "vassili@lns.infn.it"
// Date:            February 2000
//
//-------------------------------------------------------------------------
//
// Description:
//
// This file contains the source code for the member functions of 
// the class HHistogram.
// The HHistogram class provides a set of functions to maintain
// the online objects of the type "histogram".
// The HDrawingOpt class is a service class for HHistogram which allows
// to set and store drawing options for the histogrammes.
//
//-------------------------------------------------------------------------


//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
// The HHistogram class provides a set of functions to maintain
// the online objects of the type "histogram".
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// The HDrawingOpt class is a service class for HHistogram which allows
// to set and store drawing options for the histogrammes.
///////////////////////////////////////////////////////////////////////
ClassImp(HHistogram)
ClassImp(HDrawingOpt)


//---------------------------------------------------------------------
//******  HHistogram::HHistogram()
//
// Constructor of the class HHistogram.
//
//---------------------------------------------------------------------

HHistogram::HHistogram() : TNamed() {
//
// Constructor of the class HHistogram.
//

  fHist1 = 0;
  fHist2 = 0;
  fDim = 0;
  kMulti = kFALSE;
  fCType = 0;
  fCIdx = -1;
  fDSIdx1 = -1;
  fDSIdx2 = -1;
  fPar1 = 0;
  fPar2 = 0;
  fKey = 0;
  fLCondition1 = -1;
  fLCondition2 = -1;
  fLCount1 = 0;
  fLCount2 = 0;
  kConsistent = kFALSE;
  kPause = kFALSE;
  setCursors();

}

//---------------------------------------------------------------------
//******  HHistogram::~HHistogram()
//
// Destructor of the class HHistogram.
//
//---------------------------------------------------------------------

HHistogram::~HHistogram() {
//
// Destructor of the class HHistogram.
//

  if(fHist1) delete fHist1;
  if(fHist2) delete fHist2;
  if(fPar1) delete fPar1;
  if(fPar2) delete fPar2;

}


//---------------------------------------------------------------------
//******  TList* HHistogram::isSameDS(Int_t axis)
//
// This function is used only in case the histogram is bound to a global
// condition. It checks if the histogram detector set is present in one
// of the local conditions from which the global is formed. In case of
// positive result returns list of locations from the corresponding
// local condition outcome.
//
//---------------------------------------------------------------------

TList* HHistogram::isSameDS(Int_t axis) {
//
// This function is used only in case the histogram is bound to a global
// condition. It checks if the histogram detector set is present in one
// of the local conditions from which the global is formed. In case of
// the positive result returns the list of locations from the corresponding
// local condition outcome.
//

  if(!gOnline) return 0;

  HDetSet *pHSet = 0;
  HDetSet *pCSet = 0;
  HLocalCondition *pLC = 0;
  HGlobalCondition *pGC = 0;
  HNameIdx *pNameIdx = 0;
  TList* pRetList = 0;

  if(fCType==1) return 0; // local condition
  if(fCType==0) return 0; // histogram not conditioned

  pGC = gOnline->getGlobal()->at(fCIdx);
  if(!pGC) return 0;

  if(!pGC->isExpression()) return 0;

  if(axis==1) pHSet = gOnline->getDetSet()->at(fDSIdx1);
  if(axis==2) pHSet = gOnline->getDetSet()->at(fDSIdx2);

  if(!pHSet) return 0;

  TIter iter(pGC->getLCList());

  while((pNameIdx = (HNameIdx*) iter())) {

    pLC = gOnline->getLocal()->at(pNameIdx->fLCIndex);
    if(!pLC) continue;
    pCSet = gOnline->getDetSet()->at(pLC->getDetSet());
    if(!pCSet) continue;

    if(pHSet->isEqual(pCSet)) {

      pRetList = pLC->getOutput()->getLocList();
      break;
    }
  }

  return pRetList;

}


//---------------------------------------------------------------------
//******  void HHistogram::fill()
//
// This function fills the histogram.
//
//---------------------------------------------------------------------

void HHistogram::fill() {
//
// This function fills the histogram.
//

  HLocation *pLoc = 0;
  HLocation *pLoc2 = 0;
  HConditionOut *pOut = 0;
  HConditionOut *pOut1 = 0;
  HConditionOut *pOut2 = 0;
  TIterator *it = 0;
  TIterator *it2 = 0;
  TList *pList = 0;
  TList *pList2 = 0;
  HDetSet *pHSet1 = 0;
  HDetSet *pHSet2 = 0;
  HDetSet *pCSet = 0;
  Bool_t kDelList = kFALSE;

  pHSet1 = gOnline->getDetSet()->at(fDSIdx1);
  pHSet2 = gOnline->getDetSet()->at(fDSIdx2);
  if(fCType == 1) pCSet = gOnline->getDetSet()->at(gOnline->getLocal()->
						   at(fCIdx)->getDetSet());
  if(!pHSet1) return;  // det set for the x axis should be always defined

  if(fKey>0) { // local condition counter - one of parameters
    if(gOnline->getLocal()->at(fLCondition1)) {
      pOut1 = gOnline->getLocal()->at(fLCondition1)->getOutput();
      if(!pOut1) return;
    }
    if(gOnline->getLocal()->at(fLCondition2)) {
      pOut2 = gOnline->getLocal()->at(fLCondition2)->getOutput();
      if(!pOut2) return;
    }
  }

  if(fCType == 1 && !pCSet) return;

  if(fCType == 1) { // histogram bound to a local condition

    if(!pHSet1->isGeomEqual(pCSet)) return;

    pOut = gOnline->getLocal()->at(fCIdx)->getOutput();
    if(!pOut) return;

    if(fKey==10 && pHSet1->isEqual(pCSet) && fDim==1) { // lcounter on x axis 
                                                        // 1 dimension

      fHist1->Fill((Float_t)(pOut1->getCount(fLCount1)));
      return;
    }

    if(fKey==11 && pHSet1->isEqual(pCSet) && fDim==2) { // lcounter on each 
                                                        // axis; 2dim
      fHist2->Fill((Float_t)(pOut1->getCount(fLCount1)),
		   (Float_t)(pOut2->getCount(fLCount2)));
      return;
    }

    if(pHSet1->isEqual(pCSet)) { // histo and condition have indentical det set

      pList = pOut->getLocList();
    }
    else { // only geometrical part is the same

      pList = HCalcLocation::transform(pHSet1->getCategory(),
				       pOut->getIdxList());
      kDelList = kTRUE;
    }
    if(!pList) return;

    if(it) { delete it; it = 0; }
    it = pList->MakeIterator();

    while((pLoc = (HLocation*) it->Next()) != NULL) {

      if(fDim==1 && fKey==0) { // one dimensional spectrum

	fHist1->Fill(fPar1->getParameterValue(*pLoc));
      }

      if(fDim==2) { // two dimensional histogram
	if(fKey==0) 
	  fHist2->Fill(fPar1->getParameterValue(*pLoc),
		       fPar2->getParameterValue(*pLoc));
	if(fKey==10) // x axis - lcounter
	  fHist2->Fill((Float_t)(pOut1->getCount(fLCount1)),
		       fPar2->getParameterValue(*pLoc));
	if(fKey==1) // y axis - lcounter
	  fHist2->Fill(fPar1->getParameterValue(*pLoc),
		       (Float_t)(pOut2->getCount(fLCount2)));
      }
    }

  }

  if(fCType==0 || fCType == 2) { // histogram bound to a global condition 
                                 // or not conditioned at all
    if(fDim==1) { // one dimensional histo

      if(fCType==2 && fKey==10) { //lcounter on x axis; 1dim
	fHist1->Fill((Float_t)(pOut1->getCount(fLCount1)));
	return;
      }

      pList = isSameDS(1);
      if(!pList) {
	pList = pHSet1->getOutput()->getList();
      }
      if(!pList) return;

      if(it) { delete it; it=0; }
      it = pList->MakeIterator();

      while((pLoc = (HLocation*) it->Next()) != NULL) {
	fHist1->Fill(fPar1->getParameterValue(*pLoc));
      }					  
    }

    if(fDim==2) { // two dimensional histo

      if(!pHSet2) return;

      if(fKey==11 && fCType==2) { // lcounter on each axis; 2dim
	fHist2->Fill((Float_t)(pOut1->getCount(fLCount1)),
		     (Float_t)(pOut2->getCount(fLCount2)));
	return;
      }

      pList = isSameDS(1);
      if(!pList) {
	pList = pHSet1->getOutput()->getList();
      }
      if(!pList) return;

      if(fDSIdx1 == fDSIdx2) { //same det set

	if(it) { delete it; it=0; }
	it = pList->MakeIterator();

	while((pLoc = (HLocation*) it->Next()) != NULL) {

	  if(fKey==0) 
	    fHist2->Fill(fPar1->getParameterValue(*pLoc),
			 fPar2->getParameterValue(*pLoc));
	  if(fKey==10 && fCType==2) // x axis - lcounter
	    fHist2->Fill((Float_t)(pOut1->getCount(fLCount1)),
			 fPar2->getParameterValue(*pLoc));
	  if(fKey==1 && fCType==2) // y axis - lcounter
	    fHist2->Fill(fPar1->getParameterValue(*pLoc),
			 (Float_t)(pOut2->getCount(fLCount2)));
	}
      }
      else { // different det sets

	pList2 = isSameDS(2);
	if(!pList2) {
	  pList2 = pHSet2->getOutput()->getList();
	}
	if(!pList2) return;

	if(it) { delete it; it=0; }
	it = pList->MakeIterator();
	if(it2) { delete it2; it2=0; }
	it2 = pList2->MakeIterator();

	while((pLoc = (HLocation*) it->Next()) != NULL) {

	  it2->Reset();
	  while((pLoc2 = (HLocation*) it2->Next()) != NULL) {
      
	    if(fKey==0) {
	      fHist2->Fill(fPar1->getParameterValue(*pLoc),
			   fPar2->getParameterValue(*pLoc2));
	    }
	    if(fKey==10 && fCType==2) // x axis - lcounter
	      fHist2->Fill((Float_t)(pOut1->getCount(fLCount1)),
			   fPar2->getParameterValue(*pLoc));
	    if(fKey==1 && fCType==2) // y axis - lcounter
	      fHist2->Fill(fPar1->getParameterValue(*pLoc),
			   (Float_t)(pOut2->getCount(fLCount2)));
	  }
	}
      }
    }

  }

  if(it) { delete it; it = 0; }
  if(it2) { delete it2; it2 = 0; }

  if(pList && kDelList) {
    pList->Delete();
    delete pList;
    pList = 0;
  }

  return;

}



//---------------------------------------------------------------------
//******  void HHistogram::action()
//
// This function takes decision about calling fill function in the event
// loop.
//
//---------------------------------------------------------------------

void HHistogram::action() {
//
// This function takes decision about calling fill function in the event
// loop.
//

  if(kPause) return; // histogram is in the pause mode
  if(!kConsistent) return; // histogram not consistent

  //  cout <<"Inside HHistogram::action" << endl;


  HLocalCondition *pLC = 0;
  HGlobalCondition *pGC = 0;

  if(fCType==0) { // histogram not conditioned

    fill();

  }

  if(fCType==1) { // local condition

    pLC = gOnline->getLocal()->at(fCIdx);
    if(!pLC) return;

    if(pLC->isEvaluated() && pLC->getOutput()->getTrueFlag()) fill();
  }

  if(fCType==2) { // global condition

    pGC = gOnline->getGlobal()->at(fCIdx);
    if(!pGC) return;

    if(pGC->isTrue()) fill();
  }

}


//---------------------------------------------------------------------
//******  Bool_t HHistogram::checkConsistency(Bool_t kMsg = kFALSE)
//
// This function checks the histogram consistency.
//
//---------------------------------------------------------------------

Bool_t HHistogram::checkConsistency(Bool_t kMsg) {
//
// This function checks the histogram consistency.
//

  Bool_t kCounter1 = kFALSE;
  Bool_t kCounter2 = kFALSE;

  if(!gOnline) {
    if(kMsg) 
      HError::message("Object of the main control class does not exist.");
    kConsistent = kFALSE;
    return kFALSE;
  }

  if(!fHist1 && !fHist2) {
    if(kMsg) HError::message("Failed to add/modify the histogram.");
    kConsistent = kFALSE;
    return kFALSE;
  }

  HDetSet *pHSet = 0;
  HDetSet *pCSet = 0;
  HLocalCondition *pLC = 0;
  HGlobalCondition *pGC = 0;

  //
  // first make check of detector sets consistency
  //

  if(!gOnline->getDetSet()) {
    if(kMsg) HError::message("Container of detector sets not found.");
    kConsistent = kFALSE;
    return kFALSE;
  }

  Int_t idx = -1;
  Char_t nullstr[100] = "";

  if(fDim == 1) { // 1d histogram
    if(!strcmp(fDSName1,nullstr)) {
      if(kMsg) HError::message("Name of the 1st detector set is not defined.");
      kConsistent = kFALSE;
      return kFALSE;
    }
    idx = gOnline->getDetSet()->find(fDSName1);
    if(idx != -1) fDSIdx1 = idx;
    if(idx == -1 && fDSIdx1 != -1) { // changed det set name
      pCSet = gOnline->getDetSet()->at(fDSIdx1);
      if(pCSet) setDSName(pCSet->GetName(),1);
      else {
	kConsistent = kFALSE;
	return kFALSE;
      }
    }
    if(fDSIdx1 == -1) {
      if(kMsg) HError::message("1st detector set not found in the container.");
      kConsistent = kFALSE;
      return kFALSE;
    }
  }

  if(fDim == 2) { // 2d histogram
    if(!strcmp(fDSName1,nullstr)) {
      if(kMsg) HError::message("Name of the 1st detector set is not defined.");
      kConsistent = kFALSE;
      return kFALSE;
    }
    idx = gOnline->getDetSet()->find(fDSName1);
    if(idx != -1) fDSIdx1 = idx;
    if(idx == -1 && fDSIdx1 != -1) { // changed det set name
      pCSet = gOnline->getDetSet()->at(fDSIdx1);
      if(pCSet) setDSName(pCSet->GetName(),1);
      else {
	kConsistent = kFALSE;
	return kFALSE;
      }
    }
    if(fDSIdx1 == -1) {
      if(kMsg) HError::message("1st detector set not found in the container.");
      kConsistent = kFALSE;
      return kFALSE;
    }

    if(!strcmp(fDSName2,nullstr)) {
      if(kMsg) HError::message("Name of the 2nd detector set is not defined.");
      kConsistent = kFALSE;
      return kFALSE;
    }
    idx = gOnline->getDetSet()->find(fDSName2);
    if(idx != -1) fDSIdx2 = idx;
    if(idx == -1 && fDSIdx2 != -1) { // changed det set name
      pCSet = gOnline->getDetSet()->at(fDSIdx2);
      if(pCSet) setDSName(pCSet->GetName(),2);
      else {
	kConsistent = kFALSE;
	return kFALSE;
      }
    }
    if(fDSIdx2 == -1) {
      if(kMsg) HError::message("2nd detector set not found in the container");
      kConsistent = kFALSE;
      return kFALSE;
    }
  }

  //
  // make check of local condition consistency
  //

  if(fCType == 1) {

    if(!gOnline->getLocal()) {
      if(kMsg) HError::message("Container of local conditions not found.");
      kConsistent = kFALSE;
      return kFALSE;
    }

    if(!strcmp(fCName,nullstr)) {
      if(kMsg) HError::message("Name of the local condition not defined.");
      kConsistent = kFALSE;
      return kFALSE;
    }

    idx = gOnline->getLocal()->find(fCName);
    if(idx != -1) fCIdx = idx;
    if(idx == -1 && fCIdx != -1) { // changed local condition name
      pLC = gOnline->getLocal()->at(fCIdx);
      if(pLC) setCName(pLC->GetName());
      else {
	kConsistent = kFALSE;
	return kFALSE;
      }
    }
    if(fCIdx == -1) {
      if(kMsg) HError::message("Local condition not found in the container.");
      kConsistent = kFALSE;
      return kFALSE;
    }

    pLC = gOnline->getLocal()->at(fCIdx);
    if(!pLC) {
      if(kMsg) HError::message("Error when extracting local condition.");
      kConsistent = kFALSE;
      return kFALSE;
    }
    if(!pLC->checkConsistency()) {
      if(kMsg) HError::message("Local condition not consistent.");
      kConsistent = kFALSE;
      return kFALSE;
    }

    // check if det sets are the same with local condition

    if(fDim == 2) {
      if(fDSIdx1 != fDSIdx2) {
	if(kMsg) HError::message("X and Y axis detector sets are different.");
	kConsistent = kFALSE;
	return kFALSE;
      }
    }
    pHSet = gOnline->getDetSet()->at(fDSIdx1);
    pCSet = gOnline->getDetSet()->at(pLC->getDetSet());
    if(!pHSet->isEqual(pCSet) && !fKey) {
      if(!pHSet->isGeomEqual(pCSet) ||  
	 (pHSet->isGeomEqual(pCSet) && 
	  (pHSet->getCategory()==1 || pHSet->getCategory()==33))) {
	if(kMsg) 
	HError::message("Det sets of histo and local condition are different");
	kConsistent = kFALSE;
	return kFALSE;
      }
    }

  }

  //
  // make check of global condition consistency
  //

  if(fCType == 2) {

    if(!gOnline->getGlobal()) {
      if(kMsg) HError::message("Container of global conditions not found.");
      kConsistent = kFALSE;
      return kFALSE;
    }

    if(!strcmp(fCName,nullstr)) {
      if(kMsg) HError::message("Name of the global condition not defined.");
      kConsistent = kFALSE;
      return kFALSE;
    }

    idx = gOnline->getGlobal()->find(fCName);
    if(idx != -1) fCIdx = idx;
    if(idx == -1 && fCIdx != -1) { // changed global condition name
      pGC = gOnline->getGlobal()->at(fCIdx);
      if(pGC) setCName(pGC->GetName());
      else {
	kConsistent = kFALSE;
	return kFALSE;
      }
    }
    if(fCIdx == -1) {
      if(kMsg) HError::message("Global condition not found in the container.");
      kConsistent = kFALSE;
      return kFALSE;
    }

    pGC = gOnline->getGlobal()->at(fCIdx);
    if(!pGC) {
      if(kMsg) HError::message("Error when extracting global condition.");
      kConsistent = kFALSE;
      return kFALSE;
    }
    if(!pGC->checkConsistency()) {
      if(kMsg) HError::message("Global condition not consistent.");
      kConsistent = kFALSE;
      return kFALSE;
    }
  }

  //
  // check if parameters are set
  //

  if(fDim == 1 && fKey==0) { 
    if(!fPar1) {
      if(kMsg) HError::message("X axis parameter not set.");
      kConsistent = kFALSE;
      return kFALSE;
    }
  }
  if(fDim == 2 && fKey==0) { 
    if(!fPar1 || !fPar2) {
      if(kMsg) HError::message("One of parameters not set.");
      kConsistent = kFALSE;
      return kFALSE;
    }
  }
  if(fDim == 2 && fKey==10) { // lcounter - x axis
    if(!fPar2) { // must be parameter on y axis
      if(kMsg) HError::message("Y axis parameter not set.");
      kConsistent = kFALSE;
      return kFALSE;
    }
  }
  if(fDim == 2 && fKey==1) { // lcounter - y axis
    if(!fPar1) { // must be parameter on x axis
      if(kMsg) HError::message("X axis parameter not set.");
      kConsistent = kFALSE;
      return kFALSE;
    }
  }

  //
  // check if parameters and det sets belong to the same data category
  //

  HDetSet *pS1, *pS2;
  if(fDim == 1 && fKey==0) { 
    pS1 = gOnline->getDetSet()->at(fDSIdx1);
    if(pS1->getCategory() != fPar1->getCategory()) {
      if(kMsg) HError::message("X axis det set and parameter belong to different categories.");
      kConsistent = kFALSE;
      return kFALSE;
    }
  }
  if(fDim == 2) { 
    if(fKey==0 || fKey==1) { // x axis parameter must be defined
      pS1 = gOnline->getDetSet()->at(fDSIdx1);
      if(pS1->getCategory() != fPar1->getCategory()) {
	if(kMsg) HError::message("X axis det set and parameter belong to different categories.");
	kConsistent = kFALSE;
	return kFALSE;
      }
    }
    if(fKey==0 || fKey==10) { // y axis parameter must be defined
      pS2 = gOnline->getDetSet()->at(fDSIdx2);
      if(pS2->getCategory() != fPar2->getCategory()) {
	if(kMsg) HError::message("Y axis det set and parameter belong to different categories.");
	kConsistent = kFALSE;
	return kFALSE;
      }
    }
  }

  //
  // local condition counters check
  //

  if(fCType == 1) { // local condition
    if(fKey==10 || fKey==11) { // lcounter on x axis
      if(fLCondition1 == -1 || fLCondition1 != fCIdx) fLCondition1 = fCIdx;
      if(fLCondition1 == -1) {
	if(kMsg) HError::message("Local condition for the x axis counter not found.");
	kConsistent = kFALSE;
	return kFALSE;
      }
      if(fLCount1 < 1 || fLCount1 > 3) {
	if(kMsg) 
	  HError::message("Index of local condition counter is out of bounds.");
	kConsistent = kFALSE;
	return kFALSE;
      }
    }
    if(fKey==1 || fKey==11) { // lcounter on y axis
      if(fLCondition2 == -1 || fLCondition2 != fCIdx) fLCondition2 = fCIdx;
      if(fLCondition2 == -1) {
	if(kMsg) HError::message("Local condition for the y axis counter not found.");
	kConsistent = kFALSE;
	return kFALSE;
      }
      if(fLCount2 < 1 || fLCount2 > 3) {
	if(kMsg) 
	  HError::message("Index of local condition counter is out of bounds.");
	kConsistent = kFALSE;
	return kFALSE;
      }
    }
  }

  if(fCType == 2) { // global condition

    TIter iter(pGC->getLCList());
    HNameIdx *pNameIdx = 0;
 
    while((pNameIdx = (HNameIdx*) iter())) {

      if(fKey == 10 || fKey == 11) {
	if(fLCondition1 == pNameIdx->fLCIndex && fLCondition1 != -1) 
	  kCounter1 = kTRUE;
      }
      if(fKey == 1 || fKey == 11) {
	if(fLCondition2 == pNameIdx->fLCIndex && fLCondition2 != -1) 
	  kCounter2 = kTRUE;
      }
    }
    if((fKey == 10 || fKey == 11) && !kCounter1) { 
	if(kMsg) HError::message("Local condition for the x axis counter not found.");
	kConsistent = kFALSE;
	return kFALSE;
      }
    if((fKey == 1 || fKey == 11) && !kCounter2) { 
	if(kMsg) HError::message("Local condition for the y axis counter not found.");
	kConsistent = kFALSE;
	return kFALSE;
      }
      if((fKey==10 || fKey==11) && (fLCount1 < 1 || fLCount1 > 3)) {
	if(kMsg) 
	  HError::message("Index of local condition counter is out of bounds.");
	kConsistent = kFALSE;
	return kFALSE;
      }
      if((fKey==1 || fKey==11) && (fLCount2 < 1 || fLCount2 > 3)) {
	if(kMsg) 
	  HError::message("Index of local condition counter is out of bounds.");
	kConsistent = kFALSE;
	return kFALSE;
      }

  }

  //
  // check if the name of the histogram is set
  //

  if(!strcmp(nullstr,GetName())) {
    if(kMsg) HError::message("Histogram name not defined.");
    kConsistent = kFALSE;
    return kFALSE;
  }

  if(gOnline->getMemCheck()->isAlarm()) gOnline->getMemCheck()->alarm();

  kConsistent = kTRUE;
  return kTRUE;

}


//---------------------------------------------------------------------
//******  void HHistogram::setHist1(TH1F *hist)
//
// This function sets 1d histogram.
//
//---------------------------------------------------------------------

void HHistogram::setHist1(TH1F *hist) {
//
// This function sets 1d histogram.
//

  if(!hist) return;
  fHist1 = hist;

}


//---------------------------------------------------------------------
//******  void HHistogram::setHist2(TH2F *hist)
//
// This function sets 2d histogram.
//
//---------------------------------------------------------------------

void HHistogram::setHist2(TH2F *hist) {
//
// This function sets 2d histogram.
//

  if(!hist) return;
  fHist2 = hist;

}


//---------------------------------------------------------------------
//******  void HHistogram::print()
//
// This fuction prints information about the histogram.
//
//---------------------------------------------------------------------

void HHistogram::print() {
//
// This fuction prints information about the histogram.
//

  cout << "object : " << GetName() << endl;
  cout << "number of dimensions : " << fDim << endl;
  if(kMulti) { cout << "multi status : kTRUE" << endl; }
  else { cout << "multi status : kFALSE" << endl; }
  if(fCType==0) { cout << "bound to no condition" << endl; }
  if(fCType==1) { cout << "bound to local condition" << endl; }
  if(fCType==2) { cout << "bound to global condition" << endl; }
  cout << "condition index : " << fCIdx << endl;
  cout << "condition name : " << fCName << endl;
  cout << "first det set index : " << fDSIdx1 << endl;
  cout << "first det set name : " << fDSName1 << endl;
  cout << "second det set index : " << fDSIdx2 << endl;
  cout << "second det set name : " << fDSName2 << endl;
  if(fPar1) { cout << "x axis parameter : " << fPar1->GetName() << endl; }
  else { cout << "x axis parameter not defined" << endl; }
  if(fPar2) { cout << "y axis parameter : " << fPar2->GetName() << endl; }
  else { cout << "y axis parameter not defined" << endl; }
  if(kPause) { cout << "the histogram is paused" << endl; }
  else { cout << "the histogram is not paused" << endl; }
  if(kConsistent) { cout << "the histogram is consistent" << endl; }
  else { cout << "the histogram is not consistent" << endl; }
  if(fKey==10 || fKey==11) { 
    cout << "local condition on x axis: " << fLCondition1 << endl; }
  if(fKey==10 || fKey==11) { 
    cout << "local condition counter on x axis: " << fLCount1 << endl; }
  if(fKey==1 || fKey==11) { 
    cout << "local condition on y axis: " << fLCondition2 << endl; }
  if(fKey==1 || fKey==11) { 
    cout << "local condition counter on y axis: " << fLCount2 << endl; }

}


//---------------------------------------------------------------------
//******  void HHistogram::setDrawingOpt(HDrawingOpt *p, Bool_t kSave = kFALSE)
//
// This function sets drawing options for the histogram.
// If kSave = kTRUE the options are remembered.
//
//---------------------------------------------------------------------

void HHistogram::setDrawingOpt(HDrawingOpt *p, Bool_t kSave) {
//
// This function sets drawing options for the histogram.
// If kSave = kTRUE the options are remembered.
//

  if(!p) return;

  Int_t binx1, binx2;
  Int_t biny1, biny2;

  if(fDim == 1) {

    binx1 = fHist1->GetXaxis()->FindBin(p->xstart);
    binx2 = fHist1->GetXaxis()->FindBin(p->xstop);

    if(binx2 <= binx1) {

      binx1 = fHist1->GetXaxis()->FindBin(fHist1->GetXaxis()->GetXmin());
      binx2 = fHist1->GetXaxis()->FindBin(fHist1->GetXaxis()->GetXmax());

    }

    fHist1->GetXaxis()->SetRange(binx1,binx2);
    if(p->scale > 0.0) fHist1->SetMaximum(p->scale);
    else fHist1->SetMaximum();
    fHist1->SetOption((*p).option);

  }

  if(fDim == 2) {

    binx1 = fHist2->GetXaxis()->FindBin(p->xstart);
    binx2 = fHist2->GetXaxis()->FindBin(p->xstop);

    if(binx2 <= binx1) {

      binx1 = fHist2->GetXaxis()->FindBin(fHist2->GetXaxis()->GetXmin());
      binx2 = fHist2->GetXaxis()->FindBin(fHist2->GetXaxis()->GetXmax());

    }

    biny1 = fHist2->GetYaxis()->FindBin(p->ystart);
    biny2 = fHist2->GetYaxis()->FindBin(p->ystop);

    if(biny2 <= biny1) {

      biny1 = fHist2->GetYaxis()->FindBin(fHist2->GetYaxis()->GetXmin());
      binx2 = fHist2->GetYaxis()->FindBin(fHist2->GetYaxis()->GetXmax());

    }

    fHist2->GetXaxis()->SetRange(binx1,binx2);
    fHist2->GetYaxis()->SetRange(biny1,biny2);
    if(p->scale > 0.0) fHist2->SetMaximum(p->scale);
    else fHist2->SetMaximum();
    fHist2->SetOption((*p).option);

  }

  if(kSave) {

    fDrawing.xstart = p->xstart;
    fDrawing.xstop = p->xstop;
    fDrawing.ystart = p->ystart;
    fDrawing.ystop = p->ystop;
    if(p->scale > 0.0) fDrawing.scale = p->scale;
    else fDrawing.scale = -1111.;
    sprintf(fDrawing.option,"%s",(*p).option);
  }

}


//---------------------------------------------------------------------
//******  void HHistogram::setDrawingOpt(Bool_t kDefault = kTRUE)
//
// This function sets drawing options for the histogram using its own fDrawing.
// If kDefault = kTRUE the options are automatic (default ranges etc).
//
//---------------------------------------------------------------------

void HHistogram::setDrawingOpt(Bool_t kDefault) {
//
// This function sets drawing options for the histogram using its own fDrawing.
// If kDefault = kTRUE the options are automatic (default ranges etc).
//

  Int_t binx1 = 0;
  Int_t binx2 = 0;
  Int_t biny1 = 0;
  Int_t biny2 = 0;

  if(fDim == 1) {

    if(!kDefault) {

      binx1 = fHist1->GetXaxis()->FindBin(fDrawing.xstart);
      binx2 = fHist1->GetXaxis()->FindBin(fDrawing.xstop);
      
    }

    if(kDefault || (binx2 <= binx1)) {

      binx1 = fHist1->GetXaxis()->FindBin(fHist1->GetXaxis()->GetXmin());
      binx2 = fHist1->GetXaxis()->FindBin(fHist1->GetXaxis()->GetXmax());
    }

    fHist1->GetXaxis()->SetRange(binx1,binx2);

    if(kDefault) {

      fHist1->SetMaximum();
      fHist1->SetOption();
    }

    if(!kDefault) {

      if(fDrawing.scale > 0.0) fHist1->SetMaximum(fDrawing.scale);
      else {
	fHist1->SetMaximum();
	fDrawing.scale = fHist1->GetMaximum();
      }
      fHist1->SetOption(fDrawing.option);
    }
  }

  if(fDim == 2) {

    if(!kDefault) {

      binx1 = fHist2->GetXaxis()->FindBin(fDrawing.xstart);
      binx2 = fHist2->GetXaxis()->FindBin(fDrawing.xstop);
      
    }

    if(kDefault || (binx2 <= binx1)) {

      binx1 = fHist2->GetXaxis()->FindBin(fHist2->GetXaxis()->GetXmin());
      binx2 = fHist2->GetXaxis()->FindBin(fHist2->GetXaxis()->GetXmax());
    }

    fHist2->GetXaxis()->SetRange(binx1,binx2);

    if(!kDefault) {

      biny1 = fHist2->GetYaxis()->FindBin(fDrawing.ystart);
      biny2 = fHist2->GetYaxis()->FindBin(fDrawing.ystop);
      
    }

    if(kDefault || (biny2 <= biny1)) {

      biny1 = fHist2->GetYaxis()->FindBin(fHist2->GetYaxis()->GetXmin());
      biny2 = fHist2->GetYaxis()->FindBin(fHist2->GetYaxis()->GetXmax());
    }

    fHist2->GetYaxis()->SetRange(biny1,biny2);

    if(kDefault) {

      fHist2->SetMaximum();
      fHist2->SetOption();
    }

    if(!kDefault) {

      if(fDrawing.scale > 0.0) fHist2->SetMaximum(fDrawing.scale);
      else {
	fHist2->SetMaximum();
	fDrawing.scale = fHist2->GetMaximum();
      }
      fHist2->SetOption(fDrawing.option);
    }
  }

}







