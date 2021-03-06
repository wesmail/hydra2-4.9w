// File: hrichhit.h
//
// modified by Laura Fabbietti: 22/02/2000
//
#ifndef HRICHHITSIM_H
#define HRICHHITSIM_H
#include "hrichhit.h"
//#include "TArrayI.h"

class HRichHitSim: public HRichHit{
public:
 HRichHitSim();
 virtual ~HRichHitSim(){}
//LF
 short iRingPadNTrack1[256];//!
 short iRingPadNTrack2[256];//!
 short iRingTrack[256];//!
 char iRingFlag[256];//!
// for each ring maximally 3 tracks are stored.
 Int_t track1, track2, track3;
 Int_t weigTrack1, weigTrack2, weigTrack3;
// flag that distinguishes Photons from IP
 Int_t flag1,flag2,flag3;
 HRichHitSim& operator=(const HRichHitSim& source) {
   HRichHit::operator=(source);
  return *this;
 }
 HRichHitSim& operator=(const HRichHit& source) {
   HRichHit::operator=(source);
  return *this;
 }
 //LF
 ClassDef(HRichHitSim,1)
};

#endif /* !HRICHHITSIM_H */
