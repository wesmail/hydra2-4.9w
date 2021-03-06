//*-- AUTHOR : Ilse Koenig
//*-- Modified : 17/01/2002 by I. Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//HMdcLookupGeom
//
//  Container class for lookup parameters to map the hardware
//  tree of the Mdc (sectors, modules, motherboards, TDC,
//  TDC channel) to the geometrie tree (sectors, modules,
//  layer, cell).
//  This information is needed for data mapping from raw
//  level to cal1 level
//
/////////////////////////////////////////////////////////////

#include "hmdclookupgeom.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hmdcrawstruct.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hmessagemgr.h"

ClassImp(HMdcLookupChan)
ClassImp(HMdcLookupMoth)
ClassImp(HMdcLookupGMod)
ClassImp(HMdcLookupGSec)
ClassImp(HMdcLookupGeom)

HMdcLookupMoth::HMdcLookupMoth(Int_t n, Text_t* name) {
  // constructor creates an array of pointers of type
  // HMdcLookupChan (array of all Tdc channels on a
  // motherboard)
  SetName(name);
  array = new TObjArray(n);
  for (int i = 0; i < n; i++) {
    array->AddAt(new HMdcLookupChan(),i);
  }
}

HMdcLookupMoth::~HMdcLookupMoth() {
  // destructor
  array->Delete();
  delete array;
}

HMdcLookupGMod::HMdcLookupGMod(Int_t sec, Int_t mod, Int_t n) {
  // constructor creates an array of pointers of type
  // HMdcLookupMoth
  array = new TObjArray(n);
  HMdcRawStruct* pMdc=
       (HMdcRawStruct*)gHades->getRuntimeDb()->getContainer("MdcRawStruct");
  if (pMdc) {
    HMdcRawSecStru& pS=(*pMdc)[sec];
    HMdcRawModStru& pM=pS[mod];
    for (Int_t i = 0; i < n; i++) {
      array->AddAt(new HMdcLookupMoth(pM[i].getNTdcs(),(char*)pM[i].GetName()),i);
    }
  }
  else ERROR_msg(HMessageMgr::DET_MDC,"HMdcLookupGeom not created, container MdcRawStruct not found");
}

HMdcLookupGMod::~HMdcLookupGMod() {
  // destructor
  array->Delete();
  delete array;
}

HMdcLookupGSec::HMdcLookupGSec(Int_t sec,Int_t n) {
  // constructor creates an array of pointers of type
  // HMdcLookupGMod
  sector=sec;
  array = new TObjArray(n);
  for(Int_t i=0;i<n;i++) array->AddAt(new HMdcLookupGMod(sector,i,16),i);
}

HMdcLookupGSec::~HMdcLookupGSec() {
  // destructor
  array->Delete();
  delete array;
}

HMdcLookupGeom::HMdcLookupGeom(const char* name,const char* title,
                               const char* context,Int_t n)
               : HParSet(name,title,context) {
  // constructor creates an array of pointers of type HMdcLookupGSec
  strcpy(detName,"Mdc");
  array = new TObjArray(n);
  for(Int_t i=0;i<n;i++) array->AddAt(new HMdcLookupGSec(i,4),i);
}

HMdcLookupGeom::~HMdcLookupGeom() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HMdcLookupGeom::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HMdcParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HMdcLookupGeom::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HMdcParIo");
  if (out) return out->write(this);
  return -1;
}

void HMdcLookupGeom::readline(const char *buf, Int_t *set) {
  // decodes one line read from ascii file I/O
   Int_t sec, mod, mboInd, chan, lay, wire;
   sscanf(buf,"%i%i%i%i%i%i",
          &sec, &mod, &mboInd, &chan, &lay, &wire);
   Int_t n=sec*4+mod;
   if (!set[n]) return;
   HMdcLookupChan& r=(*this)[sec][mod][mboInd][chan];
   r.fill(lay,wire);
   set[n]=999;
   return;
}

void HMdcLookupGeom::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HMdcParAsciiFileIo
  header=
    "# Mdc lookup table (hardware->geometry)\n"
    "# Format:\n"
    "# sector  module  motherboard index  TDC channel  layer  cell\n";
}

Bool_t HMdcLookupGeom::writeline(char *buf, Int_t sec, Int_t mod, Int_t lay,
                               Int_t cell) {
  // writes one line to the buffer used by ascii file I/O
  HMdcLookupChan &chan=(*this)[sec][mod][lay][cell];
  Int_t nCell=chan.getNCell();
  if (nCell==-1) return kFALSE;
  Int_t nLay =chan.getNLayer();
  sprintf(buf,"%1i %1i %1i %3i %1i %3i\n", sec, mod, lay, cell, nLay, nCell);
  return kTRUE;
}

void HMdcLookupGeom::clear() {
  // clears the container
  for(Int_t s=0;s<6;s++) {
    HMdcLookupGSec& sec=(*this)[s];
    for(Int_t m=0;m<4;m++) {
      HMdcLookupGMod& mod=sec[m];
      Int_t nl=mod.getSize();
      for(Int_t l=0;l<nl;l++) {
        HMdcLookupMoth& moth=mod[l];
        Int_t nm=moth.getSize();
        for(Int_t c=0;c<nm;c++) {
          HMdcLookupChan& chan=moth[c];
          chan.clear();
        }
      }
    }
  }
  status=kFALSE;
  resetInputVersions();
}
void HMdcLookupGeom::printParam() {
  // prints the container
    SEPERATOR_msg("-",60);
    INFO_msg(10,HMessageMgr::DET_MDC,"HMdcLookupGeom :");
    for(Int_t s=0;s<6;s++) {
    HMdcLookupGSec& sec=(*this)[s];
    for(Int_t m=0;m<4;m++) {
      HMdcLookupGMod& mod=sec[m];
      Int_t nl=mod.getSize();
      for(Int_t l=0;l<nl;l++) {
        HMdcLookupMoth& moth=mod[l];
        Int_t nm=moth.getSize();
        for(Int_t c=0;c<nm;c++) {
          HMdcLookupChan& chan=moth[c];
	  Int_t nCell=chan.getNCell();
	  Int_t nLay =chan.getNLayer();

	  gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
				 ,"%1i %1i %1i %3i %1i %3i\n",
				 s, m, l, c, nLay, nCell);
	}
      }
    }
  }
    SEPERATOR_msg("-",60);
}
