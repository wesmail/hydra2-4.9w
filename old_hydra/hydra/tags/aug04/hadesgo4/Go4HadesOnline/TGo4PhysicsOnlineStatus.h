/* Generated by Together */

#ifndef TGO4PHYSICSONLINESTATUS_H
#define TGO4PHYSICSONLINESTATUS_H
#include "Go4StatusBase/TGo4Status.h"
#include "TObjArray.h"
#include "TIterator.h"
#include "TObject.h"

class TGo4PhysicsOnlineStatus : public TGo4Status {
public:    

    ~TGo4PhysicsOnlineStatus();

    TGo4PhysicsOnlineStatus(Text_t *Name, Text_t * Title);
    TObject* OverWriteLocalObject(TNamed * fxData_Local);
    TIterator *GetLocalListIter(){return LocalListIter;}
    TObjArray *GetLocalListStatus();
    TObject* GetObjectfromLocalList(const char *ObjectAt);
    ClassDef(TGo4PhysicsOnlineStatus,1);

private:
	
	TObjArray *LocalListContent;
	TIterator* LocalListIter;        
    
};
#endif //TGO4PHYSICSONLINESTATUS_H
