//*-- Author : M. Palka

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////
//
// HHypKpKmAlg
//
// HHypKpKm is an algorithm which selects KpKm (+X) events.
// This is the starting Algo for any Kp Km elastic or PP Pi0 Algo.
// F.e. HHypPPElastic needs this as input.
//
// This Algorythm will select 2, 3, 4 particle evens with the following
// conditions: 2pos, 2pos+1neg, 3pos, 4pos, 3pos+1neg, 2pos+2neg
// for the 3 particle events one particle is threaten as FAKE, and
// removed from the list, for 4 part two are removed
// => Output is in ANY case a 2pos event!!!
//
// 1 Fake events are only taken if ONEFAKE is set.
// 2 Fake events are only taken if TWOFAKES is set.
// Note: TWOFAKES will not add ONEFAKE option!
//
// Note for adding more fillers:
// Make sure you cover ALL cases!
// F.e. 3fakes: 2+p3 2+p2n1 2+p1n2 2+n3
//
////////////////////////////////////////////////////////////////////////

#warning "This code is NOT working correctly!!!!"
#warning "1) behaviour is completely different if inclusive or not is used"
#warning "2) comments/documentation/description is rong and refers to different algo"
#warning "3) code is superseeded by HHypAllFiller"
#warning "-> code should eighter be fixed or removed!!!"
#error "This code is NOT working correctly!!!!"
using namespace std;

#include <stdlib.h>
#include <iostream>
#include "hhypKpKmAlg.h"
#include "hypinfodef.h"

ClassImp(HHypKpKmAlg)

HHypKpKmAlg::HHypKpKmAlg(Char_t *name_i, Option_t par[])
  :HHypBaseAlgorithm(name_i,par)
{
  // 2 Particles
  fillerInclusive = NULL;
  // 2 Particles
  filler2 = NULL;
  // 2 + 1 fake
  filler2p1 = NULL;
  filler2m1 = NULL;
  // 2 + 2 fakes
  filler2p2 = NULL;
  filler2p1m1 = NULL;
  filler2m2 = NULL;
}

HHypKpKmAlg::~HHypKpKmAlg()
{
  if (fillerInclusive)
    delete fillerInclusive;
	
  if (filler2)
    delete filler2;

  if (filler2p1)
    delete filler2p1;
  if (filler2m1)
    delete filler2m1;
  
  if (filler2p2)
    delete filler2p2;
  if (filler2p1m1)
    delete filler2p1m1;
  if (filler2m2)
    delete filler2m2;
}

Bool_t HHypKpKmAlg::execute()
{
	// In principle, the first question should be:
	// How many particles are in that events!
	// Depending on that the appopriate filler should be called.
	// Doing it in the following way is a waste of cpu time ...
	

	if( use_InclusiveFiller){
		if(exitIdx!=ERROR_IDX){ exitIdx = fillerInclusive->execute(); }
		else cerr << " HHypKpKmAlg::execute() : exitIdx = "<< ERROR_IDX;
	    if (exitIdx > -1) return kTRUE;
	
	    return kFALSE;
	}
	
	// 2 Particles
	exitIdx = filler2->execute();
	
	// 2+1 fake
	if( use_2_1){
		// if no 3p 1n then try next filler
		if(exitIdx==ERROR_IDX) exitIdx = filler2m1->execute();
		// if no 3p 2n then try next filler
		if(exitIdx==ERROR_IDX) exitIdx = filler2p1->execute();
		// if no 2p 1n then forget about it
	}

	// 2+2 fakes
	if( use_2_2){
		if(exitIdx==ERROR_IDX) exitIdx = filler2m2->execute();
		if(exitIdx==ERROR_IDX) exitIdx = filler2p1m1->execute();
		if(exitIdx==ERROR_IDX) exitIdx = filler2p2->execute();
	}
	
	if (exitIdx > -1) return kTRUE;
	
	return kFALSE;
}

Bool_t HHypKpKmAlg::init()
{
	Bool_t flag;
	
	use_2_1 = kFALSE;
	use_2_1 = (GetOpt("ONEFAKE") != NULL);
	
	use_2_2 = kFALSE;
	use_2_2 = (GetOpt("TWOFAKES") != NULL);

	use_InclusiveFiller = kFALSE;
	use_InclusiveFiller = (GetOpt("use_InclusiveFiller") != NULL);

	/* say filler which particles I want to have */
	filler2 = new HHypListFiller("filler2", "filler2");
	filler2->SetExitList(exitList);
	
	filler2->AddTrack(HPidPhysicsConstants::pid("p"));
	filler2->AddTrack(HPidPhysicsConstants::pid("K+"));
      	filler2->AddTrack(HPidPhysicsConstants::pid("K-"));

	flag=filler2->init();

    if(use_InclusiveFiller)
	{
	    fillerInclusive = new HHypListFillerInclusive("fillerInclusive", "fillerInclusive");
		
	    fillerInclusive->SetExitList(exitList);
		
	    fillerInclusive->AddTrack(HPidPhysicsConstants::pid("K+"));
	    fillerInclusive->AddTrack(HPidPhysicsConstants::pid("K-"));
	    flag=fillerInclusive->init();
	}
	// 2 + 1 part filler
	if(use_2_1){
		cout << "HHypKpKmAlg will use option ONEFAKE" << endl;
	
		filler2p1 = new HHypListFiller("filler2p1", "filler2p1");
		filler2p1->SetExitList(exitList);
		
		filler2p1->AddTrack(HPidPhysicsConstants::pid("p"));
		filler2p1->AddTrack(HPidPhysicsConstants::pid("p"));
		filler2p1->AddTrack(HPidPhysicsConstants::fakePos());
		
		flag &= filler2p1->init();
		
		filler2m1 = new HHypListFiller("filler2m1", "filler2m1");
		filler2m1->SetExitList(exitList);
		
		filler2m1->AddTrack(HPidPhysicsConstants::pid("p"));
		filler2m1->AddTrack(HPidPhysicsConstants::pid("p"));
		filler2m1->AddTrack(HPidPhysicsConstants::fakeNeg());
		
		flag &= filler2m1->init();
	}	
	
	// 2 + 2 part filler
	if(use_2_2){
		cout << "HHypKpKmAlg will use option TWOFAKES" << endl;
	
		filler2p2 = new HHypListFiller("filler2p2", "filler2p2");
		filler2p2->SetExitList(exitList);
		
		filler2p2->AddTrack(HPidPhysicsConstants::pid("p"));
		filler2p2->AddTrack(HPidPhysicsConstants::pid("p"));
		filler2p2->AddTrack(HPidPhysicsConstants::fakePos());
		filler2p2->AddTrack(HPidPhysicsConstants::fakePos());
		
		flag &= filler2p2->init();

		filler2p1m1 = new HHypListFiller("filler2p1m1", "filler2p1m1");
		filler2p1m1->SetExitList(exitList);
		
		filler2p1m1->AddTrack(HPidPhysicsConstants::pid("p"));
		filler2p1m1->AddTrack(HPidPhysicsConstants::pid("p"));
		filler2p1m1->AddTrack(HPidPhysicsConstants::fakePos());
		filler2p1m1->AddTrack(HPidPhysicsConstants::fakeNeg());
		
		flag &= filler2p1m1->init();
				
		filler2m2 = new HHypListFiller("filler2m2", "filler2m2");
		filler2m2->SetExitList(exitList);
		
		filler2m2->AddTrack(HPidPhysicsConstants::pid("p"));
		filler2m2->AddTrack(HPidPhysicsConstants::pid("p"));
		filler2m2->AddTrack(HPidPhysicsConstants::fakeNeg());
		filler2m2->AddTrack(HPidPhysicsConstants::fakeNeg());
		
		flag &= filler2m2->init();
	}	
	
	return flag;
}

Bool_t HHypKpKmAlg::reinit()
{
  return kTRUE;
}

Bool_t HHypKpKmAlg::finalize()
{
  return kTRUE;
}
