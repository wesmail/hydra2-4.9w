// @(#)$Id: hpidparticle.cc,v 1.3 2002-11-20 17:22:18 jaskula Exp $
//*-- Author : Marcin Jaskula 30/11/2002
//  Modified : Marcin Jaskula 11/11/2002
//             fExpMass added

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HPidParticle                                                               //
//                                                                            //
// Identified particle                                                        //
//                                                                            //
// fMassExp member defines the mass calulated from experimental parameters    //
// momentum and beta. The value of this memeber may have two artifical values://
// -1 (kMassExpInTLV) - means that fMassExp was used to build TLorentzVector  //
// -2 (kMassExpNoPhy) - the experimental beta was not physical one.           //
//                                                                            //
// Proper values of both masses: experimental and ideal can be got by methods://
// getMassExp(), getMassIdeal() which returns propper mass independently      //
// on the mass used to make TLorentzVector                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "hpidparticle.h"
#include "hpidphysicsconstants.h"

#include "hades.h"
#include "hevent.h"
#include "hlinearcategory.h"

#include <TError.h>

#include "piddef.h"

// -----------------------------------------------------------------------------

ClassImp(HPidParticle)

// -----------------------------------------------------------------------------

HPidParticle::HPidParticle(Double_t dM0, Double_t dPx,
                            Double_t dPy, Double_t dPz)
{
// Constuctor with kinematic parameters of the particle
// dM0 - mass (MeV/c^2), dPx, dPy, dPz (MeV/c),

    setDefault();
    SetXYZM(dPx, dPy, dPz, dM0);
}

// -----------------------------------------------------------------------------

HPidParticle::HPidParticle(Short_t nPid, Double_t dPx,
                            Double_t dPy, Double_t dPz)
{
// Constuctor with kinematic parameters of the particle and its id
// Particle id, Px, Py, Pz (MeV/c)

    setDefault();

Double_t dM;

    if((dM = HPidPhysicsConstants::mass(nPid)) < 0.0)
        dM = 0.0;

    SetXYZM(dPx, dPy, dPz, dM);
    setPid(nPid);
}

// -----------------------------------------------------------------------------

HPidParticle::HPidParticle(const Char_t *sPid, Double_t dPx,
                            Double_t dPy, Double_t dPz)
{
// Constuctor with kinematic parameters of the particle and its name
// Particle id, Px, Py, Pz (MeV/c)

    setDefault();

Short_t  nPid = HPidPhysicsConstants::pid(sPid);
Double_t dM;

    if((nPid == 0) || ((dM = HPidPhysicsConstants::mass(nPid)) < 0.0))
        dM = 0.0;

    SetXYZM(dPx, dPy, dPz, dM);
    setPid(nPid);
}

// -----------------------------------------------------------------------------

void HPidParticle::setDefault(void)
{
    nPidCandidate = -1;

    fBetaExp = 0.0f;
    fMassExp = Float_t(kMassExpNoPhy);
    fR       = 0.0f;
    fZ       = 0.0f;
    nSystem  = 0;

    nPid     = 0;
    fPdf     = 0.0f;
    fBayes   = 0.0f;
    fCL      = 0.0f;
}

// -----------------------------------------------------------------------------

void HPidParticle::Clear(Option_t *)
{
// Clear all variables

    setDefault();
    SetXYZT(0.0, 0.0, 0.0, 0.0);
}

// -----------------------------------------------------------------------------

void HPidParticle::print(void) const
{
// Print info about the particle

    printf("Particle : %d \"%s\"\n", nPid, HPidPhysicsConstants::pid(nPid));
    printf("4Momentum: (%8.4f, %8.4f, %8.4f, %8.4f)\nMom. mag.: %8.4f MeV/c\n",
                X(), Y(), Z(), T(), P());
    printf("Theta/Phi: %8.4f  %8.4f\n", thetaDeg(), phiDeg());
    printf("Sect./Sys: %d %d\n", sector(), nSystem);
    printf("R / Z    : %8.4f  %8.4f\n", fR, fZ);
    printf("Beta calc: %8.4f  Exp: %8.4f\n", Beta(), getBetaExp());
    printf("Mass ide.: %8.4f  Exp: %8.4f\n", getMassIdeal(), getMassExp());
    printf("PDF      : %8.4f  Intens: %8.4f  CL: %8.4f\n", fPdf, fBayes, fCL);
}

// -----------------------------------------------------------------------------

HPidCandidate* HPidParticle::getPidCandidate(HCategory *pCat) const
{
// Returns HPidCandidate object corresponding to nPidCandidate (if exists)
// Works when pCat is set or gHades->getCurrentEvent() is accessable

    if(nPidCandidate < 0)
        return NULL;

    if(pCat == NULL)
    {
    HEvent *pEvent;

        if((gHades == NULL) || ((pEvent = gHades->getCurrentEvent()) == NULL))
        {
            Error("getPidCandidate", "Cannot access current event");
            return NULL;
        }

        if((pCat = pEvent->getCategory(catPidCandidate)) == NULL)
            Error("getPidCandidate", "No catPidTrackCand category");
    }

    return (HPidCandidate *) pCat->getObject(nPidCandidate);
}

// -----------------------------------------------------------------------------

HCategory* HPidParticle::buildPidParticleCategory(void)
{
// Static function for making the category HPidParticle

HCategory    *pCat;
HEvent       *pEvent;

    if((gHades == NULL) || ((pEvent = gHades->getCurrentEvent()) == NULL))
    {
        ::Error("HPidParticle::buildPidParticleCategory",
                    "Cannot access current event");

        return NULL;
    }

    if((pCat = pEvent->getCategory(catPidPart)) != NULL)
        return pCat;

    if((pCat = new HLinearCategory("HPidParticle", 1000)) == NULL)
    {
        ::Error("HPidParticle::buildPidParticleCategory",
                    "Cannot create new category");

        return NULL;
    }

    pEvent->addCategory(catPidPart, pCat, "Pid");

    return pCat;
}

// -----------------------------------------------------------------------------

Float_t HPidParticle::getMassExp(void) const
{
// Return experimental value of the mass of the particle.
// Negative value means that the beta was not proper one.

    return (getTLVUseExpMass()) ? (Float_t)M() : fMassExp;
}

// -----------------------------------------------------------------------------

Float_t HPidParticle::getMassIdeal(void) const
{
// Return value of the ideal mass of the particle.

    return (getTLVUseExpMass())
                ? HPidPhysicsConstants::mass(nPid) : (Float_t)M();
}

// -----------------------------------------------------------------------------

void HPidParticle::Streamer(TBuffer &R__b)
{
// Stream an object of class HPidParticle.

UInt_t R__s, R__c;

    if(R__b.IsReading())
    {
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c);

        TLorentzVector::Streamer(R__b);
        R__b >> nPidCandidate;
        R__b >> fBetaExp;

        if(R__v > 1)
            R__b >> fMassExp;
        else
            fMassExp = Float_t(kMassExpNoPhy);

        R__b >> fR;
        R__b >> fZ;
        R__b >> nSystem;
        R__b >> nPid;
        R__b >> fPdf;
        R__b >> fBayes;
        R__b >> fCL;
        R__b.CheckByteCount(R__s, R__c, HPidParticle::IsA());
    }
    else
    {
        R__c = R__b.WriteVersion(HPidParticle::IsA(), kTRUE);
        TLorentzVector::Streamer(R__b);
        R__b << nPidCandidate;
        R__b << fBetaExp;
        R__b << fMassExp;
        R__b << fR;
        R__b << fZ;
        R__b << nSystem;
        R__b << nPid;
        R__b << fPdf;
        R__b << fBayes;
        R__b << fCL;
        R__b.SetByteCount(R__c, kTRUE);
    }
}