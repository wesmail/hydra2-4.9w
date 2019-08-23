// @(#)$Id: hpidcandidate_7_5.h,v 1.1 2004-04-30 16:21:06 christ Exp $

#ifndef HPIDCANDIDATE_7_5_H
#define HPIDCANDIDATE_7_5_H

// -----------------------------------------------------------------------------

#include "hpidcandidate.h"

////////////////////////////////////////////////////////////////////////////////
//
// HPidCandidate_7_5
//
// HPidCandidate for 7 algorithms and 5 particle spices
//
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------

#define NUM_ALGORITHMS 7
#define NUM_PARTICLES  5

// -----------------------------------------------------------------------------

class HPidCandidate_7_5 : public HPidCandidate
{
public:
                        HPidCandidate_7_5(void) { Clear(); };
                       ~HPidCandidate_7_5(void) {};


    // -------------------------------------------------------------------------

private:

    Int_t               aAlgorithms[NUM_ALGORITHMS];
    Short_t             aPidObjIds[NUM_ALGORITHMS];
    Short_t             aParticles[NUM_PARTICLES];
    Float_t             aValues[NUM_ALGORITHMS][NUM_PARTICLES];

    // -------------------------------------------------------------------------

public:

    // -------------------------------------------------------------------------

    virtual UInt_t      getNParticles(void) const   { return NUM_PARTICLES;    }
    virtual UInt_t      getNAlgorithms(void) const  { return NUM_ALGORITHMS;   }

    // -------------------------------------------------------------------------

    virtual Int_t*      getAlgorithms(void) const
                                { return (Int_t *) aAlgorithms;                }

    virtual Short_t*    getPidObjIds(void) const
                                { return (Short_t *) aPidObjIds;               }

    virtual Short_t*    getParticles(void) const
                                { return (Short_t *) aParticles;               }

    virtual Float_t*    getValues(void) const
                                { return (Float_t *) aValues;                  }

    // -------------------------------------------------------------------------

    ClassDef(HPidCandidate_7_5, 1)
};

// -----------------------------------------------------------------------------

#undef NUM_ALGORITHMS
#undef NUM_PARTICLES

#endif // HPIDCANDIDATE_7_5_H