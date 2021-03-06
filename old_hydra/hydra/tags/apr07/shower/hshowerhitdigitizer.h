#ifndef HShowerHitDigitizer_H
#define HShowerHitDigitizer_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"
#include "hshowerdigitizer.h"
#include "hgeantshower.h"
#include "TRandom.h"

class HCategory;

class HShowerHitDigitizer : public HShowerDigitizer {
public:
    HShowerHitDigitizer();
    HShowerHitDigitizer(Text_t *name,Text_t *title, Float_t unit=10.);
   ~HShowerHitDigitizer();

    Bool_t digitize(TObject *pHit);
    HShowerHitDigitizer &operator=(HShowerHitDigitizer &c);

    Bool_t init(void);
    Bool_t finalize(void) {return kTRUE;}

private:
    virtual Bool_t digiHits(HGeantShower *simhit);
    //virtual Float_t calcHit(Float_t fBeta,
    //        const Float_t *pfMeanParams, const Float_t *pfSigmaParams);

    Float_t fUnit;        // Length unit conversion for HGeant
    double  piconst;


    ClassDef(HShowerHitDigitizer,0) // Shower hit digitizer
};

#endif
