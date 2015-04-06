#ifndef __WebTools__
#define __WebTools__

#include "plugin.h"
#include "JavaScriptSyntaxColourThread.h"
#include "cl_command_event.h"
#include "JSCodeCompletion.h"
#include <wx/timer.h>

class JavaScriptSyntaxColourThread;
class WebTools : public IPlugin
{
    friend class JavaScriptSyntaxColourThread;

    JavaScriptSyntaxColourThread* m_jsColourThread;
    JSCodeCompletion::Ptr_t m_jsCodeComplete;
    time_t m_lastColourUpdate;
    wxTimer* m_timer;
    
protected:
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnEditorChanged(wxCommandEvent& event);
    void OnRefreshColours(clCommandEvent& event);
    void OnThemeChanged(wxCommandEvent& event);
    void OnCodeComplete(clCodeCompletionEvent& event);
    void OnCodeCompleteFunctionCalltip(clCodeCompletionEvent& event);
    void ColourJavaScript(const JavaScriptSyntaxColourThread::Reply& reply);
    void OnSettings(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);

private:
    bool IsJavaScriptFile(const wxString& filename);
    bool IsJavaScriptFile(const wxFileName& filename);

public:
    WebTools(IManager* manager);
    ~WebTools();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual clToolBar* CreateToolBar(wxWindow* parent);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void UnPlug();
};

#endif // WebTools
