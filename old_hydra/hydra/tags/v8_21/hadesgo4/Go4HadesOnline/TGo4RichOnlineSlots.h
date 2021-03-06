/* Generated by Together */

#ifndef TGO4RICHONLINESLOTS_H
#define TGO4RICHONLINESLOTS_H
#include "Go4GUIBase/TGo4SlotsBaseClass.h"
#include "TObject.h"
class TNamed;
class TGo4Status;
class TGo4EditPolyCond;
class TGo4RichOnline;
class TGo4GUIRegistry;
class TGo4RichOnlineStatus;
class TQRootCanvas;
class TPad;

class TGo4RichOnlineSlots : public TGo4SlotsBaseClass {
public:    

    ~TGo4RichOnlineSlots();
    TGo4RichOnlineSlots(const Char_t * name, const Char_t * title, TGo4RichOnline * p);

    void SetGUI(TGo4RichOnline *);
    TGo4RichOnline *GetGUI(){return fxTGo4RichOnline; }
    TGo4Status * GetStatus();

    void StartMonitor();
    void StopMonitor(); 
    void Reset();

    void AddObject(TNamed * );
    void AddObject(TNamed * ,TQRootCanvas*,TPad*);
    void UpdateGUI(TObject*);
    ClassDef(TGo4RichOnlineSlots, 1);
    
private:    
    TGo4RichOnline *fxTGo4RichOnline; //!
    TGo4RichOnlineStatus *fxTGo4RichOnlineStatus; //!
    TGo4GUIRegistry  *fxTGo4GUIRegistry; //!
};
#endif //TGO4RICHONLINESLOTS_H
