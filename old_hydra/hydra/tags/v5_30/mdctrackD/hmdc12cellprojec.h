#ifndef HMDC12CELLPROJEC_H
#define HMDC12CELLPROJEC_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TArrayI.h"
#include "hparset.h"
#include "hmdcgeomobj.h"
#include "hmdcsizescells.h"
#include "hgeomtransform.h"
#include "hspecgeompar.h"



class HMdc12CellProjecCell : public TObject {
protected:
  HMdcTrapPlane cellPr;  //Projection of cell (4 poins on the plane) ???
  Bool_t set;
public:
  HMdc12CellProjecCell():set(kFALSE) {;}
  ~HMdc12CellProjecCell() {;}
  HMdcTrapPlane& cellProjec(){return cellPr;}
  Bool_t isCell(){return set;} 
  void setFALSE() {set=kFALSE;}
  void setTRUE() {set=kTRUE;}  
  void clear();
  ClassDef(HMdc12CellProjecCell,0)
}; 

class HMdc12CellProjecLayer : public TObject {
protected:
  // array of pointers of type HMdc12CellProjecCell
  TClonesArray* array;
public:
  HMdc12CellProjecLayer(Int_t sec, Int_t mod, Int_t layer);
  ~HMdc12CellProjecLayer();
  HMdc12CellProjecCell& operator[](Int_t i) {
      return *static_cast<HMdc12CellProjecCell*>((*array)[i]);
  }
  Int_t getSize();
  ClassDef(HMdc12CellProjecLayer,0)
}; 
      

class HMdc12CellProjecMod : public TObject {
protected: 
  // array of pointers of type HMdc12CellProjecLayer 
  TObjArray* array;
public:
  HMdc12CellProjecMod(Int_t sec, Int_t mod);
  ~HMdc12CellProjecMod();
  HMdc12CellProjecLayer& operator[](Int_t i) {
      return *static_cast<HMdc12CellProjecLayer*>((*array)[i]);
    }
  Int_t getSize();
  ClassDef(HMdc12CellProjecMod,0)
};


class HMdc12CellProjecSec : public TObject {
protected:
    TObjArray* array;   // array of pointers of type HMdc12CellProjecMod
    HGeomTransform coordSys;
    HGeomVector targetF;  // First point of target in coor. sys. of sector
    HGeomVector targetL;  // Last point of target in coor. sys. of sector
    Int_t sector;
public:
    HMdc12CellProjecSec(Int_t sec, Int_t maxNMod=2);
    ~HMdc12CellProjecSec();
    void setSys(const HGeomTransform &sys){coordSys.setTransform(sys);}
    void setTargetF(const HGeomVector &vec){targetF=vec;}
    void setTargetL(const HGeomVector &vec){targetL=vec;}
    const HGeomVector& getTargetF(){return targetF;}
    const HGeomVector& getTargetL(){return targetL;}
    const HGeomTransform& getSys(){return coordSys;}
    HMdc12CellProjecMod& operator[](Int_t i) {
      return *static_cast<HMdc12CellProjecMod*>((*array)[i]);
    }
    Int_t getSize();
    ClassDef(HMdc12CellProjecSec,0)
};


class HMdc12CellProjec : public TObject {
protected:
    TObjArray* array;     // array of pointers of type HMdc12CellProjecSec
    Bool_t  isContainer; 
public:
    HMdc12CellProjec(Bool_t coilOff=kFALSE);
    ~HMdc12CellProjec();
    HMdc12CellProjecSec& operator[](Int_t i) {
      return *static_cast<HMdc12CellProjecSec*>((*array)[i]);
    }
    Int_t getSize();
    Bool_t init() {return isContainer;}
    void clear();
    
    ClassDef(HMdc12CellProjec,0)
};

//--------------------------------------------------------

class HMdcCalc12CellProjec : public TObject {
protected:
//    TObjArray* array;     // array of pointers of type HMdcCalcLookUpTbSec 
//    TNtuple* ntuple;      // !Ntuple with all calibration parameters 
    HMdcSizesCells* fSizesCells;
    HMdcGeomPar* fGeomPar;
    HMdcDetector* fMdcDet;
    HSpecGeomPar* fSpecGeomPar;
    
    Double_t Ap,Bp,Cp,Dp;
    HGeomVector pointsProj[16];
    HGeomVector targetPs[2];    //Target
    
public:
    HMdcCalc12CellProjec();
    ~HMdcCalc12CellProjec();
    Bool_t calcMdc12Proj(Int_t nSec, HMdc12CellProjec* f12CellProjec);
    
protected:
    Bool_t calcProjPlain(Int_t sec, HGeomTransform* coordSys,
                                   HMdc12CellProjecSec* p12CPrSec);
    void setParContainers();
    void calcProjPoint(HMdcTrap& cell, HGeomTransform& secsys);
    void calcPoint(HMdcPointPlane& proj, HGeomVector& p1l1, HGeomVector& p2l1, 
                                         HGeomVector& p1l2, HGeomVector& p2l2);
    Double_t calcDist(HGeomVector& p1, HGeomVector& p2);

    ClassDef(HMdcCalc12CellProjec,0)
};

#endif  /*!HMDC12CELLPROJEC_H*/