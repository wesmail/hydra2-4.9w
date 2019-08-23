#ifndef PTOOLS__H
#define PTOOLS__H

////////////////////////////////////////////////////////////////////////////////
//
// PTools
// Peter's Tools
// 
//            Author: Peter W. Zumbruch
//           Contact: P.Zumbruch@gsi.de
//           Created: Mar 21, 2002
// Last modification: May 20, 2004
// 
// File: $RCSfile: ptools.h,v $ 
// Version: $Revision: 1.26 $
// Modified by $Author: zumbruch $ on $Date: 2004-07-06 15:57:30 $  
////////////////////////////////////////////////////////////////////////////////

#include "TCanvas.h"
#include "TCutG.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THStack.h"
#include "TLatex.h" 
#include "TLegend.h" 
#include "TObjArray.h"
#include "TPaveText.h"
#include "TPaveStats.h"
#include "TStopwatch.h"
#include "TInterpreter.h"

class TArray;
class TGraphErrors;

class PTools 
{
 protected:
  static TH1*                 copyToTHxD(TH1 * hist);
  static void                 printHeader(const Char_t *titlehead=0, const Char_t *mytextfilename=0, const Int_t nentries=0, TPad * pad = 0);

 public:
  static TPad*                getDividedSubPad(TCanvas  * canv, 
					       Int_t xpads, 
					       Int_t ypads);

  static TPad*                getDividedSubPad(TCanvas  * canv, 
					       Int_t xpads, 
					       Int_t ypads,
					       const Char_t * title);
  
  static TPad*                getDividedSubPad(TCanvas  * canv, 
					       Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename);

  static TPad*                getDividedSubPad(TCanvas  * canv, 
					       Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename, 
					       Int_t nentries);
  
  static TPad*                getDividedSubPad(TCanvas  * canv, 
					       Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename, 
					       Int_t nentries, 
					       TPaveText * info);
  
  static TPad*                getDividedSubPad(TCanvas  * canv,
					       Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename,
					       Int_t nentries, 
					       const Char_t * info);
  
  static TPad*                getDividedSubPad(TCanvas  * canv, 
					       Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename, 
					       Int_t nentries, 
					       TString info);

  static TPad*                getDividedSubPad(Int_t xpads, 
					       Int_t ypads);

  static TPad*                getDividedSubPad(Int_t xpads, 
					       Int_t ypads,
					       const Char_t * title);
  
  static TPad*                getDividedSubPad(Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename);

  static TPad*                getDividedSubPad(Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename, 
					       Int_t nentries);
  
  static TPad*                getDividedSubPad(Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename, 
					       Int_t nentries, 
					       TPaveText * info);
  
  static TPad*                getDividedSubPad(Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename,
					       Int_t nentries, 
					       const Char_t * info);
  
  static TPad*                getDividedSubPad(Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename, 
					       Int_t nentries, 
					       TString info);

  static TPad*                getDividedSubPad(Int_t xpads, Int_t ypads, 
					       Int_t xsize, 
					       Int_t ysize);

  static TPad*                getDividedSubPad(Int_t xpads, Int_t ypads, 
					       Int_t xsize, 
					       Int_t ysize,
					       const Char_t * title);
  
  static TPad*                getDividedSubPad(Int_t xpads, Int_t ypads, 
					       Int_t xsize, 
					       Int_t ysize, 
					       const Char_t * title, 
					       const Char_t * filename);

  static TPad*                getDividedSubPad(Int_t xpads, Int_t ypads, 
					       Int_t xsize, 
					       Int_t ysize, 
					       const Char_t * title, 
					       const Char_t * filename, 
					       Int_t nentries);
  
  static TPad*                getDividedSubPad(Int_t xpads, Int_t ypads, 
					       Int_t xsize, 
					       Int_t ysize, 
					       const Char_t * title, 
					       const Char_t * filename, 
					       Int_t nentries, 
					       TPaveText * info);
  
  static TPad*                getDividedSubPad(Int_t xsize, Int_t ysize,
					       Int_t xpads, 
					       Int_t ypads, 
					       const Char_t * title, 
					       const Char_t * filename,
					       Int_t nentries, 
					       const Char_t * info);
  
  static TPad*                getDividedSubPad(Int_t xpads, Int_t ypads, 
					       Int_t xsize, 
					       Int_t ysize, 
					       const Char_t * title, 
					       const Char_t * filename, 
					       Int_t nentries, 
					       TString info);
  
  static void                 evalStopwatch(TStopwatch *timer, Int_t nevts);
  static void                 setTitleArts( TH1* histogramm, const Char_t *xtitle, const Char_t *ytitle, int color = 4, int type = 42);
  static void                 setTitleArts( TH1* histogramm, const TString xtitle, const TString ytitle, int color = 4, int type = 42);
  static void                 setTitleArts( THStack* histogrammStack, const Char_t *xtitle, const Char_t *ytitle, int type = 42);
  static void                 setTitleArts( THStack* histogrammStack, const TString xtitle, const TString ytitle, int type = 42);
  static void                 setTitleArts( TH1* histogramm, const Char_t *xtitle, const Char_t *ytitle, TString color, int type = 42);
  static void                 setTitleArts( TH1* histogramm, const TString xtitle, const TString ytitle, TString color, int type = 42);

  static TCanvas*             drawCanvas(const Char_t * , const Char_t * , TH1 **, Int_t , Int_t );
  static TCanvas*             draw32Canvas(const Char_t * , const Char_t * , TH1F **);
  static TCanvas*             draw32CanvasTH2(const Char_t * , const Char_t * , TH2F **, const Char_t * option=0);

  static void                 cutEnd(Char_t * , Char_t * );
  static char*                myexec(char * );
  static TString              myexec(TString );
  static void                 mdb(int b=-1);

  static TDirectory *         Mkdir(TDirectory *dirOld, const Char_t *newDirName, Int_t i = -99, Int_t precision = 1); 
  static TDirectory *         Mkdir(TDirectory *dirOld, const TString newDirName, Int_t i = -99, Int_t precision = 1); 

  static void                 getHistogramLimits(TH1* hist, 
						 Int_t &nbinsx  , Axis_t &xmin  , Axis_t& xmax);

  static void                 getHistogramLimits(TH1* hist, 
						 Int_t &nbinsx  , Axis_t &xmin  , Axis_t& xmax, 
						 Int_t &nbinsy  , Axis_t &ymin  , Axis_t& ymax);

  static void                 getHistogramLimits(TH1* hist, 
						 Int_t &nbinsx  , Axis_t &xmin  , Axis_t& xmax, 
						 Int_t &nbinsy  , Axis_t &ymin  , Axis_t& ymax,
						 Int_t &nbinsz  , Axis_t &zmin  , Axis_t& zmax);

  static Int_t                getNFilledBins(TH1 * hist, Double_t min=-1, Double_t max=-1);

  static Bool_t               compareHistogramLimits(TH1* hist1, TH1* hist2, Bool_t exit_on_failure);

  static void                 printHistogramLimits(TH1* hist);
  static void                 printHistogramLimits(TH2* hist);
  static void                 printHistogramLimits(TH3* hist);
  static void                 cleanHistogram(TH1 * hist, Double_t min, Double_t max);
  static void                 cutHistogram(TH2* hist, TCutG *cut, Bool_t complement=kFALSE);
  static void                 cutHistogram(TH2* hist, TObjArray *cutarray, Bool_t complement=kFALSE);

  static void                 prefixToName(TNamed *name, TString prefix);
  //  static void             prefixToName(TNamed *name, Char_t *prefix);
  static void                 prefixToTitle(TNamed *name, TString prefix);
  //  static void             prefixToTitle(TNamed *name, Char_t * prefix);
  static void                 prefixToNameTitle(TNamed *name, TString prefix);
  //  static void             prefixToNameTitle(TNamed *name, Char_t * prefix);
  static void                 postfixToName(TNamed *name, TString postfix);
  //  static void             postfixToName(TNamed *name, Char_t * postfix);
  static void                 postfixToTitle(TNamed *name, TString postfix);
  //  static void             postfixToTitle(TNamed *name, Char_t * postfix);
  static void                 postfixToNameTitle(TNamed *name, TString postfix);
  //  static void             postfixToNameTitle(TNamed *name, Char_t * postfix);

  static void                 suffixToName(TNamed *name, TString suffix){postfixToName(name, suffix); };
  //  static void             suffixToName(TNamed *name, Char_t * suffix){postfixToName(name,  suffix);};
  static void                 suffixToTitle(TNamed *name, TString suffix){postfixToTitle(name, suffix);};
  //  static void             suffixToTitle(TNamed *name, Char_t * suffix){postfixToTitle(name,  suffix);};
  static void                 suffixToNameTitle(TNamed *name, TString suffix){postfixToNameTitle(name, suffix);};
  //  static void             suffixToNameTitle(TNamed *name, Char_t * suffix){postfixToNameTitle(name,  suffix);};

  static void                 plotCounts(TH1 *hist, Int_t color=1);

  static TH1F*                getTH1F(    TString name,     TString title, Int_t xbin, Double_t xmin, Double_t xmax,      TString xtitle,      TString ytitle);
  static TH1F*                getTH1F(const char* name, const char* title, Int_t xbin, Double_t xmin, Double_t xmax,  const char* xtitle, const char*  ytitle)
    {
      return                  getTH1F(   TString(name),    TString(title),       xbin,          xmin,          xmax,     TString(xtitle),     TString(ytitle));
    };
  static TH2F*                getTH2F(TString name, TString title, 
				      Int_t xbin, Double_t xmin, Double_t xmax, 
				      Int_t ybin, Double_t ymin, Double_t ymax, 
				      TString xtitle, TString ytitle, TString ztitle="");
  static TH2F*                getTH2F(const char* name, const char* title, 
				      Int_t xbin, Double_t xmin, Double_t xmax, 
				      Int_t ybin, Double_t ymin, Double_t ymax, 
				      const char* xtitle, const char*  ytitle, const char* ztitle="")
    {
      return getTH2F(TString(name), TString(title), xbin, xmin, xmax, ybin, ymin, ymax, TString(xtitle), TString(ytitle));
    };

  static TH3F*                getTH3F(TString name, TString title, 
				      Int_t xbin, Double_t xmin, Double_t xmax, 
				      Int_t ybin, Double_t ymin, Double_t ymax, 
				      Int_t zbin, Double_t zmin, Double_t zmax, 
				      TString xtitle, TString ytitle, TString ztitle);
  static TH3F*                getTH3F(const char* name, const char* title, 
				      Int_t xbin, Double_t xmin, Double_t xmax, 
				      Int_t ybin, Double_t ymin, Double_t ymax, 
				      Int_t zbin, Double_t zmin, Double_t zmax, 
				      const char* xtitle, const char*  ytitle, const char*  ztitle)
    {
      return getTH3F(TString(name), TString(title), xbin, xmin, xmax, ybin, ymin, ymax, zbin, zmin, zmax, TString(xtitle), TString(ytitle), TString(ztitle));
    };

  static TH1*                 reverseXAxis(TH1* hist);
  static TH2*                 reverseYAxis(TH2* hist);
  static TH3*                 reverseZAxis(TH3* hist);

  static void                 plotPolarAxis(Double_t min = 0.0, Double_t max = 180, Int_t divisions = 10, Float_t direction = 90);
  static void                 plotPolarAxis(TH2* hist, Int_t divisions = 18, Float_t direction=90);
  static void                 plotPolarGrid(Int_t divisions = 18);
  static void                 plotPolarSectorGridHADES(void);
  static void                 saveToPdf(TCanvas* canvas,TString path=".",TString filename = "",Bool_t separate=kFALSE, Int_t selection=-1);
  static void                 saveToPng(TCanvas* canvas,TString path=".",TString filename = "",Bool_t separate=kFALSE, Int_t selection=-1);
  static void                 saveGifToPdf(TCanvas* canvas,TString path=".",TString filename="",Bool_t separate=kFALSE, Int_t selection=-1);
  static void                 saveToPdf(TPad* pad,TString path=".",TString filename = "",Bool_t separate=kFALSE, Int_t selection=-1);
  static void                 saveToPng(TPad* pad,TString path=".",TString filename = "",Bool_t separate=kFALSE, Int_t selection=-1);
  static void                 saveGifToPdf(TPad* pad,TString path=".",TString filename="",Bool_t separate=kFALSE, Int_t selection=-1);

  static Int_t                getColorJM(Int_t i);
  static Int_t                getMarkerJM(Int_t i);
  static void                 setGraph(TGraph* graph,Int_t mycolorindex=-99,Int_t markerstyle=20,Int_t markercolor=1,Float_t markersize=1.,Int_t linecolor=1);
  static void                 setHist(TH1* hist,Int_t mycolorindex=-99,Int_t markerstyle=20,Int_t markercolor=1,Float_t markersize=1.,Int_t linecolor=1);
  static TLegend*             plotLegend(TString pos="right_top",TString Title="No Title",
					 Float_t scaleX=0.9,Float_t scaleY=0.9,
					 Float_t offsetX=0.0,Float_t offsetY=0.0,
					 TString Comment="",
					 Int_t commencolor=1);
  static void                 setLegendEntry(TLegend* legend,TObject* object,TString label,Int_t col,TString opt);
  static TLatex*              plotTopLegend(char* label,Float_t x=-1,Float_t y=-1,Float_t size=0.05,Int_t color=1);
  
  static Double_t             getMeanErrorRangeUser(TH1* hist, Axis_t minX, Axis_t maxX, TString s="center");
  static Double_t             getMeanErrorRange(TH1* hist, Int_t minX, Int_t maxX, TString s="center");
  static Double_t             getMeanRangeUser(TH1* hist, Axis_t minX, Axis_t maxX, TString s="center");
  static Double_t             getMeanRange(TH1* hist, Int_t minX, Int_t maxX, TString s="center");


  static void                 sqrt(TH1* hist);
  static TNamed*              Clone(TNamed *name, TString newname, TString title);

  static TH1D                 copyToTH1D(TH1 * hist);
  static TH2D                 copyToTH2D(TH2 * hist);
  static TH3D                 copyToTH3D(TH3 * hist);

  static TH1D*                createTH1D(TH1 * hist);
  static TH2D*                createTH2D(TH2 * hist);
  static TH3D*                createTH3D(TH3 * hist);

  static void                 importAxisAttributes(TAxis *in, TAxis *out, Bool_t labels);
  static void                 importAxisLabels(TAxis *in, TAxis *out);

  static Int_t                checkHistogramCompatibility(TH1* h1, TH1 *h2);
  static Bool_t               areHistogramsCompatible(TH1* h1, TH1 *h2);
  static Bool_t               areArraysCompatible(const TArray* array1, const TArray* array2);
  static TGraphErrors*        fillGraphErr(TH1 *hist, TString name="", TString position = "center", Bool_t xerrors=kFALSE);
  static TPaveStats*          getHistStats(TH1 *hist, Bool_t quiet=kFALSE);
  static TPaveStats*          getHistStats(TPad* pad, TString name, Bool_t quiet=kFALSE, Bool_t noUpdate=kFALSE);
  static TPaveStats*          setHistStatsXPosNDC(TH1 *hist, Double_t x1, Double_t x2);
  static TPaveStats*          setHistStatsXPosNDC(TPad* pad, TString name, Double_t x1, Double_t x2);
  static TPaveStats*          setHistStatsYPosNDC(TH1 *hist, Double_t y1, Double_t y2);
  static TPaveStats*          setHistStatsYPosNDC(TPad* pad, TString name, Double_t y1, Double_t y2);
  static TPaveStats*          setHistStatsPosNDC(TH1 *hist,  Double_t x1, Double_t x2, Double_t y1, Double_t y2);
  static TPaveStats*          setHistStatsPosNDC(TPad* pad, TString name,  Double_t x1, Double_t x2, Double_t y1, Double_t y2);
  static TPaveStats*          getHistStatsXPosNDC(TH1 *hist, Double_t& x1, Double_t& x2);
  static TPaveStats*          getHistStatsXPosNDC(TPad* pad, TString name, Double_t& x1, Double_t& x2);
  static TPaveStats*          getHistStatsYPosNDC(TH1 *hist, Double_t& y1, Double_t& y2);
  static TPaveStats*          getHistStatsYPosNDC(TPad* pad, TString name, Double_t& y1, Double_t& y2);
  static TPaveStats*          getHistStatsPosNDC(TH1 *hist,  Double_t& x1, Double_t& x2, Double_t& y1, Double_t& y2);
  static TPaveStats*          getHistStatsPosNDC(TPad* pad, TString name,  Double_t& x1, Double_t& x2, Double_t& y1, Double_t& y2);
  static TPaveStats*          setHistStatsTextColor(TH1 *hist, Int_t color);
  static TPaveStats*          setHistStatsTextColor(TPad* pad, TString name, Int_t color);
  static void                 setLatexArts(TLatex * tex, 
					   Double_t x, Double_t y, 
					   Int_t color = 1, Float_t size = 0.1, Int_t align = 11, 
					   Int_t font=62, Float_t angle =0., Bool_t ndc = kFALSE);

  static void                 setLatexArtsNDC(TLatex * tex, 
					      Double_t x, Double_t y, 
					      Int_t color = 1, Float_t size = 0.1, Int_t align = 11, 
					      Int_t font=62, Float_t angle =0.);
  static Bool_t               loadMacro(const char *macro);
  static Bool_t               loadMacro(TString macro);
  static Int_t                getLinearIndex( Int_t x1,    UInt_t x1max, 
					      Int_t x2,    UInt_t x2max=0, 
					      Int_t x3=-1, UInt_t x3max=0, 
					      Int_t x4=-1, UInt_t x4max=0, 
					      Int_t x5=-1, UInt_t x5max=0);

  static Int_t                getLinearIndex( Int_t *x, UInt_t *xmax, UInt_t dim);
  static TLatex*              drawLatex(TString text, Double_t x, Double_t y, 
					Int_t color = 1, Float_t size = 0.1, Int_t align = 11, Int_t font = 62, Float_t angle = 0.);
  static TLatex*              drawLatex(const char* text, Double_t x, Double_t y, 
					Int_t color = 1, Float_t size = 0.1, Int_t align = 11, Int_t font = 62, Float_t angle = 0.);
  static TH1*                 getErrorsOfHistogram(TH1 * hist, Bool_t relativeError=kFALSE);


  ClassDef(PTools,0) // Peter's Tools 
};

#endif