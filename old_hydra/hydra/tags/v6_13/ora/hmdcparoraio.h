#ifndef HMDCPARORAIO_H
#define HMDCPARORAIO_H

#include "hdetparoraio.h"
#include "TArrayI.h"
#include "TString.h"

class HOraConn;
class HMdcDetector;
class HMdcRawStruct;
class HMdcGeomStruct;
class HMdcMboReadout;
class HMdcCalParRaw;
class HMdcCal2ParSim;
class HMdcLookupGeom;
class HMdcLayerGeomPar;
class HMdcGeomPar;
class HMdcCellEff;
class HMdcTimeCut;
class HMdcTdcThreshold;
class HMdcClosePairCutsPar;

class HMdcParOraIo : public HDetParOraIo {
private:
  Int_t numModules;         // number of modules found in Oracle
  TArrayI* mdcIds;          // id-numbers of all Mdc's (access by sector and plane)
  TArrayI* modIds;          // id-numbers of all Mdc modules (access by id)
  TArrayI* initModules;     // array of modules initialized from Oracle
  Int_t runStart;           // start time of actual run
public:
  HMdcParOraIo(HOraConn* p=0);
  ~HMdcParOraIo();
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
  void printIds();
private:
  Int_t startIo(HParSet*);
  Int_t readIds();
  Bool_t read(HMdcRawStruct*,Int_t*);
  Bool_t read(HMdcGeomStruct*,Int_t*);
  Bool_t read(HMdcMboReadout*,Int_t*);
  Bool_t read(HMdcCalParRaw*,Int_t*);
  Bool_t read(HMdcCal2ParSim*,Int_t*);
  Bool_t read(HMdcLookupGeom*,Int_t*);
  Bool_t read(HMdcLayerGeomPar*,Int_t*);
  Bool_t read(HMdcCellEff*,Int_t*);
  Bool_t read(HMdcTimeCut*,Int_t*);
  Bool_t read(HMdcGeomPar*,Int_t*);
  Bool_t read(HMdcTdcThreshold*,Int_t*);
  Bool_t read(HMdcClosePairCutsPar*,Int_t*);
  Bool_t readModGeomNames(HMdcGeomPar*,Int_t*);
  Bool_t readLayerGeomNames(HMdcGeomPar*,Int_t*);
  Int_t getVersion(HParSet* pPar,TArrayI*);
  Int_t getVersion(HParSet* pPar,Int_t&);
  Int_t getLookupVersion(HMdcLookupGeom*);
  Int_t getReadoutVersion(HMdcMboReadout*);
  Int_t createVersion(HParSet*);
  Int_t writePar(HMdcCalParRaw*);
  Int_t writePar(HMdcCal2ParSim*);
  Int_t writePar(HMdcCellEff*);
  Int_t writePar(HMdcTimeCut*);
  Int_t writePar(HMdcClosePairCutsPar*);
  Int_t createVersion(HMdcTdcThreshold*);
  Int_t writePar(HMdcTdcThreshold*);
  Int_t countTdcChannels(Int_t);
  void printInfo(const char*);
  ClassDef(HMdcParOraIo,0) // MDC parameter I/O from Oracle
};

#endif  /* !HMDCPARORAIO_H */