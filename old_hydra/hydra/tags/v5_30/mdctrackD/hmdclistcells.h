#ifndef HMDCLISTCELLS_H
#define HMDCLISTCELLS_H

#include "hmdctbarray.h"
#include "TObject.h"
#include "hdataobject.h"
#include "TMath.h"
//#include <string.h>
#include <iostream.h>

#define SIZE 56
#define AEND 55
#define NCELLS 224
// NCELLS = SIZE*4;   AEND=SIZE-1

class HMdcListCells : public TObject {
 protected:
  UChar_t arr[SIZE];
 public:
  HMdcListCells() {clear();}
  ~HMdcListCells(void) {}
  inline Bool_t test(Int_t cell);
  inline void clear();
  inline void setTime(Int_t cell,UChar_t time);
  inline void delTime(Int_t cell,UChar_t time);
  inline Int_t getNCells();
  inline Int_t getCell(Int_t idx);
  inline Int_t getFirstCell();
  inline Int_t getLastCell();
  inline Int_t next(Int_t prCell);
  inline Int_t previous(Int_t prCell);
  inline UChar_t getTime(Int_t cell);
  void print();
  
  ClassDef(HMdcListCells,0) //List of all cells in layer
};
  
inline Bool_t HMdcListCells::test(Int_t cell) { 
  return (cell>=0 && cell < NCELLS) ? kTRUE : kFALSE;
}

void HMdcListCells::clear() {
  HMdcTBArray::clear(arr,arr+AEND);
}

void HMdcListCells::setTime(Int_t cell,UChar_t time) {
  if(test(cell)) HMdcTBArray::set(arr,cell,time);
}

void HMdcListCells::delTime(Int_t cell,UChar_t time) {
  if(test(cell)) HMdcTBArray::unset(arr,cell,time);
}

Int_t HMdcListCells::getNCells() {
  return HMdcTBArray::getNSet(arr,arr+AEND);
}

Int_t HMdcListCells::getCell(Int_t idx) {
  return HMdcTBArray::position(arr,arr+AEND,idx);
}

Int_t HMdcListCells::getFirstCell() {
  return HMdcTBArray::first(arr,arr+AEND);
} 

Int_t HMdcListCells::getLastCell() {
  return HMdcTBArray::last(arr,arr+AEND);
}

Int_t HMdcListCells::next(Int_t prCell) {
  return HMdcTBArray::next(arr,arr+AEND,prCell);
}

Int_t HMdcListCells::previous(Int_t prCell) {
  return HMdcTBArray::previous(arr,arr+AEND,prCell);
}


UChar_t HMdcListCells::getTime(Int_t cell) {
  if(test(cell)) return HMdcTBArray::get(arr,cell);
  return 0;
}
#undef SIZE
#undef AEND
#undef NCELLS

// - - - - - - - - - - - - - - - - - - - - - - - - - -

#define SIZE 12
#define AEND 11
#define NCELLS 48
// NCELLS = SIZE*4;   AEND=SIZE-1

class HMdcListGroupCells : public TObject {
 protected:
  UChar_t arr[SIZE];
  Int_t firstCell;
 public:
  HMdcListGroupCells() {clear();}
  ~HMdcListGroupCells(void) {}
  inline void clear();
  inline Int_t setTime(Int_t cell, UChar_t time);
  inline void delTime(Int_t cell, UChar_t time);
  inline Int_t getNCells();
  inline Int_t getCell(Int_t idx);
  inline Int_t getFirstCell();
  inline Int_t getLastCell();
  inline Int_t next(Int_t prCell);
  inline Int_t previous(Int_t prCell);
  inline UChar_t getTime(Int_t cell);
  inline Int_t compare(HMdcListGroupCells* lst);
  void print();
  
  ClassDef(HMdcListGroupCells,0) //List of all cells in layer
};

void HMdcListGroupCells::clear() {
  HMdcTBArray::clear(arr,arr+AEND);
  firstCell=0;
}
  
Int_t HMdcListGroupCells::setTime(Int_t cell, UChar_t time) {
  // return: 0 - ok.!;  n>0 - num.cells excl.from cl. 
  if( cell<0 || (time&3)==0) return 0;
  Int_t nCellsDel=0;
  if(arr[0] == 0) firstCell=cell & 0xFFFFFFFC;  // =(cell/4)*4
  else if(cell < firstCell) {
    Int_t sh=(firstCell>>2)-(cell>>2);
    nCellsDel=HMdcTBArray::getNSet(arr+AEND-sh+1,arr+AEND);
    HMdcTBArray::shiftRight(arr,arr+AEND,arr+AEND-sh);
    firstCell=cell & 0xFFFFFFFC;
  }
  else if( cell-firstCell >= NCELLS ) return 1; // imposs. to put cell in data str.
  HMdcTBArray::set(arr, cell-firstCell, time);
  return nCellsDel;
}

void HMdcListGroupCells::delTime(Int_t cell, UChar_t time) {
  Int_t pos=cell-firstCell;
  if(pos < 0 || pos >=  NCELLS) return;
  HMdcTBArray::unset(arr, pos, time);
  if(!HMdcTBArray::getNSet(arr, arr+AEND)) firstCell=0;
  else if( !arr[0] ) {
    Int_t sh=HMdcTBArray::first(arr,arr+AEND)>>2;
    HMdcTBArray::shiftLeft(arr,arr+AEND,arr+sh);
    firstCell+=sh*4;
  }
}

Int_t HMdcListGroupCells::getNCells() {
  return HMdcTBArray::getNSet(arr,arr+AEND);
}

Int_t HMdcListGroupCells::getCell(Int_t idx) {
  // return: -1 - idx <0 or >num.cells
  Int_t pos=HMdcTBArray::position(arr,arr+AEND,idx);
  if(pos >= 0) pos+=firstCell;
  return pos;
}

Int_t HMdcListGroupCells::getFirstCell() {
  // return: -1 - num.cells=0
  Int_t pos=HMdcTBArray::first(arr,arr+AEND);
  if(pos >= 0) pos+=firstCell;
  return pos;
}

Int_t HMdcListGroupCells::getLastCell() {
  // return: -2 - num.cells=0
  Int_t pos=HMdcTBArray::last(arr,arr+AEND);
  if(pos >= 0) pos+=firstCell;
  return pos;
}

Int_t HMdcListGroupCells::next(Int_t prCell) {
  Int_t cell=HMdcTBArray::next(arr,arr+AEND,prCell-firstCell);
  if(cell>=0) cell+=firstCell;
  return cell;
}

Int_t HMdcListGroupCells::previous(Int_t prCell) {
  Int_t cell=HMdcTBArray::previous(arr,arr+AEND,prCell-firstCell);
  if(cell>=0) cell+=firstCell;
  return cell;
}

UChar_t HMdcListGroupCells::getTime(Int_t cell) {
  Int_t pos=cell-firstCell;
  if(pos<0 || pos>=NCELLS) return 0;
  return HMdcTBArray::get(arr,pos);
}

inline Int_t HMdcListGroupCells::compare(HMdcListGroupCells* lst) {
  Int_t left=TMath::Max(firstCell,lst->firstCell);
  Int_t right=TMath::Min(firstCell,lst->firstCell)+NCELLS;
  if(left>right) return 0;
  return HMdcTBArray::compare(arr+((left-firstCell)>>2),
            lst->arr+((left-lst->firstCell)>>2),(right-left)>>2);
}

#undef SIZE
#undef AEND
#undef NCELLS

//----------------------------------------

#define NLAY 12
#define SIZE 12
#define AEND 11
#define NCELLS 48
// NCELLS = SIZE*4;   AEND=SIZE-1

class HMdcList12GroupCells : public HDataObject {

  protected:
//  UChar_t arr[NLAY*SIZE]; // NLAY layers, NLAY*4=NCELLS cells
    UChar_t arr[144]; // NLAY layers, NLAY*4=NCELLS cells
//  Int_t firstCell[NLAY];
    Int_t firstCell[12];  
  public:
    HMdcList12GroupCells() {clear();}
    ~HMdcList12GroupCells(void) {;}
    inline void clear();
    inline Int_t setTime(Int_t lay, Int_t cell, UChar_t time);
    inline Int_t add(HMdcList12GroupCells *lst);
    inline void delTime(Int_t lay, Int_t cell, UChar_t time);
    inline Int_t getNCells(Int_t lay);
    inline Int_t getNCells();    // Total num. of hits in NLAY layers
    inline Int_t getCell(Int_t lay, Int_t idx);
    inline Int_t getFirstCell(Int_t lay);
    inline Int_t getLastCell(Int_t lay);
    inline Int_t next(Int_t lay, Int_t prCell);
    inline Int_t previous(Int_t lay, Int_t prCell);
    inline UChar_t getTime(Int_t lay, Int_t cell);
    inline Int_t compare(HMdcList12GroupCells* lst, Int_t l1=-1, Int_t l2=-1);
    inline void print();
    void print(Int_t lay);

  ClassDef(HMdcList12GroupCells,1) //List of all cells in layer
};

void HMdcList12GroupCells::clear() {
  HMdcTBArray::clear(arr,arr+NLAY*SIZE-1);
  for(Int_t l=0; l<NLAY; l++) firstCell[l]=0;
}
  
Int_t HMdcList12GroupCells::setTime(Int_t lay, Int_t cell, UChar_t time) {
  // return: 0 - ok.!;  n>0 - num.cells excl.from cl. 
  if( cell<0 || (time&3)==0 || lay<0 || lay>=NLAY) return 0;
  Int_t nCellsDel=0;
  UChar_t *iLayer=arr+lay*SIZE;
  UChar_t *pEnd=iLayer+AEND;
  if(iLayer[0] == 0) firstCell[lay]=cell & 0xFFFFFFFC;  // =(cell/4)*4
  else if(cell < firstCell[lay]) {
    Int_t sh=(firstCell[lay]>>2)-(cell>>2);
    nCellsDel=HMdcTBArray::getNSet(pEnd-sh+1,pEnd);
    HMdcTBArray::shiftRight(iLayer,pEnd,pEnd-sh);
    firstCell[lay]=cell & 0xFFFFFFFC;
  }
  else if( cell-firstCell[lay] >= NCELLS ) return 1; // imposs. to put cell in data str.
  HMdcTBArray::set(iLayer, cell-firstCell[lay], time);
  return nCellsDel;
}

void HMdcList12GroupCells::delTime(Int_t lay, Int_t cell, UChar_t time) {
  Int_t pos=cell-firstCell[lay];
  if(pos < 0 || pos >=  NCELLS) return;
  UChar_t *iLayer=arr+lay*SIZE;
  UChar_t *pEnd=iLayer+AEND;
  HMdcTBArray::unset(iLayer, pos, time);
  if(!HMdcTBArray::getNSet(iLayer, pEnd)) firstCell[lay]=0;
  else if( !iLayer[0] ) {
    Int_t sh=HMdcTBArray::first(iLayer,pEnd)>>2;
    HMdcTBArray::shiftLeft(iLayer,pEnd,iLayer+sh);
    firstCell[lay]+=sh*4;
  }
}

Int_t HMdcList12GroupCells::getNCells(Int_t lay) {
  return HMdcTBArray::getNSet(arr+lay*SIZE,arr+lay*SIZE+AEND);
}

Int_t HMdcList12GroupCells::getNCells() {
  return HMdcTBArray::getNSet(arr,arr+NLAY*SIZE-1);
}

Int_t HMdcList12GroupCells::getCell(Int_t lay, Int_t idx) {
  // return: -1 - idx <0 or >num.cells
  Int_t pos=HMdcTBArray::position(arr+lay*SIZE,arr+lay*SIZE+AEND,idx);
  if(pos >= 0) pos+=firstCell[lay];
  return pos;
}

Int_t HMdcList12GroupCells::getFirstCell(Int_t lay) {
  // return: -1 - num.cells=0
  Int_t pos=HMdcTBArray::first(arr+lay*SIZE,arr+lay*SIZE+AEND);
  if(pos >= 0) pos+=firstCell[lay];
  return pos;
}

Int_t HMdcList12GroupCells::getLastCell(Int_t lay) {
  // return: -2 - num.cells=0
  Int_t pos=HMdcTBArray::last(arr+lay*SIZE,arr+lay*SIZE+AEND);
  if(pos >= 0) pos+=firstCell[lay];
  return pos;
}

Int_t HMdcList12GroupCells::next(Int_t lay, Int_t prCell) {
  Int_t cell=
      HMdcTBArray::next(arr+lay*SIZE,arr+lay*SIZE+AEND,prCell-firstCell[lay]);
  if(cell>=0) cell+=firstCell[lay];
  return cell;
}

Int_t HMdcList12GroupCells::previous(Int_t lay, Int_t prCell) {
  Int_t cell=
    HMdcTBArray::previous(arr+lay*SIZE,arr+lay*SIZE+AEND,prCell-firstCell[lay]);
  if(cell>=0) cell+=firstCell[lay];
  return cell;
}

UChar_t HMdcList12GroupCells::getTime(Int_t lay, Int_t cell) {
  Int_t pos=cell-firstCell[lay];
  if(pos<0 || pos>=NCELLS) return 0;
  return HMdcTBArray::get(arr+lay*SIZE,pos);
}

Int_t HMdcList12GroupCells::compare(HMdcList12GroupCells* lst, Int_t l1, Int_t l2) {
  // return the num. of layers which have one or more identical cells
  Int_t nLay=0;
  Int_t left,right;
  if(l1<0) {
    l1=0;
    l2=AEND;
  }
  else {
    if(l2<0) l2=l1;
    else if(l2>AEND) l2=AEND;
  }
  for(Int_t lay=l1; lay<=l2; lay++) {
    left=TMath::Max(firstCell[lay],lst->firstCell[lay]);
    right=TMath::Min(firstCell[lay],lst->firstCell[lay])+NCELLS;
    if(left>right) return 0;
    if(HMdcTBArray::compare(arr+lay*SIZE+((left-firstCell[lay])>>2),
       (lst->arr)+lay*SIZE+((left-lst->firstCell[lay])>>2),(right-left)>>2)) 
       nLay++;;
  }
  return nLay;
}

Int_t HMdcList12GroupCells::add(HMdcList12GroupCells* lst) {
  // return: 0 - ok.!;  n>0 - num.cells excl.from cl.
  Int_t nLay=0;
  for(Int_t lay=0; lay<NLAY; lay++) {
    UChar_t *lst2=(lst->arr)+lay*SIZE;
    if(HMdcTBArray::getNSet(lst2,lst2+AEND) == 0) continue;
    UChar_t *lst1=arr+lay*SIZE;
    if(HMdcTBArray::getNSet(lst1,lst1+AEND) == 0) {
      for(Int_t n=0; n<SIZE; n++) lst1[n] = lst2[n];
      firstCell[lay]=lst->firstCell[lay];
      continue;
    }
    Int_t sing=firstCell[lay] - lst->firstCell[lay];
    if(sing != 0) {
      Int_t shift=((sing<0) ? -sing:sing)>>2;  // == /4
      if(shift>12) shift=12;
      if( sing < 0 ) {
        nLay=HMdcTBArray::getNSet(lst2+SIZE-shift,lst2+AEND); //??? only for message!?
        for(Int_t n=shift; n<SIZE; n++) lst1[n] |= lst2[n-shift];
      } 
      else if( sing > 0 ){
        nLay=HMdcTBArray::getNSet(lst1+SIZE-shift,lst1+AEND); //??? only for message!?
        for(Int_t n=shift; n<SIZE; n++) lst2[n] |= lst1[n-shift];
        for(Int_t n=1; n<SIZE; n++) lst1[n] = lst2[n];
        firstCell[lay]=lst->firstCell[lay];
      } 
    }
    else for(Int_t n=0; n<SIZE; n++) lst1[n] |= lst2[n];
  }
  return nLay;
}

void HMdcList12GroupCells::print() {
  for(Int_t lay=0; lay<NLAY; lay++) print(lay);
}

#undef NLAY
#undef SIZE
#undef AEND
#undef NCELLS

#endif