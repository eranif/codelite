#ifndef __WebTools__
#define __WebTools__

#include "plugin.h"
#include "JavaScriptSyntaxColourThread.h"
#include "cl_command_event.h"

class JavaScriptSyntaxColourThread;
class WebTools : public IPlugin
{
    friend class JavaScriptSyntaxColourThread;
    
    JavaScriptSyntaxColourThread* m_jsColourThread;
    
protected:
    void OnFileLoaded(clCommandEvent& event);
    void OnThemeChanged(wxCommandEvent& event);
    void ColourJavaScript(const JavaScriptSyntaxColourThread::Reply& reply);
    
public:
    WebTools(IManager *manager);
    ~WebTools();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void UnPlug();
};

#endif //WebTools
