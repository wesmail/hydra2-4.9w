#ifndef HRPCRAW_H
#define HRPCRAW_H

#include "hlocateddataobject.h"

class HRpcRaw : public HLocatedDataObject {
 protected:
  Float_t  rightT;    //TDC: right leading time
  Float_t  leftT;     //TDC: left leading time
  Float_t  rightW;    //TDC: right width = Time-over-Threshold
  Float_t  leftW;     //TDC: left width = Time-over-Threshold
  Short_t  rightNHits; //number of hits in right side
  Short_t  leftNHits;  //number of hits in left side
  Char_t   cell;      //Location
  Int_t    chan;      // Tdc Channel
  Int_t    flag;      //Mode Flag (see hrpcunpacker.cc)
 public:
  HRpcRaw(void);    //: cell(-1), time(0.), charge(0.) {}
  ~HRpcRaw(void);
  // Funciones getVariable
  inline Float_t getRightTime(void);
  inline Float_t getLeftTime(void);
  inline Float_t getRightTot(void);
  inline Float_t getLeftTot(void);
  inline Short_t getRightNHits(void);
  inline Short_t getLeftNHits(void);
  Char_t getCell(void) {return  cell;} 
  inline Int_t getChannel(void); 
  inline Int_t getModeFlag(void); 
  inline Int_t getNLocationIndex(void);
  inline Int_t getLocationIndex(Int_t i);
  
  // Funciones setVariable
  inline void setRightTime(Float_t arightT);
  inline void setLeftTime(Float_t aleftT);
  inline void setRightTot(Float_t arightW);
  inline void setLeftTot(Float_t aleftW);
  inline void setRightNHits(Short_t nH); 
  inline void setLeftNHits(Short_t nH); 
  void incLeftNHits(void) {leftNHits++;} 
  void incRightNHits(void) {rightNHits++;} 
  void setCell(Char_t c) { cell=c;}
  inline void setChannel(Int_t chan);
  inline void setModeFlag(Int_t flag);
  
  ClassDef(HRpcRaw,1) // Raw data class of RPC detector
};
    
inline Float_t HRpcRaw::getRightTime(void) {
  return  rightT;
}

inline Float_t HRpcRaw::getLeftTime(void) {
  return  leftT;
}

inline Float_t HRpcRaw::getRightTot(void) {
  return  rightW;
}

inline Float_t HRpcRaw::getLeftTot(void) {
  return  leftW;
}

inline Short_t HRpcRaw::getRightNHits(void) {
  return  rightNHits;
}

inline Short_t HRpcRaw::getLeftNHits(void) {
  return  leftNHits;
}

inline Int_t HRpcRaw::getChannel(void) {
  return  chan;
}

inline Int_t HRpcRaw::getModeFlag(void) {
  return  flag;
}

inline void HRpcRaw::setRightTime(Float_t arightT) {
  rightT=arightT;
}
  
inline void HRpcRaw::setLeftTime(Float_t aleftT) {
   leftT=aleftT;
}

inline void HRpcRaw::setRightTot(Float_t arightW){
   rightW=arightW;
}

inline void HRpcRaw::setLeftTot(Float_t aleftW) {
   leftW=aleftW;
}

inline void HRpcRaw::setRightNHits(Short_t nH){
   rightNHits=nH;
}

inline void HRpcRaw::setLeftNHits(Short_t nH){
   leftNHits=nH;
}

inline void HRpcRaw::setChannel(Int_t ch){
   chan=ch;
}

inline void HRpcRaw::setModeFlag(Int_t fl){
   flag=fl;
}

inline Int_t HRpcRaw::getNLocationIndex(void) {       // FIXME ?????
  return 3;
}

inline Int_t HRpcRaw::getLocationIndex(Int_t i) {     // FIXME ?????
  switch (i) {
  case 0 : return getCell(); break;
  //case 1 : return getModule(); break;
  //case 2 : return getCell(); break;
  }
  return -1;
}


#endif /* !HRPCRAW_H */