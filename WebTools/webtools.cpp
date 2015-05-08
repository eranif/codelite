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
    m_longName = wxT("Support for JavScript, XML, HTML and other web development tools");
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
    m_xmlCodeComplete.Reset(new XMLCodeCompletion());
    
    // Connect the timer
    m_timer = new wxTimer(this);
    m_timer->Start(3000);
    Bind(wxEVT_TIMER, &WebTools::OnTimer, this, m_timer->GetId());
    wxTheApp->Bind(wxEVT_MENU, &WebTools::OnCommentLine, this, XRCID("comment_line"));
    wxTheApp->Bind(wxEVT_MENU, &WebTools::OnCommentSelection, this, XRCID("comment_selection"));
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
    
    wxTheApp->Unbind(wxEVT_MENU, &WebTools::OnCommentLine, this, XRCID("comment_line"));
    wxTheApp->Unbind(wxEVT_MENU, &WebTools::OnCommentSelection, this, XRCID("comment_selection"));
    
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
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
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
        if(InsideJSComment(editor) || InsideJSString(editor)) {
            // User the word completion plugin instead
            m_jsCodeComplete->TriggerWordCompletion();
        } else {
            m_jsCodeComplete->CodeComplete(editor);
        }
    } else if(editor && m_xmlCodeComplete && editor->GetCtrl()->GetLexer() == wxSTC_LEX_XML) {
        // an XML file
        event.Skip(false);
        m_xmlCodeComplete->XmlCodeComplete(editor);
    } else if(editor && m_xmlCodeComplete && IsHTMLFile(editor)) {
        // Html code completion
        event.Skip(false);
        m_xmlCodeComplete->HtmlCodeComplete(editor);
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
    if(editor && m_jsCodeComplete && IsJavaScriptFile(editor) && !InsideJSComment(editor)) {
        event.Skip(false);
        m_jsCodeComplete->CodeComplete(editor);
    }
}

void WebTools::OnWorkspaceClosed(wxCommandEvent& event) { event.Skip(); }

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
        if(m_xmlCodeComplete) {
            m_xmlCodeComplete->Reload();
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
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        int styleAtCurPos = ctrl->GetStyleAt(ctrl->GetCurrentPos());
        if(styleAtCurPos >= wxSTC_HJ_START && styleAtCurPos <= wxSTC_HJA_REGEX) {
            return true;
        }
    }
    return false;
}

bool WebTools::InsideJSComment(IEditor* editor)
{
    int curpos = editor->PositionBeforePos(editor->GetCurrentPosition());
    int styleAtCurPos = editor->GetCtrl()->GetStyleAt(curpos);
    if(FileExtManager::IsJavascriptFile(editor->GetFileName())) {
        // Use the Cxx macros
        return styleAtCurPos == wxSTC_C_COMMENT || styleAtCurPos == wxSTC_C_COMMENTLINE ||
               styleAtCurPos == wxSTC_C_COMMENTDOC || styleAtCurPos == wxSTC_C_COMMENTLINEDOC ||
               styleAtCurPos == wxSTC_C_COMMENTDOCKEYWORD || styleAtCurPos == wxSTC_C_COMMENTDOCKEYWORDERROR ||
               styleAtCurPos == wxSTC_C_PREPROCESSORCOMMENT;
    } else if(FileExtManager::IsPHPFile(editor->GetFileName())) {
        if(styleAtCurPos >= wxSTC_HJ_START && styleAtCurPos <= wxSTC_HJA_REGEX) {
            return styleAtCurPos == wxSTC_HJ_COMMENT || styleAtCurPos == wxSTC_HJ_COMMENTLINE ||
                   styleAtCurPos == wxSTC_HJ_COMMENTDOC;
        }
    }
    return false;
}

bool WebTools::InsideJSString(IEditor* editor)
{
    int curpos = editor->PositionBeforePos(editor->GetCurrentPosition());
    int styleAtCurPos = editor->GetCtrl()->GetStyleAt(curpos);
    if(FileExtManager::IsJavascriptFile(editor->GetFileName())) {
        // Use the Cxx macros
        return styleAtCurPos == wxSTC_C_STRING || styleAtCurPos == wxSTC_C_CHARACTER ||
               styleAtCurPos == wxSTC_C_STRINGEOL || styleAtCurPos == wxSTC_C_STRINGRAW ||
               styleAtCurPos == wxSTC_C_HASHQUOTEDSTRING;
    } else if(FileExtManager::IsPHPFile(editor->GetFileName())) {
        if(styleAtCurPos >= wxSTC_HJ_START && styleAtCurPos <= wxSTC_HJA_REGEX) {
            return styleAtCurPos == wxSTC_HJ_DOUBLESTRING || styleAtCurPos == wxSTC_HJ_SINGLESTRING ||
                   styleAtCurPos == wxSTC_HJ_STRINGEOL;
        }
    }
    return false;
}

void WebTools::OnCommentLine(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);
    
    if(IsJavaScriptFile(editor)) {
        e.Skip(false);
        editor->ToggleLineComment("//", wxSTC_C_COMMENTLINE);
    }
}

void WebTools::OnCommentSelection(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);
    
    if(IsJavaScriptFile(editor)) {
        e.Skip(false);
        editor->CommentBlockSelection("/*", "*/");
    }
}

bool WebTools::IsHTMLFile(IEditor* editor)
{
    CHECK_PTR_RET_FALSE(editor);
    if(FileExtManager::GetType(editor->GetFileName().GetFullName()) == FileExtManager::TypeHtml) return true;

    // We should also support Code Completion when inside a mixed PHP and HTML file
    if(FileExtManager::IsPHPFile(editor->GetFileName())) {
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        int styleAtCurPos = ctrl->GetStyleAt(ctrl->GetCurrentPos());
        if(styleAtCurPos >= wxSTC_H_DEFAULT && styleAtCurPos <= wxSTC_H_ENTITY) {
            return true;
        }
    }
    return false;
}
