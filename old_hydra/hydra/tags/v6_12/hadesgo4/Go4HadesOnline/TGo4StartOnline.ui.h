/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void TGo4StartOnline::init()
{


    setCaption(" Start Go4 Online " );
    fxTGo4GUIRegistry = TGo4GUIRegistry::Instance(); 	/** Get the pointer to the GUI Registry  **/
    
    fxTGo4StartOnlineSlots = dynamic_cast <TGo4StartOnlineSlots *> (fxTGo4GUIRegistry->GetSlotClass("TGo4StartOnlineSlots"));   
    
    /* test if  a Slot Object already exist for this GUI*/
    
   if(fxTGo4StartOnlineSlots  !=0) {
   /* if it is already there, set this GUI  as window of the slot Object */
		fxTGo4StartOnlineSlots->SetGUI(this);
   }else{
   /* if no Slot Object where found creat a new one*/
		fxTGo4StartOnlineSlots = new TGo4StartOnlineSlots ("TGo4StartOnlineSlots" , "Slot Class for Qt GUI", this);
   }
   /* Get the pointer for the Status Object */
    fxTGo4StartOnlineStatus= dynamic_cast <TGo4StartOnlineStatus *> (fxTGo4StartOnlineSlots->GetStatus());

    //########################### REGISTER HISTS ###################################
    fxTGo4GUIRegistry->ReDirectObject("hStartstrips_start_veto_raw",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas1);     fxTGo4GUIRegistry->ReDirectObject("hStartmult_start_veto_raw",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas2);
    fxTGo4GUIRegistry->ReDirectObject("hStarttime_start_raw",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas3);
    fxTGo4GUIRegistry->ReDirectObject("hStarttime_veto_raw",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas4);
    fxTGo4GUIRegistry->ReDirectObject("hStartstart_veto_corr_raw",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas5);
   fxTGo4GUIRegistry->ReDirectObject("htime_trend_start",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas6);
    //fxTGo4GUIRegistry->ReDirectObject("",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas2);
    //fxTGo4GUIRegistry->ReDirectObject("",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas3);
    //fxTGo4GUIRegistry->ReDirectObject("",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas4);
    //fxTGo4GUIRegistry->ReDirectObject("",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas5);
    //fxTGo4GUIRegistry->ReDirectObject("",fxTGo4StartOnlineSlots, true,StartfxQtRootCanvas6);
  
}

void TGo4StartOnline::destroy()
{
    fxTGo4GUIRegistry->RemoveRedirectEntry("hStartstrips_start_veto_raw",fxTGo4StartOnlineSlots);
    fxTGo4GUIRegistry->RemoveRedirectEntry("hStartmult_start_veto_raw",fxTGo4StartOnlineSlots);
    fxTGo4GUIRegistry->RemoveRedirectEntry("hStarttime_start_raw",fxTGo4StartOnlineSlots);
    fxTGo4GUIRegistry->RemoveRedirectEntry("hStarttime_veto_raw",fxTGo4StartOnlineSlots);
    fxTGo4GUIRegistry->RemoveRedirectEntry("hStartstart_veto_corr_raw",fxTGo4StartOnlineSlots);
    fxTGo4GUIRegistry->RemoveRedirectEntry("htime_trend_start",fxTGo4StartOnlineSlots);
}

void TGo4StartOnline::closeEvent( QCloseEvent * ce )
{
    TGo4LockGuard glob;
    ce->accept();
    fxTGo4StartOnlineSlots->SetGUI(0);
}

void TGo4StartOnline::StartDisplayLCD1( float t )
{
    StartLCDNumber1->display(t);
}

void TGo4StartOnline::StartDisplayLCD2( float t )
{
 StartLCDNumber2->display(t);
}

void TGo4StartOnline::StartDisplayLCD3( float t )
{
    StartLCDNumber3->display(t);
}

void TGo4StartOnline::DrawHis( HMonHistAddon* His )
{

}

void TGo4StartOnline::Start()
{
   fxTGo4StartOnlineSlots->StartMonitor();
}

void TGo4StartOnline::Stop()
{
  fxTGo4StartOnlineSlots->StopMonitor();

}

void TGo4StartOnline::Reset()
{
    	TGo4ResetParameter* reset = new TGo4ResetParameter("HistReset","HistReset");
	reset->SetStartChanged(kTRUE);
	reset->Set("Start",0);     
	TGo4ComSetParameter* com=new TGo4ComSetParameter("HistReset");
	com->SetParameter(reset);
	fxTGo4GUIRegistry->SubmitCommand(com);
}


