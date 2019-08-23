/***************************************************************************
 *   Author :                                                              *
 *             II Physikalisches Institut                                  *
 *             Universitaet Giessen                                        *
 *           								   *
 *   Project:  HYDRA Event Hypothesys  (HADES)                             *
 *   Description:   						           *
 *            								   *
 *   Created  :   2005-02-08 Ingo                                          *
 *   Modified :                                                            *
 *            								   *
 ***************************************************************************/

using namespace std;
#include <stdlib.h>
#include <iostream>



#include "hevent.h"
#include "heventheader.h"
#include "hdetector.h"
#include "hratreeext.h"
#include "hcategory.h"
#include "hmatrixcategory.h"
#include "hlinearcategory.h"
#include "hlinearcatiter.h"
#include "hlocation.h"
#include "hiterator.h"
#include "hdebug.h"
#include "hades.h"

#include "hhypPPPipPimProjector.h"
#include "hhyplist.h"
#include "hhypchannel.h"
#include "hypinfodef.h"


ClassImp(HHypPPPipPimProjector)
  
  HHypPPPipPimProjector::HHypPPPipPimProjector(char *name_i) 
{
  algoName = name_i;
  simuflag = 0;
}


HHypPPPipPimProjector::~HHypPPPipPimProjector()
{
}


Bool_t HHypPPPipPimProjector::execute()
{
  base_execute();

  if (!beam) 
    {
      cerr << algoName << " needs beam particle! " << endl;
      return kFALSE;
    }

  HHypList* p_hyplist = NULL;
  
  if (sourceIdx >=0) {
    HCategory* listCat=gHades->getCurrentEvent()->getCategory(catHypList);
    p_hyplist = (HHypList*)listCat->getObject(sourceIdx);
  }
  
  if (!p_hyplist) return kFALSE;

  //p_hyplist->setProbAlg(1, -1.);
  //p_hyplist->setProbAlg(0, -1.);
  //p_hyplist->setProbAlg(2, -1.);


      
  Int_t etaflag = 0;
  
  
  if (simuflag == 1) {
    for (int i=0; i<4; i++)
      {
	HPidParticleSim * mypart = (HPidParticleSim*)CatPartSim->getObject(i);
	if (mypart!= NULL) {
	  
	  HPidGeantTrackSet * pGeantSet = mypart->getGeantTrackSet();

	   for(int j=0;j<5;j++){
   
            Int_t geant_track = pGeantSet->getGeantTrackID(j);
   
	    if (geant_track>=0) {
	      HGeantKine * geantkine =( HGeantKine *) pGeantSet->getGeantKine(geant_track);

	      Float_t geninfo,genweight;
	      geantkine->getGenerator(geninfo,genweight);

	      cout << geninfo << ":" << genweight << endl;

	    }
	    
	   }
	   
	   
	}
      }
  }
  


  p_hyplist->CombIteratorReset();
  
  while (p_hyplist->CombIterator()) {

    //    cout << "filling ntuple " << endl;

    TLorentzVector proton1= p_hyplist->getTLorentzVector("p",1);
    TLorentzVector proton2= p_hyplist->getTLorentzVector("p",2);
    TLorentzVector pip= p_hyplist->getTLorentzVector("pi+",1);
    TLorentzVector pim= p_hyplist->getTLorentzVector("pi-",1);


    
//     cout << "proton1 mass is: " << proton1.M() << endl;
//     cout << "proton2 mass is: " << proton2.M() << endl;
//     cout << "pip mass is: " << pip.M() << endl;
//     cout << "pim mass is: " << pim.M() << endl;


    if ( p_hyplist->getIterStatus() == kTRUE) { 
      
      // calculating missing mass
      TLorentzVector pp_miss = (*beam) - (proton1 + proton2); 
      TLorentzVector miss4 = (*beam) - (proton1 + proton2 + pip + pim); 
      TLorentzVector pippim_invmass = (pip+pim);
 
      
      



      Double_t prob = p_hyplist->getProbAlg();
      if (simuflag == 0)  
	miss->Fill(pp_miss.M2() , miss4.M2(), pippim_invmass.M2(), prob);
      else
	miss->Fill(pp_miss.M2() , miss4.M2(), pippim_invmass.M2(), prob, etaflag);

     // cout << "pp_miss.M2:" << pp_miss.M2() << ", miss4.M2:" << miss4.M2() << endl;

    }
    else cerr << algoName << " got no TLorentzVector " << endl;

  }

  return kTRUE;
}

Bool_t HHypPPPipPimProjector::init()
{
  base_init();


  simCat=gHades->getCurrentEvent()->getCategory(catGeantKine);

  if (!simCat) simuflag=0;
  else {
    simuflag=1;
    cout << "Projector uses SIMULATION" << endl;

    CatPartSim = NULL;   // Category
    
    if((CatPartSim = gHades->getCurrentEvent()->getCategory( catPidPart )) == NULL) 
      {
	Error("init", "Cannot get catPidPartSim cat");
	return kFALSE;
      }
    
  }


  std::cout << "booking ntuples for " << algoName << std::endl;
  HHypChannel delme;
  
  // need to get name from channel
  TString input(delme.Get(initList));

  if (simuflag == 0)    
    miss = new TNtuple(TString("ntuple_")+input+TString("_miss"),"Demo ntuple","pp_miss:miss4:pippim_invmass:fProbAlg");
  else
    miss = new TNtuple(TString("ntuple_")+input+TString("_miss"),"Demo ntuple","pp_miss:miss4:pippim_invmass:fProbAlg:etaflag");


  std::cout << "...done" << std::endl;

  return kTRUE;
}

Bool_t HHypPPPipPimProjector::reinit()
{
  return kTRUE;
}

Bool_t HHypPPPipPimProjector::finalize()
{
  miss->Write();
  return kTRUE;
}