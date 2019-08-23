//*-- Author: Alberica Toia
//*-- Modified: Joern Wuestenfeld 07/14/2005
//*-- Version: $Id: hmuEmuleptonsSim.cc,v 1.2 2005-07-15 09:00:24 wuestenf Exp $

#include "hmuEmuleptonsSim.h"

///////////////////////////////////////////////////////////////////////////
//
// HMUEMULeptonsSim
//
///////////////////////////////////////////////////////////////////////////

HMUEMULeptonsSim::HMUEMULeptonsSim(void)
{
  // Default constructor
  //
  // Input parameters:
  //   none
  //
  // Output parameters:
  //   none
  //
  // Return code:
  //  none
  fPhiRich=fThetaRich=fPhiMeta=fThetaMeta=0.0;
  iDetBit=0;
  iSector=0;
  iFlag=0; 
  fMom=0.;
}

HMUEMULeptonsSim::~HMUEMULeptonsSim(void)
{
  // Dstructor
  //
  // Input parameters:
  //   none
  //
  // Output parameters:
  //   none
  //
  // Return code:
  //  none
  fPhiRich=fThetaRich=fPhiMeta=fThetaMeta=0.0;
  iDetBit=0;
  iSector=0;
  iFlag=0;
  fMom=0.;
}

HMUEMUDiLeptonsSim::HMUEMUDiLeptonsSim(void)
{
  // Default constructor
  //
  // Input parameters:
  //   none
  //
  // Output parameters:
  //   none
  //
  // Return code:
  //  none
  iN2=iN1=0;
  fMass=0;
  opangle=0;
}

HMUEMUDiLeptonsSim::~HMUEMUDiLeptonsSim(void)
{
  // Dstructor
  //
  // Input parameters:
  //   none
  //
  // Output parameters:
  //   none
  //
  // Return code:
  //  none
  iN2=iN1=0;
  fMass=0;
  opangle=0;
}

ClassImp(HMUEMULeptonsSim)
ClassImp(HMUEMUDiLeptonsSim)