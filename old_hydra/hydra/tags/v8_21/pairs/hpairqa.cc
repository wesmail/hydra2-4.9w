// $Id: hpairqa.cc,v 1.15 2008-09-18 13:06:49 halo Exp $
// Author: Thomas.Eberl@ph.tum.de, last modified : 2006-11-24 11:25:50
//
// ---------------------------------------------------------------------------
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//                                                                           
// HPairQA                                                                     
//                                                                           
///////////////////////////////////////////////////////////////////////////////
//
using namespace std;
// ---------------------------------------------------------------------------
#include <iostream>
#include <stdlib.h>
// ---------------------------------------------------------------------------
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include "TObjArray.h"
#include "TTree.h"
// ---------------------------------------------------------------------------
#include <hades.h>
#include "hevent.h"
#include <hiterator.h>
#include <hruntimedb.h>
#include "hgeantdef.h"

// ---------------------------------------------------------------------------
#include "hpair.h"
#include "hpaircutpar.h"
#include "hpairdata.h"
#include "hpairext.h"
#include "hpairgeantdata.h"
#include "hpairfl.h"
#include "hpairhisto.h"
#include "hpairsim.h"
#include "hpairsimext.h"
#include "pairsdef.h"
// ---------------------------------------------------------------------------
#include "hpairqa.h"
// ---------------------------------------------------------------------------
ClassImp(HPairQA)
// ---------------------------------------------------------------------------
HPairQA::HPairQA() 
{
	reset();
	if (gHades) {
	    if(gHades->isReal() &&
	       gHades->getEmbeddingMode() == 0) kIsReal = kTRUE;
            else                                kIsReal = kFALSE;
	}
	else ::Error("ctor","no gHades pointer defined");
}
// ---------------------------------------------------------------------------
HPairQA::HPairQA(const Text_t *name, const Text_t *title) 
{
	reset();
	SetName(name);
	SetTitle(title);
	if (gHades) {
	    if(gHades->isReal() &&
	       gHades->getEmbeddingMode() == 0) kIsReal = kTRUE;
            else                                kIsReal = kFALSE;
	}
	else ::Error("ctor","no gHades pointer defined");
}
// ---------------------------------------------------------------------------
HPairQA::~HPairQA() 
{

}
// ---------------------------------------------------------------------------
Bool_t HPairQA::init() 
{
        HEvent* event = gHades->getCurrentEvent();
	if(event){
	    HCategory* pCatGeantKine = event->getCategory(catGeantKine);
	    if(pCatGeantKine) kIsReal = kFALSE;
	    else              kIsReal = kTRUE;
	}

	if(cOutputFileName.Length() >0)
	{
	  pFile = new TFile(cOutputFileName.Data(),"RECREATE");
	}
	else 
	{
	    ::Error("init","output file length is 0");
	    return kFALSE;
	}


	nNbCutItems = 0;
	nNbNtupleItems = 0;

	HRuntimeDb* rtdb = gHades->getRuntimeDb();
	if (rtdb && nFlagCutQAFilling)
	{
	    HPairCutPar* pC  = (HPairCutPar*)rtdb
		->findContainer("PairCutParameters");
	    if (pC && pC->getInitFlag()==kFALSE) 
	    {
		// make sure that cut object is fully init
		pC->finalizeInit(); 
	    }

	    if (pC) nNbCutItems      = pC->getNbActiveCuts();
	    else 
	    {
	    ::Error("init","param container not found");
	    return kFALSE;
	    }
	}
	if (nFlagNTupleFilling    && ! createNTuple() ) return kFALSE;
	if (nFlagTTreeFilling     && ! createTTree()  ) return kFALSE;
	if (nFlagHistogramFilling) {pHistArray = new TObjArray(5);}
	kIsInit=1;
	return kTRUE;
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::finalize() 
{
	// nothing to write, do not create the file
	if (pNTupleOut==0 && pHistArray==0) return kTRUE;

	if (pNTupleOut && pFile) {pFile->cd();pFile->Write();};
	if (pHistArray) HPairFL::saveHistos(pFile,pHistArray);
	pFile->Close();
	return kTRUE;
}

// ---------------------------------------------------------------------------
void HPairQA::reset(void) 
{
	pFile=0;
	TString s="";
	setOutputFileName(s);
	setNTupleFillingActive(0);
	setTTreeFillingActive(0);
	setHistogramFillingActive(0);
	setCutQAFillingActive(0);
	pCatPair=0;
	pIterPair=0;
	pHistArray=0;
	pNTupleOut=0;
	pTTreeOut=0;
	kIsInit=0;
	kIsReal=0;
	nNbCutItems=0;
	nNbNtupleItems=0;
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::fillData(HIterator* pIter) 
{
	if (nFlagNTupleFilling==0 && nFlagTTreeFilling==0 && nFlagHistogramFilling==0) return kFALSE;

	if (!kIsInit && ! init()) 
	{
	Warning("fillData","init failed");
	return kFALSE;
	}
	
	pIter->Reset();

	if (kIsReal)
	{
	
	HPair* pPair = 0;
	while( (pPair = (HPair*) pIter->Next()) ) 
	{
	    if (strcmp(pPair->IsA()->GetName(),"HPairExt")==0) 
		fillData((HPairExt*)pPair);
	    else
		fillData(pPair);
	}
	}
	else
	{
	HPairSim* pPair = 0;
	while( (pPair = (HPairSim*) pIter->Next()) )
	{
	    fillData(pPair);
	}
	}
	
	return kTRUE;
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::fillData(TObjArray* t) 
{
	if (nFlagNTupleFilling==0 && nFlagTTreeFilling==0 && nFlagHistogramFilling==0) return kFALSE;
	if (!kIsInit && ! init()) 
	{
	Warning("fillData","init failed");
	return kFALSE;
	}
	for (Int_t i=0;i<t->GetLast()+1;i++) 
	{
	if (kIsReal) 
	{
	    if (strcmp(((HPair*)(*t)[i])->IsA()->GetName(),"HPairExt")==0) 
		fillData((HPairExt*)(*t)[i]);
	    else
		fillData((HPair*)(*t)[i]);
	}
	else
	{
	    if (strcmp(((HPair*)(*t)[i])->IsA()->GetName(),"HPairSimExt")==0) 
		fillData((HPairSimExt*)(*t)[i]);
	    else
		fillData((HPairSim*)(*t)[i]);
	}
	}

	return kTRUE;
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::fillData(HPair* p) 
{
	if (nFlagNTupleFilling==0 && nFlagTTreeFilling==0 && nFlagHistogramFilling==0) return kFALSE;
	if (!kIsInit && ! init()) 
	{
	Warning("fillData","init failed");
	return kFALSE;
	}

	HPairData pd(p);
	if (!pd.getInit()) 
	{	
	::Error("fillData(HPair*)","hpairdata not properly init");
	return kFALSE;
	}


	if (nFlagNTupleFilling) fillNtuple(pd);
	if (nFlagHistogramFilling) fillHistograms(pd);

	return kTRUE;
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::fillData(HPairExt* p) 
{
	if (nFlagNTupleFilling==0 && nFlagTTreeFilling==0 && 
	nFlagHistogramFilling==0) return kFALSE;
	if (!kIsInit && ! init()) 
	{
	Warning("fillData(HPairExt*)","init failed");
	return kFALSE;
	}

	HPairData pd(p);
	if (!pd.getInit()) 
	{	
	::Error("fillData(HPairExt*)","hpairdata not properly init");
	return kFALSE;
	}

	if (nFlagNTupleFilling) fillNtuple(pd);
	if (nFlagHistogramFilling) fillHistograms(pd);

	return kTRUE;
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::fillData(HPairSimExt* p) 
{
	if (nFlagNTupleFilling==0 && nFlagTTreeFilling==0 &&
	nFlagHistogramFilling==0) return kFALSE;
	if (!kIsInit && ! init()) 
	{
	Warning("fillData(HPairSimExt*)","init failed");
	return kFALSE;
	}

	HPairGeantData pd(p);
	if (!pd.getGInit()) 
	{	
	::Error("fillData(HPairSimExt*)","hpairgeantdata not properly init");
	return kFALSE;
	}
	//    pd.print();

	if (nFlagNTupleFilling) fillNtuple(pd);
	if (nFlagHistogramFilling) fillHistograms(pd);

	return kTRUE;
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::fillData(HPairSim* p) 
{

	if (nFlagNTupleFilling==0 && nFlagTTreeFilling==0 && nFlagHistogramFilling==0) return kFALSE;
	if (!kIsInit && ! init()) 
	{
	Warning("fillData","init failed");
	return kFALSE;
	}

	HPairGeantData pd(p);
	if (!pd.getGInit()) 
	{	
	::Error("fillData(HPairSim*)","hpairgeantdata not properly init");
	return kFALSE;
	}

	if (nFlagNTupleFilling) fillNtuple(pd);
	if (nFlagHistogramFilling) fillHistograms(pd);

	return kTRUE;
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::fillNtuple(const HPairGeantData& pd) 
{
    // values filled in this ntuple must match the number of
    // values in string for the ntuple definition



    // nb of data items for ntuple: cuts and pair variables
    const Int_t nNbPairDataItems = HPairData::getNbDataItems();
    const Int_t nNbPairSimDataItems = HPairGeantData::getNbSimDataItems();
    const Int_t nNbNtupleItems = nNbPairDataItems + nNbPairSimDataItems +
	nNbCutItems;

    // new array for exp
    Float_t* arr = new Float_t[nNbNtupleItems];
    for(Int_t i = 0; i < nNbNtupleItems; i++) arr[i] = -1000.0; 

    ///////////////////////////////
    // array with pair variables
    Float_t* pdarre = pd.getArray(); 
    //////////////////////////////


    ///////////////////////////////
    // array with sim pair variables
    Float_t* pdarrs = pd.getGArray(); 
    //////////////////////////////


    // copy pair and single values in new array

    for (Int_t i=0; i<HPairData::getNbDataItems();i++) arr[i]=pdarre[i];

    for (Int_t j=0; j<HPairGeantData::getNbSimDataItems();j++) 
	arr[j+HPairData::getNbDataItems()]=pdarrs[j];
	
    //////////////////////////////


    // store requested cuts in array
    if (nNbCutItems>0) 
    {
	HRuntimeDb* rtdb = gHades->getRuntimeDb();
	if (rtdb)
	{   // retrieve cut decision for this object

	    HPairCutPar* pC  = (HPairCutPar*)rtdb->findContainer("PairCutParameters");


	    Int_t nBitFieldFlags = pC->getBitFieldFlags();
	    Int_t nBitFieldPassedFlags = pd.getBitFieldPassedCuts();


	    // LOOP OVER CUTS
	    Int_t nActiveCutNbCounter = 0;
	    for (UInt_t j=2;j<8*sizeof(nBitFieldFlags);j++)
	    {
		// if cut was requested, check if it is passed, fill array
		if (HPairFL::getBit(j,nBitFieldFlags))
		{
		    if (nActiveCutNbCounter+HPairData::getNbDataItems()+
			HPairGeantData::getNbSimDataItems() < 
			nNbNtupleItems)
		    {
			arr[nActiveCutNbCounter+HPairData::getNbDataItems()+
			    HPairGeantData::getNbSimDataItems()] 
			    = (Float_t) HPairFL::getBit(j,nBitFieldPassedFlags);
			nActiveCutNbCounter++;
		    } else {
			Error("HPairQA::fillNtuple(HPairData&)",
			      "ntuple array index out of bounds: %d > %d",
			      nActiveCutNbCounter+HPairData::getNbDataItems()+
			      HPairGeantData::getNbSimDataItems(),
			      nNbNtupleItems);
		    }
		}
	    }
	    
	    // check: nb of found active cuts must match that obtained from
	    // param container
	    if (nActiveCutNbCounter != nNbCutItems)
		Error("HPairQA::fillNtuple(HPairData&)",
		      "Mismatch of number of active cuts: nActiveCutNbCounter: %d != nNbCutItems: %d", 
		      nActiveCutNbCounter, nNbCutItems);

	}
    }

    // fill array into ntuple
    pNTupleOut->Fill(arr);
    delete [] arr;
    delete [] pdarre;
    delete [] pdarrs;
    return kTRUE;
	
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::fillNtuple(const HPairData& pd) 
{
    
    // fill one pair into qa ntuple
    // fill also cut decisions on pair

    // values filled in this ntuple must match the number of
    // values in string for the ntuple definition


    // nb of data items for ntuple: cuts and pair variables

    // nNbPairDataItems: nb of variables for pair 
    const Int_t nNbPairDataItems = HPairData::getNbDataItems();
    // nNbCutItems: max nb of switched on cuts for this run from 
    // pair cut param container
    const Int_t nNbNtupleItems = nNbPairDataItems + nNbCutItems;


    // new array for ntuple filling
    Float_t* arr = new Float_t[nNbNtupleItems];

    for(Int_t k=0; k< nNbNtupleItems; k++) arr[k] = -100;
    ///////////////////////////////
    // array with pair variables
    Float_t* pdarr = pd.getArray(); 
    //////////////////////////////

    // copy values in new array
    for (Int_t i=0; i<HPairData::getNbDataItems();i++) arr[i]=pdarr[i];
	
    // store requested cuts
    if (nNbCutItems>0) 
    {
	HRuntimeDb* rtdb = gHades->getRuntimeDb();
	if (rtdb)
	{   // retrieve cut decision for this pair object
	    HPairCutPar* pC  = (HPairCutPar*)rtdb->findContainer("PairCutParameters");
	    // active cuts - one set per analysis
	    Int_t nBitFieldFlags = pC->getBitFieldFlags();
	    // passed cuts - one set per pair
	    Int_t nBitFieldPassedFlags = pd.getBitFieldPassedCuts();
	    
	    // LOOP OVER CUTS
	    // if cut was requested(=active), check if it is passed, fill array
	    
	    // counter for active cuts
	    Int_t nActiveCutNbCounter = 0;
	    
	    // start counting from 2, bit 0 and 1 are reserved, c.f. hpaircutpar.cc
	    for (UInt_t j=2;j<8*sizeof(nBitFieldFlags);j++)
	    {
		if (HPairFL::getBit(j,nBitFieldFlags)) // if cut was switched on
		{
		    if (nActiveCutNbCounter+HPairData::getNbDataItems()
			< nNbNtupleItems) // check array bounds
		    {
			// requested cut was passed (1) or not (0)
			arr[nActiveCutNbCounter+HPairData::getNbDataItems()] 
			    = (Float_t) HPairFL::getBit(j,nBitFieldPassedFlags);
			// increment active cut counter for indexing
			nActiveCutNbCounter++;
		    }
		    else
		    {
			Error("HPairQA::fillNtuple(HPairData&)",
			      "ntuple array index out of bounds: %d > %d",
			      nActiveCutNbCounter+HPairData::getNbDataItems(),
			      nNbNtupleItems);
		    }
		}
	    }

	    // check: nb of found active cuts must match that obtained from
	    // param container
	    if (nActiveCutNbCounter != nNbCutItems)
		Error("HPairQA::fillNtuple(HPairData&)",
		      "Mismatch of number of active cuts: nActiveCutNbCounter: %d != nNbCutItems: %d", 
		      nActiveCutNbCounter, nNbCutItems);

	}
    }
	
    pNTupleOut->Fill(arr);
    delete [] arr;
    delete [] pdarr;
    return kTRUE;
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::fillHistograms(const HPairData& pd) 
{
	// histogram only objects that were not cut
	if (pd.getIsCutFlag()>0) return kTRUE;

	if (pd.getCharge()==0)  fillTH1F(HPairHisto::getHInvMassBasename(),
				     "unlike",pd.getInvMass());
	if (pd.getCharge()==2)  fillTH1F(HPairHisto::getHInvMassBasename(),
				     "likep",pd.getInvMass());
	if (pd.getCharge()==-2) fillTH1F(HPairHisto::getHInvMassBasename(),
				     "likem",pd.getInvMass());
	if (pd.getCharge()==0)  fillTH1F(HPairHisto::getHOpeningAngleBasename(),
				     "unlike",pd.getOpeningAngleDeg());
	if (pd.getCharge()==2)  fillTH1F(HPairHisto::getHOpeningAngleBasename(),
				     "likep",pd.getOpeningAngleDeg());
	if (pd.getCharge()==-2) fillTH1F(HPairHisto::getHOpeningAngleBasename(),
				     "likem",pd.getOpeningAngleDeg());

	// create background histograms, do rebinning etc
	return kTRUE;
}
// ---------------------------------------------------------------------------
Bool_t HPairQA::createNTuple()
{
	TString sName="Pairs";
	sName.Append(GetName());
	TString sTitle="Pairs";
	sTitle.Append(GetTitle());

	// string with active cut list
	TString* pActiveCutNames = 0;//new TString("");
	HRuntimeDb* rtdb = gHades->getRuntimeDb();
	if (rtdb && nFlagCutQAFilling)
	{
	HPairCutPar* pC  = (HPairCutPar*)rtdb->findContainer("PairCutParameters");
	if (pC) pActiveCutNames = pC->getActiveCutList();
	}
	
	TString sVars = HPairData::getVarNames();

	if (kIsReal==kFALSE)
	{
	TString sSimVars = HPairGeantData::getSimVarNames();
	sVars.Append(":");
	sVars.Append(sSimVars);
	}

	if (pActiveCutNames && pActiveCutNames->Length() > 0) 
	{
	sVars.Append(*pActiveCutNames);
	}

	pNTupleOut = new TNtuple(sName.Data(),sTitle.Data(),
			     sVars.Data());
	cout<<"################################################################"<<endl;
	cout<<"######################## HPairQA ###############################"<<endl;
	cout<<"Name: "<<GetName()<<"\t\tTitle: "<<GetTitle()<<endl;
	cout<<"NTUPLE Variables: "<<sVars.Data()<<endl;
	cout<<"################################################################"<<endl;

	// !!! do not delete pActiveCutNames here !!!
	// it is needed in other places later on
	return kTRUE;
}
// ---------------------------------------------------------------------------

Bool_t HPairQA::createTTree()
{
	TString sName="AllPairs";
	TString sTitle="AllPairs";
	TString sVars="mass:opang:charge";
	pTTreeOut = new TTree(sName.Data(),sTitle.Data());

	return kTRUE;
}

// ---------------------------------------------------------------------------
void HPairQA::fillTH1F(const Char_t *basename,const Char_t *addtohistname,Float_t val,Float_t weight)
{
	TString base(basename);
	base.Append(addtohistname);
	
	TH1F *h = (TH1F*) pHistArray->FindObject(base.Data());
	if (!h)
	{
	h = (TH1F*) HPairHisto::getHistoByBaseName(basename,addtohistname);
	if (h) pHistArray->Add(h);
	else Error("fillTH1F","no histogram created");
	}
	h->Fill(val,weight);
}

// ---------------------------------------------------------------------------
void HPairQA::fillTH2F(const Char_t *basename,const Char_t *addtohistname,Float_t val1,Float_t val2,Float_t weight)
{
	TString base(basename);
	base.Append(addtohistname);
	TH2F *h = (TH2F*) pHistArray->FindObject(base.Data());
	if (!h)
	{
	h = (TH2F*) HPairHisto::getHistoByBaseName(basename,addtohistname);
	if (h) pHistArray->Add(h);
	else Error("fillTH2F","no histogram created");
	}
	h->Fill(val1,val2,weight);
}
