//*-- AUTHOR : I. Froehlich
//*-- Modified : 07.07.2005 by T. Perez

#ifndef HHYP_DELTATOFMEANWEIGHTALG_H
#define HHYP_DELTATOFMEANWEIGHTALG_H

#include <TObject.h>
#include <TArrayI.h>
#include <TNtuple.h>
#include <TDirectory.h>
#include <TH2I.h>

#include <stdlib.h>
#include <iostream>
#include <cmath>

#include "hades.h"

#include "hhypbasealgorithm.h"
#include "hhyplistfiller.h"
#include "hypinfodef.h"

#include "hiterator.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hcategory.h"

#include "hrktrackB.h"

#include "hpidtrackcand.h"
#include "hpidtrackcandsim.h"
#include "hpidphysicsconstants.h"
#include <hpidgeanttrackset.h>
#include <hgeantkine.h>
#include <piddef.h>


// ***************************************************************************

class HHypDeltaTofMeanWeightAlg:public HHypBaseAlgorithm
{

  // ***************************************************************************
private:

  TNtuple       * qa;
	TDirectory 		*histdir;
	TH2I					* histBefore, * histAfter;
	TH1I					* histDeltaDtof, * histDtof, * histDtofOld, * histDtof2, * histDtof2Old;
	TH1I					* histPartDtofB, * histPartDtofBW,* histPartDtof, * histPartDtofW, * histW;

  Int_t           simuflag;
  HCategory      *simCat;
  HCategory      *CatTrackCandSim;

protected:

public:

  HHypDeltaTofMeanWeightAlg(Char_t *name_i = NULL, Option_t par[] = NULL);

  virtual ~ HHypDeltaTofMeanWeightAlg(void);

  Bool_t          execute();
  Bool_t          init();
  Bool_t          reinit();
  Bool_t          finalize();
	Bool_t          is_sim_ok();

  ClassDef(HHypDeltaTofMeanWeightAlg, 0) //The DeltaTof algorithm
};

//============================================================================

#endif