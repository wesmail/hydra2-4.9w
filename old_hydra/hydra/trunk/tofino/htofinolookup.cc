//*-- AUTHOR Ilse Koenig
//*-- Modified : 14/12/2001 by Ilse Koenig
//*-- created  : 30/11/2000 by Ilse Koenig
//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////
// HTofinoLookup
//
// Container class for mapping Crate/TdcAdc/Channel to Module/Paddle
//   needed by the TOFINO unpacker
//
////////////////////////////////////////////////////////////////////
using namespace std;
#include "htofinolookup.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HTofinoLookupChan)
ClassImp(HTofinoLookupDConv)
ClassImp(HTofinoLookup)

HTofinoLookupDConv::HTofinoLookupDConv(Int_t n) {
  // constructor creates an array of pointers of type
  // HTofinoLookupChan
  nChan=n;
  crate=-1;
  slot=-1;
  dConvType='U';
  array = new TObjArray(n);
  //for(Int_t i=0;i<n;i++) (*array)[i]=new HTofinoLookupChan();
  for(Int_t i=0;i<n;i++) array->AddAt(new HTofinoLookupChan(),i);
}

HTofinoLookupDConv::~HTofinoLookupDConv() {
  // destructor
  array->Delete();
  delete array;
}

void HTofinoLookupDConv::clear() {
  crate=-1;
  slot=-1;
  dConvType='U';
  for(Int_t i=0;i<nChan;i++) (*this)[i].clear();
}

HTofinoLookup::HTofinoLookup(const Char_t* name,const Char_t* title,
                             const Char_t* context,
                             Int_t numDConv, Int_t numChan)
  : HTofinoParSet(name,title,context) {
  // constructor creates an array of pointers of type HTofinoLookupDConv
  nDConv=numDConv;
  array = new TObjArray(nDConv);
  //for (Int_t i=0;i<nDConv;i++) (*array)[i]=new HTofinoLookupDConv(numChan);
  for(Int_t i=0;i<nDConv;i++) array->AddAt(new HTofinoLookupDConv(),i);
}

HTofinoLookup::~HTofinoLookup() {
  // destructor
  array->Delete();
  delete array;
}

void HTofinoLookup::clear() {
  // clears the container
  for(Int_t i=0;i<nDConv;i++) {
    (*this)[i].clear();
  }
  status=kFALSE;
  resetInputVersions();
}

void HTofinoLookup::printParam() {
  // prints the calibration parameters
  printf("Lookup table for the TOFINO unpacker\n");
  printf("crate  slot  type  channel  sector  module  paddle\n");
  for(Int_t i=0;i<nDConv;i++) {
    HTofinoLookupDConv& dc=(*this)[i];
    Char_t dConvType=dc.dConvType;
    if (dConvType!='U') {
      for(Int_t j=0;j<dc.getSize();j++) {
        HTofinoLookupChan& chan=dc[j];
        if (chan.getModule()>=0) printf("%4i%4i   %c%4i%4i%4i%4i\n",
            dc.crate,dc.slot,dc.dConvType,
            j,chan.getSector(),chan.getModule(),chan.getCell());
      }
    }
  }
}

void HTofinoLookup::setAddress(Int_t crate, Int_t slot, Char_t dcType,
                Int_t channel, Int_t sector, Int_t module, Int_t cell) {
  // Fills the objects of type HTofinoLookupDConv.
  // Adds a new objects, if necessary (different crate and/or slot).
  for(Int_t i=0;i<nDConv;i++) {
    HTofinoLookupDConv& dc=(*this)[i];
    if (dc.crate==-1) {
      dc.setAddress(crate,slot,dcType);
      dc[channel].fill(sector,module,cell);
      return;
    }
    if (dc.crate==crate && dc.slot==slot) {
      dc[channel].fill(sector,module,cell);
      return;
    }
  }
  array->Expand(++nDConv);
  HTofinoLookupDConv* dc=new HTofinoLookupDConv((*this)[0].getSize());
  dc->setAddress(crate,slot,dcType);
  ((*dc)[channel]).fill(sector,module,cell);
  //(*array)[nDConv-1]=dc;
  array->AddAt(dc,nDConv-1); 
}

void HTofinoLookup::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ascii file I/O
  Int_t crate, slot, chan, sec, mod, cell;
  Char_t dcType[2];
  sscanf(buf,"%i%i%s%i%i%i%i",&crate,&slot,dcType,&chan,&sec,&mod,&cell);
  if (set[sec]) {
    setAddress(crate,slot,dcType[0],chan,sec,mod,cell);
    set[sec]=999;
  }
}

void HTofinoLookup::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HTofinoParAsciiFileIo
  header=
    "# Lookup table for the TOFINO unpacker\n"
    "# Format:\n"
    "# crate  slot  type  channel  sector  module  paddle\n";
}

Bool_t HTofinoLookup::writeline(Char_t *buf, Int_t nDConv, Int_t chan) {
  // writes one line to the buffer used by ascii file I/O
  HTofinoLookupDConv& dc=(*this)[nDConv];
  HTofinoLookupChan& c=dc[chan];
  if (c.getSector()>=0) {
    sprintf(buf,"%4i%4i   %c%4i%4i%4i%4i\n",
            dc.crate,dc.slot,dc.dConvType,chan,c.getSector(),c.getModule(),
            c.getCell());
    return kTRUE;
  }
  return kFALSE;
}
