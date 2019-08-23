// $Id: hrichhistofiller.h,v 1.2 2002-09-25 16:19:58 eberl Exp $
// Last update by Thomas Eberl: 02/09/25 18:11:18
//
#ifndef HRICHHISTOFILLER_H
#define HRICHHISTOFILLER_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"
#include "TFile.h"
#include "TH1.h"
class HCategory;
class HIterator;

class HRichHistoFiller : public HReconstructor {
public:

	HRichHistoFiller();
        HRichHistoFiller(Text_t *name,Text_t *title);
	~HRichHistoFiller();

        Bool_t init();
        Bool_t finalize();
        Int_t execute();
        
        HCategory* getHitCat(){return fHitCat;}
        void setHitCat(HCategory* pHitCat){fHitCat = pHitCat;}
    
        ClassDef(HRichHistoFiller,1) //ROOT extension

private:
	HCategory *fHitCat; //Pointer to the Hit data category
        HIterator* fIter;    //!
        TFile *outfile;  //!
        TH1F* richtheta; //!
};

#endif // HRICHHISTOFILLER_H
