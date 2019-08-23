#ifndef HPARIO_H
#define HPARIO_H

#include "TObject.h"
#include "TList.h"

class HDetParIo;
class HRun;

class HParIo : public TObject {
protected:
  TList* detParIoList;  // list of detector I/Os
  Bool_t autoWritable;  // flag indicating if automatic write is possible
public:
  HParIo();
  virtual ~HParIo();
  virtual HDetParIo* getDetParIo(Text_t*);
  virtual void setDetParIo(HDetParIo*);
  virtual void removeDetParIo(Text_t*);
  void setInputNumber(Int_t);
  virtual void close() {;}

  // creates input/output class for a special detector and stores pointer
  // used only for Oracle input/output
  // (code in  class OraIo)
  virtual void setDetParIo(Text_t*) {;}

  // prints information about input/output
  virtual void print() {;}

  // checks if the input/output is open
  virtual Bool_t check() { return kFALSE; }

  // reads versions of parameter containers for an event file
  virtual void readVersions(HRun*) {;}

  // sets global file pointer in ROOT if input/output is a ROOT-file
  // (code in HParRootFileIo)
  virtual void cd() {;}

  // set and get for flag autoWritable
  void setAutoWritable(Bool_t f=kTRUE) {autoWritable=f;}
  Bool_t isAutoWritable() {return autoWritable;}

  ClassDef(HParIo,0) // Base class for all parameter I/Os
};

#endif  /* !HPARIO_H */



