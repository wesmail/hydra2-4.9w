using namespace std;
#include <iostream> 
#include <iomanip>


//////////////////////////////////////////////////////
// HRichCal
//
//   This class contains one hit in one cell of the rich.
//
//////////////////////////////////////////////////////

#pragma implementation
#include "hrichcal.h"
ClassImp(HRichCal)

ostream& operator<< (ostream& output, HRichCal& raw) {
        output<<"pad fCharge:"<<raw.fCharge;
        return output;
}

inline Int_t HRichCal::getLocationIndex(Int_t i) {
  switch (i) {
   case 0 : return getSector(); break;
   case 1 : return getRow(); break;
   case 2 : return getCol(); break;
  }
  return -1;
}

void HRichCal::dumpToStdout()
{
    cout<<"CAL: "<<"sec:"<<getSector()<<" col:"<<getCol()<<" row:"<<getRow()
	<<" chrg:"<<getCharge()<<endl;


}


