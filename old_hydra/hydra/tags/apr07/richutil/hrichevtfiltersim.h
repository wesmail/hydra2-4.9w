// $Id: hrichevtfiltersim.h,v 1.2 2002-09-25 16:19:58 eberl Exp $
// Last update by Thomas Eberl: 02/09/25 18:10:41
//
#ifndef HRICHEVTFILTERSIM_H
#define HRICHEVTFILTERSIM_H

#include "hreconstructor.h"

class HCategory;
class HIterator;
class HHitMatchSim;
class HRichEvtFilterSim : public HReconstructor {
public:

	HRichEvtFilterSim();
        HRichEvtFilterSim(Text_t *name,Text_t *title);
	~HRichEvtFilterSim();

        Bool_t init();
        Bool_t finalize();
        Int_t execute();
        HCategory* getMdcSegCat() { return fMdcSeg ; };
        HCategory* getHitMatchCat(){return pHitMatchCat;};
        HCategory* getHitRichCat(){return pHitRichCat;};
        HCategory* getHitMatchHeaderCat(){return pHitMatchHeaderCat;};
        void resetCounters();
        Bool_t isFilteredEvt(Int_t);
        Bool_t isSkipNextTask(Int_t);
 
       ClassDef(HRichEvtFilterSim,1) //ROOT extension

private:
	HCategory* pHitMatchCat; //!
        HCategory* fRichPID; //!
        HCategory* pHitMatchHeaderCat; //!
        HCategory* fMdcSeg; //!
        HCategory* pHitRichCat; //!
        HIterator* pIterMatchHit; //!
        HIterator* fRichIter;    //!
        HIterator* pIterMatchHitHeader; //!
        HIterator* fMdcSegIter;    //!

    Int_t nCntProcessedEvents;//!
    Int_t nCounterNbRejectedEvts;//!

    
};

#endif // HRICHEVTFILTERSIM_H
