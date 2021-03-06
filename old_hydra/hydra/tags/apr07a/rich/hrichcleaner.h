// File: hrichpadclean.h
// ****************************************************************************
//
// HRichPadClean
//
// Author: Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
// Modified: 1999/12/04 by Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
// ----------------------------------------------------------------------------
//
// ****************************************************************************


#ifndef HRICHCLEANER_H
#define HRICHCLEANER_H

#include "TObject.h"
#include "TArrayI.h"
#include "hrichanalysis.h"
#include "hreconstructor.h"

// ****************************************************************************

class HRichCleaner: public HReconstructor {

// ****************************************************************************
protected:
    HCategory *m_pCalCat; //Pointer to the cal data category
    HIterator* fIter; //!
    HParSet *fpAnalysisPar;
    HParSet *fpGeomPar;

    short * pLeftBorder; //! left border of active pad area
    short * pRightBorder; //! dito right side (largest active col in row)

    HRichPadSignal **pPads;//!
    HRichPadSignal * pSectorPads; //points to pads of active sector

    TArrayI iPadActive;
    Int_t maxCols, maxRows,maxPads;
    Int_t maxFiredSectorPads;
    Int_t maxFiredTotalPads;
    Int_t isDirectHit;

    Int_t fPadFired[6];

    Int_t iActiveSector;
private:
    
    Int_t iCount;
   
    void DeletePulse( Int_t border, Int_t col, Int_t row);
    
public:
    
    HRichCleaner(){};
    HRichCleaner(Text_t *name,Text_t *title);
    virtual ~HRichCleaner();
    
    HRichCleaner(const HRichCleaner& source);
    HRichCleaner& operator=(const HRichCleaner& source);
    
    Int_t CleanAlonePad( Int_t border = 3, Int_t lowerThr = 1);
    Int_t CleanHighPulse( Int_t border = 2, Int_t upperThr = 200);
    
    Int_t execute();
    Bool_t init();
    Bool_t reinit();
    Bool_t  initParameters();
    Bool_t finalize(){return kTRUE;};
   
    Int_t IsOut(Int_t x, Int_t y, Int_t dx, Int_t dy);
    Int_t IsOut(Int_t nowPad, Int_t dx, Int_t dy);
    Int_t IsOut(Int_t x, Int_t y) {
	return (!(y >= 0 && y < maxRows &&
		  x >= pLeftBorder[y] && x <= pRightBorder[y]));
    }
    HCategory* getCalCat() { return m_pCalCat; }
    HRichAnalysisPar *getParams() { return ((HRichAnalysisPar*)fpAnalysisPar); }
    HRichGeometryPar* getGeometryPar() { return ((HRichGeometryPar*)fpGeomPar); }
    HParSet* getAnalysisPar() { return fpAnalysisPar; }
    HParSet* getGeomPar() { return fpGeomPar; }
    void setAnalysisPar(HParSet* pPar) { fpAnalysisPar = pPar; }
    void setGeomPar(HParSet* pPar) { fpGeomPar = pPar; }

    void SetActiveSector(Int_t sectornr);
    Int_t GetActiveSector() { return iActiveSector; }
    Int_t SetNextSector();

    ClassDef(HRichCleaner,1)
	
	
};

//============================================================================


#endif // HRICHPADCLEANER_H












