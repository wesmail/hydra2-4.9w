//*-- Author : Dan Magestro (magestro@gsi.de)
//*-- Modified : 24/01/2002
//
//  QAvarhists.h
//
//  List of "run variation" histograms to be printed by HQAOutputPS.
//
//  Note: If this file is changed, hqavariations.cc needs
//        to be recompiled

  // Active Event histograms
   "evtHeader_eventSize_Var",

  // Active Start histograms
   "stCal_meanStrip_Var",

  // Active Rich histograms
   "richCal_n_Var",
   "richHit_n_Var",

  // Active Mdc histograms
   "mdcCal1_time1_m0_Var",
   "mdcCal1_time1_m1_Var",
   "mdcCal1_time1_m2_Var",
   "mdcCal1_time1_m3_Var",
   "mdcCal1_time2m1_m0_Var",
   "mdcCal1_time2m1_m1_Var",
   "mdcCal1_time2m1_m2_Var",
   "mdcCal1_time2m1_m3_Var",

  // Active Tof histograms
   "tofHit_n_Var",

  // Active Tofino histograms

  // Active Shower histograms
   "shoHit_n_Var",
   "shoHitTof_n_Var",

  // Active Kick histograms
   "kickTrack_n_Var",

  // Active Corr histograms

  // Active Trigger histograms
  "trigRich_n_Var",  
  "trigTof_n_Var",   
  "trigShower_n_Var",
  "trigTlepton_n_Var",
  "trigSlepton_n_Var" 
