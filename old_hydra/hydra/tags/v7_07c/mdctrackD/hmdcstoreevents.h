#ifndef HMDCSTOREEVENTS_H
#define HMDCSTOREEVENTS_H

#include "TObject.h"
#include "hseqarr.h"

class HMdcEvntListCells;
class HMdcEvntListCellsAndTimes;
class HMdcClus;

class HMdcStoreWires : public TObject {
  protected:
    HSeqArr nDTPerEvent;      // array of indexis of events
    HSeqArrIter* iterEvents;  // pointer to iterator of events
    HSeqArr mdcWireAddr;      // array of addresses of fired wires
    HSeqArrIter* iterWires;   // pointer to iterator of wires in event
    
    UInt_t maxMemSize;        // upper limit for memory
    
    UInt_t nEvents;           // number of events in array (for filling only)  
    UShort_t nDrTimes;        // number of wires in current event (for filling)
    
    UShort_t nDrTmPerEv;      // number of wires in current event (for reading)
    UInt_t nRdEvents;         // number of read events  (for reading)
    UShort_t nRdDrTimes;      // number of read wires  (for reading)

  public:
    HMdcStoreWires(void);
    ~HMdcStoreWires(void);
    void setMaxMemSize(UInt_t mSize); // mSize - in Mb!
    virtual UInt_t getMemSize(void) const;
    Bool_t testMemSize(void) const;
    
    // Storing functions:
    void addWireAddr(Int_t s, Int_t m, Int_t l, Int_t c);
    void setEndEvent(void);
    
    // Getting functions:
    virtual void resetIter(void);
    Bool_t getNextEvent(void);
    Bool_t getNextWireAddr(Int_t& s, Int_t& m, Int_t& l, Int_t& c);
    inline static UShort_t packAddress(Int_t s, Int_t m, Int_t l, Int_t c);
    inline static void unpackAddress(UShort_t& addr, Int_t& s, Int_t& m, 
        Int_t& l, Int_t& c);
    
    // Storing and getting events:
    void addEvent(const HMdcEvntListCells& event);
    Bool_t getNextEvent(HMdcEvntListCells& event);
  protected:
    
  ClassDef(HMdcStoreWires,0)
};

inline UShort_t HMdcStoreWires::packAddress(Int_t s,Int_t m,Int_t l,Int_t c) {
  return UShort_t((( ((s&7)<<5) + ((m&3)<<3) + (l&7) ) <<8) + (c&255));
}

inline void HMdcStoreWires::unpackAddress(UShort_t& addr, Int_t& s, 
    Int_t& m, Int_t& l, Int_t& c) {
  s=(Int_t)((addr>>13)&7);  // 13=5+8
  m=(Int_t)((addr>>11)&3);  // 11=3+8
  l=(Int_t)((addr>>8)&7);
  c=(Int_t)(addr&255);
}

class HMdcStoreEvents : public TObject {
  protected:
    HSeqArr nDTPerEvent;      // array of indexis of events
    HSeqArrIter* iterEvents;  // pointer to iterator of events
    HSeqArr mdcCells;         // array of addresses & dr.times of fired wires
    HSeqArrIter* iterWires;   // pointer to iterator of wires in event
    
    UInt_t maxMemSize;        // upper limit for memory
    
    UInt_t nEvents;           // number of events in array (for filling only)  
    UShort_t nDrTimes;        // number of wires in current event (for filling)
    
    UShort_t nDrTmPerEv;      // number of wires in current event (for reading)
    UInt_t nRdEvents;         // number of read events  (for reading)
    UShort_t nRdDrTimes;      // number of read wires  (for reading)

  public:
    HMdcStoreEvents(void);
    ~HMdcStoreEvents(void);
    void setMaxMemSize(UInt_t mSize); // mSize - in Mb!
    virtual UInt_t getMemSize(void) const;
    Bool_t testMemSize(void) const;
    
    // Storing functions:
    void addWire(Int_t s, Int_t m, Int_t l, Int_t c, Float_t tm);
    void setEndEvent(void);
    
    // Getting functions:
    virtual void resetIter(void);
    Bool_t getNextEvent(void);
    Bool_t getNextCell(Int_t& s, Int_t& m, Int_t& l, Int_t& c, Float_t& tm);
    inline static UInt_t packCell(Int_t s, Int_t m, Int_t l, Int_t c, 
        Float_t tm);
    inline static void unpackCell(UInt_t& addr, Int_t& s, Int_t& m, 
        Int_t& l, Int_t& c, Float_t& tm);
    
    // Storing and getting events:
    void addEvent(const HMdcEvntListCellsAndTimes& event);
    Bool_t getNextEvent(HMdcEvntListCellsAndTimes& event);
  protected:
    
  ClassDef(HMdcStoreEvents,0)
};

inline UInt_t HMdcStoreEvents::packCell(Int_t s,Int_t m,Int_t l,Int_t c,
    Float_t tm) {
  Int_t dt=(Int_t(tm*10.+((tm>=0.) ? 0.5:-0.5)))<<16;
  return UInt_t((( ((s&7)<<5) + ((m&3)<<3) + (l&7) ) <<8) + (c&255) +dt);
}

inline void HMdcStoreEvents::unpackCell(UInt_t& addr, Int_t& s, 
    Int_t& m, Int_t& l, Int_t& c, Float_t& tm) {
  s=(Int_t)((addr>>13)&7);  // 13=5+8
  m=(Int_t)((addr>>11)&3);  // 11=3+8
  l=(Int_t)((addr>>8)&7);
  c=(Int_t)(addr&255);
  UShort_t dt=(UShort_t)(addr>>16);
  tm=static_cast<Float_t>(static_cast<Short_t>(dt))*0.1;
}

//----------------------------------------------

class HMdcStoreClusters : public HMdcStoreEvents {
  protected:
    HSeqArr clustPos;          // array of cluster positions
    HSeqArrIter* iterClustPos; // pointer to iterator of clusters positions
    Float_t x1,y1;          // carrent cluster parameters:
    Float_t x2,y2;          // two points in coor.sys. mdc's
  public:
    HMdcStoreClusters(void);
    ~HMdcStoreClusters(void);
    virtual UInt_t getMemSize(void) const;
    
    // Storing functions:
    void setEndCluster(void);
    void setClusterPar(Float_t xt,Float_t yt,
                       Float_t xp,Float_t yp);
    
    // Getting functions:
    virtual void resetIter(void);  // must be called ones per clusters loop
    Bool_t getNextCluster(void);
    void getClusterPar(Float_t& xt,Float_t& yt,
                       Float_t& xp,Float_t& yp) const;
    Float_t getX1(void) const {return x1;}
    Float_t getY1(void) const {return y1;}
    Float_t getZ1(void) const {return 0.;}
    Float_t getX2(void) const {return x2;}
    Float_t getY2(void) const {return y2;}
    Float_t getZ2(void) const {return 0.;}
    
    // Storing and getting clusters:
    void addClustWires(const HMdcEvntListCellsAndTimes& event, 
        const HMdcClus* cl);                        // store wires list only
    void addClustWires(const HMdcEvntListCellsAndTimes& event,
        const HMdcClus* cl1, const HMdcClus* cl2);  // store wires list only
    Bool_t getNextCluster(HMdcEvntListCellsAndTimes& event);
    
    inline static Short_t packCoor(Float_t c);
    inline static Float_t unpackCoor(Short_t c);

    // Use this function at the reading data only!!!    
    Bool_t setNewClustPar(Float_t xt,Float_t yt,Float_t xp,Float_t yp);
  
  protected:
   Bool_t getClustPar(void);
      
  ClassDef(HMdcStoreClusters,0)
};


inline Short_t HMdcStoreClusters::packCoor(Float_t c) {
  return Short_t(c*20.+((c>=0.) ? 0.5:-0.5));
}

inline Float_t HMdcStoreClusters::unpackCoor(Short_t c) {
  return static_cast<Float_t>(c)*0.05;
}

#endif  /*!HMDCSTOREEVENTS_H*/