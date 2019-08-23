#ifndef HMDCTRACKGFIELDPAR_H
#define HMDCTRACKGFIELDPAR_H
#include "TString.h"
#include "TObject.h"
#include "TNamed.h"
#include "hparcond.h"
#include "hpario.h"


//CORRECTION PARAMETERS FOR MOMENTUM RECONSTRUCTION
class HMdcTrackGField;
class HGeomVector;

class HMdcTrackGFieldPar:public HParCond {
protected:
    HMdcTrackGField* field;
public:
    HMdcTrackGFieldPar(const char* name="MdcTrackGFieldPar",
		     const char* title="Fieldmap",
		     const char* context="MdcTrackGFieldParProduction");
    ~HMdcTrackGFieldPar();
    Bool_t  init(HParIo*, Int_t*);
    Int_t   write(HParIo*);
    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);//{return kTRUE;}
    void    clear();
    void initFieldMap(TString );
    void calcField(Double_t *,Double_t *, Int_t );
    HMdcTrackGField* getPointer(){return field;}
    ClassDef(HMdcTrackGFieldPar,1)
};
#endif