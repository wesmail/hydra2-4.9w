#ifndef HRICHGEANTREADER_H
#define HRICHGEANTREADER_H

#include <TTree.h>
#include "hgeantreader.h"
#include "hmatrixcategory.h"

class HGeantRichPhoton;
class HGeantRichDirect;
class HLocation;

class HRichGeantReader : public HGeantReader {
private:
  TTree* t;                             //  Pointer to the root tree
  HMatrixCategory* fGeantRichPhotCat;   //! Rich HGeant input data
  HMatrixCategory* fGeantRichDirCat;    //! Rich HGeant input data
  Int_t fEventId;                       //! event counter
public:
  HRichGeantReader(void);
  ~HRichGeantReader(void);
  Bool_t init(void); 
  Bool_t execute(void); 
  inline HMatrixCategory* getGeantRichPhotonCat(void) {return fGeantRichPhotCat;}
  inline HMatrixCategory* getGeantRichDirectCat(void) {return fGeantRichDirCat;}
  inline HGeantRichPhoton* getGeantRichPhoton(HLocation locate) {
     return (HGeantRichPhoton*)(fGeantRichPhotCat->getObject(locate));
  }
  inline HGeantRichDirect* getGeantRichDirect(HLocation locate) {
     return (HGeantRichDirect*)(fGeantRichDirCat->getObject(locate));
  }
  ClassDef(HRichGeantReader,1) // RICH reader for HGeant Root file
};

#endif /* !HRICHGEANTREADER_H */




