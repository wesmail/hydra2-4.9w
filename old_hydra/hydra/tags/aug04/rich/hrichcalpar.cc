//*-- Author : RICH team member
using namespace std;
#include <iostream> 
#include <iomanip>
#include "hades.h"
#include "hparhadasciifileio.h"
#include "hspectrometer.h"
#include "hrichparset.h"
#include "hrichdetector.h"
#include "hrichcalpar.h"

// ***************************************************************************
//////////////////////////////////////////////////////////////////////////////
//
//  HRichCalPar
//
//  Parameter container for calibration data.
//
//////////////////////////////////////////////////////////////////////////////
// ***************************************************************************

ClassImp(HRichCalPar)

//----------------------------------------------------------------------------
HRichCalPar::HRichCalPar(const char* name,const char* title,
                         const char* context)
                : HRichParSet(name,title,context) {
  m_pReadParam = NULL;

  setSetup(6, 96, 96); //default setup 
  // setSetup(6, 64, 64); //default setup old
  setCellClassName("HRichCalParCell");
}
//============================================================================

//----------------------------------------------------------------------------
HRichCalPar::~HRichCalPar()
{ 
  m_ParamsTable.deleteTab();
}
//============================================================================

//----------------------------------------------------------------------------
void HRichCalPar::setSetup(Short_t nSectors, Short_t  nRows, Short_t nColumns)
{
  fSectors = nSectors;
  fRows = nRows;
  fColumns = nColumns;
} 
//============================================================================

//----------------------------------------------------------------------------
void HRichCalPar::setCellClassName(Char_t* pszName)
{
  strncpy(m_szClassName, pszName, sizeof(m_szClassName));
}
//============================================================================

//----------------------------------------------------------------------------
Char_t* HRichCalPar::getCellClassName()
{
  return m_szClassName;
}
//============================================================================

//----------------------------------------------------------------------------
HRichCalParCell* HRichCalPar::getSlot(HLocation &loc)
{
  return (HRichCalParCell*) m_ParamsTable.getSlot(loc);
}
//============================================================================

//----------------------------------------------------------------------------
HRichCalParCell* HRichCalPar::getObject(HLocation &loc)
{
  return (HRichCalParCell*) m_ParamsTable.getObject(loc);
}
//============================================================================

//----------------------------------------------------------------------------
Float_t HRichCalPar::getSlope(HLocation &loc)
{
  return getObject(loc)->getSlope();
}
//============================================================================

//----------------------------------------------------------------------------
Float_t HRichCalPar::getOffset(HLocation &loc)
{
  return getObject(loc)->getOffset();
}
//============================================================================

//----------------------------------------------------------------------------
Float_t HRichCalPar::getSigma(HLocation &loc)
{
  return getObject(loc)->getSigma();
}
//============================================================================

//----------------------------------------------------------------------------
void HRichCalPar::setSlope(HLocation &loc, Float_t fSlope)
{
  getObject(loc)->setSlope(fSlope);
}
//============================================================================

//----------------------------------------------------------------------------
void HRichCalPar::setOffset(HLocation &loc, Float_t fOffset)
{
  getObject(loc)->setOffset(fOffset);
}
//============================================================================

//----------------------------------------------------------------------------
void HRichCalPar::setSigma(HLocation &loc, Float_t fSigma)
{
  getObject(loc)->setSigma(fSigma);
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t HRichCalPar::initAscii(HParHadAsciiFileIo* pHadAsciiFile) {

  using namespace HadAsciiAuxiliaryFunctions;
  if (!pHadAsciiFile) return kFALSE;
  Bool_t status = kTRUE;
  Char_t sectname[60];
  Int_t i, sect, row, col;
  Float_t slope, offset, sigma;
  HLocation loc;
  HRichCalParCell *pCell;
 
  loc.setNIndex(3);
 
 try {

  HAsciiRaw &mydata = *pHadAsciiFile->GetRawAscii();

  mydata.SetActiveSection("Rich Calibrater Parameters Setup");

  mydata.SetReadKeyTypes('i', 'i', 'i');
  mydata.ReadRawLine(0,&fSectors,&fRows,&fColumns);

  setSetup(fSectors, fRows, fColumns);
  m_ParamsTable.set(3, fSectors, fRows, fColumns);
  m_ParamsTable.setCellClassName(getCellClassName());
  m_ParamsTable.makeObjTable();


  for(sect = 0; sect < fSectors; sect++) {
   sprintf(sectname, "Rich Calibrater Parameters: Sector %d", sect); 
   mydata.SetActiveSection(sectname);
   mydata.SetReadKeyTypes('i', 'i', 'f', 'f', 'f');

   loc[0] = sect;

   Int_t line = 0;
   for(i = 0; i < mydata.GetRowsNr(); i++) {
    mydata.ReadRawLine(line, &row, &col, &slope, &offset, &sigma);
    loc[1] = row;
    loc[2] = col;

    pCell = (HRichCalParCell*) getSlot(loc);
    if (pCell) {
     pCell = new(pCell) HRichCalParCell;
     pCell->setSlope(slope);
     pCell->setOffset(offset);
     pCell->setSigma(sigma);
    }
    line++;
   }
  }

 } // eof try block

 catch (Bool_t ret) {
  status = ret;
 }
 catch (...) {
  status = kFALSE;
  ErrorMsg(2,"HRichCalPar::initAscii",1,"Unidentified exception catched.");
 }

 if (status)
  ErrorMsg(0,"HRichCalPar::initAscii",
   1,"Container \'RichCalPar\' has been read from ASCII file.");
  return status;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t HRichCalPar::writeAscii(HParHadAsciiFileIo* pHadAsciiFile) {

 using namespace HadAsciiAuxiliaryFunctions;
 if (!pHadAsciiFile) return kFALSE;
 Bool_t status = kTRUE;
 Char_t sectname[60];
 Int_t sect, row, col;
 Float_t slope, offset, sigma;
 HLocation loc;

 loc.setNIndex(3);

 try {

  HAsciiRaw &mydata = *pHadAsciiFile->GetRawAscii();

  mydata.SetSeparator('\t');
  mydata.WriteSection("Rich Calibrater Parameters Setup");
  mydata.WriteKeywords(3, "Sector", "Row", "Column");
  mydata.SetWriteKeyTypes('i', 'i', 'i');
  mydata.WriteRawLine(fSectors, fRows, fColumns);
 
  for(sect = 0; sect < fSectors; sect++) {
   sprintf(sectname, "Rich Calibrater Parameters: Sector %d", sect);
   mydata.WriteSection(sectname);
   mydata.WriteKeywords(5, "Row", "Column", "Slope", "Offset", "Sigma");
   mydata.SetWriteKeyTypes('i', 'i', 'f', 'f', 'f');

   loc[0] = sect;
   // order of loop reversed to make this output identical to 
   // output of hrichpedestal
   for(col = 0; col < fColumns; col++)
    for(row = 0; row < fRows; row++) {
     loc[1] = row;
     loc[2] = col;
     slope = getSlope(loc);
     offset = getOffset(loc);
     sigma = getSigma(loc);
     //if (sigma!=0.)
     //This was commented out because root cannot handle incomplete Objects
     //of type richcalpar. The streamer bombs when they are written to a
     //root file. Therfore we fill things completele before making a root file
     //from them. (Tassilo 08.04.2002)
     mydata.WriteRawLine(row, col, slope, offset, sigma);
    }
  }
 
 } // eof try block

 catch (Bool_t ret) {
  status = ret;
 }
 catch (...) {
  status = kFALSE;
  ErrorMsg(2,"HRichCalPar::writeAscii",1,"Unidentified exception catched.");
 }

 if (status)
 ErrorMsg(0,"HRichCalPar::writeAscii",
   1,"Container \'RichCalPar\' has been written to ASCII file.");

 return status;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t HRichCalPar::defaultInit()
{
  HRichDetector *pRichDet = (HRichDetector*)gHades->getSetup()->getDetector("Rich");
  if (!pRichDet) return kFALSE;
 
  fSectors = pRichDet->getSectors();
  fRows = pRichDet->getRows();
  fColumns = pRichDet->getColumns();

  printf("%d - %d - %d\n", fSectors, fRows, fColumns);
  m_ParamsTable.set(3, fSectors, fRows, fColumns);

  m_ParamsTable.setCellClassName(getCellClassName());
  m_ParamsTable.makeObjTable();
  printf("Rich calpar default initialization finished\n");
  return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
void HRichCalPar::PrintCalPar() {
  int i, k, l;
  float slope, offset,sigma;

  HLocation loc;

  loc.setNIndex(3);
  for (i = 0; i < fSectors; i++)
      for (k = 0; k < fRows; k++)
          for (l = 0; l < fColumns; l++) {
             loc[0] = i;
             loc[1] = k;
             loc[2] = l;
             slope = getSlope(loc);
             offset = getOffset(loc);
             sigma = getSigma(loc);
             if (offset < 0) 
             printf("%d:%d:%d %f %f\n", i, k, l, slope, offset);
          }
    
}
//============================================================================








