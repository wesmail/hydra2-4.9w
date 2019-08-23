#ifndef HTOFDETECTOR_H
#define HTOFDETECTOR_H

#include "hdetector.h" 
#include "haddef.h"
#include <TArrayI.h>

class HTofDetector : public HDetector {
protected:

public:
  HTofDetector(void);
  ~HTofDetector(void);
  HTask *buildTask(Text_t name[],Option_t *opt="");
  HCategory *buildCategory(Cat_t cat);
  HCategory *buildMatrixCategory(Text_t *classname,Float_t fillRate);
  HCategory *buildCategorySplit(Text_t *className,Float_t fillRate);
  Bool_t init(void) {return kTRUE;}

  ClassDef(HTofDetector,1) // TOF detector class
};


#endif /* !HTOFDETECTOR_H */








