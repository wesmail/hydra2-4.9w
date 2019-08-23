//*-- AUTHOR : Ilse Koenig
//*-- Createded : 10/03/2000

//////////////////////////////////////////////////////////////////////////////
// HTofGeomPar
//
// Container class for the basic TOF geometry parameters
//
//////////////////////////////////////////////////////////////////////////////

#include "htofgeompar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream.h>

ClassImp(HTofGeomPar)

HTofGeomPar::HTofGeomPar(HGeomShapes* s) : HDetGeomPar("Tof",s) {
  // constructor sets the name of the container
  fName="TofGeomPar";
  fTitle="Tof geometry parameters";
}

Bool_t HTofGeomPar::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HTofParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HTofGeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HTofParIo");
  if (out) return out->write(this);
  return -1;
}