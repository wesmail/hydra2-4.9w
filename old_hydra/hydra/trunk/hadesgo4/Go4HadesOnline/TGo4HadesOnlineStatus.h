/* Generated by Together */

#ifndef TGO4HADESONLINESTATUS_H
#define TGO4HADESONLINESTATUS_H
#include "Go4StatusBase/TGo4Status.h"
#include "TObjArray.h"
#include "TIterator.h"
#include "TObject.h"

class TGo4HadesOnlineStatus : public TGo4Status {
public:    

    ~TGo4HadesOnlineStatus();    

    TGo4HadesOnlineStatus(Text_t *Name, Text_t * Title);
    TObject* OverWriteLocalObject(TNamed * fxData_Local);
    TIterator *GetLocalListIter(){return LocalListIter;}
    TObjArray *GetLocalListStatus();
    TObject* GetObjectfromLocalList(const Char_t *ObjectAt);
    ClassDef(TGo4HadesOnlineStatus,1);

private:
	
	TObjArray *LocalListContent;
	TIterator* LocalListIter;        
    
};
#endif //TGO4HADESONLINESTATUS_H
