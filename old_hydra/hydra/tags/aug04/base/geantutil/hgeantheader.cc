#include "hgeantheader.h"

//*-- Author : Romain Holzmann, GSI
//*-- Modified : 29/04/99 by Romain Holzmann
//*-- Copyright : GSI, Darmstadt

/////////////////////////////////////////////////////////////////////////////
// HGeantHeader
// 
// HGeant KINE event header
//
/////////////////////////////////////////////////////////////////////////////

ClassImp(HGeantHeader)

HGeantHeader::HGeantHeader(void) {
  eventID = 0;
  beamEnergy = 0.;
  impactParameter = 0.;
  eventPlane = 0.;  
}

HGeantHeader::HGeantHeader(HGeantHeader &aHeader) {
  eventID = aHeader.eventID;
  beamEnergy = aHeader.beamEnergy;
  impactParameter = aHeader.impactParameter;
  eventPlane = aHeader.eventPlane;
}

HGeantHeader::~HGeantHeader(void) {
}

void HGeantHeader::setEventId(Int_t aID) {
  eventID = aID;
}

void HGeantHeader::setBeamEnergy(Float_t aEnergy) {
  beamEnergy = aEnergy;
}

void HGeantHeader::setImpactParameter(Float_t aImpact) {
  impactParameter = aImpact;
}

void HGeantHeader::setEventPlane(Float_t aPlane) {
  eventPlane = aPlane;
}








