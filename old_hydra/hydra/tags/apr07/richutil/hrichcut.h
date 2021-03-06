// $Id: hrichcut.h,v 1.6 2003-01-24 15:48:36 eberl Exp $
// Last update by Thomas Eberl: 03/01/11 18:38:19
//
#ifndef HRICHCUT_H
#define HRICHCUT_H
#include <TObject.h>
#include <TFile.h>
#include <TObjArray.h>
#include <TCanvas.h>
class HHitMatch;
class HHitMatchSim;
class HDiHitMatch;
class HDiHitMatchSim;
class HLinearCategory;
class HRichHitSim;
class HRichHit;
class HRichCut : public TObject {
public:

	HRichCut();
	~HRichCut();
    static Bool_t isFullRichMdcMetaTracklet(HHitMatch*);
    static Bool_t isTOFBetween(HHitMatch*,Float_t,Float_t);
    static Bool_t isRICHBetween(HHitMatch*,Float_t,Float_t);
    static Bool_t isInKickTrack(HDiHitMatch*);
    static Bool_t isRMThetaDiff(HHitMatch*,Float_t);
    static Bool_t isRMPhiDiff(HHitMatch*,Float_t);
    static Bool_t isOpangGreaterThan(HDiHitMatch*, Float_t);
    static Bool_t isOpangBetween(HDiHitMatch*, Float_t, Float_t);
    static Bool_t isGEANTPair(HDiHitMatchSim*, HLinearCategory*,Char_t*);
    static Bool_t isNew2Tuple(Int_t,Int_t,Int_t*,Int_t);
    static Bool_t isNewIndex(Int_t,Int_t*,Int_t);
    static Bool_t isUnlikeSignPair(HDiHitMatch*);
    static Bool_t isGoodRing(HRichHitSim*);
    static Bool_t isGoodRing(HRichHit*);
    static Bool_t isGoodRing(HHitMatch*);
    static Bool_t isGoodTrack(HHitMatchSim*);
    static Int_t  NbRingsPerMDCUNLIKEPair(HDiHitMatch*);
private:

    ClassDef(HRichCut,1)
};

#endif //HRICHCUT_H



