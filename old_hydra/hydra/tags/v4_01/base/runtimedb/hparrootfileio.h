// File: hparrootfileio.h
// Author: Ilse Koenig
//
/////////////////////////////////////////////////////////////////////////
// Interface class for parameter I/O from ROOT file                    //
/////////////////////////////////////////////////////////////////////////

#ifndef HPARROOTFILEIO_H
#define HPARROOTFILEIO_H

#include "hpario.h"
#include "TFile.h"
#include "TList.h"
#include <string.h>

class HSpectrometer;
class HEventFile;

class HParRootFile : public TFile {
public:
  HEventFile* eventFile;     //! current parameter event file in ROOT file
  HParRootFile(Text_t* fname, Option_t* option="READ",
               Text_t* ftitle="",Int_t compress=0);
  ~HParRootFile();
  void readVersions(HEventFile* file);
  ClassDef(HParRootFile,0) // ROOT file for Parameter I/O 
};


class HParRootFileIo : public HParIo {
protected:
  HParRootFile* file;  // pointer to ROOT file
public:
  HParRootFileIo();
  ~HParRootFileIo();
  Bool_t open(Text_t* fname, Option_t* option="READ",
              Text_t* ftitle="",Int_t compress=0);
  void close();
  void print();
  HParRootFile* getParRootFile();
  void readVersions(HEventFile* file);
  TList* getList();
  Bool_t check() {
    // returns kTRUE if file is open
    if (file) return file->IsOpen();
    else return kFALSE;
  }
  void cd() {
    // sets the global ROOT file pointer gFile
    if (file) file->cd();
  }
  ClassDef(HParRootFileIo,0) // Parameter I/O from ROOT files
};

#endif  /* !HPARROOTFILEIO_H */
