#ifndef HPTOOLS__H
#define HPTOOLS__H

////////////////////////////////////////////////////////////////////////////////
//
// HPTools
// Hydra (Peter's) Tools
// 
//            Author: Peter W.Zumbruch
//           Contact: P.Zumbruch@gsi.de
//           Created: Mar 21, 2002
// Last modification: June 24, 2004
// 
// File: $RCSfile: hptools.h,v $ 
// Version: $Revision: 1.21 $
// Modified by $Author: zumbruch $ on $Date: 2004-07-16 11:55:34 $  
////////////////////////////////////////////////////////////////////////////////

#include "hgeomvector.h"
#include "haddef.h"
#include "hades.h"
class TString;
class HDataSource;
class TH1;
class TAxis;
class HParOraIo;

class HPTools 
{
private:
  static Double_t ptinv(Double_t pt,Double_t theta, Double_t mass);
  static Double_t PZf_pty1(Double_t *x, Double_t *par);
 public:
  static Int_t         addSourceList(HDataSource *source, TString inputFile, TString inDir, Int_t refid = -1);
  static void          geantPidAxis(TH1 *,TString axis="X");
  static void          plotGeantAxis(TAxis *axis);
  static void          geantProcessAxis(TH1 *,TString axis="X");
  static void          plotGeantProcessAxis(TAxis *axis);
  static void          plotPolarSectorGrid(void);
  static void          plot_pty_PZ(Double_t mass=139.);
  static void          printAnalysisFileInfo(Int_t startEvt,TString inputDir,TString outFile, TString outDir);
  static Bool_t        mdcDetectorSetup(TString beamtime);
  static Bool_t        mdcDetectorSetup(const char* beamtime){return mdcDetectorSetup(TString(beamtime));};
  static Bool_t        mdcDetectorSetup(Int_t (*mdcMods)[4]);
  static Double_t      getRhoVertex(HGeomVector &vector) { return sqrt(pow(vector.getX(),2.)+
								       pow(vector.getY(),2.));}
  static void          printCounter(Int_t eventiter,Int_t nEvents, Float_t stepsize=0.05);
  static Bool_t        hadesInit();
  static Int_t         hadesEventLoop(Int_t nEvents, Int_t startEvt);
  static bool          usageFileInDirOutDirNevtStart(const char *name);
  static bool          usageFileInDirOutDirNevtStart(TString name)
    {  return usageFileInDirOutDirNevtStart(name.Data());}
  ;
  static int           mainFileInDirOutDirNevtStar(int argc, char **argv,
						   int (*function) (TString outDir, 
								    TString inputDir, 
								    TString inputFile, 
								    Int_t nEvents=-1, 
								    Int_t startEvt=-1));
#ifdef ORACLE_SUPPORT
  static HParOraIo*    getOracleParIo(Bool_t exitOnFailure=kTRUE);
#endif
  static void          setPersistency(Bool_t b, Cat_t *persistentArray, UInt_t nPersistent, UInt_t clean=0);
  static void          setPersistencyClean(Bool_t b, Cat_t *persistentArray, UInt_t nPersistent)
    { setPersistency(b, persistentArray, nPersistent, 1000); }
  static TH1F*         getPairPidHistogramm(Axis_t min = 0, Axis_t max = 0, Int_t extBins=0, Double_t extBinning=1);
  static TString       getGeantPidNameLatex(Int_t index);
  static TString       getGeantProcessNameLatex(Int_t index);

ClassDef(HPTools,0) // Peters Hydra Tools 
};

/*
code fragment for Detector Setup
//   HTofDetector* tof=new HTofDetector;
//   Int_t tofMods[6][22]=
//       { {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//         {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//         {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//         {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//         {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//         {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };
//   for(Int_t i=0;i<6;i++) tof->setModules(i,tofMods[i]);
//   spec->addDetector(tof);

//   HTofinoDetector* tofino=new HTofinoDetector;
//   Int_t tofinoMods[][]  = { {1},{1},{1},{1},{1},{1} };
//   for(Int_t i=0;i<6;i++) tofino->setModules(i,tofinoMods[i]);
//   spec->addDetector(tofino);

//   HShowerDetector* shower=new HShowerDetector;
//   Int_t showerMods[6][3]=
//       { {1,1,1},
//         {1,1,1},
//         {1,1,1},
//         {1,1,1},
//         {1,1,1},
//         {1,1,1} };
//   for(Int_t i=0;i<6;i++) shower->setModules(i,showerMods[i]);
//   spec->addDetector(shower);
*/

#endif
