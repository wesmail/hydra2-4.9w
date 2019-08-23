// File: hmdcunpacker.cc
//
// Author: Walter Karig <W.Karig@gsi.de>
// Modified: 23/10/02 by Peter Zumbruch <P.Zumbruch@gsi.de>
// Modified by D.B <D.Bertini@gsi.de>

////////////////////////////////////////////////////////////
//
// HMdcUnpacker
// 
// Unpacker for mdc data
// decodes data and fills it into the HMdcRaw category
// 
////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include "hades.h"
#include "hcategory.h"
#include "hdatasource.h"
#include "hiterator.h"
#include "hldsubevt.h"
#include "hevent.h"
#include "hmdcraw.h"
#include "hmdcrawstruct.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hruntimedb.h"
#include "heventheader.h"
#include <iostream.h>
#include <fstream>
#include <TROOT.h>

#include "hmdcdataword.h"
#include "hmdcunpacker.h"

ClassImp(HMdcUnpacker)

HMdcDataWord *HMdcUnpacker::w=0;

HMdcUnpacker::HMdcUnpacker(Int_t id , Bool_t dbg, Bool_t consisCheck) : HldUnpack()
{ 
  subEvtId = id; 
  w = new HMdcDataWord(); 

  debug=dbg;
  consistencyCheck=consisCheck;
  setDefaults();
  determineDecodeVersion();
}


HMdcUnpacker::HMdcUnpacker(Int_t id, Int_t myversion, Bool_t dbg, Bool_t consisCheck)
{
  HMdcUnpacker::HMdcUnpacker(id, dbg, consisCheck); 
  if (myversion>=0) {setDecodeVersion(myversion);}
  else {determineDecodeVersion();}
}

HMdcUnpacker::HMdcUnpacker(const HMdcUnpacker& mdc) 
{ 
  subEvtId = mdc.subEvtId; 
  w=mdc.w;
}

HMdcUnpacker::~HMdcUnpacker()
{  
  if(w) delete w; 
  w=0;
} 

void HMdcUnpacker::setDefaults(void)
{
  pReadout = NULL;
  standalone=kFALSE;
  tdcMode=0;
  pMdcRawEventHeaderCat=0;
  disableSamNrConsistencyCheck=kFALSE;
  mdcRawEventHeaderCatPersistency=kFALSE;
  mdcDataWordCatPersistency=kFALSE;
  fillDataWord=kFALSE;
  subHeaderDecodeVersion=0;
  setDecodeVersion(1);
  versionSetManual=kFALSE;
  unpackerDebug=kFALSE;
  rocEvtHeaderSize = 3; 
}

void HMdcUnpacker::determineDecodeVersion(void)
{
  HDataSource* source = gHades->getDataSource();
  Int_t refId=source->getCurrentRefId();
  Int_t runId=source->getCurrentRunId();
  Int_t myId=-99;

  if (runId==refId || refId<0) 
    {
      myId=runId;
    }
  else 
    {
      myId = refId;
    }

  if (!versionSetManual)
    {
      if (myId >= versionSwitchId)
	{
	  setDecodeVersion(1,kFALSE);
	} 
      else
	{
	  setDecodeVersion(0,kFALSE);
	}
    }
}

void HMdcUnpacker::print(void)
{
  // prints out the variables of the unpacker object
  
  Info("","############################################################");
  Info("","HMdcUnpacker: ClassName %s :",this->ClassName());
  Info("","%s: %p ClassName %s","HMdcEvReadout",pReadout,(pReadout)?pReadout->ClassName():"---"); //!Lookup up table 
  Info("","%s: %3i ","sub event Id",subEvtId);
  Info("","%s: %i","TdcMode",tdcMode);
  Info("","%s: %s","debug",(debug)?"kTRUE":"kFALSE");             
  Info("","%s: %s","consistencyCheck",(consistencyCheck)?"kTRUE":"kFALSE");                  
  Info("","%s: %s","disableSamNrConsistencyCheck",(disableSamNrConsistencyCheck)?"kTRUE":"kFALSE");     
  Info("","%s: %s","mdcRawEventHeaderCatPersistency",(mdcRawEventHeaderCatPersistency)?"kTRUE":"kFALSE");   
  Info("","%s: %s","mdcDataWordCatPersistency",(mdcDataWordCatPersistency)?"kTRUE":"kFALSE");         
  Info("","%s: %s","fillDataWord",(fillDataWord)?"kTRUE":"kFALSE");                                  
  Info("","%s: %s","standalone",standalone?"kTRUE":"kFALSE"); 
  Info("","%s: %3i ","decodeVersion",decodeVersion);
  Info("","%s: %2i ","subHeaderDecodeVersion",subHeaderDecodeVersion);
  Info("","%s: %10i ","versionSwitchId (RunId) from which on to use version 1",versionSwitchId);
  Info("","%s: %3i ","ROC headerSize",rocEvtHeaderSize); 
};

Int_t HMdcUnpacker::fillMdcRawEventHeader(Int_t errorFlag, Int_t rocModule, Int_t rocNumber, 
					  Int_t rocSector, Int_t subEvtId,  Int_t subEvtSize, 
					  Int_t trigType)
{
  HLocation dummy;
  HMdcRawEventHeader *pMdcRawEventHeader= NULL;
  pMdcRawEventHeader = (HMdcRawEventHeader*) pMdcRawEventHeaderCat->getNewSlot(dummy);
  
  if (pMdcRawEventHeader) 
    {
      pMdcRawEventHeader = new (pMdcRawEventHeader) HMdcRawEventHeader();
    }
  else 
    {
      Error("fillMdcRawEventHeader()", "SubEvtId %i - can't get slot for MdcEventHeader",subEvtId);
      return -2;
    }
 
  Int_t samNr; // samNr = sam * 2 + samPos

  samNr = -1;
  samNr = getSubEvtIdToSamNr(subEvtId);

  if (samNr != -1) 
    {
      rocSector = pReadout->getSamNumberToSector(samNr);
      rocModule = pReadout->getSamNumberToModule(samNr);
    }
  else
    {
      rocSector = 0 - subEvtId; // until lookup-table sam->sector/module exists
      rocModule = 0 - subEvtId; // until lookup-table sam->sector/module exists
    }

  pMdcRawEventHeader->set(errorFlag, rocModule, rocNumber, 
			  rocSector, subEvtId,  subEvtSize, trigType);

  return 0;
}

Int_t HMdcUnpacker::fillMdcDataWord(Int_t sector,Int_t module,Int_t mbo,Int_t tdc)
{
  // function for filling the category HMdcDataword
  // 

  Int_t entry=0;
  UChar_t maxentry=10;

  maxentry=8;
  
  HLocation dataLoc;
  dataLoc.set(5,0,0,0,0,0);
  //set location indexes 
  
  dataLoc[0] = sector;
  dataLoc[1] = module;
  dataLoc[2] = mbo;
  dataLoc[3] = tdc;
  dataLoc[4] = entry;
  
  HMdcDataWord *pMdcDataWord=NULL;
  
  for (entry = 0; entry < maxentry; entry++)
    {
      pMdcDataWord = NULL;
      
      dataLoc[4] = entry;
      pMdcDataWord = (HMdcDataWord*) pMdcDataWordCat->getObject(dataLoc);
      if (!pMdcDataWord) break;
      //       Info("HMdcUnpacker","%2i %10p ",entry,pMdcDataWord);
      //       Info("HMdcUnpacker","s:%i m:%i mb:%2i td:%2i e:%2i ",dataLoc[0],dataLoc[1],dataLoc[2],dataLoc[3],dataLoc[4]);
      //       dataLoc.Dump();
      //       pMdcDataWord->print();
    }

  //   cout << ".............................." << endl;
  //   Info("HMdcUnpacker","%2i %10p ",entry,pMdcDataWord);
  //   Info("HMdcUnpacker","s:%i m:%i mb:%2i td:%2i e:%2i ",dataLoc[0],dataLoc[1],dataLoc[2],dataLoc[3],dataLoc[4]);
  //   dataLoc.Dump();
  
  if (pMdcDataWord)
    {
      if (entry==maxentry-1)
	{
	  Error("fillMdcDataWord()", " SubEvtId %i - to many entries to the same location entries>%i",subEvtId,maxentry);
	}
      else
	{
	  Error("fillMdcDataWord()", " SubEvtId %i - can't get empty slot for MdcDataWord",subEvtId);
	}
      return -2;
    }
  else
    {
      pMdcDataWord = (HMdcDataWord*) pMdcDataWordCat->getSlot(dataLoc);
      if (pMdcDataWord) 
	{
	  pMdcDataWord = new (pMdcDataWord) HMdcDataWord(*w);
	  pMdcDataWord->setAdress(sector,module,mbo,tdc,entry);
	}
      else
	{
	  //	  dataLoc.Dump();
	  Error("fillMdcDataWord()", " SubEvtId %i - can't get slot for MdcDataWord",subEvtId);
	  return -2;
	}
    }
  return 1;
}

Bool_t HMdcUnpacker::testMdcSetup(Int_t s, Int_t m)
{
    // Tests the Mdc setup if the modules are present
    // in the running analysis

    Bool_t result=kFALSE;
    if(setup[s][m]==0) result=kFALSE;
    if(setup[s][m]==1) result=kTRUE;
    return result;
}

void HMdcUnpacker::getMdcSetup()
{
    // Gets Mdc detector setup

    HMdcDetector* mdcDet=(HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector("Mdc"));
    if (!mdcDet)
    {
	Error("getMdcSetup()"," SubEvtId %i - Mdc-Detector setup (gHades->getSetup()->getDetector(\"Mdc\")) missing.",subEvtId);
    }
    for(Int_t s=0; s<6; s++) {
	for(Int_t m=0; m<4; m++) {
	    if (!mdcDet->getModule(s, m)) setup[s][m]=0;
	    if ( mdcDet->getModule(s, m)) setup[s][m]=1;
	}
    }
}

Int_t HMdcUnpacker::getSubEvtIdToSamNr(Int_t subEvtId, Bool_t expectUndefined)
{
  // determines the samNr from the subEvtId 
  // in addition it crosschecks the samNr with the DAQ specifications 
  // and does boundary checks by default.
  // You can switch it off by setDisableSamNrConsistencyCheck()

  Int_t samNr;
  // determine samNr

  switch(decodeVersion)
    {
    case 0:
      samNr = 2*(subEvtId - 201); // version before Nov02
      break;
    case 1:
      samNr = subEvtId - 201; // version since Nov02
      break;
    default:
      samNr = -1;
    }

  // additional checks 

  if (disableSamNrConsistencyCheck)
    {
      if (samNr < 0) return -1;
      return samNr;
    }

  // additional checks by default on !!

  switch(decodeVersion)
    {
    case 0:
      if (!(200 < subEvtId && subEvtId < 300))
	{
	  Error("getSubEvtIdToSamNr()"," SubEvtId %i - sub event id: %i, out of mdc range ]200,300[",subEvtId,subEvtId);
	  return -1;
	}
      break;
    case 1:
      if (!(200 <= subEvtId && subEvtId < 300))
	{
	  Error("getSubEvtIdToSamNr()"," SubEvtId %i - sub event id: %i, out of mdc range [200,300[",subEvtId,subEvtId);
	  return -1;
	}
      break;
    default:
      samNr = -1;
    }

  
  Int_t rocSector = pReadout->getSamNumberToSector(samNr);

  if (rocSector==-1 && !expectUndefined)
    {
      Error("getSubEvtIdToSamNr()",
	    "SubEvtId %i - calculated samNr: %i is not in the lookup table (HMdcEvReadout)",
	    subEvtId,samNr);
      return -1;
    }
  return samNr;
}

void HMdcUnpacker::setTdcMode(Int_t i)
{
  // In measurement mode the TDC accepts two hits per channel. The TDC can hereby
  // trigger either on on a leading and the trailing edge of a single pulse  
  // or on two leading edges.
  // This influences the polarity of the hitCounter in the HMdcRaw Class: 
  // mode 0 (default) : trigger on leading and trailing edge
  //                    (hit multiplicity nHits: +1 or +2) 
  // mode 1           : trigger on 2 leading edges:
  //                    (hit multiplicity nHits: -1 or -2)

  tdcMode=(i == 0 || i == 1)? i : 0;
  return;
}

void HMdcUnpacker::setRocEvtHeaderSize(UInt_t ui)
{
  rocEvtHeaderSize=ui;
}

Int_t HMdcUnpacker::getTdcMode(void)
{
  // returns mode the tdc is triggering on signals
  // mode 0 (default) : trigger on leading and trailing edge
  // mode 1           : trigger on 2 leading edges:

  return tdcMode;
}

HCategory* HMdcUnpacker::initCategory(Cat_t cat, TString catname, TString detector)
{
  // inits the container cat 
  // looks first whether it already exists
  // otherwise it is created
  // default for detector is "Mdc"
  // catname is used for Error handling

  HCategory * category = 0;

  category = (HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(cat));

  if (!category)
    {
      category=(HCategory*)((HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector(detector.Data()))->buildCategory(cat));
 
      if (!category)
	{
	  Error("initCategory()"," SubEvtId %i - can't create or get category \"%s\"",subEvtId,catname.Data());
	  return NULL;
	}

      if (!((HEvent*)(gHades->getCurrentEvent()))->addCategory(cat,category,detector.Data()))
	{
	  Error("initCategory()"," SubEvtId %i - can't add category \"%s\" to detector \"%s\"",subEvtId,catname.Data(),detector.Data());
	  return NULL;
	}
    }
  return category; 
}

Bool_t HMdcUnpacker::init() 
{
  // Initialisation of 
  //                  - lookup table
  //                  - category for raw-level  storage
  //                  - additional category if debug option
  //                  - additional Tree if debug option
  //                  - local mdc setup table for lookup
  //                  - decodeVersion

  getMdcSetup();

  HParSet *rawc=0;
  HRuntimeDb *rtdb = gHades->getRuntimeDb();
  if (!(rawc=rtdb->getContainer("MdcRawStruct")))
    {
      Error("init()"," SubEvtId %i - can't get container \"%s\" ",subEvtId,"MdcRawStruct");
      return kFALSE;
    }

  if (!rawc->isStatic()) rawc->init(); //Needs explicit initialization

  if (!(pReadout= (HMdcEvReadout*) rtdb->getContainer("MdcEvReadout")))
    {
      Error("init()"," SubEvtId %i - can't get MdcEvReadout", subEvtId);
      return kFALSE;
    }
  
  if (!(pRawCat = initCategory(catMdcRaw,"catMdcRaw"))) return kFALSE;

  if (!(pMdcRawEventHeaderCat = initCategory(catMdcRawEventHeader,"catMdcRawEventHeader"))) return kFALSE;
  pMdcRawEventHeaderCat->setPersistency(mdcRawEventHeaderCatPersistency);

  if (debug && fillDataWord) 
    { 
      if (!(pMdcDataWordCat = initCategory(catMdcDataWord,"catMdcDataWord"))) return kFALSE;
      pMdcDataWordCat->setPersistency(mdcDataWordCatPersistency);
    }

  determineDecodeVersion();
  if (unpackerDebug) print();
  return kTRUE;
}

Bool_t HMdcUnpacker::reinit()
 {
   determineDecodeVersion();
   return kTRUE;
 }

Int_t HMdcUnpacker::execute() 
{
  // execute funtion of the task
  // calls the fill() function
  // before it checks whether the called subEvtId corresponds 
  // to an active module in the mdc detector setup 

  if (!pSubEvt) return 1; 

  if (!pReadout) 
    {
      UInt_t evSeqNr = ((HEventHeader*)((HEvent*)(gHades->getCurrentEvent())->getHeader()))->getEventSeqNumber();
      Error("execute"," Event: %i - SubEvtId %i - pointer to Lookup-Table zero",evSeqNr,subEvtId);
      return 1;
    }

  Int_t samNr  = getSubEvtIdToSamNr(subEvtId,kTRUE);
  if (samNr < 0) 
    {
      UInt_t evSeqNr = ((HEventHeader*)((HEvent*)(gHades->getCurrentEvent())->getHeader()))->getEventSeqNumber();
      Warning("execute",
	      " Event: %i - SubEvtId %i - samNr: %i (=lookup table index+1) negative ... sub event id %i ignored",
	      evSeqNr,subEvtId,samNr,subEvtId);
      return kTRUE;
    }
  Int_t sector = pReadout->getSamNumberToSector(samNr);
  if (sector < 0) 
    {
      UInt_t evSeqNr = ((HEventHeader*)((HEvent*)(gHades->getCurrentEvent())->getHeader()))->getEventSeqNumber();
      Warning("execute",
	      " Event: %i - SubEvtId %i - no valid sector entry in lookuptable for virtual samNr: %i (=lookup table index+1) ... sub event id %i ignored",
	      evSeqNr,subEvtId,samNr,subEvtId);
      return kTRUE;
    }
  Int_t module = pReadout->getSamNumberToModule(samNr);
  if (module < 0) 
    {
      UInt_t evSeqNr = ((HEventHeader*)((HEvent*)(gHades->getCurrentEvent())->getHeader()))->getEventSeqNumber();
      Warning("execute",
	      " Event: %i - SubEvtId %i - no valid module entry in lookuptable for virtual samNr: %i (=lookup table index+1) ... sub event id %i ignored",
	      evSeqNr,subEvtId,samNr,subEvtId);
      return kTRUE;
    }

  if (testMdcSetup(sector, module)) 
    {
      Int_t err = fill();
      if (err<0) 
	{
	  UInt_t evSeqNr = ((HEventHeader*)((HEvent*)(gHades->getCurrentEvent())->getHeader()))->getEventSeqNumber();
	  Error("execute"," Event: %i - SubEvtId %i - error in unpacking",evSeqNr,subEvtId);
	  return err;
	}
    }
  return kTRUE;
}

Bool_t HMdcUnpacker::finalize()
{
  return kTRUE;
}

Bool_t HMdcUnpacker::decodeSubHeader(UInt_t *data, UInt_t &subEvtSize)
{
  // decodes the sub header (ROC-Header) 
  // and returns the size of this header
  //
  // If the errorFlag of the ROC-Header is set or 
  // the debug flag of the unpacker is set
  // the header is written out to the category HMdcRawEventHeader 
  // (set its persistency via, setPersistencyRawEventHeader())
  //
  // If the standalone mode is chosen the trigger type (id) of the whole event 
  // is set to the trigger type of the roc header if it is a calibration trigger
  // (If MDCs are not run via the common daq system, but in a standalone mode,
  // the event trig type is not provided in the event header (it is always 1).) 

  Int_t errorFlag    = 0;
  Int_t trigType     = 0;
  Int_t rocNumber    = 0;
  Int_t rocSector    = 0;
  Int_t rocModule    = 0;
  
  w->clearHeader();
  if(!w->subHeader(data,subHeaderDecodeVersion,consistencyCheck))
    { 
      Error("decodeSubHeader()","SubEvtId %i - can't decode sub header ",subEvtId); 
      return kFALSE;
    }
  
  rocNumber    = w->getRocNb();
  errorFlag    = w->getErrorFlag();
  trigType     = w->getTrigType();
  subEvtSize   = w->getSubEvtSize();
  
  
  if (errorFlag!=0 || debug) 
    {
      fillMdcRawEventHeader(errorFlag, rocModule, rocNumber, rocSector, subEvtId,  subEvtSize, trigType);
    }

  if (standalone) 
    {
      Int_t kMdc_Standalone_CalibrationEvent=9;
      if (trigType == kMdc_Standalone_CalibrationEvent)
	{
	  ((HEventHeader*)((HEvent*)(gHades->getCurrentEvent()))->getHeader())->setId(kMdc_Standalone_CalibrationEvent);
	}
    }

  return kTRUE;
}

Int_t HMdcUnpacker::fill() 
{
  // fill function:
  // performs the loop over the subevent data
  // 
  // -- if the ROC header size > 0 (change it via setRocEvtHeaderSize())
  //    then the roc header is decoded via decodeSubHeader()
  //    otherwise the whole subevent is decoded as once
  // 
  // -- The data loop performs the following steps
  //     1. call of decode for each dataword
  //     2. the hardware address is matched to the sector/module/mbo/tdc coordinates of HMdcRaw 
  //        via the lookup table HMdcEvReadout
  //     3. a new/existing slot at the corresponding address is requested
  //     4. the time information is filled to this slot
  //
  // --  if the debug and fillDataWord flag is set (set via setFillDataWord() and setDebug())
  //     the extended dataword information is written to the category HMdcDataword()
  //     (the persistency of this container can be set via setPersistencyDataWord())  
  // 
  // -- the return values of the function are:
  //     1 : success  
  //    -1 : Error while decoding the dataword
  //    -1 : Error while decoding the roc header
  //    -1 : Error while decoding the in data loop (data pointer exceeds data range)
  //    -2 : Error while filling the to category HMdcRaw
  //
  // -- in the case that there is no entry in the lookup table 
  //    for a correctly decoded dataword there the next dataword is taken

    
  if (pSubEvt && pReadout) 
    {
      HLocation loc;
      loc.set(4, 0, 0, 0, 0);
      
      UInt_t* data = pSubEvt->getData();
      UInt_t* end  = pSubEvt->getEnd();

      UInt_t subEvtSize = 0;
      UInt_t offset = 0;
      UInt_t *enddata = 0;

      ////////////////////////////////////////
      // modifiy here to implement dynamic headers

      rocEvtHeaderSize = getRocEvtHeaderSize(); // default 3 words for the roc-header

      //
      ////////////////////////////////////////

      UInt_t evSeqNr = ((HEventHeader*)((HEvent*)(gHades->getCurrentEvent())->getHeader()))->getEventSeqNumber();

      while( data < end ) 
	{
	  w->clearAll();

	  //decoding sub-header

	  if (rocEvtHeaderSize > 0)
	    {
	      if (!decodeSubHeader(data,subEvtSize)) return -1;

	      offset = subEvtSize + rocEvtHeaderSize; 
	      enddata = data+offset;
	    }
	  else 
	    {
	      enddata = end;
	    }

	  //decoding data word
	  
	  for(UInt_t* deb=data+rocEvtHeaderSize; deb<enddata; deb++)
	    {
	      w->clear();
	      w->clearHeader();

	      if (deb > end)
		{ 
		  Error("fill()",
			" Event: %i -  SubEvtId %i - data pointer %p exceeding range of sub event end %p\n with calculated end point at %p",
			evSeqNr,subEvtId,deb,end,enddata);
		  return -1;
		}
		    
	      // >>>> here the decoding of the data word is done 

	      if(!w->decode(*deb, decodeVersion,consistencyCheck))
		{
		  Error("fill()"," Event: %i - SubEvtId %i - error while decoding the data-word: 0x%08x",
			evSeqNr,subEvtId,*deb);
		  if (unpackerDebug) w->dump();
		  return -1;
		}

	      // now the hardware address is matched to the sector/module/mbo/tdc coordinates of HMdcRaw 
	      // via the lookup table HMdcEvReadout

	      Int_t nsam  = -1;
	      switch (decodeVersion)
		{
		case 0:
		  nsam = w->getSam()*2+w->getSamP();
		  break;
		case 1:
		  nsam = w->getVirtualSamNr();
		  break;
		}

	      Int_t nroc  = w->getRoc();
	      Int_t nrocP = w->getRocP();
	      Int_t nbus  = w->getBus();

	      if (nsam <0 || nroc <0 || nrocP <0 || nbus <0)
		{
		  Error("fill()"," Event: %i - SubEvtId %3i - No valid address in lookup table [HMdcEvReadout] for this dataword: 0x%08x",
			evSeqNr,subEvtId,*deb);
		  if (unpackerDebug) w->dump();
		  continue;
		}

	      HMdcEvReadoutBus& bus =(*pReadout)[nsam][nroc][nrocP][nbus];

	      Int_t sector= bus.getSec(); 
	      Int_t module= bus.getMod();
	      Int_t mbo   = bus.getMbo();
	      Int_t tdc   = getTdc(w->getTdc(),w->getChan());

	      w->setAddress(sector,module,mbo,tdc,0);

	      if (sector <0 || module <0 || mbo <0 || tdc <0)
		{
		  Error("fill()"," Event: %i - SubEvtId %3i - No valid entry in lookup table [HMdcEvReadout] for this dataword: 0x%08x",
			evSeqNr,subEvtId,*deb);
		  if (unpackerDebug) w->dump();
		  continue;
		}

	      //set location indexes 
	      
	      loc[0] = sector;
	      loc[1] = module;
	      loc[2] = mbo;
	      loc[3] = tdc;

	      // filling the data to the location
	      // first get a free or existing slot

	      HMdcRaw *pMdcRaw = (HMdcRaw*) pRawCat->getObject(loc);
	      if (!pMdcRaw) 
		{
		  pMdcRaw = (HMdcRaw*) pRawCat->getSlot(loc);
		  if (pMdcRaw) 
		    {
		      pMdcRaw = new (pMdcRaw) HMdcRaw();
		      pMdcRaw->setAddress(sector,module,mbo,tdc);
		    }
		  else 
		    {
		      Error("fill()", " Event: %i - SubEvtId %i - can't get slot for s:%i m:%i mb:%i tdc:%i",
			    evSeqNr,subEvtId,sector,module,mbo,tdc);
		      return -2;
		    }
		}
	      
	      // fill the times to this slot
	      
	      if (!pMdcRaw->setTime(w->getTime(),tdcMode))
		{
		  Error("fill()"," Event: %i - SubEvtId %i - error filling slot for dataword 0xx08%",evSeqNr,subEvtId,*deb);
		  if (unpackerDebug) w->dump();
		}
	      
	      // fill dataWord to category HMdcDataWord 

	      if(debug && fillDataWord) 
 		{
		  fillMdcDataWord(sector,module,mbo,tdc);
		}
	    }
	  data += offset;
	}
    }
  return 1;
}    
