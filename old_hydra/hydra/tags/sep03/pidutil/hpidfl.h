// $Id: hpidfl.h,v 1.3 2003-06-22 14:41:28 christ Exp $
// Last update by Thomas Eberl: 03/02/25 14:53:47
//
#ifndef HPIDFL_H
#define HPIDFL_H
#include <TObject.h>
#include <TFile.h>
#include <TObjArray.h>
#include "haddef.h"

class HGeantKine;
class HKickTrack;
class HKickTrackSim;
class HLinearCategory;
class HMatrixCategory;
class HIterator;
class HGeomVector;
class HHitMatchSim;
class HHitMatch;
class HRichHitSim;
class HParSet;
class HCategory;
class HPidFL : public TObject {
public:

	HPidFL();
	~HPidFL();

    static  void calcParticleAnglesT(HGeantKine*,Float_t&,Float_t&);
    static  void calcParticleAnglesKine(HGeantKine*,Float_t&,Float_t&);
    static  Float_t calcOpeningAngleT(Float_t,Float_t,Float_t,Float_t);
    static  Float_t calcOpeningAngleKine(HGeantKine*,HGeantKine*);
    static  Float_t openAngleDeg(Float_t, Float_t, Float_t, Float_t); 
    static  Float_t calcOpeningAngleVectors(HGeomVector&,HGeomVector&);
    static  Float_t calcInvMassKine(HGeantKine*,HGeantKine*);
    static Float_t calcRichWeight(HRichHitSim* r, Int_t ind);
    static void rotateTrackToLab(Int_t,Float_t,Float_t,Float_t &,Float_t &);
    static Float_t getDeflectionAngle(HHitMatch*);
    // ----------------------------------------------------------------
    static  void saveHistos(TFile*,TObjArray*);
    static  void saveCuts(TFile*,TObjArray*);
    // ----------------------------------------------------------------
    static  void dumpKine(HGeantKine*);
    static  void dumpKickTrack(HKickTrack*);
    static  Int_t countLeptonsInKickTrack(HKickTrackSim*);
    static  void dumpCategory(Cat_t);
    // ----------------------------------------------------------------
    static  Bool_t isLepFromPi0DalitzDecay(Int_t,Char_t*);
    static  Bool_t isLepFromPi0DalitzDecay(HGeantKine*,Char_t*);
    static  Bool_t isLepFromPi0GammaConv(Int_t);
    static  Bool_t isLepFromPi0GammaConv(HGeantKine*);
    static  HGeantKine* getSecondGammaFromPionDecay(HGeantKine*);
    static  HGeantKine* getSecondLepFromPionDalitzDecay(HGeantKine*);
    static  HGeantKine* getGammaFromPionDalitzDecay(HGeantKine*);
    static  Bool_t isGammaFromPi0Decay(HGeantKine*);
    static  Bool_t isGamma(HGeantKine*);
    static  HGeantKine* getPionDalitzGamma(HGeantKine*,HLinearCategory*);
    // ----------------------------------------------------------------
    static  Bool_t isLepOnMirror(HGeantKine*);
    static  Bool_t isLepOnMirror(HRichHitSim*,Int_t &);
    static  Bool_t isRingFromLeptonOnMirrorAndMdc(HRichHitSim*);
    static  Bool_t isDoubleLepOnMirror(HRichHitSim*,Int_t &,Int_t &);
    static  Bool_t isLepOnMDC(HGeantKine*);
    static  Int_t  matchRingMirrTracks(HRichHitSim*,Int_t,Int_t);
    // ----------------------------------------------------------------
    static  HGeantKine* findGeantParent(HGeantKine*);
    static  HGeantKine* findGeantParentFast(HGeantKine*);
    static  HGeantKine* getGeantKineObjFromTrkNb(Int_t);
    static  HGeantKine* getGeantKineObjFromTrkNbFast(Int_t);
    static  Int_t getGeantParID(Int_t);
    static  Int_t getGeantParentParID(HGeantKine*);
    static  Int_t getGeantParentParID(Int_t);
    // ----------------------------------------------------------------
    static HParSet* getParamContainer(Char_t*);
    static HCategory* getCategory(Cat_t cat, Bool_t bWarning=kFALSE);
    static Bool_t isNewIndex(Int_t,Int_t*,Int_t);
    static Bool_t isNew2Tuple(Int_t,Int_t,Int_t*,Int_t);
private:
    ClassDef(HPidFL,1)
};

#endif //HPIDFL_H



