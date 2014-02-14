#ifndef __CodeLiteDiff__
#define __CodeLiteDiff__

#include "plugin.h"

class CodeLiteDiff : public IPlugin
{
    enum {
        ID_TOOL_NEW_DIFF = 3970,
    };
protected:
    void OnNewDiff(wxCommandEvent &e);
    
public:
    CodeLiteDiff(IManager *manager);
    ~CodeLiteDiff();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void HookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnPlug();
};

#endif //CodeLiteDiff
