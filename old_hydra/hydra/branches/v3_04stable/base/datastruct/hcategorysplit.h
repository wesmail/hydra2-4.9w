#ifndef HCATEGORYSPLIT_H
#define HCATEGORYSPLIT_H

#include "hcategory.h"
#include <TArrayI.h>
#include <TString.h>

class HCategorySplitIter;

class HCategorySplit : public HCategory {
friend class HCategorySplitIter;
protected:
  TArrayI fSizesArray; //Category sizes.
  TObjArray *fTableArray; //-Array of Layers
  Text_t fClassName[200]; //-
  Text_t fBranchPattern[255]; //-
public:
  HCategorySplit(void);
  HCategorySplit(Text_t *className,Int_t n,Int_t *siz,
		 Text_t *branchPattern,TObject *header=NULL); 
  ~HCategorySplit(void);
  Text_t *getClassName(void);
  TClass *getClass(void);
  void makeBranch(TBranch *parent);
  void activateBranch(TTree *tree,Int_t splitLevel);
  virtual Bool_t setup(Int_t *moduleSizes,Float_t fillRate=0.1);
  virtual Bool_t setup(Int_t nActive,Int_t *activeModules,Int_t size,
		       Float_t fillRate=0.1);
  TObject *&getNewSlot(HLocation &aLoc);
  TObject *&getSlot(HLocation &aLoc);
  HDataObject *getObject(HLocation &aLoc);
  Bool_t filter(HFilter &aFilter);
  Bool_t filter(HLocation &aLoc,HFilter &aFilter);
  void Clear(void);
  Bool_t isSelfSplitable(void);
  TIterator *MakeIterator(Bool_t dir=kIterForward);
  ClassDef(HCategorySplit,1) //Multibranch category.
};
  
		       
#endif /* !HCATEGORYSPLIT */
