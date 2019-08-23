//*-- AUTHOR : Ilse Koenig
//*-- Modified : 21/02/2000 by I.Koenig

///////////////////////////////////////////////////////////////////////////////
// HParOraIo
//
// Interface class to database Oracle
// It is derived from interface base class HParIo.
//
// It contains a pointer to the connection class (type HOraConn)  and a list
// of interface classes for the detectors. Each detector has its own interface
// class all inherited from a common base class.
//
// This class replaces the former class HOraIo.  
///////////////////////////////////////////////////////////////////////////////

#include "hparoraio.h"
#include "hdetpario.h"
#include "horainfo.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hmdcparoraio.h"
#include <stdio.h>
#include <iostream.h>

ClassImp(HParOraIo)

HParOraIo::HParOraIo() {
  // default constructor
  // creates the connection class but doesn't open a connection
  pConn=new HOraConn();
  autoWritable=kFALSE;
}


HParOraIo::~HParOraIo() {
  // default constructor closes an open connection
  close();
  if (pConn) delete pConn;
}


Bool_t HParOraIo::open() {
  // opens connection to database Hades as user Hades
  // user Hades has Readonly access to Oracle tables
  // all open()-functions activate the detetctor I/Os
  isConnected=pConn->open();
  return activateDetIo();
}


Bool_t HParOraIo::open(char *userName) {
  // opens connection to database Hades for user given by name
  // asks for password
  isConnected=pConn->open(userName);
  return activateDetIo();
}


Bool_t HParOraIo::open(char *dbName, char *userName) {
  // opens connection to database with name dbName for user given by name
  // asks for password
  isConnected=pConn->open(dbName,userName);
  return activateDetIo();
} 


void HParOraIo::close() {
  // closes the connection with automatic ROLLBACK
  pConn->close();
  isConnected=kFALSE;
  detParIoList->Delete();
  if (pInfo) delete pInfo;
}


void HParOraIo::print() {
  // prints information about the database connection
  pConn->print();
  if (isConnected) {
    TIter next(detParIoList);
    HDetParIo* io;
    cout<<"detector I/Os: ";
    while ((io=(HDetParIo*)next())) {
      cout<<" "<<io->GetName();
    }
    cout<<'\n';
  }
}


Bool_t HParOraIo::activateDetIo() {
  if (isConnected==kFALSE) return kFALSE;
  pInfo=new HOraInfo(pConn);
  TList* detSet=gHades->getSetup()->getListOfDetectors();
  TIter next(detSet);
  HDetector* det;
  HDetParIo* io=0;
  while ((det=(HDetector*)next())) {
    const char* name=det->GetName();
    if (strcmp(name,"Mdc")==0) io=new HMdcParOraIo(pConn);
    if (io) detParIoList->Add(io);
  }
  return kTRUE;
}