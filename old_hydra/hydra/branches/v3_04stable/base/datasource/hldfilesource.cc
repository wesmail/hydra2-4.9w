//*-- Author : Ilse Koenig
//*-- Modified : 26/11/98 by Ilse Koenig
//*-- modified on 16/3/99 by R. Holzmann
//*-- Modified by D.B (Dump of events)
//////////////////////////////////////////////////////////
//HLdFileSource
//
//  This "data source" reads the LMD data from files on disk.
//  (further information in base class HldSource)
//
///////////////////////////////////////////////////////////////////////////////

#include "hldfilesource.h"
#include "hrecevent.h"
#include "hldfilevt.h"
#include "hades.h"
#include "hruntimedb.h"
#include "heventfile.h"
#include <iostream.h>

ClassImp(HldFileSource)

HldFileSource::HldFileSource() : HldSource() {
  // Default constructor for a Lmd file data source.
  fReadEvent=0;
  iter=0;
}

HldFileSource::~HldFileSource(void) {
  // Destructor for a LMD file data source
  if (fUnpackerList) fUnpackerList->Delete();
  fUnpackerList=0;
  if (fReadEvent) delete fReadEvent;
  fReadEvent=0;
  if (iter) delete iter;
}

Bool_t HldFileSource::init() {
  // initializes the unpackers and creates the fReadEvent of the first file
  if (!(*fEventAddr)) {
    (*fEventAddr) = new HRecEvent;
  }
  if (isDumped || isScanned)  return getNextFile();  
  iter=fUnpackerList->MakeIterator();
  if (initUnpacker()==kFALSE) return kFALSE;
  return getNextFile();
}

EDsState HldFileSource::getNextEvent() {
  // loops over the event files in the runtime database
  // Tries to read a new event from the LMD file and put the info into the 
  // HEvent structure:
  if(isDumped) {
        if (dumpEvt()== kDsError){
          if (getNextFile()==kFALSE) return kDsEndData;
        else return kDsEndFile;
        }
        return kDsOk;
  }
  if (isScanned) {
    if (scanEvt()== kDsError){
      if (getNextFile()==kFALSE) return kDsEndData;
      else return kDsEndFile;
    }
    return kDsOk; 
  }

  if (! (HldFilEvt*)fReadEvent->execute()) {
    //End of current file
    if (getNextFile()==kFALSE) return kDsEndData;
    else return kDsEndFile;
  }

  decodeHeader();
  iter->Reset();
  HldUnpack *unpacker;
  while ( (unpacker=(HldUnpack *)iter->Next())!=NULL) {
    if (!unpacker->execute()) return kDsError;
  }
  return kDsOk;
}
 
Bool_t HldFileSource::getNextFile() {
  HEventFile* file=gHades->getRuntimeDb()->getNextEventFile();
  if (!file) return kFALSE;
  if (fReadEvent) delete fReadEvent;
  fReadEvent=new HldFilEvt(file->GetName()); 
  
  if(isDumped || isScanned) return kTRUE; 
  
  iter->Reset();
  HldUnpack *unpacker;
  while ( (unpacker=(HldUnpack *)iter->Next())!=NULL) {
    fReadEvent->appendSubEvtIdx((*unpacker));
  }
  return kTRUE;
}



