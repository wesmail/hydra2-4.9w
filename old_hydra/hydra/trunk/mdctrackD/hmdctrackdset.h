#ifndef HMDCTRACKDSET_H
#define HMDCTRACKDSET_H

#include "hreconstructor.h"

class HMdcTrackFinder;
class HMdcClusterToHit;
class HMdc12Fit;

class HMdcTrackDSet {
  protected: 
    // HMdcTrackFinder param.:
    static HMdcTrackFinder* pTrackFinder; //!
    static Bool_t trFndrIsAdded;      // kTRUE - HMdcTrackFinder added already
    static Bool_t isCoiloff;          // kTRUE=magnetoff,kFALSE=magneton
    static Int_t  clFinderType;       // =0 - segment finder, =1 - mdc finder
    static Int_t  level4[2];          // [seg] levels for 4-layers finding
    static Int_t  level5[2];          // [seg] levels for 5-layers finding
    static Int_t  nLayersInMod[6][4]; // num.of layers in modules
    // HMdcClusterToHit param.:
    static HMdcClusterToHit* pClusterToHit; //!
    static Int_t modForSeg1;     // -1 - don't fill HMdcSeg for segment 1.
                                 //  0 & typeClustFinder=1 & sector_has_mdc1 & 2
                                 //    - fill HMdcSeg by cluster in mdc1
                                 //  1 - -/- in mdc2
    static Int_t modForSeg2;     // -1 - don't fill HMdcSeg for segment 2
                                 //  2 & typeClustFinder=1 & sector_has_mdc3&4
                                 //    & isCoilOff=kTRUE 
                                 //    - fill HMdcSeg by cluster in mdc3
                                 //  3 - -/- in mdc4
    // HMdc12Fit param.:
    static HMdc12Fit* p12Fit;    //!
    static Bool_t useAnotherFit; // another fit MUST be inherited from HMdc12Fit
    static Int_t  fitAuthor;     // 1-Alexander's fit, 2-modified version 1
    static Int_t  distTimeVer;   // 0-calc.time by polin., 1-by J.Market's fun.
    static Int_t  fitType;       // 0-segment fit, 1-mdc fit
    static Bool_t fntuple;       // kTRUE-create and fill catClusFit&catWireFit
    static Bool_t fprint;        // kTRUE-print debug
    static Int_t  useWireOffset; // remove signal time on wire from drift time
    static Int_t  mixFitCut;     // mixed cluster finder cut
    static Int_t  mixClusCut;    // mixed cluster finder cut
    
    static Bool_t isSetCall;

  public:
    static HReconstructor* hMdcTrackFinder(const Char_t* name, const Char_t* title);
    static HReconstructor* hMdcTrackFinder(const Char_t* name, const Char_t* title, 
        Bool_t coil,Int_t type);
    static void useThisTrackFinder(HMdcTrackFinder* trFndr);
    static HReconstructor* hMdcClusterToHit(const Char_t* name, const Char_t* title);
    static HReconstructor* hMdcClusterToHit(const Char_t* name, const Char_t* title,
        Int_t mSeg1, Int_t mSeg2);
    static HReconstructor* hMdc12Fit(const Char_t* name, const Char_t* title);
    static HReconstructor* hMdc12Fit(const Char_t* name, const Char_t* title, 
      Int_t author, Int_t version, Int_t indep, Bool_t nTuple, Bool_t prnt);
    static void setAnotherFit(HMdc12Fit* fit);
    
    static Bool_t isCoilOff(void)                  {return isCoiloff;}
    static Int_t  getClFinderType(void)            {return clFinderType;}
    static void   getTrFnLevels(Int_t& l4s1,Int_t& l5s1,
                                Int_t& l4s2,Int_t& l5s2);
    static const Int_t* getTrFnNLayersInMod(void)  {return &nLayersInMod[0][0];}
    static Int_t  getNLayersInMod(Int_t s,Int_t m) {return nLayersInMod[s][m];}
    static Int_t  getModForSeg1(void)              {return modForSeg1;}
    static Int_t  getModForSeg2(void)              {return modForSeg2;}
    static Int_t  getModForSeg(Int_t sg)           {return sg==0 ? modForSeg1 :
                                                                   modForSeg2;}
    
    static Int_t  getFitAuthor(void)               {return fitAuthor;}
    static Int_t  getFitVersion(void)              {return distTimeVer;}
    static Int_t  getFitType(void)                 {return fitType;}
    static Int_t  getUseWireOffset(void)           {return useWireOffset;}
    static Int_t  getMixFitCut(void)               {return mixFitCut;}
    static Int_t  getMixClusCut(void)              {return mixClusCut;}
    static Int_t  getLevel4(Int_t iseg)            {return level4[iseg];}
    static Int_t  getLevel5(Int_t iseg)            {return level5[iseg];}
    
    static HMdcTrackFinder* getMdcTrackFinder(void)   {return pTrackFinder;}
    static void   clearPTrackFinder(void)             {pTrackFinder = 0;}
    static HMdcClusterToHit* getMdcClusterToHit(void) {return pClusterToHit;}
    static void   clearPClusterToHit(void)            {pClusterToHit = 0;}
    static HMdc12Fit* getMdc12Fit(void)               {return p12Fit;}
    static void   clearP12Fit(void)                   {p12Fit = 0;}
    static Bool_t fNTuple(void)                       {return fntuple;}
    static Bool_t fPrint(void)                        {return fprint;}
    
    static void setMdcTrackFinder(Bool_t coilSt,Int_t type);
    static void setMdcClusterToHit(Int_t mSeg1,Int_t mSeg2);
    static void setMdc12Fit(Int_t author, Int_t version, Int_t indep, 
                            Bool_t nTuple, Bool_t prnt);
            
    static void setCoilStatus(Bool_t stat) {isCoiloff=stat;}
    static void setClFinderType(Int_t type);
    static void setTrFnNLayers(const Int_t *lst);
    static void setTrFnNLayers(Int_t s, Int_t m,Int_t nl);
    static void setTrFnLevel(Int_t l4s1, Int_t l5s1, Int_t l4s2, Int_t l5s2);
    static void setTrFnSegLevel(Int_t seg, Int_t l4, Int_t l5);
    static void setFitAuthor(Int_t author);
    static void setFitVersion(Int_t ver);
    static void setFitType(Int_t type);
    static void setFitOutput(Bool_t nTuple, Bool_t prnt);
    static void setUseWireOffset(Bool_t off);
    static void setMixFitCut(Int_t cut);
    static void setMixClusCut(Int_t cut);
    static void setMixCuts(Int_t cutFit,Int_t cutClus);
    
    static void setTrackFinder(HMdcTrackFinder* tr);
    static void setClusterToHit(HMdcClusterToHit* cl);
    static void set12Fit(HMdc12Fit* fit);

    static void printStatus(void);
    
  protected:
    static void exitSetup(const Char_t* funNm,Int_t npar,const Char_t* use,Int_t inst);
    static void exitSetup(const Char_t* funNm,const Char_t* className);
  private:
    HMdcTrackDSet(void) {}
    virtual ~HMdcTrackDSet(void) {}
    
  ClassDef(HMdcTrackDSet,1) // Set of tasks
};

#endif /* !HMDCTRACKDSET_H */
