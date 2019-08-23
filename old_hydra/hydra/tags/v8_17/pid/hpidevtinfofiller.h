#ifndef HPIDEVTINFOFILLER_H
#define HPIDEVTINFOFILLER_H

//-----------------------------------------------------------------------------
// HPidEvtInfoFiller                                                      
//
// Reconstructor fills event related information. 
// 
// Author: Jacek Otwinowski 18/06/2007
// Revised by Simon Lang, 22/06/2007
//
// $Id: hpidevtinfofiller.h,v 1.3 2007-11-26 09:59:11 kempter Exp $
//
//-----------------------------------------------------------------------------

#include "hreconstructor.h"

class TIterator;
class HCategory;
class HLocation;


//-----------------------------------------------------------------------------
class HPidEvtInfoFiller : public HReconstructor
{
   private:
      Bool_t    bAnalyseSimData; // analysis for simulation events
      HLocation zeroLoc;
  
      // iterators to the input categories
      TIterator *pWallHitIter;   
      TIterator *pRichHitIter;   
      TIterator *pMdcClusIter;   
      TIterator *pMdcSegIter;   
      TIterator *pTofHitIter;   
      TIterator *pTofinoCalIter;
      TIterator *pShowerHitTofIter;   
      TIterator *pPidTrackCandIter;   

      // output category
      HCategory *pPidEvtInfoCat;	

   public:
      HPidEvtInfoFiller();
      HPidEvtInfoFiller(Text_t* name, Text_t* title);
      ~HPidEvtInfoFiller() {}

      // all Hydra reconstuructor classes must have this
      virtual Bool_t init(void);
      virtual Bool_t finalize(void);
      virtual Int_t  execute(void);
      virtual Bool_t reinit(void) { return kTRUE; }

      void clearMembers(void);

   ClassDef(HPidEvtInfoFiller, 0)
};

#endif // HPIDEVTINFOFILLER_H
