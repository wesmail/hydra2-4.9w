//*-- AUTHOR: D.Magestro
//*-- Created: 28/05/01
//*-- Modified: 07/11/01
#ifndef HPARTICLESIMFILLER_H
#define HPARTICLESIMFILLER_H

#include "hparticlefiller.h"
#include "iostream.h"
#include "hlocation.h"

class HIterator;
class HCategory;

class HParticleSimFiller : public HParticleFiller {

private:
  HCategory* fPartSrcCat;       //! Pointer to Particle data category
  HCategory* fGeantKineCat;     //! Pointer to HGeantKine data category
  HIterator* iterGeant;         //! Iterator over HGeantKine

public:
  HParticleSimFiller(void);
  HParticleSimFiller(Text_t* name,Text_t* title,Bool_t skip=kFALSE);
  ~HParticleSimFiller(void) {;}

  void fillParticleSrc(void);
  Bool_t init(void);
  Bool_t finalize(void) {return kTRUE;}
  Int_t  execute(void);

public:
  ClassDef(HParticleSimFiller,1) // Filler for HParticleSim & HParticleSrc containers
};

#endif