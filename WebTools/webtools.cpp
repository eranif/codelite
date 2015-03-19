#include "webtools.h"
#include <wx/xrc/xmlres.h>
#include "fileextmanager.h"
#include <wx/stc/stc.h>
#include "event_notifier.h"
#include "codelite_events.h"

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
{
    m_longName = wxT("Support for JavScript, HTML and other web development tools");
    m_shortName = wxT("WebTools");

    // Create the syntax highligher worker thread
    m_jsColourThread = new JavaScriptSyntaxColourThread(this);
    m_jsColourThread->Create();
    m_jsColourThread->Run();

    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &WebTools::OnFileLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &WebTools::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE, &WebTools::OnCodeComplete, this);
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD, &WebTools::OnCodeComplete, this);
}

WebTools::~WebTools() {}

clToolBar* WebTools::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void WebTools::CreatePluginMenu(wxMenu* pluginsMenu) {}

void WebTools::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &WebTools::OnFileLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &WebTools::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_CODE_COMPLETE, &WebTools::OnCodeComplete, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD, &WebTools::OnCodeComplete, this);
    m_jsColourThread->Stop();
    wxDELETE(m_jsColourThread);
}

void WebTools::OnFileLoaded(clCommandEvent& event)
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
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor && IsJavaScriptFile(editor->GetFileName())) {
        // We emulate CC for JS by triggering the word-completion plugin
        wxCommandEvent ccWordComplete(wxEVT_MENU, XRCID("word_complete_no_single_insert"));
        wxTheApp->AddPendingEvent(ccWordComplete);
    } else {
        event.Skip();
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
