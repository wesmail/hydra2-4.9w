/* Generated by Together */

#ifndef TGO4TOFONLINESTATUS_H
#define TGO4TOFONLINESTATUS_H
#include "Go4StatusBase/TGo4Status.h"
#include "TObjArray.h"
#include "TIterator.h"
#include "TObject.h"

class TGo4TofOnlineStatus : public TGo4Status {
public:    

    ~TGo4TofOnlineStatus();

    TGo4TofOnlineStatus(Text_t *Name, Text_t * Title);
    TObject*  OverWriteLocalObject(TNamed * fxData_Local);
    TIterator *GetLocalListIter(){return LocalListIter;}
    TObjArray *GetLocalListStatus();
    TObject* GetObjectfromLocalList(const char *ObjectAt);
    ClassDef(TGo4TofOnlineStatus,1);

private:
	
	TObjArray *LocalListContent;
	TIterator* LocalListIter;        
    
};
#endif //TGO4TOFONLINESTATUS_H
