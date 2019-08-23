#ifndef HKICKCANDIDATEEVALUATOR_H
#define HKICKCANDIDATEEVALUATOR_H

#include <TObject.h>
#include "hgeomvector.h"
#include "hkickplane.h"
#include "hkickcandidate.h"

class HTofGeomPar;
class HSpecGeomPar;
class HKickImpact;
class HTofHit;
class HShowerHit;
class HMdcSeg;
class HMdcGeomPar;

class HKickCandidateEvaluator : public TObject {
protected:
  Double_t fGhi1,fGhi2,fPhi1,fPhi2; //Deflection angles
  Double_t fP; //Fitted momentum
  Double_t fSinGhi,fSinPhi,fPredictedSinPhi; 
  HGeomVector fFarPos,fFarDir,fFarPosErr;
  Double_t fFarPlaneSlope; //Slope of TOF rod or Shower pad
  Double_t fFarPlaneOffset; //Offset of TOF rod or Shower pad
  HKickPlane *fKickPlane; //! Not owner
  HMdcGeomPar *fMdcGeometry; //!Mdc geometry. Not owner
  HTofGeomPar *fTofGeometry; //!TOF geometry. Not owner
  HSpecGeomPar *fSpecGeometry; //!TOF geometry. Not owner
  Double_t fDistanceToFarHit; //dist. from kick impact to far hit.
  Bool_t fUseSimTof; //FIXME: SIM: Use HTofHitSim

  void evaluate(HKickImpact *imp,HKickCandidate &out);
  void transform(HTofHit *data,HGeomVector &r);
  
public:
  HKickCandidateEvaluator(HKickPlane *k) { fKickPlane=k; fUseSimTof=kFALSE;}
  ~HKickCandidateEvaluator(void) {}
  void setTofGeometry(HTofGeomPar *tof) { fTofGeometry=tof; }
  void setSpecGeometry(HSpecGeomPar *spec) { fSpecGeometry=spec; }
  void setMdcGeometry(HMdcGeomPar *mdc) { fMdcGeometry = mdc; }
  virtual void evaluate(HKickImpact *imp,HTofHit *hit,
			HKickCandidate &out);
  virtual void evaluate(HKickImpact *imp,HShowerHit *hit,HKickCandidate &out);
  virtual void evaluate(HKickImpact *imp,HMdcSeg *hit,HKickCandidate &out);
  Bool_t isTofSimulated(void) { return fUseSimTof; }
  void setSimulatedTof(void) { fUseSimTof=kTRUE; }
  ClassDef(HKickCandidateEvaluator,0) //Evaluator of track candidates
};

#endif