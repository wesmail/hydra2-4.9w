// @(#)$Id: hpidcandidate_2_8.h,v 1.1 2003-06-05 14:41:08 jaskula Exp $

#ifndef HPIDCANDIDATE_2_8_H
#define HPIDCANDIDATE_2_8_H

// -----------------------------------------------------------------------------

#include "hpidcandidate.h"

////////////////////////////////////////////////////////////////////////////////
//
// HPidCandidate_2_8
//
// HPidCandidate for 2 algorithms and 8 particle spices
//
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------

#define NUM_ALGORITHMS 2
#define NUM_PARTICLES  8

// -----------------------------------------------------------------------------

class HPidCandidate_2_8 : public HPidCandidate
{
public:
                        HPidCandidate_2_8(void) { Clear(); };
                       ~HPidCandidate_2_8(void) {};


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

    ClassDef(HPidCandidate_2_8, 1)
};

// -----------------------------------------------------------------------------

#undef NUM_ALGORITHMS
#undef NUM_PARTICLES

#endif // HPIDCANDIDATE_2_8_H