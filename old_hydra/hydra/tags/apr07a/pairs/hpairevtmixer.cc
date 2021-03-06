// $Id: hpairevtmixer.cc,v 1.11 2006-11-24 16:14:03 eberl Exp $
// Author: Thomas.Eberl@ph.tum.de, last modified : 2006-11-13 17:14:32
//
// ---------------------------------------------------------------------------
//_HADES_CLASS_DESCRIPTION  
///////////////////////////////////////////////////////////////////////////////
//                                                                           
// HPairEvtMixer 
//                                                                           
///////////////////////////////////////////////////////////////////////////////
//
using namespace std;
// ---------------------------------------------------------------------------
#include <iomanip>
#include <iostream> 
// ---------------------------------------------------------------------------
#include <TObjArray.h>
#include <TRandom.h>
// ---------------------------------------------------------------------------
#include <hades.h>
#include <hcategory.h>
#include <hevent.h>
#include <hiterator.h>
#include <hlinearcategory.h>
#include <hpidcandidate.h>
#include <hpidparticle.h>
#include <hruntimedb.h>
// ---------------------------------------------------------------------------
#include "hpaircutpar.h"
#include "hpairdata.h"
#include "hpairext.h"
#include "hpairsimext.h"
#include "hpairfl.h"
#include "hpairqa.h"
// ---------------------------------------------------------------------------
#include "hpairevtmixer.h"
// ---------------------------------------------------------------------------
ClassImp(HPairEvtMixer)
// ---------------------------------------------------------------------------
HPairEvtMixer::HPairEvtMixer(Text_t *name,Text_t *title,
			     Int_t nBufferSize, 
			     Int_t nBufferMethod,
			     Int_t nPairAna,
			     Int_t nShrinkPoolByThisNumberOfEvents,
			     Int_t rndnr,
			     Int_t nMinNbLepsPerEvt,
			     Int_t nUnlikeSignOnly) :
    HReconstructor(name,title)
{
    // set ctor arguments to class data

    // size of event pool
    evtrange = nBufferSize; 
    cout<<"Pool size: "<<evtrange<<endl;

    // number of particles per evt randomly selected for mixing
    // if rndnr==0 (default), then all particles are used
    nrMixObjs = rndnr; 
    if (nrMixObjs) cout<<"Random mode: max "<<rndnr
		       <<" tracks per event randomly selected to build pool"
		       <<endl;
    else cout<<"NO RANDOMization: \
                All valid tracks per event will be used for mixing"<<endl;

    // method how to fill event pool
    // if nBufferMethod==0, then the next evtrange events are used as pool
    // if nBufferMethod==1, then the first n events are deleted from the pool
    // and the next n events are appended to the pool
    nBufferFillingMethod = nBufferMethod;
    if (nBufferFillingMethod==0) cout<<"DISCRETE event pools!"<<endl;
    if (nBufferFillingMethod==1) cout<<"RING event pools!"<<endl;

    
    // for ring buffer method only
    // number of events substituted when shifting ring buffer
    nShrinkPool = nShrinkPoolByThisNumberOfEvents;
    if (nBufferFillingMethod==1) cout <<"SHIFT ring buffer by "<<nShrinkPool
				      <<" events"<<endl;

    nPairAnalysis = nPairAna;
    if (nPairAnalysis==0) cout<<" NO PAIR ANALYSIS " <<endl;
    if (nPairAnalysis==1) cout<<" PAIR ANALYSIS SWITCHED ON " <<endl;

    // minimum number of selected particles (leptons) in one event
    // necessary to allow event to be stored
    nMinParticlesPerEvent = nMinNbLepsPerEvt;

    // initialize pointer for qa object, created in init()

    pQA=0;
    setQAFileName("");
    kOnlyUnlikeSign = (Bool_t) nUnlikeSignOnly;

}
// ---------------------------------------------------------------------------
HPairEvtMixer::HPairEvtMixer()
{

}
// ---------------------------------------------------------------------------
HPairEvtMixer::~HPairEvtMixer(void) {

}
// ---------------------------------------------------------------------------
Bool_t HPairEvtMixer::init() 
{
        if (gHades) 
	{
	    kIsReal=gHades->isReal();
	    if (kIsReal) cout<<"REAL event mode selected!"<<endl;
	    else cout<<"SIMULATION event mode selected!"<<endl;

	    HEvent *event=gHades->getCurrentEvent();
	    HRuntimeDb *rtdb=gHades->getRuntimeDb();
	    if (rtdb)
	    {
		pCuts = (HPairCutPar*)rtdb->getContainer("PairCutParameters");
		if (!pCuts) Error("HPairFilter::init",
				  "PairCutParameters initialization \
                                   via RTDB returned a NULL pointer");
	    }
	    else
	    {
		Error("init","pointer to RTDB is NULL");
		return kFALSE;
	    }
	    if (event) 
	    {
		// INPUT (no tree output is foreseen)
		// particles
		pCatPidParticle=event->getCategory(catPidPart);
		if (!pCatPidParticle) 
		{
		    Error("init","No PIDPARTICLE category defined");
		    return kFALSE;
		}
		else pIterPidParticle = (HIterator*)getPidParticleCat()
			 ->MakeIterator("native");

		// pairs
		pCatPair=event->getCategory(catPairs);
		if (!pCatPair) 
		{
		    Warning("init","No PAIRS category defined, \
                                    event mixing done without pair analysis");
		    pIterPair=0;
		    nPairAnalysis=0;
		}
		else pIterPair = (HIterator*)getPairCat()
			 ->MakeIterator("native");

	    } // endif event
	    else
	    {
		Error("init","pointer to event is NULL");
		return kFALSE;
	    }
	}

	///////////////////////////////////////    

	// initialize event pool
	evtset = new TObjArray(evtrange);
	// no events stored by now
	evtsStoredInRange = 0;
	// counter for number of created event pairs
	nCntMixedEvts = 0; 
	// total nb of evts seen by this task
	nCounterProcessedNbEvents=0; 

	// initialize QA object, responsible for control ntuple
	// name for output root file set via cQAFileName in calling macro
	if ( cQAFileName.Length()!=0 )
	{
	    pQA = new HPairQA("Mixer","Mixer");
	    if (!pQA)
	    {
		Error("init","pointer to HPairQA is NULL");
		return kFALSE;
	    }
	    pQA->setOutputFileName(getQAFileName());
	    // create ntuple with pair info
	    pQA->setNTupleFillingActive(1);
	    // write out cut decisions to ntuple
	    pQA->setCutQAFillingActive(1);
	}

	kFirstMixing = kTRUE;

	nStoredEvts = 0;
	return kTRUE;
}
// ---------------------------------------------------------------------------
Bool_t HPairEvtMixer::finalize() 
{
    if (pQA) 
    {
	// write ntuple etc. to file
	pQA->finalize();
	delete pQA;
    }
    
    cout<<"************** EVT Mixer TASK *****************************"<<endl;
    cout<<" !!!! "<<nCntMixedEvts<<" event pairs used for mixing "<<endl;
    cout<<"***********************************************************"<<endl;
    return kTRUE;
}
// ---------------------------------------------------------------------------
Int_t HPairEvtMixer::execute()
{

#if 0
    //---------------------------------------------------------------
        cout << "START    O F   E V E N T " 
             <<nCounterProcessedNbEvents<<endl;
    //---------------------------------------------------------------
#endif    

    // if there are no particles after PID analysis, skip the event
    if (getPidParticleCat()->getEntries()<2) return kSkipEvent;

    // allow subsequent execution of tasks, as no category is filled
    Int_t kReturnValue=0;

    // number of stored particles in this event (debug value)
    Int_t nStoredParticles = 0;


    //---------------------------------------------------------------

    // number of events currently in event pool
    evtsStoredInRange = evtset->GetEntries();

    // if pool is full, start mixing
    if (evtsStoredInRange == evtrange) 
    {
	//-------------------------------------------------------
	// CALL MIXING
	if ( makeMixedPairs() ) 
	{
	    Error("execute","error in makeMixedPairs()");
	    return -1;
	}
	//-------------------------------------------------------
	

	// DISCRETE BUFFER
	// delete all particles in all events in the event pool
	// and the event pool itself, reset buffer counter
	if (nBufferFillingMethod == 0) deleteStoredEvents();

	// RING BUFFER
	// shrink event pool by deleting first event(s)
	// number is in nShrinkPool data member, set via ctor, default is 1
	if (nBufferFillingMethod == 1) shrinkEventBuffer();
    }
    

    // FILL EVENT POOL
    // store current event in event pool if valid (c.f. function)
    nStoredParticles = storeEvent();

#if 0
    //---------------------------------------------------------------
        cout << "E N D    O F   E V E N T " 
             <<nCounterProcessedNbEvents<<"   # of stored tracks: "
	     <<nStoredParticles<<"   # of stored events: "
	     <<evtsStoredInRange<<endl;

        nCounterProcessedNbEvents++;
    //---------------------------------------------------------------
#endif    
    return kReturnValue;
}
// ---------------------------------------------------------------------------
Int_t HPairEvtMixer::storeEvent()
{
    Int_t nStoredParticles = 0;

    if (nPairAnalysis==0 || pIterPair==0) 
	nStoredParticles = storeEventWithoutPairAnalysis();
    else nStoredParticles = storeEventWithPairAnalysis();

    return nStoredParticles;
}
// ---------------------------------------------------------------------------
Int_t HPairEvtMixer::storeEventWithoutPairAnalysis()
{
    // store particles for event mixing without pair analysis

    Int_t nParticlesInEvent = getPidParticleCat()->getEntries();

    // if there is no particle, skip the rest
    if (nParticlesInEvent < 1) return 0;

    // counter for stored particles
    Int_t nStoredParticles = 0;

    
    // create array to hold particles from current event
    TObjArray *arr = new TObjArray(nParticlesInEvent);

    // LOOP OVER PARTICLES -------------------------------

    HPidParticle *h=0;
    pIterPidParticle->Reset();

	while(( h= (HPidParticle *)pIterPidParticle->Next())) 
	{

	    if (!checkParticle(h)) continue;

	    // STORE PARTICLE
	    if (kIsReal)
	    {
		// make a copy of the particle
		HPidParticle* pp = new HPidParticle();
		*pp = *h;
		arr->Add(pp);
	    }
	    else
	    {
		HPidParticleSim* pp = new HPidParticleSim();
		*pp = *((HPidParticleSim*)h);
		arr->Add(pp);
	    }
	    nStoredParticles++;
	}
    // -------------------------------------------------

    // Only if there is more than nMinParticlesPerEvent particle, 
    // store the event in the pool
    if (nStoredParticles>nMinParticlesPerEvent)
    {
	// use number as name for event array
	// NB: there will be event arrays with the same name in the ring buffer
	// method: do not use the name for anything else but print-outs !
	TString nn = "evt_"; 
	nn+=nStoredEvts; 
	arr->SetName(nn.Data());

	// append new event array to event pool
	evtset->Add(arr);
	// increment counter of stored events
	nStoredEvts++;
	// increment event pool counter
	evtsStoredInRange++;//nb of current stored evt in the pool 
    }
    else
    {
	arr->Delete();
	delete arr;
    }

    return nStoredParticles;
}
// ---------------------------------------------------------------------------
Int_t HPairEvtMixer::storeEventWithPairAnalysis()
{
    // store particles for mixing with the help of a pair property analysis

    // if there is no pair skip the rest
    if (getPairCat()->getEntries()<1) return 0;

    // counter for stored particles
    Int_t nStoredParticles = 0;

    // max number of different particles in event deduced from number 
    // of created pairs
    Int_t nMaxNbParticles = 2*( ( (HLinearCategory*) getPidParticleCat() )
				->getEntries() );
    
    // create array to hold particles from current event
    TObjArray *arr = new TObjArray(nMaxNbParticles);


    // 2 different pairs can have max one particle in common
    // take care not to use the same particle twice
    
    // array to hold indices of different particles in the event
    Int_t* nParticleIndex = new Int_t[nMaxNbParticles];
    // default initialize particle index array, 
    // neccessary for helper function isNewIndex
    for (Int_t j=0;j<nMaxNbParticles;j++) nParticleIndex[j]=-2;
    
    
    // LOOP OVER PAIRS --------------------------------------
    HPair* p = 0;
    HPidParticle* pPart1 = 0;
    HPidParticle* pPart2 = 0;
    
    // loop over pairs and select only particles 
    // from pairs that have survived the pair analysis

    pIterPair->Reset();
    while ( (p = (HPair*) pIterPair->Next()) ) 
    {
	// -------------------------------------------
	// pair has been cut, do not use the particles
	if (p -> getIsCutFlag() != 0) continue;
	// if pair is not unlike (opposite) sign,
	// do not use the legs for mixing.
	if ( kOnlyUnlikeSign && p->getCharge()!= 0 ) continue; 
	// -------------------------------------------
	
	
	// check if particle has already been stored
	if ( HPairFL::isNewIndex(p->getIndexParticle1(),
				 nParticleIndex,nMaxNbParticles) )
	{
	    // not yet stored particle
	    // retrieve particle, create extended particle object and
	    // store it in the particle pool "arr" of this evt for
	    // later use in the mixing procedure
	    pPart1 = 
		(HPidParticle*)pCatPidParticle
		->getObject(p->getIndexParticle1());
	    
	    if (pPart1==0) Error("storeEventWithPairAnalysis",
				 "pointer to first particle is 0");
	    if (checkParticle(pPart1))
	    {
	      if(kIsReal)
		{
		  // make a copy of the particle
		  HPidParticle* pp = new HPidParticle();
		  *pp = *pPart1;
		  arr->Add(pp);
		}
	      else
		{
		  HPidParticleSim* pp = new HPidParticleSim();
		  *pp = *((HPidParticleSim*)pPart1);
		  arr->Add(pp);
		}
	      nStoredParticles++;
	    }
	}
	
	// same procedure for second particle of this pair
	if ( HPairFL::isNewIndex(p->getIndexParticle2(),
				 nParticleIndex,nMaxNbParticles) )
	{
	    pPart2 = 
		(HPidParticle*)pCatPidParticle
		->getObject(p->getIndexParticle2());
	    
	    if (pPart2==0) Error("storeEventWithPairAnalysis",
				 "pointer to second particle is 0");
	    if (checkParticle(pPart2))
	      {
	      if(kIsReal)
		{
		  // make a copy of the particle
		  HPidParticle* pp = new HPidParticle();
		  *pp = *pPart2;
		  arr->Add(pp);
		}
	      else
		{
		  HPidParticleSim* pp = new HPidParticleSim();
		  *pp = *((HPidParticleSim*)pPart2);
		  arr->Add(pp);

		}
	      
	      nStoredParticles++;
	    }
	}

	
    } // end of loop over pairs
    // --------------------------------------------------------


    // delete helper array
    delete [] nParticleIndex;
    
    // Only if there are at least 2 particles, store the event in the pool
    // this condition is necessary to be compatible with signal analysis
    // Remember: the signal os spectrum is composed of l+l pairs which
    // have survived all cuts! Use these same tracks for mixing! 

    if (nStoredParticles>1)
    {
	// use number as name for event array
	// NB: there will be event arrays with the same name in the ring buffer
	// method: do not use the name for anything else but print-outs !
	TString nn = "evt_"; 
	nn+=nStoredEvts; 
	arr->SetName(nn.Data());

	// append new event array to event pool
	evtset->Add(arr);
	// increment counter of stored events
	nStoredEvts++;
	// increment event pool counter
	evtsStoredInRange++;//nb of current stored evt in the pool 
    }
    else
    {
	arr->Delete();
	delete arr;
    }


    return nStoredParticles;
}
// ---------------------------------------------------------------------------
void HPairEvtMixer::deleteStoredEvents()
{
    // loop over event pool "evtset"
    for (Int_t evtcounter=0;evtcounter<evtset->GetLast()+1;evtcounter++)
    {
	// retrieve event from pool
	TObjArray* evtc = ((TObjArray*)(*evtset)[evtcounter]);


	// Remove all objects from the array AND delete all heap based objects
	evtc->Delete();
	// delete event itself
	delete evtc;
    }
    // delete event pool itself
    delete evtset;
    // create new event pool with predefined size (autoexpanding!)
    evtset = new TObjArray(evtrange);
    evtsStoredInRange=0; // reset event counter
}




// ---------------------------------------------------------------------------
TObjArray* HPairEvtMixer::doEventMixingAllTracks()
{
  // create event pairs from the event pool and
    // create particle pairs (HPairExt) from all particles in both events
    // store these pairs in a TObjArray and return it


    // check size of event pool

#if 0
    cout << " MIXING " << endl;
    for (Int_t ii=0;ii<=evtset->GetLast();ii++)
    {
	TObjArray* evt0 = ((TObjArray*)(*evtset)[ii]);
	cout << evt0->GetName() << " " ;
    }
    cout << endl;
#endif

    if (evtrange != evtset->GetLast()+1 ) 
    {
	::Error("HPairEvtMixer::doEventMixingAllTracks()",
		"event buffer has different size than requested");
	return 0;
    }

    // create array to hold mixed pairs
    // Note: autoexpanding!
    TObjArray* pMixedPairs = new TObjArray(evtrange*5);

    // -----------------------------------------------------------------------
    // MAKE EVENT PAIRS

    // -----------------------------------------------------------------------

    /*
      event array: evtset, e.g. 5 stored events (nBufferSize: 5)

      block buffer method:
      ====================
      all possible different combinations, i.e.

       (i,j) ------> j is inner index
   |   (1,2) (1,3) (1,4) (1,5)
   |         (2,3) (2,4) (2,5)
   |	           (3,4) (3,5)
   \/ 	                 (4,5)
    i is outer index

    ring buffer method:
    ===================
    _first_ nShrinkPool events from linear pool "evtset" are deleted
    then nShrinkPool new events are _appended_
    make in next step only _new_ event pairs, i.e.
    nBufferSize-nShrinkPool "old" ones should be combined with
    nShrinkPool new ones and the new ones among themselves 

    E.g. nShrinkPool = 2, nBufferSize = 5
    ==> evt # 1,2,3 are old
    ==> evt # 4,5 are new in the buffer(=pool)
    ==> combinations 
        (1,4) (1,5)        old - new
	(2,4) (2,5)        old - new
	(3,4) (3,5)        old - new
	      (4,5)        new - new

       Note that  (4,4)  e.g. must not occur!
    */



    // outer event loop: 
    // =================
    // start with first index (=0)
    Int_t nOuterStartIdx = 0;
    // go until the second last index
    Int_t nOuterEndIdx = evtset->GetLast()-1;

    for (Int_t evtcountero=nOuterStartIdx;
	 evtcountero<=nOuterEndIdx;
	 evtcountero++)
    {
	// get event array from event pool
	TObjArray* evt0 = ((TObjArray*)(*evtset)[evtcountero]);
	if (!evt0) 
	{
	    Error("doEventMixingAllTracks","no first event"); 
	    return 0;
	}
	
	// number of particles stored in this event
	Int_t evt0max = evt0->GetLast()+1;
	

	// --------------------------------------------------------------------
	// inner event loop
	// ================
	// start index 
	Int_t nInnerStartIdx = 0; // default

	// in case of ring buffer method, the second event of the event pair
	// is always a recently appended event
	// therefore start with event pool index: 
	// nInnerStartIdx = evtset->GetLast() - nShrinkPool + 1
	
	if (nBufferFillingMethod==1) // ring buffer
	{
	    nInnerStartIdx = evtset->GetLast() - nShrinkPool + 1;
	}
	
	// in case of block buffer method, the second event in the event pair
	// always has an index larger by 1 than the first one.
	// therefore start with event pool index:
	// nInnerStartIdx = evtcountero+1
	// nInnerEndIdx = evtset->GetLast(), i.e. last index in array
	if (nBufferFillingMethod==0 || kFirstMixing) // block buffer
	{
	    nInnerStartIdx = evtcountero + 1;
	}

	// last index in both cases
 	Int_t nInnerEndIdx = evtset->GetLast();

	// ----------------------------------   loop
	for (Int_t evtcounter=nInnerStartIdx;
	     evtcounter<=nInnerEndIdx;
	     evtcounter++)
	{


	    // do not pair event with itself; necessary for ring buffer case
	    if (evtcounter==evtcountero) continue;
	    // -------------------------------------------------------------

#if 0
	    cout << "EVT PAIR: " << evtcountero << " " << evtcounter << endl;
#endif
	    nCntMixedEvts++; // count number of mixed events (event pairs)

	    // get next evt from evt set
	    TObjArray* evtc = ((TObjArray*)(*evtset)[evtcounter]);
	    if (!evtc) 
	    {
		Error("doEventMixingAllTracks","no next event"); 
		return 0;
	    }

	    Int_t evtcmax = evtc->GetLast()+1; // nr of tracks in current evt

	    // --------------------------------------------------------------
	    // MAKE MIXED TRACK PAIRS
	    // --------------------------------------------------------------

	    // loop over particles in outer event
	    for (Int_t trkcntevt0=0;trkcntevt0<evt0max;trkcntevt0++)
	    {
		// loop over particles in inner event
		for (Int_t trkcntevtc=0;trkcntevtc<evtcmax;trkcntevtc++)
		{
		    // create pair with each track from first evt
		    if (kIsReal)
		    {
			HPidParticle* pPart1 = 
			    (HPidParticle*)(*evt0)[trkcntevt0];

			HPidParticle* pPart2 = 
			    (HPidParticle*)(*evtc)[trkcntevtc];

			HPairExt* pPairE = new HPairExt(pPart1,pPart2);
			if (checkMixedPair(pPairE)) pMixedPairs->Add(pPairE);
			else 
			{
			    delete pPairE ;
			}

		    }
		    else
		    {
			HPidParticleSim* pPart1 = 
			    (HPidParticleSim*)(*evt0)[trkcntevt0];

			HPidParticleSim* pPart2 = 
			    (HPidParticleSim*)(*evtc)[trkcntevtc];

			HPairSimExt* pPairE = new HPairSimExt(pPart1,pPart2);
			if (checkMixedPair(pPairE)) pMixedPairs->Add(pPairE);
			else 
			{
			    delete pPairE ;
			}
		    }

		} // end of inner particle loop
	    } // end of outer particle loop 

	} // end of inner event loop
    } // end of outer event loop

    if (kFirstMixing) kFirstMixing = kFALSE;
    return pMixedPairs;
}

#if 0
// NEEDS REVISION
// ---------------------------------------------------------------------------
TObjArray* HPairEvtMixer::doEventMixingTrackSubset()
{
    // create event pairs from the event pool and
    // create particle pairs from a random number of 
    // particles in both events
    // store these pairs in category HPairExt

    Int_t maxNbEvts = evtset->GetLast()+1;

    // create array to hold mixed pairs
    TObjArray* pMixedPairs = new TObjArray(maxNbEvts*5);


    // -----------------------------------------------------------------------
    // MAKE EVENT PAIRS

    // -----------------------------------------------------------------------
    


    // -----------------------------------------------------------
    // for the RING BUFFER method: make event pairs 
    // only with new events in the buffer!
    Int_t nOuterEventLoopLimit=maxNbEvts-1;
    if (nBufferFillingMethod==1) nOuterEventLoopLimit=nShrinkPool; 
    // -----------------------------------------------------------



    // outer event loop: m ---> n-1 
    for (Int_t evtcountero=0;evtcountero<nOuterEventLoopLimit;evtcountero++)
    {
	// get event array from event pool
	TObjArray* evt0 = ((TObjArray*)(*evtset)[evtcountero]);
	if (!evt0) 
	{
	    Error("doEventMixingAllTracks","no first event"); 
	    return 0;
	}

	// number of particles stored in this event
	Int_t evt0max = evt0->GetLast()+1;
	
	// --------------------------------------------------------------------
	// inner event loop: m+1 ---> n : "the rest"
	for (Int_t evtcounter=evtcountero+1;evtcounter<maxNbEvts;evtcounter++)
	{
	    nCntMixedEvts++; // count number of mixed events (event pairs)

	    // get next evt from evt set
	    TObjArray* evtc = ((TObjArray*)(*evtset)[evtcounter]);
	    if (!evtc) 
	    {
		Error("doEventMixingAllTracks","no next event"); 
		return 0;
	    }

	    Int_t evtcmax = evtc->GetLast()+1; // nr of tracks in current evt
	    
	    // select "nrMixObjs" particle indices from outer event randomly
	    Int_t *nIndO = new Int_t[nrMixObjs];
	    for (Int_t ii=0;ii<nrMixObjs;ii++)
	    {
		nIndO[ii] = gRandom->Integer(evt0max);
	    }

	    // -----------------------------------------------------------------
	    // loop over particles in outer event
	    for (Int_t trkcntevt0=0;trkcntevt0<evt0max;trkcntevt0++)
	    {
		// check if current outer event track index was drawn before
		Bool_t kIsFoundOuter=kFALSE;
		for (Int_t ii=0;ii<nrMixObjs;ii++)
		{
		    if (nIndO[ii] == trkcntevt0) kIsFoundOuter=kTRUE;
		}
		// if not found, try next one 
		if (!kIsFoundOuter) continue;

		// select "nrMixObjs" particle indices 
		// from inner event randomly
		Int_t *nIndI = new Int_t[nrMixObjs];
		for (Int_t iii=0;iii<nrMixObjs;iii++)
		{
		    nIndI[iii] = gRandom->Integer(evtcmax);
		}

		// -----------------------------------------------------------
		// loop over particles in inner event
		for (Int_t trkcntevtc=0;trkcntevtc<evtcmax;trkcntevtc++)
		{
		   // check if current inner event track index was drawn before
		    Bool_t kIsFoundInner=kFALSE;
		    for (Int_t ii=0;ii<nrMixObjs;ii++)
		    {
			if (nIndI[ii] == trkcntevtc) kIsFoundInner=kTRUE;
		    }
		    // if not found, try next one 
		    if (!kIsFoundInner) continue;


		    // create pair with each track from first evt
		    
		    if (kIsReal)
		    {

			HPidParticle* pPart1 = 
			    (HPidParticle*)(*evt0)[trkcntevt0];
			HPidParticle* pPart2 = 
			    (HPidParticle*)(*evtc)[trkcntevtc];

			HPairExt* pPairE = new HPairExt(pPart1,pPart2);
			
			if (checkMixedPair(pPairE)) pMixedPairs->Add(pPairE);
			else 
			{
                            delete pPairE ;
			}
		    }
		    else
		    {

			HPidParticleSim* pPart1 = 
			    (HPidParticleSim*)(*evt0)[trkcntevt0];
			HPidParticleSim* pPart2 = 
			    (HPidParticleSim*)(*evtc)[trkcntevtc];

			HPairSimExt* pPairE = new HPairSimExt(pPart1,pPart2);
			
			if (checkMixedPair(pPairE)) pMixedPairs->Add(pPairE);
			else 
			{
			    delete pPairE ;
			}
		    }

		} // end of inner particle loop
		delete [] nIndI;
	    } // end of outer particle loop 
	    delete [] nIndO;

	} // end of inner event loop
    } // end of outer event loop

    return pMixedPairs;
}
#endif
// ---------------------------------------------------------------------------
void HPairEvtMixer::shrinkEventBuffer()
{

    /*
  TObjArray* evtsetnew = new TObjArray(evtrange);
  // copy remaining event pointer in new array
  for (Int_t i=0;i<evtset->GetLast()+1;i++)
    {
      if (i<nShrinkPool)
	{
	  TObjArray* e = ((TObjArray*)(*evtset)[i]);
	  e->Delete();
	  delete e;
	}
      else
	{
	  evtsetnew->Add(((TObjArray*)(*evtset)[i]));
	}
    }
  delete evtset;
  evtset=evtsetnew;
    */

    // delete first nShrinkPool events from event pool "evtset"
    for (Int_t i=0;i<nShrinkPool;i++)
    {
	TObjArray* evtc = ((TObjArray*) evtset->RemoveAt(i));

	evtc->Delete();
	delete evtc;

    }
    evtset->Compress();
    
    
    // subtract the number of deleted events from the current event pool cnter
    evtsStoredInRange-=nShrinkPool;
}
// ---------------------------------------------------------------------------
Bool_t HPairEvtMixer::checkParticle(HPidParticle* p)
{
    // This function checks a particle and decides if it is stored
    // for mixing or not.

    // FIXME: Do this via a cut object like for pairs!

    // Do these checks also in the case of the pair property analysis
    // After pair analysis (on std sample of type B, c.f. HPairFiller)
    // we have uncut pairs of type l+l and l+h
    // l+h is cut away anyway during histogramming
    // Therefore do not store h tracks for mixing in the first place

    Bool_t kReturnValue = kTRUE;

    // SELECT PARTICLE: e+ and e-
    // use only identified leptons for mixing
    if (p->getPid()!=2 && p->getPid()!=3) kReturnValue = kFALSE;
    
    // DEFINITION!
    //Check ring, leptons must have rings!
    if (! (p->getHitData()->getRingCorrelation(p->getMomAlg())) )
	kReturnValue = kFALSE;
    
    
    return kReturnValue;
}
// ---------------------------------------------------------------------------
Bool_t HPairEvtMixer::checkMixedPair(HPairSimExt* p)
{
    Bool_t kReturnValue = checkMixedPair((HPairExt*) p);
    /*
      // simulation cut on pair goes here
	if ( p-> getSomethingUseful > ? ) 
    */
    return kReturnValue;
}
// ---------------------------------------------------------------------------
Bool_t HPairEvtMixer::checkMixedPair(HPairExt* p)
{
    // This function checks a pair and decides if it is stored
    // FIXME: Do this via the cut object used in std pairs analysis !

    Bool_t kReturnValue = kTRUE;


    // SELECT PROPERTIES OF MIXED PAIRS
    if ( ! pCuts->checkOpeningAngle(p) ) kReturnValue = kFALSE;  
    
    return kReturnValue;
}
// ---------------------------------------------------------------------------
Int_t HPairEvtMixer::makeMixedPairs()
{
    // call the event mixing routine and fill the pair ntuple

    TObjArray *pMixedPairs = 0;
    
    // There are 2 modes for event mixing:
    // 1)
    // use random number of particles from each event
    // draw only from those that were selected for the mixing

    if (nrMixObjs>0)
    {
	::Error("HPairEvtMixer::makeMixedPairs",
		"random mode not supported in this version");
	return -1;
#if 0
	pMixedPairs = doEventMixingTrackSubset();

#endif
    }
    // 2) 
    // use all particles that were selected for the mixing
    else pMixedPairs = doEventMixingAllTracks();
    
    // NTUPLE FILLING
    // fill control data structures from HPairExt objects 
    // stored in the object array pMixedPairs via HPairQA
    if (pQA)
    {
	if (pMixedPairs) pQA->fillData(pMixedPairs);
	else 
	{
	    Error("makeMixedPairs","no mixed pair array"); 
	    return -1;
	}
    }

    // discard no longer needed pair array
    if (pMixedPairs) 
    {
	pMixedPairs->Delete();
	delete pMixedPairs;
    }
    else {Error("execute","no mixed pair array"); return -1;}


    return 0;
}
// ---------------------------------------------------------------------------
