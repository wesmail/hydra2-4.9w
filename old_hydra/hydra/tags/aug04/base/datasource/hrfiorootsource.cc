//*-- Author : Peter Zumbruch
//*-- Derived from: Manuel Sanchez
//*-- Modified : 12/Nov/2003 by P. Zumbruch

////////////////////////////////////////////////////////////////////////////
// HRFIORootSource
//
// This data source can read data from a ROOT file generated by
// HYDRA, this includes both the reconstruction and simulation software.
//
// The data source takes care of instantiating the appropriate classes even
// if they are non-standard. That is, if you have an input file with 
// HMdcHitSim instead of HMdcHit; HRFIORootSource will take care of using
// HMdcHitSim during the analysis.
//
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "rawapi.h"
#include "hrfiorootsource.h"

ClassImp(HRFIORootSource)

// ----------------------------------------------------------------------

HRFIORootSource::HRFIORootSource(Bool_t fPers, Bool_t fMerg) 
 : HRootSource(fPers, fMerg) 
{
  // for fPers==kFALSE, input cats are suppressed in output
  // for fMerg==kTRUE, partial events in input tree are merged into current
  // event, if it exists already at init().
  
  fDirectory="";
  fArchive="hades";
}

// ----------------------------------------------------------------------

HRFIORootSource::~HRFIORootSource(void) {
  //Class destructor, it clears the data source.
  Clear();
}

// ----------------------------------------------------------------------

void HRFIORootSource::setArchive(Text_t archiveName[]) {
  //Sets the archive where to read files from.
  fArchive=archiveName;
}

// ----------------------------------------------------------------------

Bool_t HRFIORootSource::access(const TString &inname, Int_t mode){
  //return 0;  // always true switched off because CINT does not like rfio_fopen !! to be fixed
  TString name = inname;
  name.ReplaceAll("rfio:","");
  char *nn = new char[name.Length()];
  Bool_t val = rfio_access(nn,mode);
  delete nn;
  return val;

//   if (mode == 0)
//     {
//           RFILE *fRemote = NULL;
          
//           printf("-M- preparing access to remote file %s\n", name.Data());
// 	  Char_t tmp[400];
// 	  sprintf(tmp,name.Data());
//           fRemote = rfio_fopen(tmp, "rb");
//           if (!(fRemote == NULL))
// 	    {
//               printf("-M- access to remote file\n %s\npossible \n", name.Data());
//               if (rfio_fclose(fRemote))
// 		{
// 		  Warning("access","-E- closing remote file %s\n", name.Data());
// 		  perror("    ");
// 		}
// 	    }
// 	  return 0;
//     }
//   else
//     {
//       Warning("access","mode value (2nd argument): %i different from only supported 0",mode);
//       return 1;
//     }
}

// ----------------------------------------------------------------------
TString HRFIORootSource::getFileName(Text_t *file)
{
  TString fname;
  if (file[0] == '/') { // beginning with / ?
    fname="rfio:";
    fname+=file;
  } else {
    fname="rfio:/";
    fname+=fArchive;
    fname+="/";
    fname+=fDirectory;
    fname+="/";
    fname+=file;
  }
  return fname;
}

// ----------------------------------------------------------------------

Bool_t HRFIORootSource::setInput(Text_t *fileName,Text_t *archiveName,Text_t *treeName) {
  setArchive(archiveName);
  if (strcmp(treeName,"T") != 0) return kFALSE;
  return addFile(fileName);
}












