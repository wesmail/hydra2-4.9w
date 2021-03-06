#ifndef HSHOWERHITTOFTRACK_H
#define HSHOWERHITTOFTRACK_H

#pragma interface

#include "hshowerhittof.h"
#include "showerdef.h"

class HShowerHitTofTrack : public HShowerHitTof{
protected:
  Int_t nTrack;                    // number of Tracks
  Int_t nTracks[SH0WER_MAX_TRACK]; // array of tracks (default val == -1)


public:
    HShowerHitTofTrack(void) {
       	for(Int_t i = 0; i < SH0WER_MAX_TRACK; i ++) { nTracks[i] = -1; }
	nTrack = 0;
    }
  ~HShowerHitTofTrack(void) {}
  void clear(void);
  inline void  setTrack(const Int_t track) { if(nTrack < SH0WER_MAX_TRACK) { nTracks[nTrack] = track; nTrack ++;}} //set track number
  inline Int_t getTrack(Int_t n = 0) const { if(n < SH0WER_MAX_TRACK && n >= 0) { return nTracks[n];} else{ return 0;}} //get track number
  Int_t   getNTracks()            {return nTrack;}
  void    setNTracks(Int_t n = 0) {nTrack = n;}

  HShowerHitTofTrack& operator=(HShowerHitTofTrack& ht);
  HShowerHitTofTrack& operator=(HShowerHitTof& ht);

  ClassDef(HShowerHitTofTrack,2) // Tofino/Shower HIT data tied with track number
};

#endif /* !HSHOWERHITTOFTRACK_H */

