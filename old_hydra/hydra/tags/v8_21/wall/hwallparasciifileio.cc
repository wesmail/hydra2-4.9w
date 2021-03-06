//*-- AUTHOR : Ilse Koenig
//*-- Created : 17/01/2005
// Modified by M.Golubeva 01.11.2006

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////
//
//  HWallParAsciiFileIo
//
//  Class for Forward Wall parameter input/output from/into Ascii file
//
//////////////////////////////////////////////////////////////////////

#include "hwallparasciifileio.h"
#include "hwalllookup.h"
#include "hwallcalpar.h"
#include "hwalldigipar.h"
#include "hwallrefwinpar.h"
#include "hdetector.h"
#include "hwalldetector.h"

ClassImp(HWallParAsciiFileIo)

HWallParAsciiFileIo::HWallParAsciiFileIo(fstream* f) : HDetParAsciiFileIo(f) {
  // constructor calls the base class constructor
  fName="HWallParIo";
}

Bool_t HWallParAsciiFileIo::init(HParSet* pPar,Int_t* set) {
  // calls the appropriate read function for the container
  const  Text_t* name=pPar->GetName();
  if (pFile) {
    pFile->clear();
    pFile->seekg(0,ios::beg);
    if (!strcmp(name,"WallLookup")) return read((HWallLookup*)pPar,set);
    if (!strcmp(name,"WallCalPar")) return read((HWallCalPar*)pPar,set);
    //if (!strcmp(name,"WallDigiPar")) return read((HWallDigiPar*)pPar,set);
    //if (!strcmp(name,"WallRefWinPar")) return read((HWallRefWinPar*)pPar,set);
    if (!strcmp(name,"WallGeomPar"))
       return HDetParAsciiFileIo::read((HDetGeomPar*)pPar,set);
    cerr<<"initialization of "<<name<<" not possible from Ascii file!"<<endl;
    return kFALSE;
  }
  cerr<<"no input file open"<<endl;
  return kFALSE;
}

Int_t HWallParAsciiFileIo::write(HParSet* pPar) {
  // calls the appropriate write function for the container
  if (pFile) {
    const  Text_t* name=pPar->GetName();
    if (!strcmp(name,"WallLookup")) return writeFile3((HWallLookup*)pPar);
    if (!strcmp(name,"WallCalPar")) return writeFile1((HWallCalPar*)pPar);
    //if (!strcmp(name,"WallDigiPar")) return writeFile1((HWallDigiPar*)pPar);
    //if (!strcmp(name,"WallRefWinPar")) return writeFile1((HWallRefWinPar*)pPar);
    if (!strcmp(name,"WallGeomPar"))
        return HDetParAsciiFileIo::writeFile((HDetGeomPar*)pPar);
    cerr<<name<<" could not be written to Ascii file"<<endl;
    return -1;
  }
  cerr<<"no output file open"<<endl;
  return -1;
}

template<class T> Int_t HWallParAsciiFileIo::writeFile3(T* pPar) {
  // template function for all parameter containers with 3 levels
  // writes the header, loops over the container and calls its member
  //   function writeline(...)
  pPar->putAsciiHeader(fHeader);
  writeHeader(pPar->GetName());
  Text_t buf[155];
  Int_t n0 = (*pPar).getSize();
  for(Int_t i0=0; i0<n0; i0++) {
    Int_t n1 = (*pPar)[i0].getSize();
    for(Int_t i1=0; i1<n1; i1++) {
      Int_t n2 = (*pPar)[i0][i1].getSize();
      for(Int_t i2=0; i2<n2; i2++) {
        if (pPar->writeline(buf, i0, i1, i2))
          pFile->write(buf,strlen(buf));
	}
    }
  }
  pPar->setChanged(kFALSE);
  pFile->write(sepLine,strlen(sepLine));
  return 1;
}

template<class T> Int_t HWallParAsciiFileIo::writeFile1(T* pPar) {
  // template function for all parameter containers with 1 level
  // writes the header, loops over the container and calls its member
  //   function writeline(...)
  pPar->putAsciiHeader(fHeader);
  writeHeader(pPar->GetName());
  Text_t buf[155];
  for(Int_t i=0; i<(*pPar).getSize(); i++) {
    if (pPar->writeline(buf,i)) pFile->write(buf,strlen(buf));       
  }
  pPar->setChanged(kFALSE);
  pFile->write(sepLine,strlen(sepLine));
  return 1;
}

template<class T> Bool_t HWallParAsciiFileIo::read(T* pPar, Int_t* set) {
  // template function for all parameter containers
  // searches the container in the file, reads the data line by line and
  //   called the member function readline(...) of the container class
  //cout <<"MARINA ASCII ---------- " <<endl;
  const  Text_t* name=pPar->GetName();
  if (!findContainer(name)) return kFALSE;
  pPar->clear();
  const Int_t maxbuf=155;
  Text_t buf[maxbuf];
  while (!pFile->eof()) {
    pFile->getline(buf, maxbuf);
    if (buf[0]=='#') break;
    if (buf[0]!='/' && buf[0]!='\0') pPar->readline(buf,set);
  }
  Bool_t allFound=kFALSE;
  if (set[0]&&set[0]==999) {
    set[0]=1;
    allFound=kTRUE;
    //pPar->printParam();
    printf("%s initialized from ASCII file\n",name);
    pPar->setInputVersion(1,inputNumber);
    pPar->setChanged();
  }
  return allFound;
}

