#ifndef HShowerHitFPar_H
#define HShowerHitFPar_H
#pragma interface

#include "hparset.h"
#include "hlocation.h"
#include "hadioascii.h"
#include "hshowerpad.h"
#include "hshowerwire.h"
#include "hshowerframe.h"

class HShowerHitFPar : public HParSet {
public:
  HShowerHitFPar();
  ~HShowerHitFPar();

  void setHadIoAscii(HadIoAscii *pReadParam);
  HadIoAscii* getHadIoAscii();

  virtual Bool_t init();
  virtual void clear();

  Int_t setColBord(Int_t nSect, Int_t nMod, Int_t nLowerCol, Int_t nUpperCol);
  Int_t setRowBord(Int_t nSect, Int_t nMod, Int_t nLowerRow, Int_t nUpperRow);
  Int_t getColBord(Int_t nSect, Int_t nMod, Int_t* nLowerCol, Int_t* nUpperCol);
  Int_t getRowBord(Int_t nSect, Int_t nMod, Int_t* nLowerRow, Int_t* nUpperRow);

  Int_t getModules(){return nModules;}
  Int_t getSectors(){return nSectors;}
  Int_t getThreshold(){return nThreshold;}

  void setModules(Int_t nMod){nModules = nMod;}
  void setSectors(Int_t nSect){nSectors = nSect;}
  void setThreshold(Int_t t){nThreshold = t;}

  ClassDef(HShowerHitFPar,1) //ROOT extension
public:
  void defaultInit();
  Int_t getIndex(Int_t nSect, Int_t nMod);

  Int_t nSectors;
  Int_t nModules;
  Int_t *pRowBorder;
  Int_t *pColBorder;

  Int_t nThreshold;

  HadIoAscii *m_pReadParam;
};

#endif