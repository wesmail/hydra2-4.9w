//*-- Author : B Spruck
//*-- Created 21.04.06 --- B.Spruck

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//  HHodoCal
//
//  This class contains HODO Cal data
//
// Containing calibrated Time and ADC, up to 4 hits, same as on
// raw Level.
// Note: Until now, Calibrator will _not_ fill more than 4 hits,
// thus number of hit will (at least at the moment) differ
// from raw level and not exceed 4.
// Anyway, hits without time cannot be calibrated (...).
// But! It might have been nice to store information about
// multiplicity of "total" hits per channel (from raw)
//
/////////////////////////////////////////////////////////////

#include "hhodocal.h"

ClassImp(HHodoCal)

void HHodoCal::clear(void)
{
   nHits=0;// This is important!
   module=-1;
   strip=-1;
   time1=-1000000;
   adc1=-1;
   time2=-1000000;
   adc2=-1;
   time3=-1000000;
   adc3=-1;
   time4=-1000000;
   adc4=-1;
}

Float_t HHodoCal::getTime(const Int_t n) const
{
   // Returns the time of the nth hit (1<=n<=4)
   switch (n) {
      case 1: return time1;
      case 2: return time2;
      case 3: return time3;
      case 4: return time4;
      default:
         Error("getTime()","hit number: %i out of range [1,4]",n);
         return -1;
   }
}

Float_t HHodoCal::getAdc(const Int_t n) const
{
   // Returns the adc of the nth hit (1<=n<=4)
   switch (n) {
      case 1: return adc1;
      case 2: return adc2;
      case 3: return adc3;
      case 4: return adc4;
      default:
         Error("getAdc()","hit number: %i out of range [1,4]",n);
         return -1;
   }
}

void HHodoCal::getTimeAndAdc(const Int_t n, Float_t& time, Float_t& adc )
{
   // Returns the time and adc of the nth hit (1<=n<=4)
   switch (n) {
      case 1:
         time=time1;
         adc=adc1;
         return;
      case 2:
         time=time2;
         adc=adc2;
         return;
      case 3:
         time=time3;
         adc=adc3;
         return;
      case 4:
         time=time4;
         adc=adc4;
         return;
      default:
         Error("getTime()","hit number: %i out of range [1,4]",n);
         time=adc=-1;
         return;
   }
}

Bool_t HHodoCal::setTimeAdc(const Float_t time,const Float_t adc)
{
   // Stores the given time and adc in the next data element time* and adc*
   // and sets the multiplicity.
   // Return kFALSE if 4 hits are already stored.
   switch (nHits) {
      case 0:
         time1 = time;
         adc1 = adc;
         nHits++;
         break;
      case 1:
         time2 = time;
         adc2 = adc;
         nHits++;
         break;
      case 2:
         time3 = time;
         adc3 = adc;
         nHits++;
         break;
      case 3:
         time4 = time;
         adc4 = adc;
         nHits++;
         break;
      default:
         if( nHits>=4) nHits++;// continue counting Hits ...
         return kFALSE;
   }
   return kTRUE;
}

