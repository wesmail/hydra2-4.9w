//*-- AUTHOR : Anar
//*-- HypCopy : B. Spruck
//*-- Rewritten: 12/06/2006 by B. Spruck

#ifndef HHYP_KINE_FIT_PPPIPPIMPI0_H
#define HHYP_KINE_FIT_PPPIPPIMPI0_H

#include "TMinuit.h"
#include "TObject.h"
#include "hhypKineFit.h"

#include "hgeomtransform.h"

class HHypKineFitPPPipPimPi0:public HHypKineFit
{
protected:
	
public:
// No (De)Constructor needed
	
	Int_t minuitFit(void);
	void CalcDerNum(Float_t *DER11,Double_t *Output);
	void secondIter(Float_t *input,Double_t *foutput,Float_t *errInput,Double_t *out,Float_t *derOut);
	Float_t missing4(HGeomVector &data3,HGeomVector &data4,HGeomVector &data5,HGeomVector &data6);
	Float_t missing4(Double_t *input);
	Float_t missing4(Float_t *input);
	
	ClassDef(HHypKineFitPPPipPimPi0,0);// Fitting Alg for PP->PPPipPimPi0
};

#endif

