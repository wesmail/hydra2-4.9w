//*-- AUTHOR : J. Markert

////////////////////////////////////////////////////////////////////////////
// HMdcHistFiller
//
// Container class for the mdc hists
////////////////////////////////////////////////////////////////////////////
//#include <stdlib.h>

#include "hmdchistfiller.h"
#include "hparamlist.h"
#include "hmonhist.h"
#include "htrendhist.h"
#include "hhistarray.h"
#include "hmdcconfig.h"
#include "hmdcraw.h"
#include "hmdcraweventheader.h"
#include "hmdccal1.h"
#include "hmdccutstat.h"
#include "hades.h"
#include "hevent.h"
#include "hcategory.h"
#include "hmdcdef.h"
#include "hiterator.h"
#include <TList.h>
#include <TArrayI.h>
#include <TString.h>

// Container/List pointers
HMdcCutStat*       HMdcHistFiller::cutstat=0;
HMdcConfig*        HMdcHistFiller::mdcconfig=0;
HMdcConnectedTdc*  HMdcHistFiller::mdclookup=0;

// Color definitions
TArrayI HMdcHistFiller::colorsSector;
TArrayI HMdcHistFiller::colorsModule;
TArrayI HMdcHistFiller::colorsCuts;
Int_t HMdcHistFiller::layerhists=1;
Int_t HMdcHistFiller::mbohists=1;
Int_t HMdcHistFiller::mbotdchists=1;
Int_t HMdcHistFiller::timescalhists=1;
Int_t HMdcHistFiller::timesrawhists=1;
Int_t HMdcHistFiller::normhists=1;

// Hist Pointers
HHistArray*    HMdcHistFiller::htime1Cal1=0;
HTrendHist*    HMdcHistFiller::htime1Cal1MeanTrend=0;
HMonHist*      HMdcHistFiller::htime1Cal1MeanTrendtemp=0;
HHistArray*    HMdcHistFiller::hcutstat=0;
HMonHist2*     HMdcHistFiller::hCal1Hits=0;
HMonHist2*     HMdcHistFiller::hrawRoc_Subev=0;
HMonHist2*     HMdcHistFiller::hrawError_Stat=0;
HHistArray*    HMdcHistFiller::hlayer=0;
HHistArray2*   HMdcHistFiller::hmbo_tdc=0;
HHistArray2*   HMdcHistFiller::hmbo_tdc_calib=0;
HHistArray*    HMdcHistFiller::htime1Mod0=0;
HHistArray*    HMdcHistFiller::htime1Mod1=0;
HHistArray*    HMdcHistFiller::htime1Mod2=0;
HHistArray*    HMdcHistFiller::htime1Mod3=0;
HHistArray*    HMdcHistFiller::htime2Mod0=0;
HHistArray*    HMdcHistFiller::htime2Mod1=0;
HHistArray*    HMdcHistFiller::htime2Mod2=0;
HHistArray*    HMdcHistFiller::htime2Mod3=0;
HHistArray*    HMdcHistFiller::htime12Mod0=0;
HHistArray*    HMdcHistFiller::htime12Mod1=0;
HHistArray*    HMdcHistFiller::htime12Mod2=0;
HHistArray*    HMdcHistFiller::htime12Mod3=0;
HHistArray2*   HMdcHistFiller::htime1_12=0;
HHistArray*    HMdcHistFiller::htime1rawMod0=0;
HHistArray*    HMdcHistFiller::htime1rawMod1=0;
HHistArray*    HMdcHistFiller::htime1rawMod2=0;
HHistArray*    HMdcHistFiller::htime1rawMod3=0;
HHistArray*    HMdcHistFiller::htime2rawMod0=0;
HHistArray*    HMdcHistFiller::htime2rawMod1=0;
HHistArray*    HMdcHistFiller::htime2rawMod2=0;
HHistArray*    HMdcHistFiller::htime2rawMod3=0;
HHistArray*    HMdcHistFiller::htime12rawMod0=0;
HHistArray*    HMdcHistFiller::htime12rawMod1=0;
HHistArray*    HMdcHistFiller::htime12rawMod2=0;
HHistArray*    HMdcHistFiller::htime12rawMod3=0;
HHistArray2*   HMdcHistFiller::htime1_12raw=0;
HHistArray2*   HMdcHistFiller::hmbo=0;
HTrendArray*   HMdcHistFiller::hmbotrendMod0=0;
HTrendArray*   HMdcHistFiller::hmbotrendMod1=0;
HTrendArray*   HMdcHistFiller::hmbotrendMod2=0;
HTrendArray*   HMdcHistFiller::hmbotrendMod3=0;
HHistArray*    HMdcHistFiller::hmod_timestat_calib=0;

// Hist Addon Pointers
HMonHistAddon* HMdcHistFiller::htime1Cal1A=0;
HMonHistAddon* HMdcHistFiller::htime1Cal1MeanTrendA=0;
HMonHistAddon* HMdcHistFiller::htime1Cal1MeanTrendtempA=0;
HMonHistAddon* HMdcHistFiller::hcutstatA=0;
HMonHistAddon* HMdcHistFiller::hCal1HitsA=0;
HMonHistAddon* HMdcHistFiller::hrawRoc_SubevA=0;
HMonHistAddon* HMdcHistFiller::hrawError_StatA=0;
HMonHistAddon* HMdcHistFiller::hlayerA=0;
HMonHistAddon* HMdcHistFiller::hmbo_tdcA=0;
HMonHistAddon* HMdcHistFiller::hmbo_tdc_calibA=0;
HMonHistAddon* HMdcHistFiller::htime1Mod0A=0;
HMonHistAddon* HMdcHistFiller::htime1Mod1A=0;
HMonHistAddon* HMdcHistFiller::htime1Mod2A=0;
HMonHistAddon* HMdcHistFiller::htime1Mod3A=0;
HMonHistAddon* HMdcHistFiller::htime2Mod0A=0;
HMonHistAddon* HMdcHistFiller::htime2Mod1A=0;
HMonHistAddon* HMdcHistFiller::htime2Mod2A=0;
HMonHistAddon* HMdcHistFiller::htime2Mod3A=0;
HMonHistAddon* HMdcHistFiller::htime12Mod0A=0;
HMonHistAddon* HMdcHistFiller::htime12Mod1A=0;
HMonHistAddon* HMdcHistFiller::htime12Mod2A=0;
HMonHistAddon* HMdcHistFiller::htime12Mod3A=0;
HMonHistAddon* HMdcHistFiller::htime1_12A=0;
HMonHistAddon* HMdcHistFiller::htime1rawMod0A=0;
HMonHistAddon* HMdcHistFiller::htime1rawMod1A=0;
HMonHistAddon* HMdcHistFiller::htime1rawMod2A=0;
HMonHistAddon* HMdcHistFiller::htime1rawMod3A=0;
HMonHistAddon* HMdcHistFiller::htime2rawMod0A=0;
HMonHistAddon* HMdcHistFiller::htime2rawMod1A=0;
HMonHistAddon* HMdcHistFiller::htime2rawMod2A=0;
HMonHistAddon* HMdcHistFiller::htime2rawMod3A=0;
HMonHistAddon* HMdcHistFiller::htime12rawMod0A=0;
HMonHistAddon* HMdcHistFiller::htime12rawMod1A=0;
HMonHistAddon* HMdcHistFiller::htime12rawMod2A=0;
HMonHistAddon* HMdcHistFiller::htime12rawMod3A=0;
HMonHistAddon* HMdcHistFiller::htime1_12rawA=0;
HMonHistAddon* HMdcHistFiller::hmboA=0;
HMonHistAddon* HMdcHistFiller::hmbotrendMod0A=0;
HMonHistAddon* HMdcHistFiller::hmbotrendMod1A=0;
HMonHistAddon* HMdcHistFiller::hmbotrendMod2A=0;
HMonHistAddon* HMdcHistFiller::hmbotrendMod3A=0;
HMonHistAddon* HMdcHistFiller::hmod_timestat_calibA=0;


ClassImp(HMdcHistFiller)

HMdcHistFiller::HMdcHistFiller(const char* name,const char* title)
    : TNamed(name,title)
{
  colorsSector.Set(6);
  colorsModule.Set(4);
  colorsCuts.Set(4);

  mdcconfig   = new HMdcConfig("MdcConfig","MdcConfig");
  mdclookup   = new HMdcConnectedTdc("MdcLookup","MdcLookup");
  initCounter();
}
HMdcHistFiller::~HMdcHistFiller()
{
    delete mdcconfig;
    delete mdclookup;
}
HMdcConfig* HMdcHistFiller::getMdcConfig()
{
    return mdcconfig;
}
HMdcConnectedTdc* HMdcHistFiller::getMdcLookup()
{
    return mdclookup;
}
void HMdcHistFiller::fillRaw(HMdcRaw* raw)
{
    Int_t s,m,mb,ch;
    raw->getAddress(s,m,mb,ch);
    Float_t time1=raw->getTime(1);
    Float_t time2=raw->getTime(2);
    if(hmbo_tdc&&mbotdchists==1) hmbo_tdc ->getP(s,m)->Fill((Axis_t)mb,(Axis_t)((Int_t)(ch/8)));

    countSecMod[s][m]++;

    if(timesrawhists==1)
    {
	if(htime1rawMod0&&m==0 )htime1rawMod0 ->getP(0,s)->Fill(time1);
	if(htime1rawMod1&&m==1 )htime1rawMod1 ->getP(0,s)->Fill(time1);
	if(htime1rawMod2&&m==2 )htime1rawMod2 ->getP(0,s)->Fill(time1);
	if(htime1rawMod3&&m==3 )htime1rawMod3 ->getP(0,s)->Fill(time1);
	if(htime2rawMod0&&m==0 )htime2rawMod0 ->getP(0,s)->Fill(time2);
	if(htime2rawMod1&&m==1 )htime2rawMod1 ->getP(0,s)->Fill(time2);
	if(htime2rawMod2&&m==2 )htime2rawMod2 ->getP(0,s)->Fill(time2);
	if(htime2rawMod3&&m==3 )htime2rawMod3 ->getP(0,s)->Fill(time2);
	if(htime12rawMod0&&m==0)htime12rawMod0->getP(0,s)->Fill(time1-time2);
	if(htime12rawMod1&&m==1)htime12rawMod1->getP(0,s)->Fill(time1-time2);
	if(htime12rawMod2&&m==2)htime12rawMod2->getP(0,s)->Fill(time1-time2);
	if(htime12rawMod3&&m==3)htime12rawMod3->getP(0,s)->Fill(time1-time2);

	if(htime1_12raw)htime1_12raw->fill(s,m,time1,time1-time2);
    }

    if(mbohists==1)
    {
	if(hmbo)hmbo->fill(0,m,s,mb);
    }
}
void HMdcHistFiller::fillRawHeader(HMdcRawEventHeader* rawhead)
{
    Int_t roc          =rawhead->getRoc();
    Int_t subeventsize =rawhead->getSubEvtSize();
    Int_t errorword    =rawhead->getErrorWord();
    Int_t s            =rawhead->getSector();
    Int_t m            =rawhead->getModule();

    if(hrawError_Stat&&s>=0&&m>=0)
    {
        if(errorword&0xFF==0)hrawError_Stat->getP()->Fill(m*6+s,-0.5);
	for(Int_t i=0;i<8;i++)
	{
	    if(((errorword>>i)&1)!=0)hrawError_Stat->getP()->Fill(m*6+s,i);
	}
    }
    if(hrawRoc_Subev)
    {
        TH1* h=hrawRoc_Subev->getP();
        Int_t x=(((m*6+s)*5)+ roc);
        Float_t scale=(hrawRoc_Subev->getYUp() - hrawRoc_Subev->getYLo() )/(Float_t)hrawRoc_Subev->getNBinsY();
	h->SetBinContent(x,(Int_t)(subeventsize/scale),h->GetBinContent(x,(Int_t)(subeventsize/scale))+1);
    }
}
void HMdcHistFiller::fillCalibration(HIterator* iterraw)
{
    Int_t s,m,mb,ch;

    HMdcRaw* raw;
    while ((raw=(HMdcRaw *)iterraw->Next())!=0)
    {
	raw->getAddress(s,m,mb,ch);
        if(hmbo_tdc_calib) hmbo_tdc_calib->getP(s,m)->Fill((Axis_t)mb,(Axis_t)((Int_t)(ch/8)));

	if(hmod_timestat_calib)
	{
	    for(Int_t i=0;i<6;i++)
	    {
		if(raw->getTime(i+1)>0){hmod_timestat_calib->getP(s,m)->Fill(i);}
		else if (raw->getTime(i+1)==0){hmod_timestat_calib->getP(s,m)->Fill(6);}
                else {hmod_timestat_calib->getP(s,m)->Fill(7);}
	    }
	}
    }
}
void HMdcHistFiller::fillCal1(HMdcCal1* cal1)
{
    Int_t s,m,l,c;
    cal1->getAddress(s,m,l,c);
    Float_t time1=cal1->getTime1();
    Float_t time2=cal1->getTime2();
    if(htime1Cal1)
    {
	htime1Cal1->getP(0,0)->Fill(time1);
        htime1Cal1->getP(0,1)->Fill(time2);
    }
    if(htime1Cal1MeanTrendtemp)htime1Cal1MeanTrendtemp->getP()->Fill(time1);

    if(hCal1Hits)hCal1Hits->getP()->Fill(s,m,1);
    if(hlayer)hlayer->getP(0,s*4*6+m*6+l)->Fill(c);

    if(timescalhists==1)
    {
	if(htime1Mod0&&m==0 )htime1Mod0 ->getP(0,s)->Fill(time1);
	if(htime1Mod1&&m==1 )htime1Mod1 ->getP(0,s)->Fill(time1);
	if(htime1Mod2&&m==2 )htime1Mod2 ->getP(0,s)->Fill(time1);
	if(htime1Mod3&&m==3 )htime1Mod3 ->getP(0,s)->Fill(time1);
	if(htime2Mod0&&m==0 )htime2Mod0 ->getP(0,s)->Fill(time2);
	if(htime2Mod1&&m==1 )htime2Mod1 ->getP(0,s)->Fill(time2);
	if(htime2Mod2&&m==2 )htime2Mod2 ->getP(0,s)->Fill(time2);
	if(htime2Mod3&&m==3 )htime2Mod3 ->getP(0,s)->Fill(time2);
	if(htime12Mod0&&m==0)htime12Mod0->getP(0,s)->Fill(time2-time1);
	if(htime12Mod1&&m==1)htime12Mod1->getP(0,s)->Fill(time2-time1);
	if(htime12Mod2&&m==2)htime12Mod2->getP(0,s)->Fill(time2-time1);
	if(htime12Mod3&&m==3)htime12Mod3->getP(0,s)->Fill(time2-time1);

	if(htime1_12)htime1_12->fill(s,m,time1,time2-time1);
    }
}
void HMdcHistFiller::fillCutStat()
{

    if(hcutstat){
	cutstat=HMdcCutStat::getExObject();
	if(cutstat)
	{
	    Int_t t1,t2,t1t2,wire,t1_t1t2,cutall,allhits;
	    for(Int_t m=0; m<4; m++) {
		for(Int_t s=0; s<6; s++) {
		    cutstat->getCal1StatCut(s,m,&t1,&t2,&t1t2,&wire,&t1_t1t2,&cutall,&allhits);
		    if(allhits!=0){
			if(normhists==1)
			{
			    hcutstat->setBinContent(0,1,m*6+s+1,(Float_t)(allhits-t1)     /allhits); //3
			    hcutstat->setBinContent(0,2,m*6+s+1,(Float_t)(allhits-t1t2)   /allhits); //5
			    hcutstat->setBinContent(0,3,m*6+s+1,(Float_t)(allhits-cutall) /allhits); //4
			    hcutstat->setBinContent(0,0,m*6+s+1,(Float_t)allhits/allhits); //2
			}
			else if(normhists==0)
			{
			    hcutstat->setBinContent(0,1,m*6+s+1,(Float_t)(allhits-t1)); //3
			    hcutstat->setBinContent(0,2,m*6+s+1,(Float_t)(allhits-t1t2)); //5
			    hcutstat->setBinContent(0,3,m*6+s+1,(Float_t)(allhits-cutall)); //4
			    hcutstat->setBinContent(0,0,m*6+s+1,(Float_t)allhits); //2
			}
		    }
		}
	    }
	    if(hcutstatA->getResetable()==1)cutstat->clear();
	}
    }
}
void HMdcHistFiller::fillTrend()
{
    if(htime1Cal1MeanTrend)htime1Cal1MeanTrend->fill(htime1Cal1MeanTrendtemp->getP()->GetMean(), htime1Cal1MeanTrendtemp->getP()->GetRMS());
    if(htime1Cal1MeanTrendtemp)htime1Cal1MeanTrendtemp->getP()->Reset();

    for(Int_t s=0;s<6;s++){
	for(Int_t m=0;m<4;m++){
	    if(hmbotrendMod0&&m==0)hmbotrendMod0->fill(0,s,countSecMod[s][m]);
            if(hmbotrendMod1&&m==1)hmbotrendMod1->fill(0,s,countSecMod[s][m]);
            if(hmbotrendMod2&&m==2)hmbotrendMod2->fill(0,s,countSecMod[s][m]);
            if(hmbotrendMod3&&m==3)hmbotrendMod3->fill(0,s,countSecMod[s][m]);
	}
    }
    initCounter();

}
void HMdcHistFiller::fillMdcConfig(Int_t fCount)
{
    HCategory* catRaw=gHades->getCurrentEvent()->getCategory(catMdcRaw);
    if(catRaw)
    {
	mdcconfig->setEvtSeqNb(catRaw->getEntries());
	mdcconfig->setEvtNb(fCount);
	mdcconfig->setRawMult(catRaw->getEntries());
    }
    HCategory* catCal1=gHades->getCurrentEvent()->getCategory(catMdcCal1);
    if(catCal1)
    {
	mdcconfig->setCalMult(catCal1->getEntries());
    }
}
void   HMdcHistFiller::putMdcParams(HParamList* l,TList* histpool)
{
    // Puts all params of HMdcHistFiller to the parameter list of
    // HParamList (which ist used by the io);

       Text_t mytemp[500];
       l->add("colorsSector"           ,colorsSector  );
       l->add("colorsModule"           ,colorsModule  );
       l->add("colorsCuts"             ,colorsCuts    );
       l->add("layerhists"             ,layerhists    );
       l->add("mbohists"               ,mbohists      );
       l->add("mbotdchists"            ,mbotdchists   );
       l->add("timescalhists"          ,timescalhists );
       l->add("timesrawhists"          ,timesrawhists );
       l->add("normhists"              ,normhists     );
       htime1Cal1A->getText(mytemp);
       l->add("htime1Cal1"             ,mytemp,500);
       htime1Cal1MeanTrendA->getText(mytemp);
       l->add("htime1Cal1MeanTrend"    ,mytemp,500);
       htime1Cal1MeanTrendtempA->getText(mytemp);
       l->add("htime1Cal1MeanTrendTemp",mytemp,500);
       hcutstatA->getText(mytemp);
       l->add("hcutstat"               ,mytemp,500);
       hCal1HitsA->getText(mytemp);
       l->add("hcal1hits"              ,mytemp,500);
       hrawRoc_SubevA->getText(mytemp);
       l->add("hrawRoc_Subev"          ,mytemp,500);
       hrawError_StatA->getText(mytemp);
       l->add("hrawError_Stat"         ,mytemp,500);
       hlayerA->getText(mytemp);
       l->add("hlayer"                 ,mytemp,500);
       hmbo_tdcA->getText(mytemp);
       l->add("hmbo_tdc"               ,mytemp,500);
       hmbo_tdc_calibA->getText(mytemp);
       l->add("hmbo_tdc_calib"         ,mytemp,500);
       htime1Mod0A->getText(mytemp);
       l->add("htime1Mod0"             ,mytemp,500);
       htime1Mod1A->getText(mytemp);
       l->add("htime1Mod1"             ,mytemp,500);
       htime1Mod2A->getText(mytemp);
       l->add("htime1Mod2"             ,mytemp,500);
       htime1Mod3A->getText(mytemp);
       l->add("htime1Mod3"             ,mytemp,500);
       htime2Mod0A->getText(mytemp);
       l->add("htime2Mod0"             ,mytemp,500);
       htime2Mod1A->getText(mytemp);
       l->add("htime2Mod1"             ,mytemp,500);
       htime2Mod2A->getText(mytemp);
       l->add("htime2Mod2"             ,mytemp,500);
       htime2Mod3A->getText(mytemp);
       l->add("htime2Mod3"             ,mytemp,500);
       htime12Mod0A->getText(mytemp);
       l->add("htime12Mod0"            ,mytemp,500);
       htime12Mod1A->getText(mytemp);
       l->add("htime12Mod1"            ,mytemp,500);
       htime2Mod2A->getText(mytemp);
       l->add("htime12Mod2"            ,mytemp,500);
       htime12Mod3A->getText(mytemp);
       l->add("htime12Mod3"            ,mytemp,500);
       htime1_12A->getText(mytemp);
       l->add("htime1_12"              ,mytemp,500);
       htime1rawMod0A->getText(mytemp);
       l->add("htime1rawMod0"          ,mytemp,500);
       htime1rawMod1A->getText(mytemp);
       l->add("htime1rawMod1"          ,mytemp,500);
       htime1rawMod2A->getText(mytemp);
       l->add("htime1rawMod2"          ,mytemp,500);
       htime1rawMod3A->getText(mytemp);
       l->add("htime1rawMod3"          ,mytemp,500);
       htime2rawMod0A->getText(mytemp);
       l->add("htime2rawMod0"          ,mytemp,500);
       htime2rawMod1A->getText(mytemp);
       l->add("htime2rawMod1"          ,mytemp,500);
       htime2rawMod2A->getText(mytemp);
       l->add("htime2rawMod2"          ,mytemp,500);
       htime2rawMod3A->getText(mytemp);
       l->add("htime2rawMod3"          ,mytemp,500);
       htime12rawMod0A->getText(mytemp);
       l->add("htime12rawMod0"         ,mytemp,500);
       htime12rawMod1A->getText(mytemp);
       l->add("htime12rawMod1"         ,mytemp,500);
       htime2rawMod2A->getText(mytemp);
       l->add("htime12rawMod2"         ,mytemp,500);
       htime12rawMod3A->getText(mytemp);
       l->add("htime12rawMod3"         ,mytemp,500);
       htime1_12rawA->getText(mytemp);
       l->add("htime1_12raw"           ,mytemp,500);
       hmboA->getText(mytemp);
       l->add("hmbo"                   ,mytemp,500);
       hmbotrendMod0A->getText(mytemp);
       l->add("hmbotrendMod0"          ,mytemp,500);
       hmbotrendMod1A->getText(mytemp);
       l->add("hmbotrendMod1"          ,mytemp,500);
       hmbotrendMod2A->getText(mytemp);
       l->add("hmbotrendMod2"          ,mytemp,500);
       hmbotrendMod3A->getText(mytemp);
       l->add("hmbotrendMod3"          ,mytemp,500);
       hmod_timestat_calibA->getText(mytemp);
       l->add("hmod_timestat_calib"   ,mytemp,500);
}
Bool_t HMdcHistFiller::getMdcParams(HParamList* l,TList* histpool,TList* activeHists)
{
    Char_t *mdclabel[24] = {
	"I1","I2","I3","I4","I5","I6",
	"II1","II2","II3","II4","II5","II6",
	"III1","III2","III3","III4","III5","III6",
	"IV1","IV2","IV3","IV4","IV5","IV6"
    };


    if(!(l->fill("colorsSector"  ,&colorsSector  ))) return kFALSE;
    if(!(l->fill("colorsModule"  ,&colorsModule  ))) return kFALSE;
    if(!(l->fill("colorsCuts"    ,&colorsCuts    ))) return kFALSE;
    if(!(l->fill("layerhists"    ,&layerhists    ))) return kFALSE;
    if(!(l->fill("mbohists"      ,&mbohists      ))) return kFALSE;
    if(!(l->fill("mbotdchists"   ,&mbotdchists   ))) return kFALSE;
    if(!(l->fill("timescalhists" ,&timescalhists ))) return kFALSE;
    if(!(l->fill("timesrawhists" ,&timesrawhists ))) return kFALSE;
    if(!(l->fill("normhists"     ,&normhists     ))) return kFALSE;

    Text_t mytemp[500];
    if(!(l->fill("htime1Cal1",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1Cal1A=new HMonHistAddon(mytemp));
	if(htime1Cal1A->getActive()==1)
	{
	    activeHists->Add(htime1Cal1=new HHistArray(*htime1Cal1A));
            htime1Cal1->getP(0,0)->SetLineColor(1);
	    htime1Cal1->getP(0,1)->SetLineColor(2);
	}
    }
    if(!(l->fill("htime1Cal1MeanTrend",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1Cal1MeanTrendA=new HMonHistAddon(mytemp));
	if(htime1Cal1MeanTrendA->getActive()==1)
	{
	    activeHists->Add(htime1Cal1MeanTrend=new HTrendHist(*htime1Cal1MeanTrendA));
	}
    }
    if(!(l->fill("htime1Cal1MeanTrendTemp",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1Cal1MeanTrendtempA=new HMonHistAddon(mytemp));
	if(htime1Cal1MeanTrendtempA->getActive()==1)
	{
	    activeHists->Add(htime1Cal1MeanTrendtemp=new HMonHist(*htime1Cal1MeanTrendtempA));
	}
    }
    if(!(l->fill("hcutstat",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hcutstatA=new HMonHistAddon(mytemp));
	if(hcutstatA->getActive()==1)
	{
	    activeHists->Add(hcutstat=new HHistArray(*hcutstatA));
	    hcutstat->getP(0,0)->SetFillColor(colorsCuts[0]);
	    hcutstat->getP(0,1)->SetFillColor(colorsCuts[1]);
	    hcutstat->getP(0,2)->SetFillColor(colorsCuts[2]);
	    hcutstat->getP(0,3)->SetFillColor(colorsCuts[3]);

	    for(Int_t i=0;i<24;i++){
		hcutstat->getP(0,4)->GetXaxis()->SetBinLabel(i+1,mdclabel[i]);
	    }
            hcutstat->getP(0,4)->GetXaxis()->SetLabelSize(0.05);
	    hcutstat->getP(0,4)->LabelsOption("v");
	    hcutstat->getP(0,4)->SetTitleOffset(1.15,"X");
	}
    }
    if(!(l->fill("hcal1hits",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hCal1HitsA=new HMonHistAddon(mytemp));
	if(hCal1HitsA->getActive()==1)
	{
	    activeHists->Add(hCal1Hits=new HMonHist2(*hCal1HitsA));
	}
    }
    if(!(l->fill("hrawRoc_Subev",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hrawRoc_SubevA=new HMonHistAddon(mytemp));
	if(hrawRoc_SubevA->getActive()==1)
	{
	    activeHists->Add(hrawRoc_Subev=new HMonHist2(*hrawRoc_SubevA));

	    for(Int_t i=0;i<24;i++){
		hrawRoc_Subev->getP()->GetXaxis()->SetBinLabel(i*5+1,mdclabel[i]);
	    }
            hrawRoc_Subev->getP()->GetXaxis()->SetLabelSize(0.05);
            hrawRoc_Subev->getP()->LabelsOption("v");
	    hrawRoc_Subev->getP()->SetTitleOffset(1.15,"X");
	}
    }
    if(!(l->fill("hrawError_Stat",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hrawError_StatA=new HMonHistAddon(mytemp));
	if(hrawError_StatA->getActive()==1)
	{
	    activeHists->Add(hrawError_Stat=new HMonHist2(*hrawError_StatA));

	    for(Int_t i=0;i<24;i++){
		hrawError_Stat->getP()->GetXaxis()->SetBinLabel(i+1,mdclabel[i]);
	    }
	    hrawError_Stat->getP()->GetXaxis()->SetLabelSize(0.05);
	    hrawError_Stat->getP()->LabelsOption("v");
	    hrawError_Stat->getP()->SetTitleOffset(1.15,"X");
	}
    }
    if(!(l->fill("hlayer",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hlayerA=new HMonHistAddon(mytemp));
	if(hlayerA->getActive()==1&&layerhists==1)
	{
	    activeHists->Add(hlayer=new HHistArray(*hlayerA));
	}
    }
    if(!(l->fill("hmbo_tdc",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hmbo_tdcA=new HMonHistAddon(mytemp));
	if(hmbo_tdcA->getActive()==1&&mbotdchists==1)
	{
	    activeHists->Add(hmbo_tdc=new HHistArray2(*hmbo_tdcA));
	}

    }
    if(!(l->fill("hmbo_tdc_calib",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hmbo_tdc_calibA=new HMonHistAddon(mytemp));
	if(hmbo_tdc_calibA->getActive()==1)
	{
	    activeHists->Add(hmbo_tdc_calib=new HHistArray2(*hmbo_tdc_calibA));
	}

    }
    if(!(l->fill("hmod_timestat_calib",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hmod_timestat_calibA=new HMonHistAddon(mytemp));
	if(hmod_timestat_calibA->getActive()==1)
	{
	    Char_t *timelabel[8] = {"1","2","3","4","5","6","0","<0"};


	    activeHists->Add(hmod_timestat_calib=new HHistArray(*hmod_timestat_calibA));
	    for(Int_t i=0;i<6;i++){
		for(Int_t j=0;j<4;j++){
		    hmod_timestat_calib->getP(i,j)->SetFillColor(colorsModule[j]);
		    for(Int_t l=0;l<8;l++){
			hmod_timestat_calib->getP(i,j)->GetXaxis()->SetBinLabel(l+1,timelabel[l]);
                        hmod_timestat_calib->getP(i,j)->GetXaxis()->SetLabelSize(0.055);
		    }
		}
	    }
	   
	}

    }
    if(!getTimesCalParams(l,histpool,activeHists))return kFALSE;
    if(!getTimesRawParams(l,histpool,activeHists))return kFALSE;
    if(!getMboParams(l,histpool,activeHists))     return kFALSE;

    return kTRUE;
}
Bool_t HMdcHistFiller::getTimesCalParams(HParamList* l,TList* histpool,TList* activeHists)
{
    Text_t mytemp[500];
    if(!(l->fill("htime1Mod0",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1Mod0A=new HMonHistAddon(mytemp));
	if(htime1Mod0A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime1Mod0=new HHistArray(*htime1Mod0A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime1Mod0->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime1Mod1",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1Mod1A=new HMonHistAddon(mytemp));
	if(htime1Mod1A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime1Mod1=new HHistArray(*htime1Mod1A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime1Mod1->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime1Mod2",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1Mod2A=new HMonHistAddon(mytemp));
	if(htime1Mod2A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime1Mod2=new HHistArray(*htime1Mod2A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime1Mod2->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime1Mod3",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1Mod3A=new HMonHistAddon(mytemp));
	if(htime1Mod3A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime1Mod3=new HHistArray(*htime1Mod3A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime1Mod3->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime2Mod0",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime2Mod0A=new HMonHistAddon(mytemp));
	if(htime2Mod0A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime2Mod0=new HHistArray(*htime2Mod0A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime2Mod0->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime2Mod1",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime2Mod1A=new HMonHistAddon(mytemp));
	if(htime2Mod1A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime2Mod1=new HHistArray(*htime2Mod1A));
	    for(Int_t i=0;i<6;i++)
		{
		    htime2Mod1->getP(0,i)->SetLineColor(colorsSector[i]);
		}
	}
    }
    if(!(l->fill("htime2Mod2",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime2Mod2A=new HMonHistAddon(mytemp));
	if(htime2Mod2A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime2Mod2=new HHistArray(*htime2Mod2A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime2Mod2->getP(0,i)->SetLineColor(colorsSector[i]);
		}
	    }
    }
    if(!(l->fill("htime2Mod3",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime2Mod3A=new HMonHistAddon(mytemp));
	if(htime2Mod3A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime2Mod3=new HHistArray(*htime2Mod3A));
	    for(Int_t i=0;i<6;i++)
	    {
		    htime2Mod3->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime12Mod0",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime12Mod0A=new HMonHistAddon(mytemp));
	if(htime12Mod0A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime12Mod0=new HHistArray(*htime12Mod0A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime12Mod0->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime12Mod1",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime12Mod1A=new HMonHistAddon(mytemp));
	if(htime12Mod1A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime12Mod1=new HHistArray(*htime12Mod1A));
		for(Int_t i=0;i<6;i++)
		{
		    htime12Mod1->getP(0,i)->SetLineColor(colorsSector[i]);
		}
	}
    }
    if(!(l->fill("htime12Mod2",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime12Mod2A=new HMonHistAddon(mytemp));
	if(htime12Mod2A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime12Mod2=new HHistArray(*htime12Mod2A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime12Mod2->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime12Mod3",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime12Mod3A=new HMonHistAddon(mytemp));
	if(htime12Mod3A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime12Mod3=new HHistArray(*htime12Mod3A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime12Mod3->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime1_12",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1_12A=new HMonHistAddon(mytemp));
	if(htime1_12A->getActive()==1&&timescalhists==1)
	{
	    activeHists->Add(htime1_12=new HHistArray2(*htime1_12A));
	}
    }
    return kTRUE;
}
Bool_t HMdcHistFiller::getTimesRawParams(HParamList* l,TList* histpool,TList* activeHists)
{
    Text_t mytemp[500];
    if(!(l->fill("htime1rawMod0",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1rawMod0A=new HMonHistAddon(mytemp));
	if(htime1rawMod0A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime1rawMod0=new HHistArray(*htime1rawMod0A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime1rawMod0->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime1rawMod1",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1rawMod1A=new HMonHistAddon(mytemp));
	if(htime1rawMod1A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime1rawMod1=new HHistArray(*htime1rawMod1A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime1rawMod1->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime1rawMod2",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1rawMod2A=new HMonHistAddon(mytemp));
	if(htime1rawMod2A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime1rawMod2=new HHistArray(*htime1rawMod2A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime1rawMod2->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime1rawMod3",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1rawMod3A=new HMonHistAddon(mytemp));
	if(htime1rawMod3A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime1rawMod3=new HHistArray(*htime1rawMod3A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime1rawMod3->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime2rawMod0",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime2rawMod0A=new HMonHistAddon(mytemp));
	if(htime2rawMod0A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime2rawMod0=new HHistArray(*htime2rawMod0A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime2rawMod0->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime2rawMod1",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime2rawMod1A=new HMonHistAddon(mytemp));
	if(htime2rawMod1A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime2rawMod1=new HHistArray(*htime2rawMod1A));
	    for(Int_t i=0;i<6;i++)
		{
		    htime2rawMod1->getP(0,i)->SetLineColor(colorsSector[i]);
		}
	}
    }
    if(!(l->fill("htime2rawMod2",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime2rawMod2A=new HMonHistAddon(mytemp));
	if(htime2rawMod2A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime2rawMod2=new HHistArray(*htime2rawMod2A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime2rawMod2->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime2rawMod3",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime2rawMod3A=new HMonHistAddon(mytemp));
	if(htime2rawMod3A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime2rawMod3=new HHistArray(*htime2rawMod3A));
	    for(Int_t i=0;i<6;i++)
	    {
		    htime2rawMod3->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime12rawMod0",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime12rawMod0A=new HMonHistAddon(mytemp));
	if(htime12rawMod0A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime12rawMod0=new HHistArray(*htime12rawMod0A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime12rawMod0->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime12rawMod1",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime12rawMod1A=new HMonHistAddon(mytemp));
	if(htime12rawMod1A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime12rawMod1=new HHistArray(*htime12rawMod1A));
		for(Int_t i=0;i<6;i++)
		{
		    htime12rawMod1->getP(0,i)->SetLineColor(colorsSector[i]);
		}
	}
    }
    if(!(l->fill("htime12rawMod2",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime12rawMod2A=new HMonHistAddon(mytemp));
	if(htime12rawMod2A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime12rawMod2=new HHistArray(*htime12rawMod2A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime12rawMod2->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime12rawMod3",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime12rawMod3A=new HMonHistAddon(mytemp));
	if(htime12rawMod3A->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime12rawMod3=new HHistArray(*htime12rawMod3A));
	    for(Int_t i=0;i<6;i++)
	    {
		htime12rawMod3->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("htime1_12raw",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(htime1_12rawA=new HMonHistAddon(mytemp));
	if(htime1_12rawA->getActive()==1&&timesrawhists==1)
	{
	    activeHists->Add(htime1_12raw=new HHistArray2(*htime1_12rawA));
	}
    }
    return kTRUE;
}

Bool_t HMdcHistFiller::getMboParams(HParamList* l,TList* histpool,TList* activeHists)
{
    Text_t mytemp[500];
    if(!(l->fill("hmbotrendMod0",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hmbotrendMod0A=new HMonHistAddon(mytemp));
	if(hmbotrendMod0A->getActive()==1&&mbohists==1)
	{
	    activeHists->Add(hmbotrendMod0=new HTrendArray(*hmbotrendMod0A));
	    for(Int_t i=0;i<6;i++)
	    {
		hmbotrendMod0->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("hmbotrendMod1",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hmbotrendMod1A=new HMonHistAddon(mytemp));
	if(hmbotrendMod1A->getActive()==1&&mbohists==1)
	{
	    activeHists->Add(hmbotrendMod1=new HTrendArray(*hmbotrendMod1A));
	    for(Int_t i=0;i<6;i++)
	    {
		hmbotrendMod1->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("hmbotrendMod2",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hmbotrendMod2A=new HMonHistAddon(mytemp));
	if(hmbotrendMod2A->getActive()==1&&mbohists==1)
	{
	    activeHists->Add(hmbotrendMod2=new HTrendArray(*hmbotrendMod2A));
	    for(Int_t i=0;i<6;i++)
	    {
		hmbotrendMod2->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("hmbotrendMod3",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hmbotrendMod3A=new HMonHistAddon(mytemp));
	if(hmbotrendMod3A->getActive()==1&&mbohists==1)
	{
	    activeHists->Add(hmbotrendMod3=new HTrendArray(*hmbotrendMod3A));
	    for(Int_t i=0;i<6;i++)
	    {
		hmbotrendMod3->getP(0,i)->SetLineColor(colorsSector[i]);
	    }
	}
    }
    if(!(l->fill("hmbo",mytemp,500))) return kFALSE;
    else
    {
	histpool->Add(hmboA=new HMonHistAddon(mytemp));
	if(hmboA->getActive()==1&&mbohists==1)
	{
	    activeHists->Add(hmbo=new HHistArray2(*hmboA));
	}
    }
    return kTRUE;
}
