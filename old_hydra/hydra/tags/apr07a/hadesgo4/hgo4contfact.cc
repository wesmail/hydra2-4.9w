//*-- AUTHOR : Jochen Markert

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HGo4ContFact
//
//  Factory for the parameter containers in libGo4
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hgo4contfact.h"
#include "hruntimedb.h"
#include <iostream> 
#include <iomanip>
#include "hgo4onlineconfig.h"
#include "htriggerhistbooker.h"
#include "hstarthistbooker.h"
#include "hhodohistbooker.h"
#include "hwallhistbooker.h"
#include "hrichhistbooker.h"
#include "hmdchistbooker.h"
#include "htofhistbooker.h"
#include "htofinohistbooker.h"
#include "hshowerhistbooker.h"
#include "hphysicshistbooker.h"

ClassImp(HGo4ContFact)

static HGo4ContFact gHGo4ContFact;

HGo4ContFact::HGo4ContFact() {
  // Constructor (called when the library is loaded)
  fName="Go4ContFact";
  fTitle="Factory for parameter containers in libGo4";
  setAllContainers();
  HRuntimeDb::instance()->addContFactory(this);
}

void HGo4ContFact::setAllContainers() {
  // Creates the Container objects with all accepted contexts and adds them to
  // the list of containers for the MDC library.
    HContainer* onlineconfig= new HContainer("Go4OnlineConfig",
					     "parameters for Go4 online configuration","Go4OnlineConfigProduction");
    containers->Add(onlineconfig);

    HContainer* triggerhistbooker= new HContainer("TriggerHistBooker",
						  "parameters for Trigger Hist Booking","TriggerHistBookerProduction");
    containers->Add(triggerhistbooker);

    HContainer* starthistbooker= new HContainer("StartHistBooker",
						"parameters for Start Hist Booking","StartHistBookerProduction");
    containers->Add(starthistbooker);

    HContainer* hodohistbooker= new HContainer("HodoHistBooker",
						"parameters for Hodo Hist Booking","HodoHistBookerProduction");
    containers->Add(hodohistbooker);

    HContainer* wallhistbooker= new HContainer("WallHistBooker",
						"parameters for Wall Hist Booking","WallHistBookerProduction");
    containers->Add(wallhistbooker);

    HContainer* richhistbooker= new HContainer("RichHistBooker",
					       "parameters for Rich Hist Booking","RichHistBookerProduction");
    containers->Add(richhistbooker);

    HContainer* mdchistbooker= new HContainer("MdcHistBooker",
					      "parameters for Mdc Hist Booking","MdcHistBookerProduction");
    containers->Add(mdchistbooker);

    HContainer* tofhistbooker= new HContainer("TofHistBooker",
					      "parameters for Tof Hist Booking","TofHistBookerProduction");
    containers->Add(tofhistbooker);

    HContainer* tofinohistbooker= new HContainer("TofinoHistBooker",
						 "parameters for Tofino Hist Booking","TofinoHistBookerProduction");
    containers->Add(tofinohistbooker);

    HContainer* showerhistbooker= new HContainer("ShowerHistBooker",
						 "parameters for Shower Hist Booking","ShowerHistBookerProduction");
    containers->Add(showerhistbooker);

    HContainer* physicshistbooker= new HContainer("PhysicsHistBooker",
						 "parameters for Physics Hist Booking","PhysicsHistBookerProduction");
    containers->Add(physicshistbooker);

}
HParSet* HGo4ContFact::createContainer(HContainer* c) {
  // Calls the constructor of the corresponding parameter container.
  // For an actual context, which is not an empty string and not the default context
  // of this container, the name is concatinated with the context.
  const char* name=c->GetName();
  if (strcmp(name,"Go4OnlineConfig")==0)
      return new HGo4OnlineConfig(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"TriggerHistBooker")==0)
      return new HTriggerHistBooker(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"StartHistBooker")==0)
      return new HStartHistBooker(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"HodoHistBooker")==0)
      return new HHodoHistBooker(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"WallHistBooker")==0)
      return new HWallHistBooker(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RichHistBooker")==0)
      return new HRichHistBooker(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcHistBooker")==0)
      return new HMdcHistBooker(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"TofHistBooker")==0)
      return new HTofHistBooker(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"TofinoHistBooker")==0)
      return new HTofinoHistBooker(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"ShowerHistBooker")==0)
      return new HShowerHistBooker(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"PhysicsHistBooker")==0)
      return new HPhysicsHistBooker(c->getConcatName().Data(),c->GetTitle(),c->getContext());

  return 0;
}
