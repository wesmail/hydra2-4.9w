//*-- Author : M.Sanchez (24.05.2000)
//*-- Modified : M.Sanchez(24.05.2000)
//*-- Modified : D.Zovinec(23.09.2002) ToF hits clustering
//*-- Modified : D.Zovinec(31.03.2003) META overlap correction
//*-- Modified : P.Cabanelas (15.12.2003) 2 targets for nov02
//*-- Modified : (01.07.2003) A.Sadovsky && P.Cabanelas && V.Pechenov: HBaseTrack style output && link to HMetaMatch
//*-- Copyright : GENP
#include "hkicktrackfb.h"
#include "hmdcseg.h"
#include "hmdchit.h"
#include "hgeomvector.h"
#include "hgeomtransform.h"
#include "hgeomcompositevolume.h"
#include "hgeomvolume.h"
#include "hdetgeompar.h"
#include "htofgeompar.h"
#include "htofclusterfpar.h"
#include "hmdcgeompar.h"
#include "hades.h"
#include "hevent.h"
#include "hcategory.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hruntimedb.h"
#include "hmdcdef.h"
#include "hmdctrackgdef.h"
#include "hmatrixcategory.h"
#include "hiterator.h"
#include "hspecgeompar.h"
#include "hkicktrackfpar.h"
#include "hkicktrack.h"
#include "hkicktrackB.h"
#include "hmetamatch.h"
#include "hmdctrkcand.h"
#include "hkickparasciifileio.h"
#include "hparasciifileio.h"
#include "kickdef.h"
#include "showertofinodef.h"
#include "hshowergeometry.h"
#include "tofdef.h"
#include "showerdef.h"
#include "htofinogeompar.h"
#include "TClonesArray.h"
#include "TRandom.h"
#include "TMath.h"
#include <stdio.h>
#include "TH1.h"
#include "heventheader.h" 
#include "hmdctrackgspline.h"
#include "hgeomtransform.h"

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////
// HKickTrackFB
//
//  This class is intended to match Mdc segments before and after de
//  magnet associating the matching pairs a momentum The momentum
//  calculation and matching is done using
//  a kick plane technique to parametrize the deflection of tracks
//  in the HADES inhomogeneous magnet.
//
// Additional documentation available <a href="http://www-hades.gsi.de/persons/sanchez/hadesdocs/kickdoc.ps.gz">here</a>
//
//  ToF hits clustering implemented. Use option "tofclust" in HKickTaskSet
//  to invoke this functionality.
//
//  ----------------( Modification scince 01.06.2004 )--------------
//  This modification involves an output into HBaseTrack
//  and it also establish a link to HMetaMatch so one
//  will be able to use it from HMetaMatch 
//  This is done to satisfy "GeneralizedTrackingProject"
//
//  To find correspondence of low resolution HKickTrack
//  with HMetaMatch we tooke a simplest way - we did no rewrite
//  Manuel's class HKickTrackF but rather changed its name
//  into HKickTrackFB . To then we compared Manuel's track candidate
//  components {HMdcSeg, Shower/TOFINO-hit, TOF(hit/cluster)}
//  with the coresponding HMetaMatch track components
//  {HMdcSeg, Shower/TOFINO-hit, TOF(hit/cluster)} and if
//  such coincidence was found we simply provided HKickTrackB
//  object into HMetaMatch. As the result user has HKickTrack
//  (with the HKickTrackB output stile) into HMetaMatch
//
//  Warning - now user has to use HKickTrackB task !AFTER! 
//  HMetaMatch task - becouse HKickTrackB uses HMetaMatch
//  information, and writes itself into HMetaMatch
//
////////////////////////////////////////////////

const Int_t HKickTrackFB::kInactiveSector=0;//Sector is not active
const Int_t HKickTrackFB::kLowResSector=1; //Sector has not outer chambers

HKickTrackFB::HKickTrackFB(const Text_t name[],const Text_t title[])
  : HKickTask(name,title),fOuterHitArray(1000) {
  //Default constructor.
  fImpacts=new TClonesArray("HKickImpact",20);
  fInput=0;
  fNImpacts=0;
  fHasTof=fHasShower=kFALSE;
  fNTofHits=fNShowerHits=0;
  fEvaluator=0;
  fConservativeMode=kTRUE;
  fTofClusterMode = kFALSE;

  fShowerCatId = catShowerHitTof;
  fSectors.Set(6); //FIXME: Nb of sectors should not be hardcoded.
  fHistograms=new TList;
  fControlHistograms=kFALSE;
  fDebug = kFALSE;
  fUsingVertex = kTRUE;
  fMultipleTarget = kFALSE;
  fIter      =0;
  fTofIter   =0;
  fTofCIter  =0;
  fShowerIter=0;
  fMetaMatchIter=0;
  fillOldKickTrackCat=kFALSE;
  fCandidateOutOld=0;
  fTofClustGarb=0;
}

HKickTrackFB::~HKickTrackFB(void) {
  //Guess it?
  if (fEvaluator) delete fEvaluator;
  if(fImpacts) delete fImpacts;
  if(fIter)      {delete fIter;      fIter      =0;}
  if(fTofIter)   {delete fTofIter;   fTofIter   =0;}
  if(fTofCIter)  {delete fTofCIter;  fTofCIter  =0; }
  if(fShowerIter){delete fShowerIter;fShowerIter=0;}
  if(fMetaMatchIter){ delete fMetaMatchIter; fMetaMatchIter=0;}
  if(fTofClustGarb) { delete fTofClustGarb ; fTofClustGarb =0;}
  if(Spline) {delete Spline; Spline=0;}
  HMdcGetContainers::deleteCont();
  //if (tRans) delete [] tRans; // DO not delete array, that is defined in header!!!
}

Int_t HKickTrackFB::execute(void) {
  //Launchs reconstruction
  if (fKickPlane->getFieldStrength() < 0.001) return 1;
  return execute_mult();
}

Int_t HKickTrackFB::execute_mult(void) {
  //This is the brain of this class. It directs the reconstruction for each event
  Int_t r=0;
  Float_t pStep;
  HLocation levArmLoc;

  fSectorLoc.set(1,0);
  levArmLoc.set(2,0,0);

  for (fSectorLoc[0]=0; fSectorLoc[0] < fSectors.GetSize(); fSectorLoc[0]++) {
    if ( fSectors[fSectorLoc[0]] != kInactiveSector ) { 
      fOuterHitArray.Clear();
      levArmLoc[1] = 0;
      levArmLoc[0] = fSectorLoc[0];
      readHits(levArmLoc);

      if (fSectors[fSectorLoc[0]] == kLowResSector) {
  	readMetaHits(fSectorLoc); 
	fCandidates.resize(fNImpacts, fNTotalHits);
	evaluateMetaCandidates(); //Fill fCandidates matrix
      } else {
	Error("execute",
	      "Internal error, please report bug code 1 to M. Sanchez");
      }

      pStep = (fKickPar->getMaxXPull2()-fKickPar->getMaxXPull1()) / 
	fKickPar->getXPullStep();
      pStep = (fKickPar->getPMin2() - fKickPar->getPMin()) / pStep;
    
      for (fMaxXPull = fKickPar->getMaxXPull1(), fMinMomentum=fKickPar->getPMin(); 
	   fMaxXPull < fKickPar->getMaxXPull2();
	   fMaxXPull += fKickPar->getXPullStep(), fMinMomentum += pStep) {
	assignUniqueCombinations();
      }
      fMaxXPull = fKickPar->getMaxXPull2();
      if (fConservativeMode) {
	assignRemainingCombinations();
      }

      fImpacts->Clear();
      fNImpacts=0;
      if(fTofClustGarb) fTofClustGarb->Clear();
    }
  }
  return r; 
}

void HKickTrackFB::fillControl(HKickCandidate &candidate, HKickImpact *imp) {
  //Fills control variables, histograms....
  //
  HMdcSeg *seg = imp->getOriginalSegment();
  fRecStat->Fill(candidate.getXPull(),candidate.getSystem(),
      candidate.getMomentum(),candidate.getMass(),seg->getTheta(),
      seg->getPhi(),candidate.getTof(),candidate.getPolarity(),candidate.getSector());
  
}


void HKickTrackFB::evaluateMetaCandidates(void) {
  // Loops on all identified candidates evaluating each of them. 
  // That is, calculating its associated momentum and other useful variables 
  // for the algorithm
  Int_t dx=fCandidates.getDimX();

  HGeomVector tofR;
  Int_t i,j,k;
  HKickImpact *imp=0;
  HTofHit *tofHit=0;
  HTofCluster *tofCluster=0;
  HShowerHit *shoHit=0;
  // cout << "ll" << endl;FIXME:el resto de las cosas a partir de la linea 578
  for (i=0;i<dx;i++) {
    imp=(HKickImpact *)fImpacts->UncheckedAt(i);
    
   HVertex &event_vertex = gHades->getCurrentEvent()->getHeader()->getVertex();
   HGeomVector &vertex = event_vertex.getPos();

   fTargetFlag = 0;
   if (fMultipleTarget) {
     if (fKickPlaneList[0] != 0) {
       if (fUsingVertex && event_vertex.getIterations()>0) {
	 if (vertex(2) <= -30.29) {
	   fEvaluator->setKickPlane(fKickPlaneList[1],fMatchParList[1]); 
	   fTargetFlag = 2;
	 } else {
	   fEvaluator->setKickPlane(fKickPlaneList[0],fMatchParList[0]);
	   fTargetFlag = 1;
	 }
       }
       //-FIXME: vertex algorithm can be out of the "for" bucle to do the evaluation per each event. Here, is done per each track. 
       else {  	
	 if (imp->getOriginalSegment()->getZ() > -30.29) {
	   fEvaluator->setKickPlane(fKickPlaneList[0],fMatchParList[0]);
	   fTargetFlag = -1;
	 } else {
	   fEvaluator->setKickPlane(fKickPlaneList[1],fMatchParList[1]);
	   fTargetFlag = -2;
	 }
       }
     } else {
       Warning("evaluateMetaCandidates","Asked for multiple target but only one target initialized");
     }
   }
   
    if(fTofClusterMode){
      for (j=0; j<fNTofHits; j++) {
        HKickCandidate &candidate=fCandidates(i,j);
        tofCluster = (HTofCluster *)fOuterHitArray.UncheckedAt(j);
        fEvaluator->evaluate(imp, tofCluster, candidate);
        if (fDebug) fillControl(candidate,imp);
      }
      selectTofClusters(i);
    } else {
      for (j=0; j<fNTofHits; j++) {
        HKickCandidate &candidate=fCandidates(i,j);
        tofHit = (HTofHit *)fOuterHitArray.UncheckedAt(j);
        fEvaluator->evaluate(imp, tofHit, candidate);
        if (fDebug) fillControl(candidate,imp);
        // candidate.Dump();
      }
    }
    for (j=fNTofHits,k=0; j<fNTofHits+fNShowerHits; j++,k++) {
      HKickCandidate &candidate = fCandidates(i,j);
      shoHit = (HShowerHit *)fOuterHitArray.UncheckedAt(j);
      fEvaluator->evaluate(imp, shoHit, candidate); 
      checkMETAoverlap(i,j); 
      if (fDebug) fillControl(candidate,imp);
    }
  }
}

void HKickTrackFB::checkMETAoverlap(Int_t i,Int_t j){
  HGeomVector canddir, candtofdir;
  canddir = fCandidates(i,j).getMETAdir();
  for(Int_t k=0; k<fNTofHits; k++){
    candtofdir=fCandidates(i,k).getMETAdir();
    if((canddir.scalarProduct(candtofdir)) > 0.999) fCandidates(i,j).markMETAoverlap();
  }  
}

Bool_t HKickTrackFB::passesCuts(HKickCandidate &c) {
  //Checks if a given candidate passes all cuts to be considered a good candidate
  Float_t aPhi=TMath::Abs(c.getXPull());
  return ( !c.isAssigned() && 
	   ( aPhi < fMaxXPull ) &&
           ( c.isClAccepted() ) &&
	   ( !c.isMETAoverlap() ) &&
	   (c.getMomentum() > fMinMomentum) );
}

void HKickTrackFB::assignRemainingCombinations(void) {
  Int_t i,j;
  Int_t impacts = fCandidates.getDimX();
  Int_t dy = fCandidates.getDimY();

  for (i=0;i<impacts;i++) { //FIXME: optimize (list of remaining candidates)
    for (j = 0; j < dy; j++) {
      if ( passesCuts(fCandidates(i,j)) ) {
	fillData(fCandidates(i,j),(HKickImpact *)fImpacts->At(i));
      }
    }
  } 
}

void HKickTrackFB::assignUniqueCombinations(void) {
  //Match those candidates which pass all cuts and verifying that
  //neither the META hit or the MDC hit are used in other valid candidates. 
  //In case such a conflict is detected, this method tries to find a solution.
  Int_t impacts=fCandidates.getDimX();
  Int_t dy=fCandidates.getDimY();
  Int_t maxMatchs=0,matchsFound;
  Int_t i,j;

  maxMatchs=(impacts < dy)?impacts:dy;
  if (maxMatchs > 0) {
    do {
      matchsFound=0;
      for (i=0;i<impacts;i++) {
	for (j=0;j<dy;j++) {
	  if ( passesCuts(fCandidates(i,j)) ) {
	    if ( isUnique(i,j) ) { 
	      fillData(fCandidates(i,j),(HKickImpact *)fImpacts->At(i));
	      matchsFound++;
	      //maxMatchs--;
	    }
	  }
	}
      }
      if ( matchsFound >= maxMatchs ) matchsFound=0;    
    } while (matchsFound>0);
  }
}

HKickTrack* HKickTrackFB::makeOldTrack(void) {
  TObject *slot = 0;
  HKickTrack *track = 0;

  slot = fCandidateOutOld->getNewSlot(fSectorLoc);
  if (slot) {
    track = new(slot) HKickTrack;
  } else Error("makeTrack","No slot available");

  return track;
}

HCategory *HKickTrackFB::makeOldOutputCategory(HEvent *event) {
  fCandidateOutOld=event->getCategory(catKickTrack);
  if (!fCandidateOutOld) {
    Int_t ssize[2]={6,100};
    fCandidateOutOld = new HMatrixCategory("HKickTrack",2,ssize,0.5);
    if (fCandidateOutOld) event->addCategory(catKickTrack,
                                        fCandidateOutOld,
                                        "Tracks");
    else {
      Error("init","Unable to setup output to catKickTrack");
      return 0;
    }
  }
  return fCandidateOutOld;
}

Float_t HKickTrackFB::CalcTarDist(HMdcSeg *seg)
{
   Float_t tarDist;
   Int_t sector;
   sector=(Int_t)seg->getSec();
   if(tRans[sector]==0)
      {
         tRans[sector]=new HGeomTransform();
         if(!fGetCont->getLabTransSec(*(tRans[sector]),sector))
            {
               return kFALSE;
            }
      }
   
   HVertex &vertex = gHades->getCurrentEvent()->getHeader()->getVertex();
   tarDist=Spline->calcTarDist(vertex,seg,tRans[sector]);

   return tarDist;
}


HKickTrack * HKickTrackFB::fillDataInOldCat(HKickCandidate &candidate,
                                 HKickImpact *imp) {
  //Fills output data in the output category
  HKickTrack *c=0;
  HMdcSeg *seg=0;

  c = makeOldTrack();
  if (c) {
    seg = imp->getOriginalSegment();
    c->setSector(seg->getSec());
    c->setSegment(imp->getOriginalSegmentId());
    c->setZ(seg->getZ(),seg->getErrZ());
    c->setR(seg->getR(),seg->getErrR());
    c->setTheta(seg->getTheta(),seg->getErrTheta());
    c->setPhi(seg->getPhi(),seg->getErrPhi());
    c->setP(candidate.getMomentum(),candidate.getMomentumError());
    c->setCharge(char(candidate.getPolarity()));
    c->setMass(candidate.getMass(),100.); //FIXME: Do proper error calculation
    c->setQuality(candidate.isAssigned()?1:0);
    c->setPTof(candidate.getPTof());
    c->setErrPTof(candidate.getPTofErr());
    c->setPID(candidate.getPID());
    c->setSystem(candidate.getSystem());
    c->setTof(candidate.getTof());
    c->setMetaEloss(candidate.getMetaEloss());
    c->setBeta(candidate.getBeta());
    c->setPull(candidate.getXPull());
    c->setFlag(fTargetFlag);
  
    if (candidate.getSystem() == 0) { //Specific stuff for Shower
      HShowerHit *oHit = 0;
      oHit = dynamic_cast<HShowerHit *>(candidate.getOuterHit());
      if (!oHit) {
        Error("fillDataInOldCat","Internal error. Please report bug!");
      } else {
        c->setOuterHitId(fShowerHits->getIndex(oHit)); //FIXME: Optimize
        c->setShowerSum10(oHit->getSum(1) / oHit->getSum(0));
        c->setShowerSum20(oHit->getSum(2) / oHit->getSum(0));
      }
    } else {                          //Specific stuff for TOF
      HTofHit *oHit = dynamic_cast<HTofHit *>(candidate.getOuterHit());
      if (!oHit) Error("fillDataInOldCat","Internal error. Please report bug!");
      else c->setOuterHitId(
        oHit->getSector()*1000 + oHit->getModule()*100 + oHit->getCell());
    }
  }
  return c;
}

HKickTrackB *HKickTrackFB::makeTrack(void) {
  TObject     *slot  = 0;
  HKickTrackB *track = 0;
  slot = fCandidateOut->getNewSlot(fSectorLoc);
  if (slot) {
    track = new(slot) HKickTrackB;
  } else Error("makeTrackB","No slot available");
  return track;
}

HCategory *HKickTrackFB::makeOutputCategory(HEvent *event) {
  fCandidateOut=event->getCategory(catKickTrackB);
  if (!fCandidateOut) {
    Int_t ssize[2]={6,100};
    fCandidateOut = new HMatrixCategory("HKickTrackB",2,ssize,0.5);
    if (fCandidateOut) event->addCategory(catKickTrackB,
					  fCandidateOut,
					  "Tracks");
    else {
      Error("init","Unable to setup output");
      return 0;
    }
  }
  return fCandidateOut;
}

HKickTrackB * HKickTrackFB::fillData(HKickCandidate &candidate, HKickImpact *imp) {
  //Fills output data in the output category
  if(fCandidateOutOld) fillDataInOldCat(candidate,imp);
  HKickTrackB *c=0;
  HMdcSeg *seg=0;
  HLocation hLoc;    //to get TOF hit index
  hLoc.set(3,0,0,0); //to get TOF hit index

  c = makeTrack();
  if (c) {
    Bool_t isTofClusterMode = kFALSE;
    seg = imp->getOriginalSegment();
    c->setSector(seg->getSec());
    c->setSegment(imp->getOriginalSegmentId());
    c->setZ(seg->getZ(),seg->getErrZ());
    c->setR(seg->getR(),seg->getErrR());
    c->setTheta(seg->getTheta(),seg->getErrTheta());
    c->setPhi(seg->getPhi(),seg->getErrPhi());
    c->setP(candidate.getMomentum(),candidate.getMomentumError());
    c->setPolarity(char(candidate.getPolarity()));
    c->setMass2(candidate.getMass(),100.); //FIXME: Do proper error calculation
    c->setQuality(candidate.isAssigned()?1:0);
    c->setPTof(candidate.getPTof());
    c->setErrPTof(candidate.getPTofErr());
    c->setPID(candidate.getPID());
    c->setTarDist(CalcTarDist(seg));
    //c->setSystem(candidate.getSystem()); //not any more in HBaseTrack -> look it in HMetaMatch
    c->setTof(candidate.getTof());
    c->setMetaEloss(candidate.getMetaEloss());
    c->setBeta(candidate.getBeta());
    c->setPull(candidate.getXPull());
    c->setFlag(fTargetFlag);

    Short_t showerHitInd   = -1; //Useful also for HKickTrack <-> HMetaMatch relation
    Short_t tofHitInd      = -1; //Useful also for HKickTrack <-> HMetaMatch relation

    if (candidate.getSystem() == 0) { //Specific stuff for Shower
      HShowerHit *oHit = 0;
      oHit = dynamic_cast<HShowerHit *>(candidate.getOuterHit());
      if (!oHit) {
	Error("fillData","Internal error. Please report bug!");
      } else {
        showerHitInd=fShowerHits->getIndex(oHit);
        c->setShowerHitInd(showerHitInd);
	c->setOuterHitId(  showerHitInd); //FIXME: Optimize
	c->setShowerSum10(oHit->getSum(1) / oHit->getSum(0));
	c->setShowerSum20(oHit->getSum(2) / oHit->getSum(0));
      }
    } else {                          //Specific stuff for TOF
      HTofHit *oHit = dynamic_cast<HTofHit *>(candidate.getOuterHit());
      if (!oHit) {
	Error("fillData","Internal error. Please report bug!");
        return c;
      } else {
        hLoc.set(3,oHit->getSector(),oHit->getModule(),oHit->getCell());
	c->setOuterHitId(hLoc[0]*1000 + hLoc[1]*100 + hLoc[2]);
      }
      // TofClusterSize >=1 - catTofCluster
      // TofClusterSize <=0 - catTofHit

      if(fTofClusterMode && ((HTofCluster*)oHit)->getClusterSize()>0){
        isTofClusterMode = kTRUE;
        tofHitInd=fTofClusters->getIndex(hLoc);
      } else {
        //TOF-hit-mode--
        isTofClusterMode = kFALSE;
        tofHitInd=fTofHits->getIndex(hLoc); //vladimir
      }
      
      c->setTofHitInd(tofHitInd);
      
      //-META-overlap:(begin)----------------------------------
      // In sase of TOF signal we still can have signal from
      // Shower/TOFINO from a small overlap region
      // in case of overlap both indexes are not equal to -1
      // in case of no shower showerHitInd=-1 but tofHitInd!=-1
      TObject* shower=candidate.getShowerHit();
      showerHitInd=(shower) ? fShowerHits->getIndex(shower):-1;
      c->setMetaHitInd(showerHitInd, tofHitInd); 
      //-META-overlap:( end )----------------------------------
    }
    //
    // Here we establish one to one correspondence between HKickTrackB and HMetaMatch objects
    // This is an addition to Manuel's original code to make it compatible with the
    // HMetaMatch approach with minimal changes to Manuel's code...
    // 
    // Here we have KickTrack filled into HKickTrackB
    // KickPlane functionality is obtained via "candidate.get****()"
    //

    //
    // HMetaMatch loop to find HMetaMatch which is based on the same information as current HKickTrackB
    //
    fMetaMatchIter->Reset();
    locMetaMatch.set(1, imp->getSector() );
    //while((pMetaMatch=(HMetaMatch*)(fMetaMatchIter->Next()))!=0){
    fMetaMatchIter->gotoLocation(locMetaMatch);
    Bool_t isKickMMmatching=kFALSE;
    while((pMetaMatch=(HMetaMatch*)(fMetaMatchIter->Next()))!=0) {
      Int_t system = pMetaMatch->getSystem();     //get current system
      if(system<0) continue; //If no TOF && no SHOWER/TOFINO - then no LowRes HKickTrack is possible

      Short_t indTrkCand = pMetaMatch->getTrkCandInd(); //index of track candidate
      pMdcTrkCand=(HMdcTrkCand*)fCatMdcTrkCand->getObject(indTrkCand); //pointer to track candidate

      //
      // Here we compare HMdcSeg from KickPlane and from HMetaMatch
      //
      // one way of doing this is compare pointers 
      // HMdcSeg *seg = imp->getOriginalSegment();         
      // and 
      // (HMdcSeg*)fCatMdcSegSim->getObject(index1);
      // another way is to use index inside the HMdcSeg category we stick to it
      // calling the function getObject(index) we get unique HMdcSeg
      //

      Int_t indexMdcSeg = imp->getOriginalSegmentId();     //3rd index in Matrix category
      segLoc.set(3,imp->getSector(),0,indexMdcSeg);        //we set loocation of this object to HMatrix Category
      Int_t indexMdcSegANHMC=fCatMdcSeg->getIndex(segLoc); //Here we get indexHdcSegment "A"bsolute "N"umber of the object in "HM"atrix "C"ategory
      if(indexMdcSegANHMC<0) {
        Error("executeTrMatch","No HMdcSeg object for HKickTrack");
        continue;
      }
      //cout << "indTrkCand" <<indTrkCand<< endl;
      //cout << "pMdcTrkCand=" << pMdcTrkCand << endl;
      //cout << "pMdcTrkCand->getSeg1Ind()=" << pMdcTrkCand->getSeg1Ind() << endl;
      //cout << "         indexMdcSegANHMC=" << indexMdcSegANHMC          << endl;
      if(pMdcTrkCand->getSeg1Ind() != indexMdcSegANHMC) continue;

      //
      // By now we have selected HKichTrack and HMetaMatch who has same incoming HMdcSeg track
      // Lets compare META indexes
      //

      //For META="TOFINO/Shower"
      if(system==0 && pMetaMatch->getShowerHitInd()!=showerHitInd) continue;

      //For META-TOF (overlap can be here but not explicitly treated be CAREFUL)
      if(system>0){  
        Bool_t isMetaClusterMode = (pMetaMatch->getTofClusterSize()<=0) ? kFALSE:kTRUE; //Tells in which mode HMetaMatch treated this TOF-hit_or_cluster
        if(isMetaClusterMode  !=  isTofClusterMode) continue; //if Cluster-Hit or Hit-Cluster - skip this HMetaMatch object (becouse both should be the same to be compared)
        if(pMetaMatch->getTofHitInd() != tofHitInd) continue; //if KickTrack and MetaMatch  not coinside skip
      }//-end-if-system>0--

      //For the case of TOF&SHOWER overlap
      //In this case we have bouth objects: (TOF and SHOWER) in kickplane
      //to be compared  with bouth objects  (TOF and SHOWER) in HMetaMatch
      //But we have them already compared one by one see upwards so nothing more is needed

      //
      // If our HMetaMatch survived all the chekings - this is 
      // the real corresponding object to our HKickTrack.
      // This is index of our created HKickTrackB to be provided to HMetaMatch
      Int_t indexKickTrack=fCandidateOut->getIndex(c);
      pMetaMatch->setKickInd(indexKickTrack);
      isKickMMmatching=kTRUE;
    }//-end-while-pMetaMatch--
    if(!isKickMMmatching) Error("fillData","No matching KickTrack <-> MetaMatch");
  }
  return c;
}



Bool_t HKickTrackFB::isUnique(Int_t i,Int_t j) {
  //Checks if a candidate is unique. The mechanism for solving conflicts is implemented
  //here
  Int_t k,l;
  
  for (k=0;k<fCandidates.getDimY(); k++) 
    if (k!=j && passesCuts(fCandidates(i,k)) ) return kFALSE;
  for (k=0;k<fCandidates.getDimX(); k++) 
    if (k!=i && passesCuts(fCandidates(k,j)) ) {
      for (l=0; 
	   l<fCandidates.getDimY() && (!passesCuts(fCandidates(k,l)) || l==j);
	   l++) ;
      if (l==fCandidates.getDimY()) 
	return kFALSE;
    }
  for (k=0;k<fCandidates.getDimX();k++) 
    fCandidates(k,j).markAssigned();
  return kTRUE;
}


HKickCandidateEvaluator *HKickTrackFB::makeEvaluator(void) {
  //Creates a candidate evaluator. Factory method
  return new HKickCandidateEvaluator(fKickPlane,fMatchPar);
}

void HKickTrackFB::checkTofClusterParticipants(Int_t i,Int_t j,Int_t sz){
  // Tof hits clustering auxiliary function.
  // It checks the pair of tracks composed from hits
  // on their momentum difference and polarity and on
  // energy deposited in ToF wall corrected on path
  // length of the track in ToF wall.
  // In case that the kick-candidate composed from
  // cluster-candidate and mdc segment was not accepted
  // by clustering conditions the fClAccept flag is set to 0.
  // In case that it was accepted the fClAccept flags for
  // appropriate kick-candidates composed from hits participating on the
  // cluster and mdc segment are set to 0.

  Float_t pol1, pol2, mom1, mom2, admom, eloss;
  Bool_t flagIsCl;
  HTofCluster *clust=0;
  HGeomVector imppos, tofpos, tofimp, impposlab;

  flagIsCl=kTRUE;

  if(sz == 2){
    // momentum and polarity conditions
    HKickCandidate &cand1=fCandidates(i,j+1);
    HKickCandidate &cand2=fCandidates(i,j+2);
    pol1=cand1.getPolarity();
    mom1=cand1.getMomentum();
    pol2=cand2.getPolarity();
    mom2=cand2.getMomentum();
    if(pol1 != pol2) flagIsCl=kFALSE;
    if((pol1 == 0) && (pol2 == 0)) flagIsCl=kFALSE;
    if(fControlHistograms==kTRUE){
      histDiffMom->Fill(mom1-mom2);
    }
    admom=TMath::Abs(mom1-mom2);
    if(admom > (fTofClFPar->getMaxPDiff())){
      flagIsCl=kFALSE;
    } else {
      if(fControlHistograms==kTRUE){
        histDiffMomCut->Fill(mom1-mom2);
      }
    }
    // energy loss condition
    if(flagIsCl == kTRUE){
      HKickCandidate &cand=fCandidates(i,j);
      clust = (HTofCluster *)(cand.getOuterHit());
      if((clust->getTof()) < (fTofClFPar->getMIPLimit())){
        if(clust->getAdcFlag() != 2){
          flagIsCl=kFALSE;
        } else {
          eloss=cand.getMetaEloss();
          if(eloss <= 0.0){
            flagIsCl=kFALSE;
            Warning("checkTofClustParticipants","negative or zero energy loss. cluster not accepted\n");
          } else {
            if(fControlHistograms==kTRUE){
              histCorELoss->Fill(eloss);
            }
            if(eloss > (fTofClFPar->getMaxELoss())){
              flagIsCl=kFALSE; //condition on energy loss
            } else {
              if(fControlHistograms==kTRUE){
                histCorELossCut->Fill(eloss);
              }
            }
          }
        }
      }
    }
    // decision
    if(flagIsCl == kFALSE){
      fCandidates(i,j).unmarkClAccept();
    } else {
      fCandidates(i,j+1).unmarkClAccept();
      fCandidates(i,j+2).unmarkClAccept();
    }
  } //else for(k=0; k<(sz+1); k++) fCandidates(i,j+k).unmarkClAccept();  //discard cases of size>2
}

void HKickTrackFB::selectTofClusters(Int_t i){
  // Body of the ToF hits clustering mechanism.
  // It calls function checkTofClusterParticipants
  // for each combination cluster - mdc segment.
  Int_t j;
  HTofCluster *tofCluster=0;
  Int_t size_cl;

  for (j=0; j<fNTofHits; j++) {
    HKickCandidate &candid=fCandidates(i,j);
    tofCluster = (HTofCluster *)(candid.getOuterHit());
    size_cl=tofCluster->getClusterSize();
    if(size_cl>1){
      checkTofClusterParticipants(i,j,size_cl);
    }
  }
}


Bool_t HKickTrackFB::init(void) {
  //Initializes
  if(fTofClustGarb==0) fTofClustGarb=new TClonesArray("HTofCluster",6); 
  if (gHades) {
    HEvent *event=gHades->getCurrentEvent();
    HRuntimeDb *rtdb=gHades->getRuntimeDb();
    HSpectrometer *spec=gHades->getSetup();
    Spline=new HMdcTrackGSpline("Spline","Spline");
    for(Int_t i=0; i<6;i++)
       {
	  tRans[i]=0;
       }
    if (event && rtdb) {
       
      HDetector *tof=spec->getDetector("Tof");
      HDetector *shower = spec->getDetector("Shower");
      HDetector *mdc = spec->getDetector("Mdc");
      HDetector *tofino = spec->getDetector("Tofino");
      fGetCont=HMdcGetContainers::getObject();
      fGetCont->getMdcGeomPar();
      fGetCont->getSpecGeomPar();
      
      setKickParIo(rtdb);

      if (tof) { //Has the user set up a TOF?
	fTofHits=event->getCategory(catTofHit);
	if (!fTofHits) {
	  fTofHits=tof->buildCategory(catTofHit);
	  if (!fTofHits) {
	    Error("init","No TOF input");
	    return kFALSE;
	  }
	}
	fTofIter=(HIterator *)fTofHits->MakeIterator();
        if(fTofClusterMode){
          fTofClusters=event->getCategory(catTofCluster);
          if(fTofClusters==0) {
            Error("init",
                "fTofClusterMode==kTRUE but catTofCluster is absent, fTofClusterMode will setted to kFALSE.");
            fTofClusterMode=kFALSE;
          } else {
            fTofCIter=(HIterator *)fTofClusters->MakeIterator();
            fTofClFPar=(HTofClusterFPar *)rtdb->getContainer("TofClusterFPar");
          }
        }

	fTofGeometry=(HTofGeomPar *)rtdb->getContainer("TofGeomPar");

	fHasTof=kTRUE; //TOF found!
     }
     
     
     if (shower) { //Has the user set up a Shower?
       
       if (tofino) { //Require Tofino only if there is Shower.
	 fTofinoGeometry = (HTofinoGeomPar *)rtdb->getContainer("TofinoGeomPar");
	 
       } else {
	 Warning("init","The kickplane requires the Tofino detector to be defined");
	 return kFALSE;
       }
       
       fShowerGeometry = (HShowerGeometry *)rtdb->getContainer("ShowerGeometry");

       if(gHades->getCurrentEvent()->getCategory(catGeantKine) &&
	  gHades->getEmbeddingMode()>0)
       {   // only for embedding
	   fShowerCatId = catShowerHitTofTrack;
       }

       fShowerHits=event->getCategory(fShowerCatId);
       if (!fShowerHits) {
	 //fShowerHits = shower->buildCategory(fShowerCatId);
	 if (!fShowerHits) {
	   Error("init","No Shower input available");
	   return kFALSE;
	 }
       }
       fShowerIter = (HIterator *)fShowerHits->MakeIterator();
       fHasShower=kTRUE; //Shower found!
     }
     
     if (mdc) { //Has the user set up a MDC?
       
       fInput=event->getCategory(catMdcSeg);
       if (!fInput) {
	 Error("init","No MDC segment category defined");
	 return kFALSE;
       }
       else fIter=(HIterator *)fInput->MakeIterator();
       
       for (Int_t i=0;i<fSectors.GetSize();i++) { //Sector configuration
	 if (mdc->isSectorActive(i)) {
	   fSectors[i] = kLowResSector; //By default
	 } else {
	   fSectors[i] = kInactiveSector;
	 }
       }
       fMdcGeometry=(HMdcGeomPar *)rtdb->getContainer("MdcGeomPar");
       
       // 	fKickPlane = HKickPlane2::getMeta();
       // 	fMatchPar  = HKickMatchPar::getMeta();
       if (fMultipleTarget) {
	 fKickPlaneList[0] = (HKickPlane2   *)rtdb->getContainer("KickPlane2Meta");
	 fKickPlaneList[1] = (HKickPlane2   *)rtdb->getContainer("KickPlane2MetaT2");
	 fMatchParList[0]  = (HKickMatchPar *)rtdb->getContainer("KickMatchParMeta");
	 fMatchParList[1]  = (HKickMatchPar *)rtdb->getContainer("KickMatchParMetaT2");
	 fKickPlane = fKickPlaneList[0];
	 fMatchPar = fMatchParList[0];
       } else {
	 fKickPlaneList[0] = 0;
	 fKickPlaneList[1] = 0;
	 fMatchParList[0] = 0;
	 fMatchParList[1] = 0;
	 fKickPlane = (HKickPlane2 *)rtdb->getContainer("KickPlane2Meta");
	 fMatchPar = (HKickMatchPar *)rtdb->getContainer("KickMatchParMeta");
       }
       
       //The evaluator is constructed after initialization of TOF and Shower
       //because the information of which class was used in TOF or Shower can
       //be of relevance here.
       if (!fEvaluator) fEvaluator=makeEvaluator();
       fEvaluator->setMdcGeometry(fMdcGeometry);
       fEvaluator->setTofGeometry(fTofGeometry);
       fEvaluator->setShowerGeometry(fShowerGeometry);
       fEvaluator->setTofinoGeometry(fTofinoGeometry);
       fKickPar=(HKickTrackFPar *)rtdb->getContainer("KickTrackFPar");
     } else {
       Error("init","No MDC detector defined");
       return kFALSE;
     }
    
 
     fSpecGeometry = (HSpecGeomPar *)rtdb->getContainer("SpecGeomPar");
     fEvaluator->setSpecGeometry(fSpecGeometry);
     
     // make Output
     makeOutputCategory(event);
     if(fillOldKickTrackCat) makeOldOutputCategory(event);
    } else {
      Error ("init","! (event && rtdb)");
      return kFALSE; //! (event && rtdb)
    }


    //
    // This categories needed by HMetaMatch to compare with
    //  
    fCatMdcTrkCand=event->getCategory(catMdcTrkCand);
    if(!fCatMdcTrkCand) {
        Error ("init()","! No catMdcTrkCand in input!");
	return kFALSE;
    }

//  cout << "HKickTrackFB::init()--enD--" << endl;

    fCatMetaMatch=event->getCategory(catMetaMatch);
    if (!fCatMetaMatch) {
	Error ("init()","! No catMetaMatch in input!");
	return kFALSE;
    }

    fMetaMatchIter=(HIterator*)fCatMetaMatch->MakeIterator();
    if(!fMetaMatchIter) {
	Error ("init()","! failed to create iterator of catMetaMatch!");
	return kFALSE;
    }

    fCatMdcSegSim=event->getCategory(catMdcSeg);
    fCatMdcSeg=event->getCategory(catMdcSeg);
    fCatTof=event->getCategory(catTofHit);


  } else {
    Error ("init","! (gHades)");
    return kFALSE; //! (gHades)		
  }

  if(fControlHistograms==kTRUE){
    histCorELoss = new TH1F("corrELoss","energy loss corrected on path",600,0.0,30.0);
    fHistograms->Add(histCorELoss);
    histCorELossCut = new TH1F("corrELossCut","energy loss corrected on path (after cut)",600,0.0,30.0);
    fHistograms->Add(histCorELossCut);
    histDiffMom = new TH1F("diffMom","hits momentum difference",400,-20.0,20.0);
    fHistograms->Add(histDiffMom);
    histDiffMomCut = new TH1F("diffMomCut","hits momentum difference (after cut)",400,-20.0,20.0);
    fHistograms->Add(histDiffMomCut);
  }

  if (Hades::instance()->getOutputFile() && fDebug) {
    fRecStat = new TNtuple("trackf_recstat","Control","xpull:s:p:mass:theta:phi:tof:charge:sector");
  } else fDebug=kFALSE;

//  cout << "HKickTrackFB::init()--end--" << endl;

  return kTRUE;
}

Bool_t HKickTrackFB::reinit(void) {
  fEvaluator->setTofXResolution(fKickPar->getTofXResolution());
  /*HSurfXCilinder *surface = (HSurfXCilinder *)fKickPlane->getSurface();
  surface->move(-132);*/
  return kTRUE;
}

Bool_t HKickTrackFB::finalize(void) {
  //Finalization code
  if (fDebug && fRecStat) {
    Hades::instance()->getOutputFile()->cd();
    fRecStat->Write();
  }
  return kTRUE;
}

void HKickTrackFB::clearImpactList(void) {
  fImpacts->Clear();
}

void HKickTrackFB::readMetaHits(HLocation &loc) {
  //Reads hits from MDC and TOF detectors
  HTofHit *tofHit=0;
  HShowerHit *shoHit=0;
  HTofCluster *tofCluster=0;
  HTofCluster *tofClHit=0;
  Int_t i=0;
  if (hasTof()) {
    fTofIter->Reset(); //FIXME: Remove ??
    fTofIter->gotoLocation(loc);
    if(fTofClusterMode){
      Int_t nClustGarb=0;
      fTofCIter->Reset();
      fTofCIter->gotoLocation(loc);
      while ((tofCluster=(HTofCluster *)fTofCIter->Next()) != 0) {
        Int_t size_cl=tofCluster->getClusterSize();
        // Outer TOF and clusters with size=2 selecting
        if (size_cl <= 2 && (!hasShower() || tofCluster->getModule() < 8)) {
          if((tofCluster->getTof() > fKickPar->getMinTof()) &&
             (tofCluster->getTof() < fKickPar->getMaxTof())) {
            fOuterHitArray.AddAtAndExpand(tofCluster,i);
            i++;
            if(size_cl < 2) continue;
            Char_t clSec=tofCluster->getSector();
            Char_t clMod=tofCluster->getModule();
            Char_t clCell=tofCluster->getCell();
            while((tofHit=(HTofHit *)fTofIter->Next()) != 0) {
              if(tofHit->getSector()!=clSec || tofHit->getModule()!=clMod ||
                 tofHit->getCell()!=clCell) continue;
              tofClHit=new((*fTofClustGarb)[nClustGarb++]) HTofCluster(tofHit);
              tofClHit->setClusterSize(0);   // tofClusterSize=0 for TofHits from tofCluster only
              fOuterHitArray.AddAtAndExpand(tofClHit,i);
              i++;
              for(Int_t j=0; j<(size_cl-1); j++){
                tofHit=(HTofHit *)fTofIter->Next();
                tofClHit=new((*fTofClustGarb)[nClustGarb++]) HTofCluster(tofHit);
                tofClHit->setClusterSize(0);   // tofClusterSize=0 for TofHits from tofCluster only
                fOuterHitArray.AddAtAndExpand(tofClHit,i);
                i++;
              }
              break;
            }
          }
        }
      }
    } else {
      while ( (tofHit=(HTofHit *)fTofIter->Next()) != 0) {
        if (!hasShower() || tofHit->getModule() < 8) { //Outer TOF
          if (tofHit->getTof() > fKickPar->getMinTof() &&
              tofHit->getTof() < fKickPar->getMaxTof() ) {
            fOuterHitArray.AddAtAndExpand(tofHit,i);
            i++;
          }
        }
      }
    }
    fNTofHits=i;
  } else fNTofHits=0;
  if (hasShower()) {
    fShowerIter->Reset(); //FIXME: remove ??
    while ( (shoHit=(HShowerHit *)fShowerIter->Next()) != 0) {
      if (shoHit->getSector()==fSectorLoc[0] && shoHit->getModule()==0) {
	fOuterHitArray.AddAtAndExpand(shoHit,i);
	i++;
      }
    }
    fNShowerHits=i - fNTofHits;
  } else fNShowerHits=0;  
  fNTotalHits=fNTofHits + fNShowerHits;
}

void HKickTrackFB::readHits(HLocation &loc) {
  //Loops on the input category reading objects and adding objects to
  //the list of candidates fImpacts.
  HMdcSeg *hit=0;
  HGeomVector r;
  HKickImpact *imp=0;
  Int_t i=0;
  Int_t mdcSector=0;
  fIter->Reset();
  fIter->gotoLocation(loc);
  while ( (hit=(HMdcSeg *)fIter->Next()) != 0 ) {
    mdcSector=hit->getSec();
    imp=new( (*fImpacts)[fNImpacts++] ) HKickImpact;
    transform(hit,r,imp->direction());
    fKickPlane->calcIntersection(r,imp->direction(),imp->position());
    imp->setSector(mdcSector);
    imp->setOriginalSegment(hit);
    imp->setOriginalSegmentId(i);
    i++;
  }
}

void HKickTrackFB::transform(HMdcSeg *hit,
			    HGeomVector &r,HGeomVector &alpha) {
  //Calculates position and direction vector for a HMdcHit
  Float_t theta,phi;
  Double_t pi2=TMath::Pi()/2.;

  theta=hit->getTheta();
  phi=hit->getPhi();
  r.setZ(hit->getZ());
  r.setX(hit->getR()*TMath::Cos(phi+pi2));
  r.setY(hit->getR()*TMath::Sin(phi+pi2));
  alpha.setZ(TMath::Cos(theta));
  alpha.setY(TMath::Sin(theta)*TMath::Sin(phi));
  alpha.setX(TMath::Sin(theta)*TMath::Cos(phi));
}



ClassImp(HKickTrackFB)

