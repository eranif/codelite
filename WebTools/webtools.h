#ifndef __WebTools__
#define __WebTools__

#include "plugin.h"
#include "JavaScriptSyntaxColourThread.h"
#include "cl_command_event.h"
#include "JSCodeCompletion.h"
#include <wx/timer.h>
#include "ieditor.h"
#include "XMLCodeCompletion.h"
#include "CSSCodeCompletion.h"

class NodeJSDebuggerPane;
class NodeJSWorkspaceView;
class JavaScriptSyntaxColourThread;
class WebTools : public IPlugin
{
    friend class JavaScriptSyntaxColourThread;

    JavaScriptSyntaxColourThread* m_jsColourThread;
    JSCodeCompletion::Ptr_t m_jsCodeComplete;
    XMLCodeCompletion::Ptr_t m_xmlCodeComplete;
    CSSCodeCompletion::Ptr_t m_cssCodeComplete;

    time_t m_lastColourUpdate;
    wxTimer* m_timer;

    /// Node.js
    bool m_clangOldFlag;
    NodeJSDebuggerPane* m_nodejsDebuggerPane;
    wxString m_savePerspective;
    bool m_hideToolBarOnDebugStop;

protected:
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnWorkspaceLoaded(wxCommandEvent& event);
    void OnEditorChanged(wxCommandEvent& event);
    void DoRefreshColours(const wxString& filename);
    void OnFileLoaded(clCommandEvent& event);
    void OnEditorContextMenu(clContextMenuEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void OnThemeChanged(wxCommandEvent& event);
    void OnCodeComplete(clCodeCompletionEvent& event);
    void OnCodeCompleteFunctionCalltip(clCodeCompletionEvent& event);
    void OnFindSymbol(clCodeCompletionEvent& event);
    void ColourJavaScript(const JavaScriptSyntaxColourThread::Reply& reply);
    void OnSettings(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnCommentLine(wxCommandEvent& e);
    void OnCommentSelection(wxCommandEvent& e);
    void OnNodeJSDebuggerStarted(clDebugEvent& event);
    void OnNodeJSDebuggerStopped(clDebugEvent& event);
    void OnIsDebugger(clDebugEvent& event);

private:
    bool IsJavaScriptFile(const wxString& filename);
    bool IsJavaScriptFile(const wxFileName& filename);
    bool IsJavaScriptFile(IEditor* editor);
    bool IsHTMLFile(IEditor* editor);
    bool IsCSSFile(IEditor* editor);
    bool InsideJSComment(IEditor* editor);
    bool InsideJSString(IEditor* editor);
    void EnsureAuiPaneIsVisible(const wxString& paneName, bool update);

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
