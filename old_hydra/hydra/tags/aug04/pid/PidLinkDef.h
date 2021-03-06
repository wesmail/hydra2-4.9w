// @(#)$Id: PidLinkDef.h,v 1.14 2004-07-08 13:45:51 christ Exp $
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link off all enum;

#pragma link C++ class HPidTrackCand;
#pragma link C++ class HPidGeantTrackSet-;
#pragma link C++ class HPidCommonTrack;
#pragma link C++ class HPidTrackFiller;
#pragma link C++ class HPidTrackFillerSim;
#pragma link C++ class HPidAlgorithm;
#pragma link C++ class HPidReconstructor;
#pragma link C++ class HPidCandidate;
#pragma link C++ class HPidAlgMomVsBeta;
#pragma link C++ class HPidAlgMomVsEloss;
#pragma link C++ class HPidAlgRingProperties;
#pragma link C++ class HPidAlgShower;

#pragma link C++ class HPidCandidate_3_8;
#pragma link C++ class HPidCandidate_3_5;
#pragma link C++ class HPidCandidate_4_8;
#pragma link C++ class HPidCandidate_2_8;
#pragma link C++ class HPidCandidate_1_8;
#pragma link C++ class HPidCandidate_5_8;
#pragma link C++ class HPidCandidate_7_8;

#pragma link C++ class HPidCandidate_2_9;
#pragma link C++ class HPidCandidate_3_9;
#pragma link C++ class HPidCandidate_4_9;
#pragma link C++ class HPidCandidate_5_9;
#pragma link C++ class HPidCandidate_6_9;
#pragma link C++ class HPidCandidate_3_2;
#pragma link C++ class HPidCandidate_5_5;
#pragma link C++ class HPidCandidate_7_5;
#pragma link C++ class HPidCandidate_9_5;
#pragma link C++ class HPidCandidate_7_7;
#pragma link C++ class HPidCandidate_5_7;
#pragma link C++ class HPidCandidate_3_7;
#pragma link C++ class HPidContFact;
#pragma link C++ class HPidParRootFileIo;
#pragma link C++ class HPidParAsciiFileIo;
#pragma link C++ class HPidParCont;
#pragma link C++ class HPidHistogramsCont-;

#pragma link C++ class HPidTrackFillerPar-;
#pragma link C++ class HPidReconstructorPar;
#pragma link C++ class HPidAlgMomVsBetaPar;
#pragma link C++ class HPidAlgMomVsElossPar;
#pragma link C++ class HPidElossMPVShiftPar;
#pragma link C++ class HPidAlgRingPropertiesPar;
#pragma link C++ class HPidAlgShowerPar;
#pragma link C++ class HPidAlgLeptonCand;

#pragma link C++ class HPidGaussConf;

#pragma link C++ class HPidParticle-;
#pragma link C++ class HPidParticleSim-;
#pragma link C++ class HPidParticleSimExt-;
#pragma link C++ class HPidHitData-;
#pragma link C++ class HPidMdcFunc;
#pragma link C++ class HPidParticleHit;
#pragma link C++ class HPidParticleSimExtHit;
#pragma link C++ class HPidParticleFiller;

#pragma link C++ class HPidDilepton;
#pragma link C++ class HPidDileptonFiller;

#pragma link C++ class HPidSimTrigger;

#pragma link C++ class HPidMomImp;
#pragma link C++ class HPidMomImpBeta;

#pragma link C++ global catPidTrackCand;
#pragma link C++ global catPidCandidate;
#pragma link C++ global catPidPart;
#pragma link C++ global catPidDilepton;

#pragma link C++ enum EnumPidAlgorithm_t;

#endif
