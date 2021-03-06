
// File: hpidrichtable.h
//
// Author: Tassilo Christ <tassilo.christ@ph.tum.de>
// Last update by Tassilo Christ
//
#ifndef HPIDRICHTABLE_H
#define HPIDRICHTABLE_H
#include "hlocation.h"
#include "hreconstructor.h"
#include "hrichpad.h"
#include "TNtuple.h"
#include "TH1.h" 
#include "TH2.h"
#include "TFile.h"
#include "hrichhitsim.h"
#include "htrackinfo.h"
class HCategory;
class HIterator;
class HKickTrack;
class HPidTrackCand;
class HGeantRichMirror;
class HParSet;
// -----------------------------------------------------------------------------

class HPidRichTable : public HReconstructor
{
public:
    HPidRichTable(void);
    HPidRichTable(const Text_t name[],const  Text_t title[]);
    ~HPidRichTable(void);

    Int_t execute(void);
    Bool_t init(void);
    Bool_t reinit(void);
    Bool_t finalize(void);
    Float_t checkDalitz(HTrackInfo *tOb);
    Float_t checkConv(HTrackInfo *tOb);
    //void calculaProbRich();

 private:
    Int_t evtCount, sector;
    Int_t pmq, padNr, locMax;
    Float_t centroid, avCharge, theta, phi, deltaPhi, deltaTheta;   
    
    HRichPadTab *pPadsPar;
    HParSet* fGeometryPar; //Pointer to geometry parameters
    HCategory  *pCatHitMatch;       // input category
    HIterator  *pHitMatchIterator;        // iterator: input category
    HIterator  *pHitMatchIterator2;        // iterator: input category
    
    TNtuple* pRichHitMatchPropertiesSingle;
    TNtuple* pRichHitMatchPropertiesDouble;
    TNtuple* pRichHitMatchPropertiesFake;

    TFile *out;
    ClassDef(HPidRichTable,0)
};
// -----------------------------------------------------------------------------


#endif /* !HPIDRICHTABLE_H */
