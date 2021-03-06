// $Id: hrichobjfilter.cc,v 1.6 2009-07-15 11:39:17 halo Exp $
// Last update by Thomas Eberl: 03/07/15 18:24:01
//
using namespace std;
#include "hrichobjfilter.h"
#include "hlinearcategory.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hrichdetector.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hades.h"
#include "richdef.h"
#include "hrichcorrelatorpar.h"
#include "hruntimedb.h"
#include "hrichhitfilter.h"
#include "hhitmatchfilter.h"
#include "hdihitmatchfilter.h"
#include <iostream> 
#include <iomanip>
ClassImp(HRichObjFilter)

HRichObjFilter::HRichObjFilter(const Text_t *name,const Text_t *title,const Char_t *swt) :
  HReconstructor(name,title)
{
    // set a string that switches on the requested filter 
    pFilterSwitch = new TString(swt);
}

HRichObjFilter::HRichObjFilter()
{

}

HRichObjFilter::~HRichObjFilter(void) 
{
 
}

Bool_t HRichObjFilter::init() 
{
    if (gHades) 
    {
	HEvent *event=gHades->getCurrentEvent();
	HRuntimeDb *rtdb=gHades->getRuntimeDb();
	HSpectrometer *spec=gHades->getSetup();
	if (event && rtdb) {
	    // filters need cut params
	    HRichCorrelatorPar *pCorrPar =
		(HRichCorrelatorPar*)rtdb->getContainer("RichCorrelatorParameters");
	    if (!pCorrPar) 
	    {
		Error("init","no correlation param container available from RTDB");
		return kFALSE;
	    }
	    else setCorrelationPar(pCorrPar);
	    // data cat input
	    HDetector *rich = spec->getDetector("Rich");
	    if (rich) 
	    {
		if(pFilterSwitch->Contains("RICH_HIT"))
		{
		    pRichHitCat=gHades->getCurrentEvent()->getCategory(catRichHit);
		    if (!pRichHitCat) 
		    {
			Error("init()","no Rich hit cat found");
			return kFALSE;
		    }
		    pRichHitIter = (HIterator*) pRichHitCat->MakeIterator();
		}

		if(pFilterSwitch->Contains("MATCH_HIT"))
		{
		    pHitMatchCat=gHades->getCurrentEvent()->getCategory(catMatchHit);
		    if (!pHitMatchCat) 
		    {
			Error("init()","no Match hit cat found");
			return kFALSE;
		    }
		    pHitMatchIter = (HIterator*) pHitMatchCat->MakeIterator();
		}
		if(pFilterSwitch->Contains("DIMATCH_HIT"))
		{
		    pDiHitMatchCat=gHades->getCurrentEvent()->getCategory(catDiMatchHit);
		    if (!pDiHitMatchCat) 
		    {
			Error("init()","no Match hit cat found");
			return kFALSE;
		    }
		    pDiHitMatchIter = (HIterator*) pDiHitMatchCat->MakeIterator();
		}
	    }	
	}
    }
    
    return kTRUE;
   
}

Bool_t HRichObjFilter::finalize() 
{
    return kTRUE;
}

Int_t HRichObjFilter::execute()
{
    if(pFilterSwitch->Contains("DIMATCH_HIT"))
    {
	HDiHitMatchFilter pairfilter;
	pairfilter.initCuts(getCorrelationPar());
	((HLinearCategory*)pDiHitMatchCat)->filter(pairfilter);
    }
    
    
    if(pFilterSwitch->Contains("MATCH_HIT"))
    {
	HHitMatchFilter hitmatchfilter;
	hitmatchfilter.initCuts(getCorrelationPar());
	((HLinearCategory*)getHitMatchCat())->filter(hitmatchfilter);
    }

    if(pFilterSwitch->Contains("RICH_HIT"))
    {
	HRichHitFilter richhitfilter;
	richhitfilter.initCuts(getCorrelationPar());
	((HLinearCategory*)getRichHitCat())->filter(richhitfilter);
    }
    

//     HRichHit *pRichHit=0;
//     pRichHitIter->Reset();
//     while((pRichHit = (HRichHit *)fRichIter->Next()))
//     {
//     }	

    //nCntProcessedEvents++;




    Int_t kReturnValue=0;//proceed normally
    //    if (isEmptyEvt()) kReturnValue = kSkipEvent;
    return kReturnValue;
}

