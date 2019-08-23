#ifndef HMDCLISTCELLS_H
#define HMDCLISTCELLS_H

#include "hmdctbarray.h"
#include "TObject.h"

class HMdcList12GroupCells;

class HMdcLayListCells : public TObject {
  protected:
    enum { arrSize=56,    // array size 
           maxCells=224}; // max.number of cells in layer (= arrSize*4)
    UChar_t arr[arrSize]; // cells lis (two bits per wire)
    UChar_t* arrEnd;      // pointer of last element in "arr"
    Float_t* drTime1;     // array of drift times (time1!)
    Float_t* drTime2;     // array of drift times (time2!)
    Int_t*   gnTrackNum1; // array of geant track num. (time1!)
    Int_t*   gnTrackNum2; // array of geant track num. (time2!)
    Float_t* tof1;        // array of TOFs (time1!)
    Float_t* tof2;        // array of TOFs (time2!)
  public:
    HMdcLayListCells() {init();}
    ~HMdcLayListCells(void);
    void copyListOfCells(HMdcLayListCells& lst);
    inline void    clear(void);
    inline Bool_t  setTime(Int_t cell,UChar_t time);
    inline void    delTime(Int_t cell,UChar_t time);
    inline Int_t   getNCells(void) const;
    inline Int_t   getCell(Int_t idx) const;
    inline Int_t   getFirstCell(void) const;
    inline Int_t   getLastCell(void) const;
    inline Int_t   getGeantTrackT1(Int_t cell) const;
    inline Int_t   getGeantTrackT2(Int_t cell) const;
    inline Int_t   getGeantTrack(Int_t cell,UChar_t tm) const;
    inline Int_t   next(Int_t prCell) const;
    inline Bool_t  nextCell(Int_t& cell) const;
    inline Int_t   previous(Int_t prCell) const;
    inline UChar_t getTime(Int_t cell) const;
    inline Bool_t  isCell(Int_t cell) const;
    void           print(void) const;
    
    inline Bool_t  addTime1(Int_t cell, Float_t timeValue);
    inline Bool_t  nextTime(Int_t& cell, Float_t& timeValue) const;
    inline Float_t getTimeValue(Int_t cell) const;
    Bool_t         addCell(Int_t c,UChar_t tm,Float_t dt1,Float_t dt2);
    Bool_t         addCellSim(Int_t c,UChar_t tm,Float_t dt1,Float_t dt2,
                              Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2);
    UChar_t        nextCell(Int_t& c,Float_t& dt1,Float_t& dt2) const;
    UChar_t        nextCellSim(Int_t& c,Float_t& dt1,Float_t& dt2,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    UChar_t        nextCellSim(Int_t& c,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;

  protected:
    void init(void);
    inline Bool_t boundsOk(Int_t cell) const;

  ClassDef(HMdcLayListCells,0) //List of all cells in layer
};

inline void HMdcLayListCells::copyListOfCells(HMdcLayListCells& lst) {
  memcpy(arr,lst.arr,arrSize);
}
  
inline Bool_t HMdcLayListCells::boundsOk(Int_t cell) const {
  return (cell>=0 && cell < maxCells) ? kTRUE : kFALSE;
}

inline void HMdcLayListCells::clear(void) {
  HMdcTBArray::clear(arr,arrEnd);
}

inline Bool_t HMdcLayListCells::setTime(Int_t cell,UChar_t time) {
  if(!boundsOk(cell)) return kFALSE;
  HMdcTBArray::set(arr,cell,time);
  return kTRUE;
}

inline void HMdcLayListCells::delTime(Int_t cell,UChar_t time) {
  if(boundsOk(cell)) HMdcTBArray::unset(arr,cell,time);
}

inline Int_t HMdcLayListCells::getNCells(void) const {
  return HMdcTBArray::getNSet(arr,arrEnd);
}

inline Int_t HMdcLayListCells::getCell(Int_t idx) const {
  return HMdcTBArray::position(arr,arrEnd,idx);
}

inline Int_t HMdcLayListCells::getFirstCell(void) const {
  return HMdcTBArray::first(arr,arrEnd);
}

inline Int_t HMdcLayListCells::getLastCell(void) const {
  return HMdcTBArray::last(arr,arrEnd);
}

inline Int_t HMdcLayListCells::next(Int_t prCell) const {
  return HMdcTBArray::next(arr,arrEnd,prCell);
}

inline Bool_t HMdcLayListCells::nextCell(Int_t& cell) const {
  cell=HMdcTBArray::next(arr,arrEnd,cell);
  return (cell<0) ? kFALSE:kTRUE;
}

inline Int_t HMdcLayListCells::previous(Int_t prCell) const {
  return HMdcTBArray::previous(arr,arrEnd,prCell);
}


inline UChar_t HMdcLayListCells::getTime(Int_t cell) const {
  if(boundsOk(cell)) return HMdcTBArray::get(arr,cell);
  return 0;
}

inline Bool_t HMdcLayListCells::isCell(Int_t cell) const {
  return (getTime(cell)) ? kTRUE:kFALSE;
}

inline Bool_t HMdcLayListCells::addTime1(Int_t cell, Float_t timeValue) {
  // use this function for for storing time1 only
  if(!boundsOk(cell)) return kFALSE;
  if(drTime1==0) drTime1 =new Float_t [maxCells];
  HMdcTBArray::set(arr,cell,1);  // time1 only !!!
  drTime1[cell]=timeValue;
  return kTRUE;
}

inline Bool_t HMdcLayListCells::nextTime(Int_t& cell, Float_t& timeValue) 
    const {
  if(!nextCell(cell) || drTime1==0) return kFALSE;
  timeValue=drTime1[cell];
  return kTRUE;
}

inline Float_t HMdcLayListCells::getTimeValue(Int_t cell) const {
  return (drTime1 && getTime(cell)) ? drTime1[cell]:-999.F;
}

inline Int_t HMdcLayListCells::getGeantTrackT1(Int_t cell) const {
  if(boundsOk(cell) && gnTrackNum1!=0) return gnTrackNum1[cell];
  return 0;
}

inline Int_t HMdcLayListCells::getGeantTrackT2(Int_t cell) const {
  if(boundsOk(cell) && gnTrackNum2!=0) return gnTrackNum2[cell];
  return 0;
}

inline Int_t HMdcLayListCells::getGeantTrack(Int_t cell,UChar_t tm) const {
  if(tm == 1) return getGeantTrackT1(cell);
  if(tm == 2) return getGeantTrackT2(cell);
  return 0;
}
    
// - - - - - - - - - - - - - - - - - - - - - - - - - -

class HMdcModListCells : public TObject {
  protected:
    HMdcLayListCells list[6];
  public:
    HMdcModListCells(void) {clear();}
    ~HMdcModListCells(void) {}
    inline HMdcLayListCells& operator[](Int_t l);
    
    inline void    clear(void);
    inline Bool_t  setTime(Int_t lay, Int_t cell, UChar_t time);
    inline void    delTime(Int_t lay, Int_t cell, UChar_t time);
    inline Int_t   getNCells(void) const;
    inline Bool_t  nextCell(Int_t& lay, Int_t& cell) const;
    inline UChar_t getTime(Int_t lay, Int_t cell) const;
    inline Bool_t  isCell(Int_t lay, Int_t cell) const;
    inline Int_t   getNLayers(void) const;
    void           print(void) const;
    
    inline Bool_t  addTime1(Int_t l, Int_t c, Float_t tm);
    inline Bool_t  nextTime(Int_t& l, Int_t& c, Float_t& tm) const;
    inline Float_t getTimeValue(Int_t l, Int_t c) const;
    inline Bool_t  addCell(Int_t l,Int_t c,UChar_t tm,Float_t dt1,Float_t dt2);
    inline Bool_t  addCellSim(Int_t l,Int_t c,UChar_t tm,
                              Float_t dt1,Float_t dt2,
                              Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2);
    UChar_t        nextCell(Int_t& l,Int_t& c,Float_t& dt1,Float_t& dt2) const;
    UChar_t        nextCellSim(Int_t& l,Int_t& c,Float_t& dt1,Float_t& dt2,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    UChar_t        nextCellSim(Int_t& l,Int_t& c,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;

  protected:
    inline Bool_t  nLayOk(Int_t l) const {return (l>=0&&l<6) ? kTRUE:kFALSE;}

  ClassDef(HMdcModListCells,0) //List of all cells in layer
};

inline HMdcLayListCells& HMdcModListCells::operator[](Int_t l) {
  if(nLayOk(l)) return list[l];
  HMdcLayListCells* null=0;
  return *null;
}
    
inline void HMdcModListCells::clear(void) {
  for(Int_t l=0;l<6;l++) list[l].clear();
}

inline Bool_t HMdcModListCells::setTime(Int_t lay, Int_t cell, UChar_t time) {
  if(nLayOk(lay)) return list[lay].setTime(cell,time);
  return kFALSE;
}

inline Bool_t HMdcModListCells::nextCell(Int_t& lay, Int_t& cell) const {
  if(lay<0) {
    lay=0;
    cell=-1;
  }
  for(;lay<6;lay++) if(list[lay].nextCell(cell)) return kTRUE;
  lay=-1;
  return kFALSE;
}

inline void HMdcModListCells::delTime(Int_t lay, Int_t cell, UChar_t time) {
  if(nLayOk(lay)) list[lay].delTime(cell,time);
}

inline Int_t HMdcModListCells::getNCells(void) const {
  Int_t nCells=0;
  for(Int_t l=0;l<6;l++) nCells+=list[l].getNCells();
  return nCells;
}

inline UChar_t HMdcModListCells::getTime(Int_t lay, Int_t cell) const {
  if(nLayOk(lay)) return list[lay].getTime(cell);
  return 0;
}

inline Int_t HMdcModListCells::getNLayers(void) const {
  Int_t nLay=0;
  for(Int_t l=0;l<6;l++) if(list[l].getFirstCell()>=0) nLay++;
  return nLay;
}

inline Bool_t HMdcModListCells::isCell(Int_t lay, Int_t cell) const {
  if(nLayOk(lay)) return list[lay].isCell(cell);
  return kFALSE;
}


inline Bool_t HMdcModListCells::addTime1(Int_t l, Int_t c, Float_t tm) {
  if(nLayOk(l)) return list[l].addTime1(c,tm);
  return kFALSE;
}

inline Bool_t HMdcModListCells::nextTime(Int_t& l, Int_t& c, Float_t& tm) 
    const {
  if(!nextCell(l,c)) return kFALSE;
  tm=getTimeValue(l,c);
  return kTRUE;
}

inline Float_t HMdcModListCells::getTimeValue(Int_t l,Int_t c) 
    const {
  if(nLayOk(l)) return list[l].getTimeValue(c);
  return -999.F;
}

inline Bool_t HMdcModListCells::addCell(Int_t l,Int_t c,UChar_t tm,
    Float_t dt1,Float_t dt2) {
  if(nLayOk(l)) return list[l].addCell(c,tm,dt1,dt2);
  return kFALSE; 
}

inline Bool_t HMdcModListCells::addCellSim(Int_t l,Int_t c,
    UChar_t tm,Float_t dt1,Float_t dt2,
    Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2) {
  // tm can be eq. 1(time1 only), 2(time2 only) or 3(both)
  // dt1 and dt2 - driftTime1 and driftTime2 from HMdcCal1
  // tr1 and tr2 - geant track numbers for driftTime1 and driftTime2
  if(nLayOk(l)) return list[l].addCellSim(c,tm,dt1,dt2,tr1,tr2,tf1,tf2);
  return kFALSE; 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - -

class HMdcSecListCells : public TObject {
  protected:
    HMdcModListCells list[4];
  public:
    HMdcSecListCells(void) {clear();}
    ~HMdcSecListCells(void) {}
    inline HMdcModListCells& operator[](Int_t m);
    
    inline void    clear(void);
    inline Bool_t  setTime(Int_t m, Int_t l, Int_t c, UChar_t t);
    inline void    delTime(Int_t m, Int_t l, Int_t c, UChar_t t);
    inline Int_t   getNCells(void) const;
    inline Bool_t  nextCell(Int_t& m, Int_t& l, Int_t& c) const;
    inline UChar_t getTime(Int_t m, Int_t l, Int_t c) const;
    inline Int_t   getNLayers(void) const;
    void           print(void) const;

    inline Bool_t  addTime1(Int_t m, Int_t l, Int_t c, Float_t tm);
    inline Bool_t  nextTime(Int_t& m, Int_t& l, Int_t& c, Float_t& tm) const;
    inline Float_t getTimeValue(Int_t m, Int_t l, Int_t c) const;
    inline Bool_t  addCell(Int_t m,Int_t l,Int_t c,UChar_t tm,
                           Float_t dt1,Float_t dt2);
    inline Bool_t  addCellSim(Int_t m,Int_t l,Int_t c,UChar_t tm,
                              Float_t dt1,Float_t dt2,
                              Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2);
    UChar_t        nextCell(Int_t& m,Int_t& l,Int_t& c,
                            Float_t& dt1,Float_t& dt2) const;
    UChar_t        nextCellSim(Int_t& m,Int_t& l,Int_t& c,
                       Float_t& dt1,Float_t& dt2,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    UChar_t        nextCellSim(Int_t& m,Int_t& l,Int_t& c,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;

  protected:
    inline Bool_t  nModOk(Int_t m) const {return (m>=0&&m<4) ? kTRUE:kFALSE;}

  ClassDef(HMdcSecListCells,0) //List of all cells in layer
};

inline HMdcModListCells& HMdcSecListCells::operator[](Int_t m) {
  if(nModOk(m)) return list[m];
  HMdcModListCells* null=0;
  return *null;
}
  
inline void HMdcSecListCells::clear(void) {
  for(Int_t m=0;m<4;m++) list[m].clear();
}

inline Bool_t HMdcSecListCells::setTime(Int_t m, Int_t l, Int_t c, UChar_t t) {
  if(nModOk(m)) return list[m].setTime(l,c,t);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::nextCell(Int_t& m, Int_t& l, Int_t& c) const {
  if(m<0) {
    m=l=0;
    c=-1;
  }
  for(;m<4;m++) if(list[m].nextCell(l,c)) return kTRUE;
  m=-1;
  return kFALSE;
}

inline void HMdcSecListCells::delTime(Int_t m, Int_t l, Int_t c, UChar_t t) {
  if(nModOk(m)) list[m].delTime(l,c,t);
}

inline Int_t HMdcSecListCells::getNCells(void) const {
  Int_t nCells=0;
  for(Int_t m=0;m<4;m++) nCells+=list[m].getNCells();
  return nCells;
}

inline UChar_t HMdcSecListCells::getTime(Int_t m, Int_t l, Int_t c) const {
  if(nModOk(m)) return list[m].getTime(l,c);
  return 0;
}

inline Int_t HMdcSecListCells::getNLayers(void) const {
  Int_t nLay=0;
  for(Int_t m=0;m<4;m++) nLay+=list[m].getNLayers();
  return nLay;
}

inline Bool_t HMdcSecListCells::addTime1(Int_t m, Int_t l, Int_t c, Float_t tm) {
  if(nModOk(m)) return list[m].addTime1(l,c,tm);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::nextTime(Int_t& m, Int_t& l, Int_t& c, 
    Float_t& tm) const {
  if(!nextCell(m,l,c)) return kFALSE;
  tm=getTimeValue(m,l,c);
  return kTRUE;
}

inline Float_t HMdcSecListCells::getTimeValue(Int_t m, Int_t l, Int_t c)
    const {
  if(nModOk(m)) return list[m].getTimeValue(l,c);
  return -999.F;
}

inline Bool_t HMdcSecListCells::addCell(Int_t m, Int_t l,Int_t c,UChar_t tm,
    Float_t dt1,Float_t dt2) {
  if(nModOk(m)) return list[m].addCell(l,c,tm,dt1,dt2);
  return kFALSE; 
}

inline Bool_t HMdcSecListCells::addCellSim(Int_t m, Int_t l,Int_t c,
    UChar_t tm,Float_t dt1,Float_t dt2,
    Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2) {
  // tm can be eq. 1(time1 only), 2(time2 only) or 3(both)
  // dt1 and dt2 - driftTime1 and driftTime2 from HMdcCal1
  // tr1 and tr2 - geant track numbers for driftTime1 and driftTime2
  if(nModOk(m)) return list[m].addCellSim(l,c,tm,dt1,dt2,tr1,tr2,tf1,tf2);
  return kFALSE; 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - -
class HCategory;
class HIterator;
class HMdcTimeCut;

class HMdcEvntListCells : public TObject {
  protected:
    HMdcSecListCells list[6];
    HCategory*                pMdcCal1Cat;   // category MdcCal1
    HIterator*                iterMdcCal1;   // iterator for MdcCal1 category
    Bool_t                    isGeant;       // =kTRUE for GEANT data.
    HMdcTimeCut*              pDrTimeCut;    // drift time cuts 
    static HMdcEvntListCells* pGlobalLCells; // pointer to the global object
  public:
    HMdcEvntListCells(void);
    ~HMdcEvntListCells(void);
    static HMdcEvntListCells* getExObject(void) {return pGlobalLCells;}
    static HMdcEvntListCells* getObject(void);
    static void               deleteCont(void);
    inline HMdcSecListCells& operator[](Int_t s);
    Bool_t setMdcCal1Cat(void);
    Int_t  collectWires(Int_t sec=-1);
    
    inline void    clear(void);
    inline Bool_t  setTime(Int_t s, Int_t m, Int_t l, Int_t c, UChar_t t);
    inline void    delTime(Int_t s, Int_t m, Int_t l, Int_t c, UChar_t t);
    inline Int_t   getNCells(void) const;
    inline Bool_t  nextCell(Int_t& s, Int_t& m, Int_t& l, Int_t& c) const;
    inline Bool_t  nextCell(Int_t& s,Int_t& m,Int_t& l,Int_t& c,UChar_t& tm) const;
    inline UChar_t getTime(Int_t s, Int_t m, Int_t l, Int_t c) const;
    void print(void) const;

    inline Bool_t  addTime1(Int_t s, Int_t m, Int_t l, Int_t c, Float_t tm);
    inline Bool_t  nextTime(Int_t& s, Int_t& m, Int_t& l, Int_t& c, Float_t& tm) const;
    inline Float_t getTimeValue(Int_t s, Int_t m, Int_t l, Int_t c) const;
    inline Bool_t  addCell(Int_t s,Int_t m,Int_t l,Int_t c,UChar_t tm,
                           Float_t dt1,Float_t dt2);
    inline Bool_t  addCellSim(Int_t s,Int_t m,Int_t l,Int_t c,UChar_t tm,
                           Float_t dt1,Float_t dt2,
                           Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2);
    UChar_t        nextCell(Int_t& s,Int_t& m,Int_t& l,Int_t& c,
                            Float_t& dt1,Float_t& dt2) const;
    UChar_t        nextCellSim(Int_t& s,Int_t& m,Int_t& l,Int_t& c,
                       Float_t& dt1,Float_t& dt2,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    UChar_t        nextCellSim(Int_t& s,Int_t& m,Int_t& l,Int_t& c,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
  protected:
    inline Bool_t nSecOk(Int_t s) const {return (s>=0&&s<6) ? kTRUE:kFALSE;}

  ClassDef(HMdcEvntListCells,0) //List of all cells in layer
};

inline HMdcSecListCells& HMdcEvntListCells::operator[](Int_t s) {
  if(nSecOk(s)) return list[s];
  HMdcSecListCells* null=0;
  return *null;
}
  
inline void HMdcEvntListCells::clear(void) {
  for(Int_t s=0;s<6;s++) list[s].clear();
}

inline Bool_t HMdcEvntListCells::setTime(Int_t s, Int_t m, Int_t l, Int_t c, 
    UChar_t t) {
  if(nSecOk(s)) return list[s].setTime(m,l,c,t);
  return kFALSE;
}

inline Bool_t HMdcEvntListCells::nextCell(Int_t& s,Int_t& m,Int_t& l,Int_t& c) 
    const {
  if(s<0) {
    s=m=l=0;
    c=-1;
  }
  for(;s<6;s++) if(list[s].nextCell(m,l,c)) return kTRUE;
  s=-1;
  return kFALSE;
}

inline Bool_t HMdcEvntListCells::nextCell(Int_t& s,Int_t& m,Int_t& l,Int_t& c,
    UChar_t& tm) const {
  if(nextCell(s,m,l,c)) {
    tm=getTime(s,m,l,c);
    if(tm) return kTRUE;
  }
  return kFALSE;
}

inline void HMdcEvntListCells::delTime(Int_t s, Int_t m, Int_t l, Int_t c, 
    UChar_t t) {
  if(nSecOk(s)) list[s].delTime(m,l,c,t);
}

inline Int_t HMdcEvntListCells::getNCells(void) const {
  Int_t nCells=0;
  for(Int_t s=0;s<6;s++) nCells+=list[s].getNCells();
  return nCells;
}

inline UChar_t HMdcEvntListCells::getTime(Int_t s, Int_t m, Int_t l, Int_t c) 
    const {
  if(nSecOk(s)) return list[s].getTime(m,l,c);
  return 0;
}

inline Bool_t HMdcEvntListCells::addTime1(Int_t s, Int_t m, Int_t l, Int_t c, 
    Float_t tm) {
  if(nSecOk(s)) return list[s].addTime1(m,l,c,tm);
  return kFALSE;
}

inline Bool_t HMdcEvntListCells::nextTime(Int_t& s, Int_t& m, Int_t& l, Int_t& c, 
    Float_t& tm) const {
  if(!nextCell(s,m,l,c)) return kFALSE;
  tm=getTimeValue(s,m,l,c);
  return kTRUE;
}

inline Float_t HMdcEvntListCells::getTimeValue(Int_t s, Int_t m, Int_t l, 
    Int_t c) const {
  if(nSecOk(s)) return list[s].getTimeValue(m,l,c);
  return -999.F;
}

inline Bool_t HMdcEvntListCells::addCell(Int_t s, Int_t m, Int_t l,Int_t c,
    UChar_t tm,Float_t dt1,Float_t dt2) {
  // tm can be eq. 1(time1 only), 2(time2 only) or 3(both)
  // dt1 and dt2 - driftTime1 and driftTime2 from HMdcCal1
  if(nSecOk(m)) return list[s].addCell(m,l,c,tm,dt1,dt2);
  return kFALSE; 
}

inline Bool_t HMdcEvntListCells::addCellSim(Int_t s, Int_t m, Int_t l,Int_t c,
    UChar_t tm,Float_t dt1,Float_t dt2,
    Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2) {
  // tm can be eq. 1(time1 only), 2(time2 only) or 3(both)
  // dt1 and dt2 - driftTime1 and driftTime2 from HMdcCal1
  // tr1 and tr2 - geant track numbers for driftTime1 and driftTime2
  if(nSecOk(m)) return list[s].addCellSim(m,l,c,tm,dt1,dt2,tr1,tr2,tf1,tf2);
  return kFALSE; 
}

#endif