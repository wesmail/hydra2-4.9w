// File: hrichunpacker100.h
//
// Author: Walter Schoen <W.Schoen@physik.tu-muenchen.de>
// Last update by Walter Schoen: 99/12/17 17:17:17
//
#ifndef HRichUnpackerRaw_H
#define HRichUnpackerRaw_H
#include <TROOT.h>
#include <TVector.h>
#include <TFile.h>
#include "hldunpack.h"
#include "hcategory.h"
#include "haddef.h"
#include "hades.h"
#include "hevent.h"
#include "haddef.h"

class HRichUnpackerRaw: public HldUnpack {
public:
    HRichUnpackerRaw(int);
    ~HRichUnpackerRaw(void);
    int getSubEvtId(void) const;
    Bool_t init(void);
    int execute(void); 
    void diagnose(int,int,int,int);
private:
    int richId;
    TVector * uipTable;   
    TVector * uip2xyPosition;   
    TVector * uip2xPosition;  
    TVector * uip2yPosition;
    TVector * xyPosition2uip;  
    int charge;
    struct Upi{
	int sector;
	int controler;
	int port;
	int modul;
	int channel;
    } upi;
    int getUipIdent(int controler,int port, int modul, int channel){
	return controler * 10000 + port * 1000 + modul * 100 + channel;
    }
public:
    ClassDef(HRichUnpackerRaw,1)        // unpack RICH data
};
#endif /* !HRichUnpackerRaw_H */





