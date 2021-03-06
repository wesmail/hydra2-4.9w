#ifndef HRTTRACKSIMINSPECT_H
#define HRTTRACKSIMINSPECT_H

#include "hreconstructor.h"
#include "TNtuple.h"

class HCategory;
class HIterator;

class HRtTrackSimInspect : public HReconstructor {
  public:
    HRtTrackSimInspect(const Text_t name[]="rt.tsins",const Text_t title[]="rt.tsins");
    ~HRtTrackSimInspect(void);
    Bool_t init(void);
    Int_t execute(void);
    Bool_t finalize(void);
  protected:
    HCategory *fCatTrack; //!
    HCategory *fCatKine; //!
    HIterator *fTrackIter; //!
    TNtuple *fOutput;//!
  public:
    ClassDef(HRtTrackSimInspect,1)
};

#endif
