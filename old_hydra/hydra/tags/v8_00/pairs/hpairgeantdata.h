// $Id: hpairgeantdata.h,v 1.8 2006-08-16 15:42:18 halo Exp $
// Author: Thomas.Eberl@ph.tum.de, last modified : 2006-06-28 13:46:32
//
// $Id: hpairgeantdata.h,v 1.8 2006-08-16 15:42:18 halo Exp $
// Author: Thomas.Eberl@ph.tum.de, last modified : 2005-10-26 12:24:33
// Last change by J. Otwinowski, 2005-10-26
// ---------------------------------------------------------------------------
#ifndef HPAIRGEANTDATA_H
#define HPAIRGEANTDATA_H
// ---------------------------------------------------------------------------
#include "hpairdata.h"
// ---------------------------------------------------------------------------
class HPairSim;
class HPairSimExt;
class HPidParticleSim;
// ---------------------------------------------------------------------------


class HPairGeantData : public HPairData {


public:

    HPairGeantData();
    HPairGeantData(HPairSim*);
    HPairGeantData(HPairSimExt*);
    virtual ~HPairGeantData();
    virtual void print(void);
    Float_t* getGArray() const;
    static const TString& getSimVarNames(void);
    static const Int_t getNbSimDataItems(){return SIMDATAITEMS;};
    Int_t getParentTrackNumber1(void){return nGParentTrackNumber1;}
    Int_t getParentTrackNumber2(void){return nGParentTrackNumber2;}
    Int_t getTrackNumber1(void){return nGTrackNumber1;}
    Int_t getTrackNumber2(void){return nGTrackNumber2;}
    Short_t getCommonDetectors1(void){return sGCommonDetectors1;}
    Short_t getCommonDetectors2(void){return sGCommonDetectors2;}
    Bool_t getGInit(){return kIsGInit;};
    void   setGInit(void){kIsGInit = kTRUE;};

protected:

    virtual void reset(void);
    virtual Bool_t init(HPairSim*);
    virtual Bool_t init(HPairSimExt*);
    virtual Bool_t initData(HPairSim*,HPidParticleSim*,HPidParticleSim*);

    Int_t calculateDecayID();

    // init flag
    Bool_t kIsGInit;//!

    // the most common particle

    Int_t nGPID1;
    Int_t nGParentPID1;
    Int_t nGProcessID1;
    Int_t nGmed1;
    Int_t nGGrandParentPID1;
    //    Int_t nGGrandProcessID1;
    TLorentzVector GLorentzVector1;

    Int_t nGPID2;
    Int_t nGParentPID2;
    Int_t nGProcessID2;
    Int_t nGmed2;
    Int_t nGGrandParentPID2;
    //Int_t nGGrandProcessID2;
    TLorentzVector GLorentzVector2;

    Int_t nGTrackNumber1;
    Int_t nGTrackNumber2;
    Int_t nGParentTrackNumber1;
    Int_t nGParentTrackNumber2;
    Int_t nGGrandParentTrackNumber1;
    Int_t nGGrandParentTrackNumber2;

    Float_t fGmom1;
    Float_t fGmom2;
    
    Float_t fGMass;
    Float_t fGOpang;
    Float_t fGRapidity;
    Float_t fGTransverseMomentum;
    Char_t cGCharge;

    Int_t nDecayId; // pi0 Dalitz, etc.

    Short_t sGCommonDetectors1;
    Short_t sGCommonDetectors2;

    Float_t fGvx1, fGvy1, fGvz1, fGvx2, fGvy2, fGvz2;

    Float_t fGgeninfo1, fGgenweight1;
    Float_t fGgeninfo2, fGgenweight2;
    Float_t fGgeninfo1_1, fGgeninfo2_1;
    Float_t fGgeninfo1_2, fGgeninfo2_2;
   
    static const TString sSimVars;
    static const Int_t SIMDATAITEMS;

    enum eDecayIds {PI0DALITZ,CONVERSION,ETADALITZ};
    //                 0          1          2
// ---------------------------------------------------------------------------
    ClassDef(HPairGeantData,1)
};
#endif // HPAIRGEANTDATA_H