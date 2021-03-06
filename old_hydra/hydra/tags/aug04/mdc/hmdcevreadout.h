#ifndef HMDCEVREADOUT_H
#define HMDCEVREADOUT_H

#include "TObjArray.h"
#include "hparset.h"

class HParIo;

class HMdcEvReadoutBus: public TObject {
protected:
  Int_t sec;  // module index
  Int_t mod;   // sector index
  Int_t mbo;  // Motherboard index
public:
  HMdcEvReadoutBus(Int_t s=-1,Int_t m=-1, Int_t l=-1) {
    sec=s;
    mod=m;
    mbo=l;
  }
  ~HMdcEvReadoutBus() {}
  void setSec(Int_t n) {sec=n;}
  void setMod(Int_t n) {mod=n;}
  void setMbo(Int_t n) {mbo=n;}
  void fill(Int_t s,Int_t m, Int_t l) {
    sec=s;
    mod=m;
    mbo=l;
  }
  Int_t getSec() {return sec;}
  Int_t getMod() {return mod;}
  Int_t getMbo() {return mbo;}
  void clear() {
    sec=-1;
    mod=-1;
    mbo=-1;
  }
  ClassDef(HMdcEvReadoutBus,1) // Bus level of MDC Readout table 
};


class HMdcEvReadoutBusPos: public TObject {
protected:
  TObjArray* array;
public:
  HMdcEvReadoutBusPos(Int_t n=4);
  ~HMdcEvReadoutBusPos();
  HMdcEvReadoutBus& operator[](Int_t i) {
    return *static_cast<HMdcEvReadoutBus*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcEvReadoutBusPos,1) // Bus position level of MDC Readout table
};


class HMdcEvReadoutRoc: public TObject {
protected:
  TObjArray* array;
public:
  HMdcEvReadoutRoc(Int_t n=2);
  ~HMdcEvReadoutRoc();
  HMdcEvReadoutBusPos& operator[](Int_t i) {
    return *static_cast<HMdcEvReadoutBusPos*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcEvReadoutRoc,1) // ROC level of MDC Readout table
};

class HMdcEvReadoutSam: public TObject {
protected:
  TObjArray* array;
  Int_t sector;
public:
  HMdcEvReadoutSam(Int_t n=8);
  ~HMdcEvReadoutSam();
  HMdcEvReadoutRoc& operator[](Int_t i) {
    return *static_cast<HMdcEvReadoutRoc*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcEvReadoutSam,1) // SAM level of MDC Readout table
};


class HMdcEvReadout : public HParSet {
protected:
  TObjArray* array;         // array of pointers
  Int_t *samNumberToSector; //! fast lookup table from samNumber = samNo + samPosition to sector
  Int_t *samNumberToModule; //! fast lookup table from samNumber = samNo + samPosition to module 
  Int_t arrayBaseSize;      //!
public:
  HMdcEvReadout(const char* name="MdcEvReadout",
                const char* title="Mdc subevent readout addresses",
                const char* context="",
                Int_t n=48);
  ~HMdcEvReadout();
  HMdcEvReadoutSam& operator[](Int_t i) {
    return *static_cast<HMdcEvReadoutSam*>((*array)[i]);
  }
  Bool_t init(HParIo* input);
  Int_t write(HParIo* output);
  Int_t getArrayBaseSize(){return arrayBaseSize;};
  Int_t getSize() {return array->GetEntries();};
  Int_t getSamNumberToSector(Int_t samNo) 
    {
      if (0 <= samNo && samNo < arrayBaseSize) return samNumberToSector[samNo];
      Error("getSamNumberToSector()","sam number %i out of range [%i,%i[ of lookup table",samNo,0,arrayBaseSize);
      return -1;
    };
  Int_t getSamNumberToModule(Int_t samNo) 
    {
      if (0 <= samNo && samNo < arrayBaseSize) return samNumberToModule[samNo];
      Error("getSamNumberToModule()","sam number %i out of range [%i,%i[ of lookup table",samNo,0,arrayBaseSize);
      return -1;
    };
  Int_t getModuleAndSectorToSamNumber(Int_t moduleNo, Int_t sectorNo) 
			{
					for (Int_t index = 0; index < arrayBaseSize; index++)
							{
									if (moduleNo == samNumberToModule[index]  && 
											sectorNo == samNumberToSector[index]    ) return index;
							}
					Error("getModuleAndSectorToSamNumber()","no matching sam number to module %i and sector %i found", moduleNo, sectorNo);
					return -1;
			};
  void clear();
  void printParam();
  ClassDef(HMdcEvReadout,1) //  MDC Event Readout addresses
};

#endif /* !HMDCEVREADOUT_H */







