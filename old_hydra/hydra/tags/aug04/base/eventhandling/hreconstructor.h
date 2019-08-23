#ifndef HRECONSTRUCTOR_H
#define HRECONSTRUCTOR_H

#include "htask.h"
#include "hlocation.h"
#include <TBrowser.h>
#include <TObjArray.h>
#include <TStopwatch.h>

class HReconstructor : public HTask {
protected:
  TList *fHistograms; // List of histograms generated by this reconstructor.
  Bool_t fActive;     // Active flag
  TObjArray fOutputs; //
  TStopwatch fTimer;  // Task timer

public:
  HReconstructor(void);
  ~HReconstructor(void);
  HReconstructor(Text_t *name,Text_t *title);
  virtual Int_t execute(void)=0;
  HTask *next(Int_t &errCode);
  HTask *getTask(Char_t *name);
  Bool_t connectTask(HTask *task,Int_t n=0);
  TObjArray getTable() {return fOutputs;}
  void getConnections();
  HTask* getComposite(){return NULL;}
  void setActive(Bool_t state) {fActive=state;} //*MENU*
  Bool_t IsFolder(void) const;
  void Browse(TBrowser *b);
  void isTimed(Bool_t flag=kTRUE) {fIsTimed=flag;}
  void resetTimer(void);
  void printTimer(void);
  ClassDef(HReconstructor,1) // Reconstruction algorithm Class 
};
  

#endif /* !HRECONSTRUCTOR */
