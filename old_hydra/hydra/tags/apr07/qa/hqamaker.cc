//*-- Modified : 23/01/2004 A.Sadovski
//*-- Modified : 04/01/2005 Jacek Otwinowski
//*-- Modified : 08/02/2005 A.Sadovski
//*-- Modified : 15/05/2006 J.Pietraszko	
using namespace std;
#include <iostream> 
#include <iomanip>
#include <math.h>
//---- Root includes
#include <TH2.h>
#include <TProfile.h>
//---- Hydra base includes
#include "hades.h"
#include "htree.h"
#include "hcategory.h"
#include "hevent.h"
#include "hiterator.h"
#include "hruntimedb.h" //asd
#include "hmessagemgr.h"
//---- Data container includes
#include "heventheader.h"
#include "hstartcal.h"
#include "hstarthit.h"
#include "htboxchan.h"
#include "hrichcal.h"
#include "hrichhit.h"
#include "hmdcseg.h"
#include "hmdcraw.h"
#include "hmdccal1.h"
#include "hmdccal1sim.h"
#include "hmdchit.h"
#include "hmdctimecut.h" //asd
#include "hmdccutstat.h" //asd
#include "htofhit.h"
#include "htofcluster.h"
#include "htofinocal.h"
#include "hshowerhittof.h"
#include "hkicktrack.h"
#include "hkicktrackB.h"
#include "hkicktrack123B.h"
#include "hsplinetrack.h"
#include "hrktrackB.h"
#include "hbasetrack.h"
#include "hmetamatch.h"
//---- Category definition includes
#include "hstartdef.h"
#include "richdef.h"
#include "hmdcdef.h"
#include "tofdef.h"
#include "tofinodef.h"
#include "showerdef.h"
#include "showertofinodef.h"
#include "kickdef.h"
#include "hmdctrackgdef.h"
#include "hmatchurich.h"
#include "hmatchutof.h"
#include "hmatchushower.h"
#include "hmuleptons.h"
#include "triggerinfodef.h"
//---- QA includes
#include "hqaoutputps.h"
#include "hqamaker.h"
#include "hqahistograms.h"
#include "hqavariations.h"
#include "hqascalers.h"

#include "hpidtrackcand.h"

#define DTHETA 5
#define DPHI 5

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
//*-- Author : Dan Magestro (magestro@gsi.de)
//*-- Modified : 26/04/2002 Joern Wuestenfeld
//*-- Modified : 24/01/2002
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HQAMaker
//
// HQAMaker is a Hydra event-loop task which iterates over events and fills
// histograms and scalers, defined in separate classes.
//
// BEGIN_HTML<b><font color="maroon">For more information, see the <a href="http://www-hades.gsi.de/computing/qa/">QA home page</a>.</font></b> END_HTML
//
//////////////////////////////////////////////////////////////////////////////

Int_t HQAMaker::cutResults[4]={0,0,0,0};

ClassImp(HQAMaker)

HQAMaker::HQAMaker(void) {
// Default constructor
  nEvent = 0;
  nProcessed = 0; //A.Sadovski 21.Oct.2002
  nCountCalEvents = 0;
  hists = 0;
  varHists = 0;
  kFIRST = kTRUE;
  kPDF = kTRUE;

  samplingRate = 1;
  intervalSize = 1000;
  nInterval = 0;
  
  lTrack.set(1,0);
  lMdc.set(2,0,0);
	 
  initIteratorPointers();
  cutStat=0;          //asd
  setParContainers(); //asd
  betaMin=0.3;
}

HQAMaker::HQAMaker(Text_t *n,Text_t *t) : HReconstructor(n,t) {
// Constructor: Name, title
  nEvent = 0;
  nProcessed = 0; //A.Sadovski 21.Oct.2002
  hists = 0;
  varHists = 0;
  kFIRST = kTRUE;
  kPDF = kTRUE;

  samplingRate = 1;
  intervalSize = 1000;
  nInterval = 0;
  isSim=kFALSE;

  lTrack.set(1,0);
  lMdc.set(2,0,0);

  initIteratorPointers();
  cutStat=0;          //asd
  setParContainers(); //asd
  betaMin=0.3;
}

HQAMaker::~HQAMaker(void) { 
  if (cutStat) HMdcCutStat::deleteHMdcCutStat(); 
}

void HQAMaker::initIteratorPointers() {
// Set iterator pointers to 0
  iterStCal = 0;
  iterStHit = 0;
  iterRichCal = 0;
  iterRichHit = 0;
  iterMdcRaw = 0;
  iterMdcCal1 = 0;
  iterMdcHit = 0;
  iterMdcSeg = 0;
  iterShoHit = 0;
  iterShoHitTof = 0;
  iterTofHit = 0;
  iterTofCluster = 0;
  iterTfnCal = 0;
  iterKickTrk = 0;
  iterKickTrk123 = 0;
  iterSplineTrk = 0;
  iterRungeKuttaTrk = 0;
  iterMetaMatch = 0;
  iterPidTrackCand = 0;
  iterHMatchURich = 0;
  iterHMatchUTof = 0;
  iterHMatchUShower = 0;
  iterHMULeptons = 0;
 
 
  catKicTrk = 0;
  catSplineTrk = 0;
  catShoHitTof = 0;
  catTfHit = 0;
  catTfClust = 0;
  catPidTrk = 0;
}

Bool_t HQAMaker::init(void) {
  // Create and book QA components, get iterator pointers if the categories
  // exist in current event

  TH1*       hist          = NULL;
  TIterator* hist_iterator = NULL;
  TString    old_directory = gDirectory->GetPath();

  // create the QA histograms in the root directory
  gROOT->cd();

  hists    = new HQAHistograms();
  varHists = new HQAVariations(intervalSize);
  scalers  = new HQAScalers();

  hists->bookHist();
  varHists->bookHist();
  scalers->bookScalers();

  hist_iterator = hists->getHistList()->MakeIterator();
  while ((hist = (TH1*)hist_iterator->Next()))
  {
     hist->SetDirectory( 0 );
  }
  delete hist_iterator;

  hist_iterator = varHists->getHistList()->MakeIterator();
  while ((hist = (TH1*)hist_iterator->Next()))
  {
     hist->SetDirectory( 0 );
  }
  delete hist_iterator;

  cutStat=HMdcCutStat::getObject();
  if (!cutStat) {
    ERROR_msg(HMessageMgr::DET_QA,"RETRIEVED 0 POINTER FOR HMDCCUTSTAT!");
    exit(1);
  }

  gDirectory->Cd( old_directory.Data() );

  return getIteratorPointers();
}

const void HQAMaker::Print(Option_t *) {
// Print box showing QA summary

  Int_t nHists=0, nVarHists=0;

  if(hists) nHists = hists->getHistList()->GetSize();
  if(varHists) nVarHists = varHists->getHistList()->GetSize();

  SEPERATOR_msg("-",70);
  INFO_msg(10,HMessageMgr::DET_QA,"HQAMaker: HADES data quality assessment");
  gHades->getMsg()->info(10,HMessageMgr::DET_QA,this->GetName(),"%-35s%s","QA statistics","Histogram summary");
  gHades->getMsg()->info(10,HMessageMgr::DET_QA,this->GetName(),"%-35s%s","-------------","-----------------");
  gHades->getMsg()->info(10,HMessageMgr::DET_QA,this->GetName(),"%-23s%-5i%9i%s","Event sampling rate: 1/",samplingRate,nHists," general hists");
  gHades->getMsg()->info(10,HMessageMgr::DET_QA,this->GetName(),"%-15s%-5i%5s%9i%s","Interval size:",intervalSize,"events  ",nVarHists," run var hists");
  gHades->getMsg()->info(10,HMessageMgr::DET_QA,this->GetName(),"%5i%-41s",intervalSize/samplingRate,"QA'd events/interval");
  gHades->getMsg()->info(10,HMessageMgr::DET_QA,this->GetName(),"%s%-8i","Max # events for var hists (=200*intervalSize): ",200*intervalSize);
  INFO_msg(10,HMessageMgr::DET_QA,"*extra events are added to overflow bins*");
  if(kPDF) {
    TString pdfFile = psFile(0,psFile.Last('.'))+".pdf";
    gHades->getMsg()->info(10,HMessageMgr::DET_QA,this->GetName(),"%s%-57s","PDF file:",(const char*)pdfFile);
    INFO_msg(10,HMessageMgr::DET_QA,"*ps2pdf will be used to generate pdf file*");
    TString txtFile = psFile(0,psFile.Last('.'))+".txt";
    gHades->getMsg()->info(10,HMessageMgr::DET_QA,this->GetName(),"%s%-57s","scalers:",(const char*)txtFile);
  }
  else
    gHades->getMsg()->info(10,HMessageMgr::DET_QA,this->GetName(),"%s%-57s","PS file:",(const char*)psFile);
  SEPERATOR_msg("-",70);
}

Bool_t HQAMaker::getIteratorPointers() {
// Search current event for relevant categories... make an iterator
// if category exists

  HEvent *event = gHades->getCurrentEvent();
  if(!event) {
    ERROR_msg(HMessageMgr::DET_QA,"QA maker needs a Hydra event structure");
    return kFALSE;
  }

  // Start
  HCategory *catStCal = event->getCategory(catStartCal);
  if (catStCal)
       iterStCal = (HIterator *)catStCal->MakeIterator("native");
  HCategory *catStHit = event->getCategory(catStartHit);
  if (catStHit)
       iterStHit = (HIterator *)catStHit->MakeIterator("native");

  // Daq Scalers
  HCategory *catDaqScal = event->getCategory(catTBoxChan);
  if (catDaqScal)
       iterHTBoxChan = (HIterator *)catDaqScal->MakeIterator("native");

  // Rich
  HCategory *catRicCal = event->getCategory(catRichCal);
  if (catRicCal)
       iterRichCal = (HIterator *)catRicCal->MakeIterator("native");
  HCategory *catRicHit = event->getCategory(catRichHit);
  if (catRicHit)
       iterRichHit = (HIterator *)catRicHit->MakeIterator("native");

  // Mdc
  HCategory *catMdRaw = event->getCategory(catMdcRaw);
  if (catMdRaw)
       iterMdcRaw = (HIterator *)catMdRaw->MakeIterator("native");
  HCategory *catMdCal1 = event->getCategory(catMdcCal1);
  if (catMdCal1)
       iterMdcCal1 = (HIterator *)catMdCal1->MakeIterator("native");
  if (catMdCal1)(catMdCal1->getClass()!=HMdcCal1Sim::Class())?isSim=kFALSE:isSim=kTRUE;

  //if(!isSim)timecut  =(HMdcTimeCut*)   gHades->getRuntimeDb()->getContainer("MdcTimeCut");

  HCategory *catMdHit = event->getCategory(catMdcHit);
  if (catMdHit)
       iterMdcHit = (HIterator *)catMdHit->MakeIterator("native");

  HCategory *catMdSeg = event->getCategory(catMdcSeg);
  if (catMdSeg)
     iterMdcSeg = (HIterator *)catMdSeg->MakeIterator();



  // Tof
  catTfHit = event->getCategory(catTofHit);
  if (catTfHit)
       iterTofHit = (HIterator *)catTfHit->MakeIterator("native");

  // Tof-Cluster
  catTfClust = event->getCategory(catTofCluster);
  if (catTfClust)
       iterTofCluster = (HIterator *)catTfClust->MakeIterator("native");
  
  // Tofino
  HCategory *catTfnCal = event->getCategory(catTofinoCal);
  if (catTfnCal)
       iterTfnCal = (HIterator *)catTfnCal->MakeIterator("native");

  // Shower
  // HCategory *catShoHit = event->getCategory(catShowerHit);
  // if (catShoHit)
  //     iterShoHit = (HIterator *)catShoHit->MakeIterator("native");
  // catShoHitTof = event->getCategory(catShowerHitTof);
  // if (catShoHitTof)
  //     iterShoHitTof = (HIterator *)catShoHitTof->MakeIterator("native");

  // Shower
  // in case of Simulation MetaMatching is using catShowerHitTofTrack
  // we have to make sure that we use the same cat
  //

  HCategory *catShoHit = event->getCategory(catShowerHit);
  if (catShoHit)
       iterShoHit = (HIterator *)catShoHit->MakeIterator("native");
    //if catShowerHitTofTrack exist (sim data) use this cat
    //if not use catShowerHitTof
  catShoHitTof = event->getCategory(catShowerHitTofTrack);   
  if (catShoHitTof){ // cat catShowerHitTofTrack exist
       iterShoHitTof = (HIterator *)catShoHitTof->MakeIterator("native");
  } 
  else{ // catShowerHitTofTrack does not exist use catShowerHitTof
     catShoHitTof = event->getCategory(catShowerHitTof);   
     if (catShoHitTof){
       iterShoHitTof = (HIterator *)catShoHitTof->MakeIterator("native");
     }
  }



  //// Low res. Kickplane
  //HCategory *catKicTrk = event->getCategory(catKickTrack);
  //if (catKicTrk)
  //     iterKickTrk = (HIterator *)catKicTrk->MakeIterator("native");
  // Low res. Kickplane with output to HBaseTrack [temporary]
  catKicTrk = event->getCategory(catKickTrackB);
  if (catKicTrk){
       //iterKickTrk = (HIterator *)catKicTrk->MakeIterator("native");
       iterKickTrk = (HIterator *)catKicTrk->MakeIterator();
  }
  // Kickplane123
  HCategory *catKicTrk123 = event->getCategory(catKickTrack123B);
  if (catKicTrk123){
       iterKickTrk123 = (HIterator *)catKicTrk123->MakeIterator("native");
  }
  // SplineTrack
  catSplineTrk = event->getCategory(catSplineTrack);
  if (catSplineTrk){
       iterSplineTrk = (HIterator *)catSplineTrk->MakeIterator("native");
  }
  // RungeKuttaTrack
  HCategory *catRungeKuttaTrk = event->getCategory(catRKTrackB);
  if (catRungeKuttaTrk){
       iterRungeKuttaTrk = (HIterator *)catRungeKuttaTrk->MakeIterator("native");
  }

  // MetaMatch
  HCategory *catMeta = event->getCategory(catMetaMatch);
  if (catMeta){
       iterMetaMatch = (HIterator *)catMeta->MakeIterator("native");
  }

   // PidTrackCand
  catPidTrk = event->getCategory(catPidTrackCand);
  if (catPidTrk){
       iterPidTrackCand = (HIterator *)catPidTrk->MakeIterator("native");
  }

  // Trigger
  HCategory *catMURich = event->getCategory(catMatchURich);
  if (catMURich)
       iterHMatchURich = (HIterator *)catMURich->MakeIterator("native");

  HCategory *catMUTof = event->getCategory(catMatchUTof);
  if (catMUTof)
       iterHMatchUTof = (HIterator *)catMUTof->MakeIterator("native");

  HCategory *catMUShower = event->getCategory(catMatchUShower);
  if (catMUShower)
       iterHMatchUShower = (HIterator *)catMUShower->MakeIterator("native");

  HCategory *catMULeptons = event->getCategory(catMatchULeptons);
  if (catMULeptons)
       iterHMULeptons = (HIterator *)catMULeptons->MakeIterator("native");

  return kTRUE;
}

void HQAMaker::setParContainers() { //asd
    // creates the parameter containers MdcLookupGeom/MdcLookupRaw/MdcTimeCut if they
    // exist and adds them to the list of parameter containers in the runtime
    // database
    //lookup   =(HMdcLookupGeom*)gHades->getRuntimeDb()->getContainer("MdcLookupGeom");
    //lookupRaw=(HMdcLookupRaw*) gHades->getRuntimeDb()->getContainer("MdcLookupRaw");
}



Int_t HQAMaker::execute() {
//Fill histograms for events with a frequency of 1/samplingRate

  if(kFIRST) {
    kFIRST = kFALSE;
    buildPSFilename();
    Print();
  }

  //fill daq scaler data. The daq scalers are read out for calibration trigger.
  //Histograms are filled for each calibration event.
  //Int_t eventnb = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
  Int_t eventid = gHades->getCurrentEvent()->getHeader()->getId();
  //cout<<"---Event nb= "<<eventnb<<endl; 
  //if(eventid != 1) cout<<"---Eventid = "<<eventid<<endl; 
  if( eventid == 9) {  // calibration event 
    fillHistDaqScaler();
    nCountCalEvents++;
    //cout<<"---Eventid = "<<eventid<<" --total cal events: "<<nCountCalEvents<<endl;
  } 
     
  if(!(gHades->isCalibration())){ 
    if(nEvent%intervalSize == 0 ) nInterval++;
    if( nEvent%samplingRate == 0 ) {

      Int_t eventSize = gHades->getCurrentEvent()->getHeader()->getEventSize();
      varHists->evtHeader_eventSize_Var->Fill( nEvent,eventSize );

      fillHistStart();
      fillHistRich();
      fillHistMdc();
      fillHistTof();
      fillHistTofino();
      fillHistShower();
      fillHistMatching();
      fillHistKick();
      fillHistKick123();
      fillHistSpline();
      fillHistRungeKutta();
      fillHistTrig();
      fillHistRichMDC();
      fillHistRichKick();
      fillHistPid();
      nProcessed++;
    }
  } // fill detector qa plots only for real events
  nEvent++;
  return 0;
}

void HQAMaker::buildPSFilename() {
// Build Postscript file name, either from user-supplied name or from DST filename

     if(fDir.IsNull() && (!psFile.Contains("/")))
       WARNING_msg(HMessageMgr::DET_QA,10,"dir not specified, QA output written to present dir");
     //Warning("makePS","dir not specified, QA output written to present dir");

     // If psFile is not given, build PS filename from gHades filename
     if(psFile.IsNull())
     {
       dstFile = gHades->getOutputFile()->GetName();
       psFile  = dstFile( dstFile.Last('/')+1,dstFile.Last('.')-dstFile.Last('/')-1 )+".ps";
     }

     // Add "/" at end of directory
     if( !fDir.IsNull()) {
       if(!(fDir[fDir.Length()-1] == '/')) fDir += "/";
     }
     if(!psFile.Contains("/")) psFile = fDir + psFile +".ps";
}

Bool_t HQAMaker::finalize(void) {
// Finalize histograms and fill scalers

  if(nProcessed == 0){
    cout<<"--------------------------------------------------------- \n";  
    cout<<"-- WARNING: QA MAKER, Number of processed events is 0. -- \n";  
    cout<<"------ qa pdf document will not be created -------------- \n";  
    cout<<"--------------------------------------------------------- \n";  
    return 1;
  }else { 
	
    fillScalers();

    finalizeHistStart();
    finalizeHistDaqScaler(); 
    finalizeHistRich();
    finalizeHistMdc();
    finalizeHistTof();
    finalizeHistTofino();
    finalizeHistShower();
    finalizeHistKick();
    finalizeHistTrig();
    finalizeHistRichMDC();
    finalizeHistRichKick();
    finalizeHistPid();

    return makePS();
  }
}

Bool_t HQAMaker::makePS() {
// After finalizing, generate PS file using HQAOutputPS

  HQAOutputPS *outPS = new HQAOutputPS(psFile);

  outPS->setDSTFileName(dstFile);
  outPS->setStats(nEvent,nProcessed);
  outPS->generatePDF(kPDF);

  outPS->makeHist( hists->getHistList());
  outPS->writeHist(hists->getHistList(), varHists->getHistList(), psFile ); //asd

  outPS->setNHistPerPage(3);
  outPS->makeHist(varHists->getHistList());

  outPS->makeText(scalers->getScalerList());
  outPS->saveScal(scalers->getScalerList(), psFile); //asd

  //outPS->write(varHists->getHistList());

  delete outPS;
  return kTRUE;
}

void HQAMaker::fillHistStart() {
  Int_t triggerBit = (gHades->getCurrentEvent()->getHeader()->getTBit()) & 0xff;
  HStartCal *stCal = 0;
  HStartHit *stHit = 0;
  HTofHit *tofHit = 0;
  Float_t  tof_corr = 0;
  Float_t dist = 0;
  
  //get the TBit (from 1 to 8) from triggerBit (int)
  for(int ii=0; ii<9 ; ii++) {
    if((triggerBit & (1<<ii))!=0) {
      hists->stLatchHist->Fill(ii);
    }
  }

  fillGlobalVertex(); // fill vertex information
  
  if(iterStCal) {
    iterStCal->Reset();
    while((stCal=(HStartCal*) iterStCal->Next())!=0) {
      if(stCal->getModule()==0 || stCal->getModule()==3){ 
        varHists->stCal_meanStrip_Var->Fill(nEvent,stCal->getStrip());
        hists->stCal_strip->Fill(stCal->getStrip());
        hists->stCal_tof->Fill(stCal->getTime());
      }
    }
  }

  if(iterStHit) {
    iterStHit->Reset();
    while((stHit=(HStartHit*) iterStHit->Next())!=0) {
      if(stHit->getModule()==0) {
      //fill StartTime vs StartStrip 2-dim
        hists->stHit_vs_stStrip->Fill(stHit->getStrip(),stHit->getTime());
        //get the TBit (from 1 to 8) from triggerBit (int)
        for(int ii=0; ii<9 ; ii++) {
          if((triggerBit & (1<<ii))!=0) {
            hists->stHit_vs_stStrip_Trigg[ii+1]->Fill(stHit->getStrip(),stHit->getTime());
          }
        }

        if(iterTofHit) {
          iterTofHit->Reset();
          while((tofHit=(HTofHit*) iterTofHit->Next())!=0) {
            tofHit->getDistance(dist);
            tof_corr = tofHit->getTof() + (2100.-dist)/300.;

            hists->stHit_tof_vs_startstripe->Fill(stHit->getStrip(),tof_corr);
          }
        }
      } //St module 0
    } 
  } // iterStHit
}

void HQAMaker::fillHistRich() {

  HRichCal *richCal = 0;
  Int_t nDataObjs;

  if(iterRichCal) {
    iterRichCal->Reset();
    nDataObjs = ((TObjArray*)iterRichCal->GetCollection())->GetEntries();
    varHists->richCal_n_Var->Fill(nEvent,nDataObjs);
    while((richCal=(HRichCal*) iterRichCal->Next())!=0) {

      hists->richCal_row->Fill( richCal->getRow() );
      hists->richCal_column->Fill( richCal->getCol() );
      hists->richCal_nSec->Fill( richCal->getSector() );

    }
  }

  HRichHit *richHit = 0;
  Float_t theta,phi,r2d=57.29578;

  if(iterRichHit) {
    iterRichHit->Reset();
    nDataObjs = ((TObjArray*)iterRichHit->GetCollection())->GetEntries();
    varHists->richHit_n_Var->Fill(nEvent,nDataObjs);
    while((richHit=(HRichHit*) iterRichHit->Next())!=0) {

      theta = richHit->getTheta();
      phi = richHit->getPhi();

      hists->richHit_theta->Fill(theta);
      hists->richHit_phi->Fill(phi);
      hists->richHit_nSec->Fill( richHit->getSector() );

      hists->richHit_scat->Fill(sin(theta/r2d)*sin((phi-90)/r2d),sin(theta/r2d)*cos((phi-90)/r2d));

      hists->richHit_radius->Fill( richHit->getRadius() );
      hists->richHit_centroid->Fill( richHit->getCentroid() );
      hists->richHit_chargeAmpl->Fill( richHit->getRingAmplitude() );
      hists->richHit_ringCol->Fill( richHit->getRingCenterX() );
      hists->richHit_ringRow->Fill( richHit->getRingCenterY() );
      hists->richHit_ringLocMax4->Fill( richHit->getRingLocalMax4() );
      hists->richHit_houTraVsPatMat->Fill( richHit->getRingHouTra() , richHit->getRingPatMat() );
      hists->richHit_patMatVsTheta->Fill( richHit->getRingPatMat() , theta );
      hists->richHit_houTraVsTheta->Fill( richHit->getRingHouTra() , theta );
      hists->richHit_chargeAmplVsTheta->Fill( richHit->getRingAmplitude() , theta );
      hists->richHit_radiusVsTheta->Fill( richHit->getRadius() , theta );

    }
  }
}

void HQAMaker::fillHistMdc() {

  HMdcRaw *mdcRaw = 0;

  if(iterMdcRaw) {
    iterMdcRaw->Reset();
    while((mdcRaw=(HMdcRaw*) iterMdcRaw->Next())!=0) {

      Int_t sector = mdcRaw->getSector();
      Int_t module = mdcRaw->getModule();

      // mdcRaw: sector vs. mbo mult.
      Int_t mbo = mdcRaw->getMbo();
      if(module == 0) hists->mdcRaw_mboVsSector_m0->Fill(mbo,sector);
      else if(module == 1) hists->mdcRaw_mboVsSector_m1->Fill(mbo,sector);
      else if(module == 2) hists->mdcRaw_mboVsSector_m2->Fill(mbo,sector);
      else if(module == 3) hists->mdcRaw_mboVsSector_m3->Fill(mbo,sector);

    }
  }

  HMdcCal1 *mdcCal1 = 0;

  if(iterMdcCal1) {
    iterMdcCal1->Reset();
    Int_t n[4][6];
    for(Int_t i=0;i<4;i++) for(Int_t j=0;j<6;j++) n[i][j] = 0;
    while((mdcCal1=(HMdcCal1*) iterMdcCal1->Next())!=0) {

      Int_t sector = mdcCal1->getSector();
      Int_t module = mdcCal1->getModule();
      n[module][sector]++;

      Float_t time1 = mdcCal1->getTime1();
      Float_t time2 = mdcCal1->getTime2();

      // mdcCal1: t2-t1 vs t1
      hists->mdcCal1_t2mt1_vs_t1[sector][module]->Fill(time2-time1, time1);
      hists->mdcCal1_t2mt1_vs_t1_plane[  module]->Fill(time2-time1, time1);

      //mdcCal1: t2-t1
      hists->mdcCal1_t2mt1[sector][module]->Fill(time2-time1);

      //mdcCal1: t1
      hists->mdcCal1_t1[sector][module]->Fill(time1);

      // mdcCal1: sector vs. t1
      if(     module == 0) hists->mdcCal1_time1VsSector_m0->Fill(time1,sector);
      else if(module == 1) hists->mdcCal1_time1VsSector_m1->Fill(time1,sector);
      else if(module == 2) hists->mdcCal1_time1VsSector_m2->Fill(time1,sector);
      else if(module == 3) hists->mdcCal1_time1VsSector_m3->Fill(time1,sector);

      if(     module == 0) varHists->mdcCal1_time1_m0_Var->Fill(nEvent,time1);
      else if(module == 1) varHists->mdcCal1_time1_m1_Var->Fill(nEvent,time1);
      else if(module == 2) varHists->mdcCal1_time1_m2_Var->Fill(nEvent,time1);
      else if(module == 3) varHists->mdcCal1_time1_m3_Var->Fill(nEvent,time1);

      // mdcCal1: module vs. t2-t1
      if(     module == 0) varHists->mdcCal1_time2m1_m0_Var->Fill(nEvent,time2-time1);
      else if(module == 1) varHists->mdcCal1_time2m1_m1_Var->Fill(nEvent,time2-time1);
      else if(module == 2) varHists->mdcCal1_time2m1_m2_Var->Fill(nEvent,time2-time1);
      else if(module == 3) varHists->mdcCal1_time2m1_m3_Var->Fill(nEvent,time2-time1);

      // mdcCal1: sector vs. t.a.t.
      Float_t tat = mdcCal1->getTime2()-mdcCal1->getTime1();
      if(module == 0) hists->mdcCal1_tatVsSector_m0->Fill(tat,sector);
      else if(module == 1) hists->mdcCal1_tatVsSector_m1->Fill(tat,sector);
      else if(module == 2) hists->mdcCal1_tatVsSector_m2->Fill(tat,sector);
      else if(module == 3) hists->mdcCal1_tatVsSector_m3->Fill(tat,sector);

    }
    for(Int_t sector=0;sector<6;sector++) {
      hists->mdcCal1_nVsSector_m0->Fill(n[0][sector],sector);
      hists->mdcCal1_nVsSector_m1->Fill(n[1][sector],sector);
      hists->mdcCal1_nVsSector_m2->Fill(n[2][sector],sector);
      hists->mdcCal1_nVsSector_m3->Fill(n[3][sector],sector);
    }
  }

  HMdcHit *mdcHit = 0;

  if(iterMdcHit) {
    iterMdcHit->Reset();
    while((mdcHit=(HMdcHit*) iterMdcHit->Next())!=0) {

      Int_t sector,module;
      Float_t x1,y1,x2,y2,angle;
      mdcHit->getSecMod(sector,module);

      // Scatter plots (made difficult by chamber coordinate system)
      x1 =-mdcHit->getX(); //minus is becouse our coordinate system has Ox looking to the left, when looking from a target to one of MDC planes
      y1 = mdcHit->getY();

      if(module == 0) y1 = (y1 + 650)*.65;
      if(module == 1) y1 = (y1 + 950)*.58;
      if(module == 2) y1 = (y1 + 1800)*.58;
      if(module == 3) y1 = (y1 + 1900)*.68;

      angle = ((float) sector)*60./57.2967;
      x2 =  x1*cos(angle) + y1*sin(angle);
      y2 = -x1*sin(angle) + y1*cos(angle);

      if(module == 0) hists->mdcHit_scat_m0->Fill(x2,y2);
      if(module == 1) hists->mdcHit_scat_m1->Fill(x2,y2);
      if(module == 2) hists->mdcHit_scat_m2->Fill(x2,y2);
      if(module == 3) hists->mdcHit_scat_m3->Fill(x2,y2);

    }
  }

}


//***---------------------------------------------------------


void HQAMaker::finalizeHistMdc(){
  Int_t c[7];
  char buf[500];

  //DEBUG_msg(1,HMessageMgr::DET_QA,"MYOUTPUT----MYOUTPUT----MYOUTPUT----MYOUTPUT----");

  Int_t ib;
  for(Int_t m=0; m<4; m++) { //better to have first all sectors of the same module
     for(Int_t s=0; s<6; s++) {
          cutStat->getCal1StatCut(s,m,&c[0],&c[1],&c[2],&c[3],&c[4],&c[5],&c[6]);
          sprintf(buf,"%i %i %i %i %i %i %i %i %i \n",
          s,m,
          c[0],c[1],c[2],c[3],c[4],c[5],c[6] 
          );
	  DEBUG_msg(1,HMessageMgr::DET_QA,buf);

          ib = m*6+s+1;  //before I have an error here: ib = m*4+s+1; which caused lost of sector0 Dubna && Rossendorf for nov01-gen3 DST

          // 0   1    2     3     4          5        6 
          //t1  t2  t1t2  wire  t1+t12  cutall  allhits

          hists->mdcCutCounts[0]->SetBinContent(ib, c[6]); //allhits
          hists->mdcCutCounts[1]->SetBinContent(ib, c[0]); //t1
          hists->mdcCutCounts[2]->SetBinContent(ib, c[1]); //t2
          hists->mdcCutCounts[3]->SetBinContent(ib, c[2]); //t1t2
          hists->mdcCutCounts[4]->SetBinContent(ib, c[3]); //wire
          hists->mdcCutCounts[5]->SetBinContent(ib, c[4]); //t1+t12
          hists->mdcCutCounts[6]->SetBinContent(ib, c[5]); //cutall
     }
  }
  DEBUG_msg(1,HMessageMgr::DET_QA,buf);

  if (cutStat) cutStat->printParam();
}


void HQAMaker::fillHistTof() {

  HTofHit *tofHit = 0;
  Float_t x,y,z,theta,phi;
  Int_t nDataObjs;

  if(iterTofHit) {
    iterTofHit->Reset();
    nDataObjs = ((TObjArray*)iterTofHit->GetCollection())->GetEntries();
    hists->tofHit_n->Fill(nDataObjs);
    varHists->tofHit_n_Var->Fill(nEvent,nDataObjs);

    while((tofHit=(HTofHit*) iterTofHit->Next())!=0) {

      tofHit->getTheta(theta);
      tofHit->getPhi(phi);

      hists->tofHit_nSec->Fill( tofHit->getSector() );
      tofHit->getXYZLab(x,y,z);
      hists->tofHit_scat->Fill(-x,y);
      hists->tofHit_tof->Fill(tofHit->getTof());
      hists->tofHit_phi->Fill(phi);
      hists->tofHit_theta->Fill(theta);
    }
  }
}

void HQAMaker::fillHistTofino() {

  //recently introduced for Tofino
  Int_t iIndex;
  Int_t tofinoraw_sec;
  Int_t tofinoraw_cell;
  

  HTofinoCal *tfnCal = 0;

  if(iterTfnCal) {
    iterTfnCal->Reset();
    hists->tfnCal_n->Fill( ((TObjArray*)iterTfnCal->GetCollection())->GetEntries() );

    while((tfnCal=(HTofinoCal*) iterTfnCal->Next())!=0) {
      hists->tfnCal_tdc->Fill(tfnCal->getTime());

      //recently
      tofinoraw_sec  = tfnCal->getSector();
      tofinoraw_cell = tfnCal->getCell(); 

      iIndex = tofinoraw_sec*4+tofinoraw_cell;
      hists->histProfTofino->Fill((Float_t)(iIndex+1));

      //fprofT[iIndex+1]++;
      //cout <<"iIndex,fprofT = " <<iIndex <<" " <<fprofT[iIndex] <<endl;

    }
    //hists->histProfTofino->SetContent(fprofT);
    //Float_t tt = hists->histProfTofino->GetMaximum();
    //for(Int_t iii=0; iii<27; iii++){
    //   cout <<"iii,fprofT = " <<iii <<" " <<fprofT[iii] <<endl;
    //}
    //cout <<"tt = " <<tt <<endl;
    //for(Int_t iii=0; iii<27; iii++) { 
    //   if(tt > 0.) fprofT[iii] = fprofT[iii]/tt;
    //}
    //hists->histProfTofino->SetContent(fprofT);
  }
}



void HQAMaker::fillHistShower() {

  Float_t sum,dummy1,phi,theta,charge,x,y,z;
  Int_t row,col,mod,sec;
  Int_t nDataObjs;

  HShowerHit *shoHit = 0;

  if(iterShoHit) {
    iterShoHit->Reset();
    nDataObjs = ((TObjArray*)iterShoHit->GetCollection())->GetEntries();
    varHists->shoHit_n_Var->Fill(nEvent,nDataObjs);

    while((shoHit=(HShowerHit*) iterShoHit->Next())!=0) {

      row = shoHit->getRow();
      col = shoHit->getCol();
      mod = shoHit->getModule();
      sec = shoHit->getSector();
      charge = shoHit->getCharge();
      hists->shoHit_nSec->Fill( sec );
      hists->shoHit_sectorVsModule->Fill( sec,mod );
      hists->shoHit_nRow->Fill(row);  
      hists->shoHit_nCol->Fill(col);  
      if((sum=shoHit->getSum(mod))>0.0){
              hists->shoHitSums[sec][mod]->Fill(sum);
      }
      if(mod==0) hists->shoHit_chargeVsSector_m0->Fill(charge,sec);
      else if(mod==1) hists->shoHit_chargeVsSector_m1->Fill(charge,sec);
      else if(mod==2) hists->shoHit_chargeVsSector_m2->Fill(charge,sec);

      if(mod==0) hists->shoHit_rowVsSector_m0->Fill(row,sec);
      else if(mod==1) hists->shoHit_rowVsSector_m1->Fill(row,sec);
      else if(mod==2) hists->shoHit_rowVsSector_m2->Fill(row,sec);

      if(mod==0) hists->shoHit_colVsSector_m0->Fill(col,sec);
      else if(mod==1) hists->shoHit_colVsSector_m1->Fill(col,sec);
      else if(mod==2) hists->shoHit_colVsSector_m2->Fill(col,sec);

      shoHit->getSphereCoord(&dummy1,&phi,&theta);
      hists->shoHit_phi->Fill( phi );
      hists->shoHit_theta->Fill( theta );

      shoHit->getLabXYZ(&x,&y,&z);
      hists->shoHit_scat->Fill(-x,y);

    }
  }

  HShowerHitTof *shoHitTof = 0;

  if(iterShoHitTof) {
    iterShoHitTof->Reset();
    nDataObjs = ((TObjArray*)iterShoHitTof->GetCollection())->GetEntries();
    varHists->shoHitTof_n_Var->Fill(nEvent,nDataObjs);

    while((shoHitTof=(HShowerHitTof*) iterShoHitTof->Next())!=0) {

      sec = shoHitTof->getSector();
      hists->shoHitTof_nSec->Fill(sec);

      shoHitTof->getLabXYZ(&x,&y,&z);
      hists->shoHitTof_scat->Fill(-x,y);

      //sorry for two Tofino related histogram to be filled here :-)
      hists->histMultTofinoM->Fill( shoHitTof->getTofinoMult() );

      hists->histTofinoTof->Fill( shoHitTof->getTof() );
    }
  }

}

void HQAMaker::fillHistKick() {

  Float_t mass,theta,phi,r2d=57.29578;
  Int_t system,charge,sec,nDataObjs;

  HKickTrackB *kickTrk = 0;

  if(iterKickTrk) {
    iterKickTrk->Reset();
    nDataObjs = ((TObjArray*)iterKickTrk->GetCollection())->GetEntries();
    varHists->kickTrack_n_Var->Fill(nEvent,nDataObjs);

    while((kickTrk=(HKickTrackB*) iterKickTrk->Next())!=0) {

      if( kickTrk->getBeta()>betaMin){
        system =      kickTrk->getSystem();
        charge =      kickTrk->getPolarity(); //getCharge();
        mass   = sqrt(fabs(kickTrk->getMass2()));   //getMass());
        sec    =      kickTrk->getSector();

        hists->kickTrack_pullVsSector->Fill( kickTrk->getPull(),sec );
        hists->kickTrack_massCharge->Fill( mass*charge );

        theta = kickTrk->getTheta()*r2d;
        phi   = kickTrk->getPhi()*r2d+60.*sec; if(phi>360.) phi-=360.;

        hists->kickTrack_scat->Fill(sin(theta/r2d)*sin((phi-90)/r2d),sin(theta/r2d)*cos((phi-90)/r2d));
        hists->kickTrack_nSec->Fill( kickTrk->getSector() );

        if(system==0){hists->trackingKick12M_sys0[sec]->Fill( mass*charge );}
        if(system==1){hists->trackingKick12M_sys1[sec]->Fill( mass*charge );}
      }
    }
  }
}

void HQAMaker::fillHistKick123() {

  Float_t mass,theta,phi,r2d=57.29578;
  Int_t system,charge,sec,nDataObjs;

  HKickTrack123B *kickTrk123 = 0;

  if(iterKickTrk123) {
    //cout << "QA:Spile" << endl;
    iterKickTrk123->Reset();
    nDataObjs = ((TObjArray*)iterKickTrk123->GetCollection())->GetEntries();
    //varHists->kickTrack_n_Var->Fill(nEvent,nDataObjs);

    while((kickTrk123=(HKickTrack123B*) iterKickTrk123->Next())!=0) {
      //cout << "Fill" << endl;

      if( kickTrk123->getBeta()>betaMin){
        system =      kickTrk123->getSystem();
        charge =      kickTrk123->getPolarity();
        mass   = sqrt(kickTrk123->getMass2());
        sec    =      kickTrk123->getSector();

        hists->kickTrack123_massCharge->Fill( mass*charge );

        theta = kickTrk123->getTheta()*r2d;
        phi   = kickTrk123->getPhi()*r2d+60.*sec; if(phi>360.) phi-=360.;
        hists->kickTrack123_scat->Fill(sin(theta/r2d)*sin((phi-90)/r2d),sin(theta/r2d)*cos((phi-90)/r2d));

        //hists->kickTrack123_nSec->Fill( kickTrk->getSector() );
        
        if(system==0){hists->trackingKick123_sys0[sec]->Fill( mass*charge );}
        if(system==1){hists->trackingKick123_sys1[sec]->Fill( mass*charge );}
      }
    }
  }
}



void HQAMaker::fillHistSpline() {

  Float_t mass,theta,phi,r2d=57.29578;
  Int_t system,charge,sec,nDataObjs;

  HSplineTrack *splineTrk = 0;

  //cout << "QA:Spile" << endl;

  if(iterSplineTrk) {
    //cout << "QA:Spline" << endl;
    iterSplineTrk->Reset();
    nDataObjs = ((TObjArray*)iterSplineTrk->GetCollection())->GetEntries();
    //varHists->kickTrack_n_Var->Fill(nEvent,nDataObjs);

    while((splineTrk=(HSplineTrack*) iterSplineTrk->Next())!=0) {
      //cout << "Fill" << endl;

      if( splineTrk->getBeta()>betaMin){
        if( splineTrk->getQSpline()<0.0) continue;
        system =      splineTrk->getSystem();
        charge =      splineTrk->getPolarity();
        mass   =      splineTrk->getMass2();
        sec    =      splineTrk->getSector();

        if(mass>=0)
        {
   	  mass   = sqrt(mass);
   	  hists->splineTrack_massCharge->Fill( mass*charge );
        }

        theta = splineTrk->getTheta()*r2d;
        phi   = splineTrk->getPhi()*r2d+60.*sec; if(phi>360.) phi-=360.;
        hists->splineTrack_scat->Fill(sin(theta/r2d)*sin((phi-90)/r2d),sin(theta/r2d)*cos((phi-90)/r2d));
        
        if(system==0){hists->trackingSpline_sys0[sec]->Fill( mass*charge );}
        if(system==1){hists->trackingSpline_sys1[sec]->Fill( mass*charge );}
      }
    }
  }
}


void HQAMaker::fillHistRungeKutta() {

  Float_t mass,theta,phi,r2d=57.29578;
  Int_t system,charge,sec,nDataObjs;

  HRKTrackB *rungeKuttaTrk = 0;

  //cout << "QA:RungeKutta" << endl;

  if(iterRungeKuttaTrk) {
    //cout << "QA:RungeKutta" << endl;
    iterRungeKuttaTrk->Reset();
    nDataObjs = ((TObjArray*)iterRungeKuttaTrk->GetCollection())->GetEntries();

    while((rungeKuttaTrk=(HRKTrackB*) iterRungeKuttaTrk->Next())!=0) {
      //cout << "Fill" << endl;

      if( rungeKuttaTrk->getBeta()>betaMin){
        if( rungeKuttaTrk->getChiq()>100000.) continue;

        system =      rungeKuttaTrk->getSystem();
        charge =      rungeKuttaTrk->getPolarity();
        mass   =      rungeKuttaTrk->getMass2();
        sec    =      rungeKuttaTrk->getSector();

        if(mass>=0)
        {
   	  mass   = sqrt(mass);
   	  hists->rungeKuttaTrack_massCharge->Fill( mass*charge );
        }

        theta = rungeKuttaTrk->getTheta()*r2d;
        phi   = rungeKuttaTrk->getPhi()*r2d+60.*sec; if(phi>360.) phi-=360.;
        hists->rungeKuttaTrack_scat->Fill(sin(theta/r2d)*sin((phi-90)/r2d),sin(theta/r2d)*cos((phi-90)/r2d));
        
        if(system==0){hists->trackingRK_sys0[sec]->Fill( mass*charge );}
        if(system==1){hists->trackingRK_sys1[sec]->Fill( mass*charge );}
      } 
   }
  }
}


void HQAMaker::finalizeHistTof() {
  if(hists->tofHit_nSec->GetEntries() > 0)
    {
      if(nProcessed == 0)
	hists->tofHit_nSec->Scale(1.);
      else
	hists->tofHit_nSec->Scale(1./nProcessed);
    }
}

void HQAMaker::finalizeHistStart() {
  if(hists->stLatchHist->GetEntries() > 0){
    if(nProcessed == 0) hists->stLatchHist->Scale(1.);
    else hists->stLatchHist->Scale(1./nProcessed);
  }
}


void HQAMaker::finalizeHistDaqScaler() {
  // show only not empty bins:
  //  ( (TAxis *)h->GetXaxis()) ->SetRange(0,nCountCalEvents) 
  //---- set Xaxis range for  Daq Scalers


  for(Int_t ii = 0; ii < 8; ii++){
    ((TAxis *)(hists->histReduce[ii]) ->GetXaxis()) ->SetRange(0,nCountCalEvents);
    ((TAxis *)(hists->histInput[ii]) ->GetXaxis()) ->SetRange(0,nCountCalEvents);
    ((TAxis *)(hists->histStartScaler[ii]) ->GetXaxis()) ->SetRange(0,nCountCalEvents);
    ((TAxis *)(hists->histVetoScaler[ii]) ->GetXaxis()) ->SetRange(0,nCountCalEvents);
    // and request that Y axis should start from 0 
    hists->histReduce[ii] -> SetMinimum(0.0);
    hists->histInput[ii] -> SetMinimum(0.0);
    hists->histStartScaler[ii] -> SetMinimum(0.0);
    hists->histVetoScaler[ii] -> SetMinimum(0.0); 

  }
}    



void HQAMaker::finalizeHistShower() {
  if(hists->shoHit_nSec->GetEntries() > 0)
    {
      if(nProcessed == 0)
	hists->shoHit_nSec->Scale(1.);
      else{
		hists->shoHit_nSec->Scale(1./nProcessed);
		hists->shoHit_nCol->Scale(1./nProcessed);
		hists->shoHit_nRow->Scale(1./nProcessed);
		hists->shoHit_sectorVsModule->Scale(1./nProcessed);
		hists->shoHit_scat->Scale(1./nProcessed);
		hists->shoHit_theta->Scale(1./nProcessed);
		hists->shoHit_phi->Scale(1./nProcessed);
    	for(Int_t s=0;s<6;s++){
			for(Int_t m=0;m<3;m++){
				hists->shoHitSums[s][m]->Scale(1./nProcessed);
			}
		}
		}
	}
  // hists->shoHit_avgCharge_m0->Divide(hists->shoHit_rowVsCol_m0);
}


void HQAMaker::fillHistTrig() {
  Int_t nDataObjs;
  
  Float_t cosTab[6];
  Float_t sinTab[6];
  Int_t transX, transY, sector;
  Int_t shiftX, shiftY;
  Int_t tempX, tempY;
  Float_t rAngle;

  shiftX = -48; shiftY = 14;
  for (Int_t cSec=0;cSec<6;cSec++) {
    rAngle = cSec*60.*TMath::Pi()/180.;
    sinTab[cSec] = sin(rAngle);
    cosTab[cSec] = cos(rAngle);
  }



  HMatchURich *MURich = 0;
  if(iterHMatchURich) {
    iterHMatchURich->Reset();
    nDataObjs = ((TObjArray*)iterHMatchURich->GetCollection())->GetEntries();
    varHists->trigRich_n_Var->Fill(nEvent,nDataObjs);

    hists->trigRich_nhits->Fill(nDataObjs);
    while((MURich=(HMatchURich*) iterHMatchURich->Next())!=0) {
      hists->trigRich_nSec->Fill(MURich->getSegmentId());
      hists->trigRich_theta->Fill(MURich->getTheta() );
      hists->trigRich_phi->Fill(MURich->getPhi() );

      sector = MURich->getSegmentId();
      tempX = MURich->getRow() + shiftX;
      tempY = MURich->getColumn() + shiftY;
      transX = (Int_t) (tempX*cosTab[sector]+tempY*sinTab[sector]);  
      transY = (Int_t) (-tempX*sinTab[sector]+tempY*cosTab[sector]);

      hists->trigRich_rowVsCol->Fill(transX,transY);
    }
  }

  HMatchUTof *MUTof = 0;
  if(iterHMatchUTof) {
    iterHMatchUTof->Reset();
    nDataObjs = ((TObjArray*)iterHMatchUTof->GetCollection())->GetEntries();
    varHists->trigTof_n_Var->Fill(nEvent,nDataObjs);
    
    hists->trigTof_nhits->Fill(nDataObjs);
    while((MUTof=(HMatchUTof*) iterHMatchUTof->Next())!=0) {
      hists->trigTof_nSec->Fill(MUTof->getSector());
      hists->trigTof_theta->Fill(MUTof->getTheta() );
      hists->trigTof_phi->Fill(MUTof->getPhi() );
      hists->trigTof_time->Fill(MUTof->getTime() );
      //hists->trigTof_xVsy->Fill();
    }
  }

  shiftX = -16; shiftY = 24;

  HMatchUShower *MUShower = 0;
  if(iterHMatchUShower) {
    iterHMatchUShower->Reset();
    nDataObjs = ((TObjArray*)iterHMatchUShower->GetCollection())->GetEntries();
    varHists->trigShower_n_Var->Fill(nEvent,nDataObjs);

    hists->trigShower_nhits->Fill(nDataObjs);
    while((MUShower=(HMatchUShower*) iterHMatchUShower->Next())!=0) {
      hists->trigShower_nSec->Fill(MUShower->getSector());
      hists->trigShower_theta->Fill(MUShower->getTheta() );
      hists->trigShower_phi->Fill(MUShower->getPhi() );

      sector = MUShower->getSector();
      tempX = MUShower->getColumn() + shiftX;
      tempY = MUShower->getRow() + shiftY;
      transX = (Int_t) (tempX*cosTab[sector]+tempY*sinTab[sector]);  
      transY = (Int_t) (-tempX*sinTab[sector]+tempY*cosTab[sector]);

      hists->trigShower_rowVsCol->Fill(transX,transY);

    }
  }

  HMULeptons *MULeptons = 0;
  Int_t nTofLeptons = 0;
  Int_t nShowerLeptons = 0;
  Float_t phi=0.;

  if(iterHMULeptons) {
    iterHMULeptons->Reset();
    nDataObjs = ((TObjArray*)iterHMULeptons->GetCollection())->GetEntries();

    hists->trigMULep_nhits->Fill(nDataObjs);
    while((MULeptons=(HMULeptons*) iterHMULeptons->Next())!=0) {

      hists->trigMULep_momVsDTh->Fill( fabs(MULeptons->getThetaRich() - MULeptons->getThetaMeta()),
				       (MULeptons->getMom() * (1-2*MULeptons->getFlag() ) ) );


      phi = MULeptons->getPhiRich();
      if (MULeptons->getSector() < 5)  phi -= 60*(MULeptons->getSector()+1);

      hists->trigMULep_phiVsDPh->Fill(phi,(MULeptons->getPhiRich()-MULeptons->getPhiMeta()));
      if ( MULeptons->getDetBit() ) {
      hists->trigMULep_SnSec->Fill(MULeptons->getSector());
	nShowerLeptons++;
      } else {
	hists->trigMULep_TnSec->Fill(MULeptons->getSector());
	nTofLeptons++;
      }
    }
    varHists->trigTlepton_n_Var->Fill(nEvent,nTofLeptons);
    varHists->trigSlepton_n_Var->Fill(nEvent,nShowerLeptons);
  }
}

void HQAMaker::fillHistRichMDC()
{
HMdcSeg *mdcSeg = 0;
HRichHit *richHit = 0;

Int_t   richSec;
Float_t mdcPhi, mdcTheta;
Float_t richPhi, richTheta;

Int_t iSec;
Float_t rad2deg = 180.0/TMath::Pi();

    for(iSec = 0; iSec < 6; iSec++)
    {
        lMdc[0] = iSec;
        lMdc[1] = 0;

        if(iterMdcSeg)
        {
            iterMdcSeg->Reset();
            iterMdcSeg->gotoLocation(lMdc);

            while((mdcSeg=(HMdcSeg*) iterMdcSeg->Next())!=0)
            {
                mdcPhi = mdcSeg->getPhi()*rad2deg ;
                if(iSec != 5) mdcPhi = mdcPhi+iSec*60;
                else mdcPhi = mdcPhi-60.;

                mdcTheta = mdcSeg->getTheta()*rad2deg;

                if(iterRichHit)
                {
                    iterRichHit->Reset();
                    while((richHit=(HRichHit*) iterRichHit->Next())!=0)
                    {
                        richTheta = richHit->getTheta();
                        richPhi = richHit->getPhi();
                        richSec = richHit->getSector();

                        if( richSec != iSec) continue;

                        hists->richmdc_dtheta[richSec]->Fill(richTheta-mdcTheta);
                        hists->richmdc_dphi[richSec]->Fill((richPhi-mdcPhi)*sin(mdcTheta/rad2deg));

                        if(fabs((richPhi-mdcPhi)*sin(mdcTheta/rad2deg)) > DPHI) continue;
                        if(fabs((richTheta-mdcTheta)) > DTHETA) continue;

                        hists->richmdc_lep->Fill(iSec);
                    }
                }
            }
        }
    }
}

void HQAMaker::fillHistRichKick()
{
HKickTrackB *kickTrack = 0;
HRichHit *richHit = 0;
HShowerHitTof *showtofHit = 0;
HTofHit *tofHit = 0;
HStartHit *startHit = 0;

Int_t   richSec;
Float_t kickPhi, kickTheta;
Float_t richPhi, richTheta;

HLocation loc;
loc.setNIndex(3);

Int_t tofinoCell;
Int_t tofCell;
Float_t tof_corr;

const Float_t dist0 = 2100;
const Float_t c = 300; // light velocity

Int_t iSec;
Float_t rad2deg = 180.0/TMath::Pi();

    for(iSec=0; iSec<6; iSec++)
    {
        lTrack[0] = iSec;

        if(iterKickTrk)
        {
            iterKickTrk->Reset();
            iterKickTrk->gotoLocation(lTrack);

            while((kickTrack=(HKickTrackB*) iterKickTrk->Next())!=0)
            {
                kickPhi = kickTrack->getPhi()*rad2deg ;
                if(iSec != 5) kickPhi = kickPhi+iSec*60;
                else kickPhi = kickPhi-60.;

                kickTheta = kickTrack->getTheta()*rad2deg;

                if(iterRichHit)
                {
                    iterRichHit->Reset();
                    while((richHit=(HRichHit*) iterRichHit->Next())!=0)
                    {
                        richTheta = richHit->getTheta();
                        richPhi = richHit->getPhi();
                        richSec = richHit->getSector();

                        if( richSec != iSec) continue;

                        hists->richkick_dtheta[richSec]->Fill(richTheta-kickTheta);
                        hists->richkick_dphi[richSec]->Fill((richPhi-kickPhi)*sin(kickTheta/rad2deg));

                        if(fabs((richPhi-kickPhi)*sin(kickTheta/rad2deg)) > DPHI) continue;
                        if(fabs((richTheta-kickTheta)) > DTHETA) continue;

                        // calculate corrected time of flight
     					tof_corr =  dist0/((kickTrack->getBeta())*c);

                        hists->richkick_lep->Fill(iSec);
                        hists->richkickS_lep[kickTrack->getSystem()]->Fill(iSec);

                        if(kickTrack->getSystem()==0) hists->richkickS0_tof[richSec]->Fill(tof_corr);
                        if(kickTrack->getSystem()==1) hists->richkickS1_tof[richSec]->Fill(tof_corr);

                        //if(kickTrack->getCharge()==-1) hists->richkick_ele->Fill(iSec);
                        //if(kickTrack->getCharge()==1) hists->richkick_pos->Fill(iSec);
                        if(kickTrack->getPolarity()==-1) hists->richkick_ele->Fill(iSec);
                        if(kickTrack->getPolarity()==1) hists->richkick_pos->Fill(iSec);

						tofinoCell = -1;
						tofCell = -1;


						if(kickTrack->getSystem()==0)
                        {
                          if ( (showtofHit =
						    (HShowerHitTof*)catShoHitTof->getObject(kickTrack->getOuterHitId())) == NULL )
						   break;

		                  tofinoCell = showtofHit->getSector()*4 + showtofHit->getTofinoCell();
					      hists->richkickS_tof_vs_rod[kickTrack->getSystem()]->Fill(tofinoCell,tof_corr);
						 }
						 else 
						 {
							loc[0] = kickTrack->getOuterHitId()/1000;
							loc[1] = (kickTrack->getOuterHitId()%1000)/100;
							loc[2] = kickTrack->getOuterHitId()%100;

							if ( (tofHit = (HTofHit*)catTfHit->getObject(loc)) == NULL ) break;

							tofCell = tofHit->getSector()*64+tofHit->getModule()*8+tofHit->getCell();

							//printf("loc[0]=%d,loc[1]=%d,loc[2]=%d tofCell=%d \n",loc[0],loc[1],loc[2],tofCell);

							hists->richkickS_tof_vs_rod[kickTrack->getSystem()]->Fill(tofCell,tof_corr);
						 }
                        if(iterStHit) 
						{
						  iterStHit->Reset();
						  while((startHit=(HStartHit*) iterStHit->Next())!=0)
						  {
						    if(startHit->getModule() == 0 && kickTrack->getSystem()==1)
						    hists->richkick_tof_vs_startstripe->Fill(startHit->getStrip(),tof_corr);
						  }
						}
                    }
               }
            }
        }
    }
}


void HQAMaker::finalizeHistRichMDC() 
{
Float_t scaleF;

    if(nProcessed == 0) scaleF = 1.0;
    else scaleF = 1./nProcessed;

    for(Int_t s=0;s<6;s++)
    {
        hists->richmdc_dtheta[s]->Scale(scaleF);
        hists->richmdc_dphi[s]->Scale(scaleF);
    }
        hists->richmdc_lep->Scale(scaleF);
}

void HQAMaker::finalizeHistPid()
{
  
}

void HQAMaker::finalizeHistRichKick() 
{
Float_t scaleF;

    if(nProcessed == 0) scaleF = 1.0;
    else scaleF = 1./nProcessed;

    for(Int_t s=0;s<6;s++)
    {
        hists->richkick_dtheta[s]->Scale(scaleF);
        hists->richkick_dphi[s]->Scale(scaleF);
        hists->richkickS0_tof[s]->Scale(scaleF);
        hists->richkickS1_tof[s]->Scale(scaleF);
    }
        hists->richkick_lep->Scale(scaleF);
        hists->richkick_ele->Scale(scaleF);
        hists->richkick_pos->Scale(scaleF);

    for(Int_t s=0;s<2;s++)
    {
        hists->richkickS_lep[s]->Scale(scaleF);
    }
}

void HQAMaker::fillScalers() {
// Fill scalar quantities with operations on QA histograms

	
  TH1D *proj = 0;

  // Sectorwise scalers
  for(Int_t sec=0; sec<6; sec++) {

    //---- Rich scalers
    if(hists->richCal_nSec->GetEntries() > 0)
       (*scalers->richCal_n).fData[sec] = (float) hists->richCal_nSec->GetBinContent(sec+1)/nProcessed;
    if(hists->richHit_nSec->GetEntries() > 0)
       (*scalers->richHit_n)[sec] = 1000. * (float) hists->richHit_nSec->GetBinContent(sec+1)/nProcessed;

    //---- Mdc scalers
    if(hists->mdcCal1_nVsSector_m0->GetEntries() > 0) {
       proj = hists->mdcCal1_nVsSector_m0->ProjectionX("proj",sec+1,sec+1);
       (*scalers->mdcCal1_n_m0)[sec] = proj->GetMean();
       delete proj;
    }
    if(hists->mdcCal1_nVsSector_m1->GetEntries() > 0) {
       proj = hists->mdcCal1_nVsSector_m1->ProjectionX("proj",sec+1,sec+1);
       (*scalers->mdcCal1_n_m1)[sec] = proj->GetMean();
       delete proj;
    }
    if(hists->mdcCal1_nVsSector_m2->GetEntries() > 0) {
       proj = hists->mdcCal1_nVsSector_m2->ProjectionX("proj",sec+1,sec+1);
       (*scalers->mdcCal1_n_m2)[sec] = proj->GetMean();
       delete proj;
    }
    if(hists->mdcCal1_nVsSector_m3->GetEntries() > 0) {
       proj = hists->mdcCal1_nVsSector_m3->ProjectionX("proj",sec+1,sec+1);
       (*scalers->mdcCal1_n_m3)[sec] = proj->GetMean();
       delete proj;
    }

    //---- Tof scalers
    if(hists->tofHit_nSec->GetEntries() > 0)
       (*scalers->tofHit_n)[sec] = (float) hists->tofHit_nSec->GetBinContent(sec+1)/nProcessed;

    // Shower scalers
    if(hists->shoHit_nSec->GetEntries() > 0)
       (*scalers->shoHit_n)[sec] = (float) hists->shoHit_nSec->GetBinContent(sec+1)/nProcessed;
    if(hists->shoHitTof_nSec->GetEntries() > 0)
       (*scalers->shoHitTof_n)[sec] = (float) hists->shoHitTof_nSec->GetBinContent(sec+1)/nProcessed;

    //---- Kick scalers
    if(hists->kickTrack_nSec->GetEntries() > 0)
       (*scalers->kickTrack_n)[sec] = (float) hists->kickTrack_nSec->GetBinContent(sec+1)/nProcessed;

    //---- Trigger scalers
    if(hists->trigRich_nSec->GetEntries() > 0)
       (*scalers->trigRich_n).fData[sec] = (float) hists->trigRich_nSec->GetBinContent(sec+1)/nProcessed;
    if(hists->trigTof_nSec->GetEntries() > 0)
       (*scalers->trigTof_n).fData[sec] = (float) hists->trigTof_nSec->GetBinContent(sec+1)/nProcessed;
    if(hists->trigShower_nSec->GetEntries() > 0)
       (*scalers->trigShower_n).fData[sec] = (float) hists->trigShower_nSec->GetBinContent(sec+1)/nProcessed;

    if(hists->trigMULep_TnSec->GetEntries() > 0)
       (*scalers->trigTlepton_n).fData[sec] = (float) hists->trigMULep_TnSec->GetBinContent(sec+1)/nProcessed;
    if(hists->trigMULep_SnSec->GetEntries() > 0)
       (*scalers->trigSlepton_n).fData[sec] = (float) hists->trigMULep_SnSec->GetBinContent(sec+1)/nProcessed;


  }

  // Detector-wise scalers

}



void HQAMaker::fillHistMatching() {

  HKickTrackB   *kick   =0;
  HSplineTrack  *track  =0;
  HTofCluster   *Cluster=0;
  HShowerHitTof *Tofino =0;
  HMetaMatch    *meta   =0;


  if (iterMetaMatch) {
  iterMetaMatch->Reset();
  while ((meta=(HMetaMatch *)iterMetaMatch->Next()) != 0) {

    if( meta->getKickInd()!=-1 ){
        kick = (HKickTrackB *)catKicTrk->getObject(meta->getKickInd());

        hists->hseckick                          ->Fill(kick->getSector());
        if(kick->getSystem()==1) hists->hseckick1->Fill(kick->getSector());
        if(kick->getSystem()==0) hists->hseckick0->Fill(kick->getSector());
    }//-end-of-loop-over-kick--

    if( (meta->getSplineInd()!=-1)&&meta->isSplineAccepted() ){
        track = (HSplineTrack *)catSplineTrk->getObject(meta->getSplineInd());

        if(track->getSystem()>-1)hists->hsecspline ->Fill(track->getSector());
        if(track->getSystem()==1)hists->hsecspline1->Fill(track->getSector());
        if(track->getSystem()==0)hists->hsecspline0->Fill(track->getSector());

        if( track->getSystem()==1&&meta->getTofClusterSize()>0){//-system==1 (TOF case)
            Cluster = (HTofCluster *)catTfClust->getObject(meta->getTofHitInd());
            //calculation of tofXSeg, tofYSeg - positions of TOF hit in module coordinates
            Float_t tofXSeg,tofYSeg;
            tofXSeg=Cluster->getXpos();
            if(Cluster->getModule()<4){
              tofYSeg =106.75 - Cluster->getCell()*30.5;
            }else{
              tofYSeg = 71.75 - Cluster->getCell()*20.5;
            }
            hists->hXdiffvstofstrip->Fill((Cluster->getSector()*64+(Cluster->getModule()*8)+Cluster->getCell()),(tofXSeg-meta->getXSegCross()));
            hists->hYdiffvstofstrip->Fill((Cluster->getSector()*64+(Cluster->getModule()*8)+Cluster->getCell()),(tofYSeg-meta->getYSegCross()));
            hists->htof_quality->Fill(meta->getSector(),meta->getQualityTOF());
        }//-system==1


        if( track->getSystem()==0) { //-system==0 (TOFINO case)
            Tofino = (HShowerHitTof *)catShoHitTof->getObject(track->getShowerHitInd());
            Float_t ShwX,ShwY, ShwSigmaX, ShwSigmaY;
            Char_t ShwRow, ShwCol;
            Tofino->getXY(&ShwX,&ShwY);
            ShwRow = Tofino->getRow(); 
            ShwCol = Tofino->getCol();
            ShwSigmaX = Tofino->getSigmaX();
			ShwSigmaY = Tofino->getSigmaY();
            if(ShwSigmaX==0 || ShwSigmaY==0.0) cout<<"--Error: ShowerSigma could not be 0! "<<endl;

			hists->hXdiffvsshowersector->Fill(meta->getSector(),(ShwX-meta->getXSegCross()));
            hists->hYdiffvsshowersector->Fill(meta->getSector(),(ShwY-meta->getYSegCross()));
			
            hists->hXdiffvsshw->Fill(meta->getSector(),(ShwX-meta->getXSegCross()) / ShwSigmaX );
			hists->hYdiffvsshw->Fill(meta->getSector(),(ShwY-meta->getYSegCross()) / ShwSigmaY);

			hists->hshower_quality     ->Fill(meta->getSector(),  meta->getQualitySHOWER() );

            hists->hXdiffvsshoCol      ->Fill(ShwCol + (meta->getSector() * 33), ShwX-meta->getXSegCross());  
            hists->hXdiffvsshoRow      ->Fill(ShwRow + (meta->getSector() * 33), ShwX-meta->getXSegCross());
            hists->hYdiffvsshoCol      ->Fill(ShwCol + (meta->getSector() * 33), ShwY-meta->getYSegCross());
            hists->hYdiffvsshoRow      ->Fill(ShwRow + (meta->getSector() * 33), ShwY-meta->getYSegCross());
        }//-system==0


    }//-end-of-loop-over-spline--

  }//-end-while--
  }
}

void HQAMaker::fillHistPid() {

  HPidTrackCand    *pTrackCand  = 0 ;
  HPidHitData      *pHitData    = 0 ;
  HPidTrackData    *pTrackData  = 0 ;
  HStartHit        *pStartHit   = 0 ;
  
  // Mult. counters
  Int_t nKickMult=0, nRKMult=0;

  //-----------------------StartDet part-----------------------------
  // first step is to get global timing information
  // there is only one "time 0" and is located in 
  // StartHit category 
  Float_t fStartTime = -1000.0;
  Int_t nStartStrip = -1;
  
  if(iterStHit){
    iterStHit->Reset();
    while ((pStartHit=(HStartHit*)iterStHit->Next())!= 0){
      if(pStartHit->getModule()==0){
        nStartStrip=pStartHit->getStrip();
        fStartTime=pStartHit->getTime();
      }
    }  //end of start iteration
  } // if iterStart 
  //-------------------End of Start part-----------------------------
  if (!iterPidTrackCand) {
    return ;  // do nothing if PidTrackCandidate does not exist
  } 

  iterPidTrackCand->Reset();
  Int_t nDataObjs = ((TObjArray*)iterPidTrackCand->GetCollection())->GetEntries();
  varHists->pidTrackCand_n_Var->Fill(nEvent,nDataObjs);
  Int_t nLepCandKick=0;

  //----------------- loop over pidTrackCand ----------------------- 
  while ((pTrackCand=(HPidTrackCand *)iterPidTrackCand->Next()) != 0) { //PidTrackCandIter
    pHitData=pTrackCand->getHitData();
    pTrackData=pTrackCand->getTrackData();

    if(pTrackData->nKickTrackInd>-1){ 
       // PID Kick Tracks 
       hists->hNTrackCandPerSecKick->Fill(pHitData->getSector());
    }
    if((pTrackData->nRKTrackInd>-1 ) && (pTrackData->bIsAccepted[4])) {
       // PID RK Tracks 
       hists->hNTrackCandPerSecRK->Fill(pHitData->getSector());
    }
    if(pHitData->getFlagRICH()) {
	  nLepCandKick++;
      if(pTrackData->nKickTrackInd>-1){
          // PID electrons in Kick
	    hists->hNTrackCandLepPerSecKick->Fill(pHitData->getSector());
      }
      if((pTrackData->nRKTrackInd>-1 ) && (pTrackData->bIsAccepted[4])) {
          // PID electrons in RK
	    hists->hNTrackCandLepPerSecRK->Fill(pHitData->getSector());
      }
	}

    //--------------- KickTrack in Pid ----------------------------
    if(pTrackData->nKickTrackInd>-1){ // pid KickTrack info
      // times
      Float_t fPidTimeCorr=0.0;     
      if(pTrackData->getBeta(0)!=0.){
        fPidTimeCorr = 7./pTrackData->getBeta(0);
      }
      if(pHitData->getFlagRICH()) { 
        if(pHitData->iSystem==0){
          hists->hpidtrk_richkick_tof_vs_rod_sys0
                 ->Fill(pHitData->nSector*4+pHitData->nMetaCell,fPidTimeCorr);
        }
        else{
          hists->hpidtrk_richkick_tof_vs_rod_sys1
                 ->Fill(pHitData->nSector*64+pHitData->nMetaCell,fPidTimeCorr);
        }
        if(pHitData->iSystem==1){
          hists->hpidtrk_richkick_tof_vs_startstrip
                 ->Fill(nStartStrip,fPidTimeCorr);
        } 
      }

      // theta and phi
     hists->hpidtrk_KICK_theta_sec[pHitData->nSector]->Fill(pHitData->fMdcTheta);
     hists->hpidtrk_KICK_phi->Fill(pHitData->fMdcPhi);
     if(pTrackData->nPolarity[0]==-1&&pTrackData->fMomenta[0]>300){
        hists->hpidtrk_KICK_neg_theta_sec[pHitData->nSector]->Fill(pHitData->fMdcTheta);
        hists->hpidtrk_KICK_neg_phi->Fill(pHitData->fMdcPhi);
     }
     //proton momentum calculated from velocity vs reconstructed momentum
     if(pTrackData->nPolarity[0]>0&&pTrackData->getBeta(0)<1){
        if(pHitData->iSystem==0) hists->hpidtrk_kick_momdif_sys0_sec[pHitData->nSector]
               ->Fill(pTrackData->fMomenta[0], pTrackData->fMomenta[0]-(938.*pTrackData->getBeta(0)*1./sqrt(1.-pTrackData->getBeta(0)*pTrackData->getBeta(0))));
        if(pHitData->iSystem==1) hists->hpidtrk_kick_momdif_sys1_sec[pHitData->nSector]
               ->Fill(pTrackData->fMomenta[0],pTrackData->fMomenta[0]-(938.*pTrackData->getBeta(0)*1./sqrt(1.-pTrackData->getBeta(0)*pTrackData->getBeta(0))));
      }
      //multiplicity
      nKickMult++;
      
    } // pid KickTrack info
    //------------End of KickTrack in Pid ----------------------------

    //------------------- Spline in Pid ----------------------------------
    if(pTrackData->nSplineTrackInd>-1&&pTrackData->bIsAccepted[2]) {
      //Spline theta and phi
      if(pHitData->iSystem>-1) {
        hists->hpidtrk_SPLINE_theta_sec[pHitData->nSector]->Fill(pHitData->fMdcTheta);
        hists->hpidtrk_SPLINE_phi->Fill(pHitData->fMdcPhi);
        if(pTrackData->nPolarity[2]==-1&&pTrackData->fMomenta[2]>300){
          hists->hpidtrk_SPLINE_neg_theta_sec[pHitData->nSector]->Fill(pHitData->fMdcTheta);
          hists->hpidtrk_SPLINE_neg_phi->Fill(pHitData->fMdcPhi);
        }
      }
    }
    //------------------- End of Spline in Pid ----------------------------------
	
    //------------------- RK in Pid ----------------------------------
    if(pTrackData->nRKTrackInd>-1&&pTrackData->bIsAccepted[4]) {

      //times
      Float_t fPidTimeCorr=0.0;
      if(pTrackData->getBeta(4)!=0.) fPidTimeCorr = 7./pTrackData->getBeta(4);
      if(pHitData->getFlagRICH()) {
        //be carefull, system can be -1 (no meta matching)
        if(pHitData->iSystem==0) hists->hpidtrk_richrk_tof_vs_rod_sys0->Fill(pHitData->nSector*4+pHitData->nMetaCell,fPidTimeCorr);
        if(pHitData->iSystem==1) hists->hpidtrk_richrk_tof_vs_rod_sys1->Fill(pHitData->nSector*64+pHitData->nMetaCell,fPidTimeCorr);
        if(pHitData->iSystem==1) hists->hpidtrk_richrk_tof_vs_startstrip->Fill(nStartStrip,fPidTimeCorr);
      }

      //theta and phi
      if(pHitData->iSystem>-1) {
        hists->hpidtrk_RK_theta_sec[pHitData->nSector]->Fill(pHitData->fMdcTheta);
        hists->hpidtrk_RK_phi->Fill(pHitData->fMdcPhi);
        if(pTrackData->nPolarity[4]==-1&&pTrackData->fMomenta[4]>300){
          hists->hpidtrk_RK_neg_theta_sec[pHitData->nSector]->Fill(pHitData->fMdcTheta);
          hists->hpidtrk_RK_neg_phi->Fill(pHitData->fMdcPhi);
        }
      }

      //proton momentum calculated from velocity vs reconstructed momentum
      if(pTrackData->nPolarity[4]>0&&pTrackData->getBeta(4)<1){
        if(pHitData->iSystem==0) hists->hpidtrk_rk_momdif_sys0_sec[pHitData->nSector]
                ->Fill(pTrackData->fMomenta[4],pTrackData->fMomenta[4]-(938.*pTrackData->getBeta(4)*1./sqrt(1.-pTrackData->getBeta(4)*pTrackData->getBeta(4))));
        if(pHitData->iSystem==1) hists->hpidtrk_rk_momdif_sys1_sec[pHitData->nSector]
                ->Fill(pTrackData->fMomenta[4],pTrackData->fMomenta[4]-(938.*pTrackData->getBeta(4)*1./sqrt(1.-pTrackData->getBeta(4)*pTrackData->getBeta(4))));
      }
      //multiplicity
      if(pHitData->iSystem>-1) nRKMult++;
    }
    //------------End of Runge-Kutta in Pid ----------------------------
  } //PidTrackCandIter
  //----------------- End of loop over pidTrackCand ----------------------- 
  hists->hpidtrk_multrk->Fill(nRKMult);
  hists->hpidtrk_multkick->Fill(nKickMult);
 
  varHists->pidTrackCandLep_n_Var->Fill(nEvent,nLepCandKick);
} // fillHistPid()


void HQAMaker::fillHistDaqScaler() {
   HTBoxChan *pTBoxChannel;
   UInt_t scaler1=0,scaler2=0,scaler3=0,scaler4=0;
   Int_t chan = 0;

   if(!iterHTBoxChan){
      return;
   }

   iterHTBoxChan->Reset(); 

   while (( pTBoxChannel= (HTBoxChan *)iterHTBoxChan->Next()) != 0) {

      pTBoxChannel->get(chan,scaler1,scaler2,scaler3,scaler4,(Int_t)0);

      hists->histInput[chan]->Fill(nCountCalEvents,scaler1);
      hists->histReduce[chan]->Fill(nCountCalEvents,scaler2);
      hists->histStartScaler[chan]->Fill(nCountCalEvents,scaler3);
      hists->histVetoScaler[chan]->Fill(nCountCalEvents,scaler4);
   }
}


void HQAMaker::fillGlobalVertex() {
  HVertex pGlobVertex = gHades->getCurrentEvent()->getHeader()->getVertex();
  Float_t fVertX = pGlobVertex.getX();
  Float_t fVertY = pGlobVertex.getY();
  Float_t fVertZ = pGlobVertex.getZ();
  Float_t fVertChi2 = pGlobVertex.getChi2();

  if(fVertChi2 > -1.0){
    hists->stVertexXY->Fill(fVertX,fVertY);
    hists->stVertexZ->Fill(fVertZ);
  }
}
