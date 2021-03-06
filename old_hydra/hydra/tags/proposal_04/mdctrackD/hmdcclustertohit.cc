//*-- Author : V.Pechenov
//*--Modified : 29.10.2003 by V.Pechenov
//*--Modified : 12.05.2002 by V.Pechenov
//*--Modified : 17.01.2002 by V.Pechenov
//*--Modified : 31.05.2001
//*--Modified : 09.03.2001
//*--Modified : 10.08.2000

///////////////////////////////////////////////////////////////////////////////
//
//  HMdcClusterToHit("","", Int_t flagSeg1, Int_t flagSeg2)
//
//  fills the data objects of HMdcHit and HMdcSeg
//  after track candidate search task (HMdcTrackFinder).
//
// flagSeg1=-1;0;1
//   -1 - don't fill HMdcSeg
//   0 (or 1) - at the typeClustFinder=1 (see hmdctrackfinder.cc)
//     HMdcSeg will filled by hits from MDC1 (or MDC2)
// If in segment 1 is only one MDC (1 or 2) and
// flagSeg1=0 or 1  HMdcSeg will filled by Hit in this MDC.
// flagSeg2= -1;2;3
//    -/- -/-
//
//  HMdcHit and HMdcSeg:
//  Chi2 - size of cluster (in proj. plot bins)
//  user flag - "number of wires in cluster"*100 + "number of merged clusters"
//
//  Using:
//  Insert this task in task list after HMdcTrackFinder.
//
///////////////////////////////////////////////////////////////////////////////

#include "hmdcclustertohit.h"
#include "hmdcdef.h"
#include "hades.h"
#include "hmdchitsim.h"
#include "hmdcsegsim.h"
#include "hmdctrkcand.h"
#include "hmdcdetector.h"
#include "hspectrometer.h"
#include "hmdcgetcontainers.h"
#include "hiterator.h"
#include "hcategory.h"
#include "hmdcclussim.h"
#include "hmdcclusinfsim.h"
#include "hmdctrackdset.h"
#include "hmdcsizescells.h"
#include <stdlib.h>

HMdcClusterToHit::HMdcClusterToHit() {
  setParContainers();
}

HMdcClusterToHit::HMdcClusterToHit(Text_t *name, Text_t *title) :
    HReconstructor(name,title) {
  setParContainers();
}

HMdcClusterToHit::HMdcClusterToHit(Int_t fSeg1, Int_t fSeg2) {
  HMdcTrackDSet::setMdcClusterToHit(fSeg1,fSeg2);
  setParContainers();
}

HMdcClusterToHit::HMdcClusterToHit(Text_t *name, Text_t *title,
    Int_t fSeg1, Int_t fSeg2) : HReconstructor(name,title) {
  HMdcTrackDSet::setMdcClusterToHit(fSeg1,fSeg2);
  setParContainers();
}

HMdcClusterToHit::~HMdcClusterToHit(void) {
  HMdcTrackDSet::pClusterToHit=0;
}

void HMdcClusterToHit::setParContainers(void) {
  prntSt=kFALSE;
  HMdcTrackDSet::setClusterToHit(this);
  tFillSeg[0]=HMdcTrackDSet::modForSeg1;
  tFillSeg[1]=HMdcTrackDSet::modForSeg2;
}

Bool_t HMdcClusterToHit::init(void) {
  fGetCont=HMdcGetContainers::getObject();
  if( !fGetCont ) return kFALSE;
  fmdc=fGetCont->getMdcDetector();
  fClustCat = gHades->getCurrentEvent()->getCategory(catMdcClus);
  if( !fmdc || !fClustCat) return kFALSE;
  isGeant=HMdcGetContainers::isGeant();
  HMdcDetector* fMdcDet=(HMdcDetector*)(gHades->getSetup()->getDetector("Mdc"));
  if(!fMdcDet) return kFALSE;

  fClusInfCat = HMdcGetContainers::getCatMdcClusInf(kTRUE);
  
  fHitCat = gHades->getCurrentEvent()->getCategory(catMdcHit);
  if (!fHitCat) {
    if(isGeant) fHitCat = fMdcDet->buildMatrixCategory("HMdcHitSim",0.5F);
    else fHitCat = fMdcDet->buildCategory(catMdcHit);
    if (!fHitCat) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catMdcHit,fHitCat,"Mdc");
  }

  if(tFillSeg[0]>=0 || tFillSeg[1]>=2) {
    fSegCat = gHades->getCurrentEvent()->getCategory(catMdcSeg);
    if (!fSegCat) {
      if(isGeant) fSegCat = fMdcDet->buildMatrixCategory("HMdcSegSim",0.5F);
      else fSegCat = fMdcDet->buildCategory(catMdcSeg);
      if (!fSegCat) return kFALSE;
      else gHades->getCurrentEvent()->addCategory(catMdcSeg,fSegCat,"Mdc");
    }
  } else fSegCat=0;
  iter=(HIterator *)fClustCat->MakeIterator("native");
  
  if(tFillSeg[0]>=0) {
    fTrkCandCat = HMdcGetContainers::getCatMdcTrkCand(kTRUE);
    if(!fTrkCandCat) return kFALSE;
  } else fTrkCandCat=0;
  
  fSizesCells=HMdcSizesCells::getObject();

  locHit.set(2,0,0);
  locSeg.set(2,0,0);
  loc.set(1,0);
  locTrkCand.set(1,0);
  if(!prntSt) printStatus();
  return kTRUE;
}

Bool_t HMdcClusterToHit::reinit(void) {
  if(!fSizesCells->initContainer()) return kFALSE;
  if(!fClusInfCat) return kFALSE;
  return kTRUE;
}

Int_t HMdcClusterToHit::execute(void) {
  iter->Reset();
  while ((fClst=(HMdcClus*)iter->Next())!=0) {
    clusIndex=fClustCat->getIndex(fClst);
    Short_t seg=fClst->getIOSeg();
    locTrkCand[0]=fClst->getSec();
    fTrkCand=0;
    if(seg==0) {
      fillContainers();
      Int_t first,last;
      fClst->getIndexRegChilds(first,last);
      for(Int_t ind=first;ind<=last;ind++) {
        fClst=(HMdcClus*)fClustCat->getObject(ind);
        if(fClst==0) {
          Error("execute"," Mismatching in clusters category!");
          continue;
        }
        fillContainers();
      }
    } else if(fClst->getIndexParent()<0) fillContainers();
  }
  return 0;
}

void HMdcClusterToHit::fillContainers(void) {
  Short_t sec=fClst->getSec();
  Short_t modCl=fClst->getMod();
  Short_t seg=fClst->getIOSeg();
  Short_t nMods=0;
  if(fmdc->getModule(sec,seg*2)) nMods++;
  if(fmdc->getModule(sec,seg*2+1)) nMods++;
  locHit[0]=sec;
  Int_t typeClFn=fClst->getTypeClFinder();
  // (modCl&1)^1 = mod 0,2 => 1; mod 1,3 => 0    For modCl<0 nlays=6
  Int_t nlays=(typeClFn==2 && modCl>=0) ? fClst->getNLayersMod((modCl&1)^1):6;
  fSeg = 0;
  hitSegFiller.setClusParam(fClst,HMdcTrackDSet::isCoilOff());
  if(fSegCat && tFillSeg[seg]!=-1 && (typeClFn==0 || nMods==1 || 
      (typeClFn==1 && modCl==tFillSeg[seg]) || (typeClFn==2 && nlays>2))) {
    locSeg[0]=sec;
    locSeg[1]=seg;  
    Int_t indexSeg;
    fSeg = (HMdcSeg*)fSegCat->getNewSlot(locSeg,&indexSeg);
    if(!fSeg) {
      Error("execute","S.%i No HMdcSeg slot available",sec+1);
      return;
    }
    fSeg = (isGeant) ? (HMdcSeg*)(new(fSeg) HMdcSegSim) : new(fSeg) HMdcSeg;
    fSeg->setSec(sec);
    fSeg->setIOSeg(seg);
    fSeg->setChi2(-1.);
    fillSeg();
    if(fTrkCandCat) {
      if(fTrkCand==0) {
        if(seg==0) fillTrkCandISeg(sec,indexSeg);
        else fillTrkCandOSeg(sec,indexSeg);
      } else fillTrkCandOSeg(indexSeg);
    }
  }
  for(Int_t imod=0; imod<2; imod++) {
    Int_t mod=imod+seg*2;
    if(modCl>=0 && modCl!=mod) continue;
    if(!fmdc->getModule(sec,mod)) continue;

    locHit[1]=mod;
    Int_t indexHit;
    fHit = (HMdcHit*)fHitCat->getNewSlot(locHit,&indexHit);
    if(!fHit) {
      Error("execute","S.%i No HMdcHit slot available",sec+1);
      continue;
    }
    if(isGeant) fHit=(HMdcHit*) new(fHit) HMdcHitSim;
    else fHit= new(fHit) HMdcHit;
    fHit->setSecMod(sec,mod);
    if(!fillHit(mod)) return;

    Int_t t[4];
    for(Int_t layer=0; layer<6; layer++) {
      Int_t iLayer=layer+imod*6;
      Int_t firstCell=fClst->get4FirstCells(iLayer,t);
      if(firstCell<0) continue;
      fHit->setSignId(layer,firstCell,t[0],t[1],t[2],t[3]);
      if(fSeg) fSeg->setSignId(iLayer,firstCell,t[0],t[1],t[2],t[3]);
    }
    if(fSeg) fSeg->setHitInd(imod,indexHit);
    if(isGeant) ((HMdcHitSim*)fHit)->calcNTracks();
  }
  if(fSeg) {
    Int_t nMrCls=fClst->getNMergClust();
    if(nMrCls>99) nMrCls=99;
    Int_t flag=(fClst->getNBins()&32767) <<16;
    flag += (fClst->getNDrTimes()*100 + nMrCls) & 65535;
    fSeg->setFlag(flag);
    fHit->setChi2(-1.);
    if(isGeant) ((HMdcSegSim*)fSeg)->calcNTracks();
  }
}

void HMdcClusterToHit::fillTrkCandISeg(Short_t sec,Int_t indexSeg) {
  Int_t index;
  fTrkCand = (HMdcTrkCand*)fTrkCandCat->getNewSlot(locTrkCand,&index);
  if(fTrkCand) fTrkCand = new(fTrkCand) HMdcTrkCand(sec,indexSeg,index);
  else Error("fillTrkCandISeg"," No slot available in catMdcTrkCand");
}

void HMdcClusterToHit::fillTrkCandOSeg(Short_t sec,Int_t indexSeg) {
  Int_t index;
  fTrkCand = (HMdcTrkCand*)fTrkCandCat->getNewSlot(locTrkCand,&index);
  if(fTrkCand) {
    fTrkCand = new(fTrkCand) HMdcTrkCand(sec,indexSeg,index);
    fTrkCand->setSeg2Ind(index);
    fTrkCand->setSeg1Ind(-1);
  } else Error("fillTrkCandOSeg"," No slot available in catMdcTrkCand");
}

void HMdcClusterToHit::fillTrkCandOSeg(Int_t indexSeg) {
//   if(fTrkCand->getSeg2Ind() <0) {
//     fTrkCand->addSeg2Ind(indexSeg);
//   } else {
    Int_t index;
    HMdcTrkCand* newTr=(HMdcTrkCand*)fTrkCandCat->getNewSlot(locTrkCand,&index);
    if(newTr) fTrkCand=new(newTr) HMdcTrkCand(fTrkCand,indexSeg,index);
    else Error("fillTrkCandOSeg"," No slot available in catMdcTrkCand");
//   }
}

void HMdcClusterToHit::fillSeg(void) {
  HMdcSizesCellsSec& fSCSec=(*fSizesCells)[fSeg->getSec()];
  const HGeomVector* targ=(&fSCSec) ? &(fSCSec.getTargetMiddlePoint()) : 0;
  hitSegFiller.fillMdcSegByClus(fSeg,targ);
}

Bool_t HMdcClusterToHit::fillHit(Short_t mod) {
  HMdcSizesCellsMod& fSCMod=(*fSizesCells)[fClst->getSec()][mod];
  if(!&fSCMod) return kFALSE;
  fillClusInf(mod&1);
  hitSegFiller.fillMdcHitByClus(&fSCMod,fHit);
  return kTRUE;
}

void HMdcClusterToHit::fillClusInf(Int_t iMod) {
  Int_t index;
  HMdcClusInf* fClusInf=(HMdcClusInf*)fClusInfCat->getNewSlot(loc,&index);
  if(!fClusInf) {
    Error("fillFitCat","No slot HMdcClusInf available");
    return;
  }
  fClusInf = (isGeant) ? (HMdcClusInf*)(new(fClusInf) HMdcClusInfSim) : 
                                        new(fClusInf) HMdcClusInf;
  fHit->setTrackFinder(1);
  fHit->setClusInfIndex(index);
  fHit->setChi2(-1.);
  fClusInf->setIsNotFitted();
  fClusInf->setLevelClFinding(fClst->getMinCl(iMod));
  Int_t nmods=fClst->getMod();
  fClusInf->setNModInCl( (nmods<0) ? -nmods:1 );
  fClusInf->setClusIndex(clusIndex);
  fClusInf->setNModInFit(0);
  if(iMod==0) {
    fClusInf->setClusSize(fClst->getClusSizeM1());
    fClusInf->setNMergClust(fClst->getNMergClustM1());
    fClusInf->setNDrTimes(fClst->getNDrTimesM1());
    fClusInf->setSigma1(fClst->getSigma1M1());
    fClusInf->setSigma2(fClst->getSigma2M1());
    fClusInf->setAlpha(fClst->getAlphaM1());
  } else {
    fClusInf->setClusSize(fClst->getClusSizeM2());
    fClusInf->setNMergClust(fClst->getNMergClustM2());
    fClusInf->setNDrTimes(fClst->getNDrTimesM2());
    fClusInf->setSigma1(fClst->getSigma1M2());
    fClusInf->setSigma2(fClst->getSigma2M2());
    fClusInf->setAlpha(fClst->getAlphaM2());
    
  }
  if(isGeant) {
    HMdcClusSim* fClSim=(HMdcClusSim*)fClst;
    ((HMdcClusInfSim*)fClusInf)->setTracksList(fClSim->getNTracksMod(iMod),
        fClSim->getListTrM(iMod),fClSim->getNTimesM(iMod));
  }
}

void HMdcClusterToHit::printStatus(void) {
  // prints the parameters to the screen
  char* hFSeg1a ="-1 = don't fill HMdcSeg for segment 1,";
  char* hFSeg1b =" 0 = fill HMdcSeg by cluster in MDC1 if typeClFinder=1";
  char* hFSeg1c ="     and sector has mdc1&2, else by cluster in existing MDC,";
  char* hFSeg1d =" 1 =  -/- in mdc2";
  char* hFSeg2a ="-1 = don't fill HMdcSeg for segment 2,";
  char* hFSeg2b =" 2 = fill HMdcSeg by cluster in MDC3 if typeClFinder=1";
  char* hFSeg2c ="     and sector has mdc3&4 and MagnetOff,";
  char* hFSeg2d ="     else by cluster in existing MDC,";
  char* hFSeg2e =" 3 =  -/- in MDC4";
  printf("--------------------------------------------------------------------------------------------\n");
  printf("HMdcClusterToHitSetup:\n");
  printf("ModForSeg1      = %2i :  %s\n",tFillSeg[0],hFSeg1a);
  printf("                        %s\n",hFSeg1b);
  printf("                        %s\n",hFSeg1c);
  printf("                        %s\n",hFSeg1d);
  printf("ModForSeg2      = %2i :  %s\n",tFillSeg[1],hFSeg2a);
  printf("                        %s\n",hFSeg2b);
  printf("                        %s\n",hFSeg2c);
  printf("                        %s\n",hFSeg2d);
  printf("                        %s\n",hFSeg2e);
  printf ("--------------------------------------------------------------------------------------------\n");
  prntSt=kTRUE;
}

ClassImp(HMdcClusterToHit)
