#ifndef __WordCompletion__
#define __WordCompletion__

#include "plugin.h"

class WordCompletionThread;
class WordCompletionPlugin : public IPlugin
{
    WordCompletionThread* m_thread;
    
public:
    WordCompletionPlugin(IManager* manager);
    ~WordCompletionPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual clToolBar* CreateToolBar(wxWindow* parent);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
};

#endif // WordCompletion
