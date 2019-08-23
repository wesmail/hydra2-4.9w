/***************************************************************************
 *   Author :  Tiago Perez <tiago.perez@exp2.physik.uni-giessen.de>        *
 *             II Physikalisches Institut                                  *
 *             Universitaet Giessen                                        *
 *            								   *
 *   Title  :  HHypBaseAlgorithm.h                                         *
 *   Project:  HYDRA Event Hypothesys  (HADES)                             *
 *   Description:   						           *
 *            								   *
 *   Created  :   2005-01-05 Tiago Perez                                   *
 *   Modified :                                                            *
 *            								   *
 ***************************************************************************/
#ifndef HHYPBASEALGORITHM_H
#define HHYPBASEALGORITHM_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hhyplist.h"
#include "TArrayI.h"
#include "hevent.h"
#include "heventheader.h"
#include "hdetector.h"
#include "hratreeext.h"
#include "hmatrixcategory.h"
#include "hlinearcategory.h"
#include "hlinearcatiter.h"
#include "hlocation.h"
#include "hiterator.h"
#include "hdebug.h"
#include "hades.h"
#include "hypinfodef.h"
#include <hphysicsconstants.h>

#include <TObject.h>
#include <TArrayI.h>


//#include "hmatrixcatiter.h"

/* class HCategory; */
/* class HIterator; */
/* class HSpectrometer; */
/* class HRuntimeDb; */
// ***************************************************************************

class HHypBaseAlgorithm
{

    // ***************************************************************************
protected:

    // Variables Int
    Int_t sourceIdx, exitIdx, exitList, initList;
    char *algoName;  //!
    HHypList * mylist;
    HHypList * GetList(void); 

    TLorentzVector * beam;  //!

    // Methods internal

public:


    HHypBaseAlgorithm(char *name_i=NULL);


    virtual ~HHypBaseAlgorithm(void);

    void SetBeam(TLorentzVector * mybeam) {beam=mybeam;};

    Bool_t SetSourceChannelAlgIdx(Int_t i_id);
    Bool_t SetExitList(Int_t e_list);
    Bool_t SetInitList(Int_t i_list);
    Bool_t SetExitChannelAlgIdx(Int_t e_id);
    Int_t GetExitChannelAlgIdx();
    Char_t * GetName();
    Int_t GetSourceChannelAlgIdx();

    virtual Bool_t execute();
    virtual Bool_t init();
    virtual Bool_t reinit();
    virtual Bool_t finalize();

    Bool_t base_execute();
    Bool_t base_init();
    Bool_t base_reinit();
    Bool_t base_finalize();

    ClassDef(HHypBaseAlgorithm,0)
};

//============================================================================

#endif  // HHYPBASEALGORITHM_H
