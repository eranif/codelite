#include "webtools.h"
#include <wx/xrc/xmlres.h>
#include "fileextmanager.h"
#include <wx/stc/stc.h>
#include "event_notifier.h"
#include "codelite_events.h"
#include "WebToolsSettings.h"

static WebTools* thePlugin = NULL;

// Define the plugin entry point
extern "C" EXPORT IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new WebTools(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("WebTools"));
    info.SetDescription(wxT("Support for JavScript, HTML and other web development tools"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

WebTools::WebTools(IManager* manager)
    : IPlugin(manager)
    , m_lastColourUpdate(0)
{
    m_longName = wxT("Support for JavScript, HTML and other web development tools");
    m_shortName = wxT("WebTools");

    // Create the syntax highligher worker thread
    m_jsColourThread = new JavaScriptSyntaxColourThread(this);
    m_jsColourThread->Create();
    m_jsColourThread->Run();

    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &WebTools::OnRefreshColours, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &WebTools::OnRefreshColours, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &WebTools::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE, &WebTools::OnCodeComplete, this);
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD, &WebTools::OnCodeComplete, this);
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &WebTools::OnCodeCompleteFunctionCalltip, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &WebTools::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &WebTools::OnEditorChanged, this);
    
    Bind(wxEVT_MENU, &WebTools::OnSettings, this, XRCID("webtools_settings"));
    m_jsCodeComplete.Reset(new JSCodeCompletion());
    
    // Connect the timer
    m_timer = new wxTimer(this);
    m_timer->Start(3000);
    Bind(wxEVT_TIMER, &WebTools::OnTimer, this, m_timer->GetId());
}

WebTools::~WebTools() {}

clToolBar* WebTools::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void WebTools::CreatePluginMenu(wxMenu* pluginsMenu) 
{
    wxMenu* menu = new wxMenu;
    menu->Append(XRCID("webtools_settings"), _("Settings..."));
    pluginsMenu->Append(wxID_ANY, _("WebTools"), menu);
    menu->SetNextHandler(this);
    this->SetPreviousHandler(menu);
}

void WebTools::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &WebTools::OnRefreshColours, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &WebTools::OnRefreshColours, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &WebTools::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_CODE_COMPLETE, &WebTools::OnCodeComplete, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD, &WebTools::OnCodeComplete, this);
    EventNotifier::Get()->Unbind(
        wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &WebTools::OnCodeCompleteFunctionCalltip, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &WebTools::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &WebTools::OnEditorChanged, this);
    
    // Disconnect the timer events
    Unbind(wxEVT_TIMER, &WebTools::OnTimer, this, m_timer->GetId());
    m_timer->Stop();
    wxDELETE(m_timer);
    
    m_jsColourThread->Stop();
    wxDELETE(m_jsColourThread);
    m_jsCodeComplete.Reset(NULL);
}

void WebTools::OnRefreshColours(clCommandEvent& event)
{
    event.Skip();
    if(FileExtManager::GetType(event.GetFileName()) == FileExtManager::TypeJS) {
        m_jsColourThread->QueueFile(event.GetFileName());
    }
}

void WebTools::ColourJavaScript(const JavaScriptSyntaxColourThread::Reply& reply)
{
    IEditor* editor = m_mgr->FindEditor(reply.filename);
    if(editor) {
        wxStyledTextCtrl* ctrl = editor->GetSTC();
        ctrl->SetKeyWords(1, reply.properties);
        ctrl->SetKeyWords(3, reply.functions);
        m_lastColourUpdate = time(NULL);
    }
}

void WebTools::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    IEditor::List_t editors;
    m_mgr->GetAllEditors(editors);
    IEditor::List_t::iterator iter = editors.begin();
    for(; iter != editors.end(); ++iter) {
        // Refresh the files' colouring
        if(IsJavaScriptFile((*iter)->GetFileName())) {
            m_jsColourThread->QueueFile((*iter)->GetFileName().GetFullPath());
        }
    }
}

void WebTools::OnCodeComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor && m_jsCodeComplete && IsJavaScriptFile(editor)) {
        event.Skip(false);
        m_jsCodeComplete->CodeComplete(editor);
    }
}

bool WebTools::IsJavaScriptFile(const wxFileName& filename)
{
    return FileExtManager::GetType(filename.GetFullName()) == FileExtManager::TypeJS;
}

bool WebTools::IsJavaScriptFile(const wxString& filename)
{
    return FileExtManager::GetType(filename) == FileExtManager::TypeJS;
}

void WebTools::OnCodeCompleteFunctionCalltip(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor && m_jsCodeComplete && IsJavaScriptFile(editor)) {
        event.Skip(false);
        m_jsCodeComplete->CodeComplete(editor);
    }
}

void WebTools::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
}

void WebTools::OnEditorChanged(wxCommandEvent& event)
{
    // If we have no JS files opened, cleanup the resources
    event.Skip();
}

void WebTools::OnSettings(wxCommandEvent& event)
{
    WebToolsSettings settings(m_mgr->GetTheApp()->GetTopWindow());
    if(settings.ShowModal() == wxID_OK) {
        if(m_jsCodeComplete) {
            m_jsCodeComplete->Reload();
        }
    }
}

void WebTools::OnTimer(wxTimerEvent& event)
{
    event.Skip();
    
    time_t curtime = time(NULL);
    if((curtime - m_lastColourUpdate) < 5) return;
    IEditor* editor = m_mgr->GetActiveEditor();
    
    // Sanity
    CHECK_PTR_RET(editor);
    CHECK_PTR_RET(editor->IsModified());
    if(!IsJavaScriptFile(editor->GetFileName())) return;
        
    // This file is a modified JS file
    m_lastColourUpdate = time(NULL);
    m_jsColourThread->QueueBuffer(editor->GetFileName().GetFullPath(), editor->GetTextRange(0, editor->GetLength()));
}

bool WebTools::IsJavaScriptFile(IEditor* editor)
{
    CHECK_PTR_RET_FALSE(editor);
    if(FileExtManager::IsJavascriptFile(editor->GetFileName())) return true;
    
    // We should also support Code Completion when inside a PHP/HTML file, but within a script area
    if(FileExtManager::IsPHPFile(editor->GetFileName())) {
        wxStyledTextCtrl* ctrl = editor->GetSTC();
        int styleAtCurPos = ctrl->GetStyleAt(ctrl->GetCurrentPos());
        if(styleAtCurPos >= wxSTC_HJ_START && styleAtCurPos <= wxSTC_HJA_REGEX) {
            return true;
        }
    }
    return false;
}
