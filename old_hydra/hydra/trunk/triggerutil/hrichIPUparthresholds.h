//*-- Author: Alberica Toia
//*-- Modified: Joern Wuestenfeld 05/19/2005

#ifndef HRICHIPUPARTHRESHOLDS_H
#define HRICHIPUPARTHRESHOLDS_H


#include "hparset.h"

class HParIO;
class TString;

class HRichIPUParThresholds : public HParSet
{
private:
  Int_t thresholds[2][6];  

public:
  HRichIPUParThresholds(const Char_t* name="RichIPUParThresholds",
                        const Char_t* title="Parameters for Rich IPU Thresholds ",
                        const Char_t* context="RICHIPUEmulation");
  ~HRichIPUParThresholds(void);

  Int_t getRegionSize() {return 2;}
  Int_t getThresholds(Int_t re, Int_t sec){return thresholds[re][sec];}
  void setThresholds(Int_t re, Int_t sec, Int_t thr){thresholds[re][sec]=thr;}

  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  void clear();
  void printParam();
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  Bool_t writeline(Char_t*, Int_t, Int_t);
  void setDefaults(void);

  ClassDef(HRichIPUParThresholds,1)  // Container for the RICH IPU Thresholds parameters
};

#endif /* !HRICHIPUPARTHRESHOLDS_H */
