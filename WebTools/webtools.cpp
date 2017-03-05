#include "webtools.h"
#include <wx/xrc/xmlres.h>
#include "fileextmanager.h"
#include <wx/stc/stc.h>
#include "event_notifier.h"
#include "codelite_events.h"
#include "WebToolsSettings.h"
#include <wx/menu.h>
#include "NoteJSWorkspace.h"
#include "NodeJSWorkspaceView.h"
#include "clWorkspaceManager.h"
#include "globals.h"
#include "clWorkspaceView.h"
#include <wx/dirdlg.h>
#include "tags_options_data.h"
#include "ctags_manager.h"
#include "PhpLexerAPI.h"
#include "PHPSourceFile.h"
#include "NodeJSDebuggerPane.h"
#include "WebToolsConfig.h"
#include "fileutils.h"
#include "NodeJSEvents.h"
#include "WebToolsBase.h"
#include "bitmap_loader.h"

static WebTools* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new WebTools(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("WebTools"));
    info.SetDescription(_("Support for JavaScript, CSS/SCSS, HTML, XML and other web development tools"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

WebTools::WebTools(IManager* manager)
    : IPlugin(manager)
    , m_lastColourUpdate(0)
    , m_clangOldFlag(false)
    , m_nodejsDebuggerPane(NULL)
    , m_hideToolBarOnDebugStop(false)
{
    m_longName = _("Support for JavaScript, CSS/SCSS, HTML, XML and other web development tools");
    m_shortName = wxT("WebTools");

    // Register our new workspace type
    NodeJSWorkspace::Get(); // Instantiate the singleton by faking a call
    clWorkspaceManager::Get().RegisterWorkspace(new NodeJSWorkspace(true));

    WebToolsImages images;
    // BitmapLoader::RegisterImage(FileExtManager::TypeWorkspaceNodeJS, images.Bitmap("m_bmpNodeJS"));

    // Create the syntax highligher worker thread
    m_jsColourThread = new JavaScriptSyntaxColourThread(this);
    m_jsColourThread->Create();
    m_jsColourThread->Run();

    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &WebTools::OnFileLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &WebTools::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &WebTools::OnThemeChanged, this);

    // Context menu
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &WebTools::OnEditorContextMenu, this);

    // Code completion related events
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE, &WebTools::OnCodeComplete, this);
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD, &WebTools::OnCodeComplete, this);
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &WebTools::OnCodeCompleteFunctionCalltip, this);
    EventNotifier::Get()->Bind(wxEVT_CC_FIND_SYMBOL, &WebTools::OnFindSymbol, this);

    // Workspace related events
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &WebTools::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &WebTools::OnWorkspaceLoaded, this);

    // Theme management
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &WebTools::OnEditorChanged, this);

    // Debugger related
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STARTED, &WebTools::OnNodeJSDebuggerStarted, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STOPPED, &WebTools::OnNodeJSDebuggerStopped, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_PLUGIN_DEBUGGER, &WebTools::OnIsDebugger, this);

    Bind(wxEVT_MENU, &WebTools::OnSettings, this, XRCID("webtools_settings"));
    m_jsCodeComplete.Reset(new JSCodeCompletion(""));
    m_xmlCodeComplete.Reset(new XMLCodeCompletion());
    m_cssCodeComplete.Reset(new CSSCodeCompletion());
    m_jsctags.Reset(new clJSCTags());
    
    // Connect the timer
    m_timer = new wxTimer(this);
    m_timer->Start(3000);
    Bind(wxEVT_TIMER, &WebTools::OnTimer, this, m_timer->GetId());
    wxTheApp->Bind(wxEVT_MENU, &WebTools::OnCommentLine, this, XRCID("comment_line"));
    wxTheApp->Bind(wxEVT_MENU, &WebTools::OnCommentSelection, this, XRCID("comment_selection"));
}

WebTools::~WebTools() { NodeJSWorkspace::Free(); }

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
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &WebTools::OnEditorContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &WebTools::OnFileLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &WebTools::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &WebTools::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_CODE_COMPLETE, &WebTools::OnCodeComplete, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD, &WebTools::OnCodeComplete, this);
    EventNotifier::Get()->Unbind(
        wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &WebTools::OnCodeCompleteFunctionCalltip, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &WebTools::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &WebTools::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &WebTools::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_STARTED, &WebTools::OnNodeJSDebuggerStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_STOPPED, &WebTools::OnNodeJSDebuggerStopped, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_IS_PLUGIN_DEBUGGER, &WebTools::OnIsDebugger, this);

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

void WebTools::DoRefreshColours(const wxString& filename)
{
    if(FileExtManager::GetType(filename) == FileExtManager::TypeJS) {
        m_jsColourThread->QueueFile(filename);
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
    } else if(editor && m_cssCodeComplete && IsCSSFile(editor)) {
        // CSS code completion
        event.Skip(false);
        m_cssCodeComplete->CssCodeComplete(editor);
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
    settings.ShowModal();
    if(m_jsCodeComplete) {
        m_jsCodeComplete->Reload();
        m_jsCodeComplete->ClearFatalError();
    }
    if(m_xmlCodeComplete) {
        m_xmlCodeComplete->Reload();
        m_jsCodeComplete->ClearFatalError();
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
    } else if(IsHTMLFile(editor)) {
        e.Skip(false);
        editor->CommentBlockSelection("<!-- ", " -->");
    }
}

bool WebTools::IsHTMLFile(IEditor* editor)
{
    CHECK_PTR_RET_FALSE(editor);
    if(FileExtManager::GetType(editor->GetFileName().GetFullName()) == FileExtManager::TypeHtml) return true;

    // We should also support Code Completion when inside a mixed PHP and HTML file
    if(FileExtManager::IsPHPFile(editor->GetFileName())) {

        // Check to see if we are inside a PHP section or not
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        wxString buffer = ctrl->GetTextRange(0, ctrl->GetCurrentPos());
        return !PHPSourceFile::IsInPHPSection(buffer);
    }
    return false;
}

void WebTools::OnNodeJSDebuggerStarted(clDebugEvent& event)
{
    event.Skip();
    m_savePerspective = clGetManager()->GetDockingManager()->SavePerspective();

    wxWindow* parent = m_mgr->GetDockingManager()->GetManagedWindow();
    // Show the debugger pane
    if(!m_nodejsDebuggerPane) {
        m_nodejsDebuggerPane = new NodeJSDebuggerPane(parent);
        clGetManager()->GetDockingManager()->AddPane(m_nodejsDebuggerPane, wxAuiPaneInfo()
                                                                               .Layer(5)
                                                                               .Name("nodejs_debugger")
                                                                               .Caption("Node.js Debugger")
                                                                               .CloseButton(false)
                                                                               .MaximizeButton()
                                                                               .Bottom()
                                                                               .Position(0));
    }

    wxString layout;
    wxFileName fnNodeJSLayout(clStandardPaths::Get().GetUserDataDir(), "nodejs.layout");
    fnNodeJSLayout.AppendDir("config");
    if(FileUtils::ReadFileContent(fnNodeJSLayout, layout)) {
        m_mgr->GetDockingManager()->LoadPerspective(layout);
    }
    EnsureAuiPaneIsVisible("nodejs_debugger", true);

    m_hideToolBarOnDebugStop = false;
    if(!m_mgr->AllowToolbar()) {
        // Using native toolbar
        m_hideToolBarOnDebugStop = !m_mgr->IsToolBarShown();
        m_mgr->ShowToolBar(true);
    }
}

void WebTools::OnNodeJSDebuggerStopped(clDebugEvent& event)
{
    event.Skip();

    wxFileName fnNodeJSLayout(clStandardPaths::Get().GetUserDataDir(), "nodejs.layout");
    fnNodeJSLayout.AppendDir("config");
    FileUtils::WriteFileContent(fnNodeJSLayout, m_mgr->GetDockingManager()->SavePerspective());

    if(!m_savePerspective.IsEmpty()) {
        m_mgr->GetDockingManager()->LoadPerspective(m_savePerspective);
        m_savePerspective.clear();
    }

    if(m_hideToolBarOnDebugStop) {
        m_mgr->ShowToolBar(false);
    }
}

void WebTools::EnsureAuiPaneIsVisible(const wxString& paneName, bool update)
{
    wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(paneName);
    if(pi.IsOk() && !pi.IsShown()) {
        pi.Show();
    }
    if(update) {
        m_mgr->GetDockingManager()->Update();
    }
}

void WebTools::OnWorkspaceLoaded(wxCommandEvent& event)
{
    event.Skip();
    wxFileName workspaceFile = event.GetString();
    if(FileExtManager::GetType(workspaceFile.GetFullPath()) == FileExtManager::TypeWorkspaceNodeJS) {
        m_jsCodeComplete.Reset(new JSCodeCompletion(workspaceFile.GetPath()));
    } else {
        // For non NodeJS workspaces, create the .tern files under
        // the .codelite folder
        workspaceFile.AppendDir(".codelite");
        m_jsCodeComplete.Reset(new JSCodeCompletion(workspaceFile.GetPath()));
    }
}

bool WebTools::IsCSSFile(IEditor* editor)
{
    return (FileExtManager::GetType(editor->GetFileName().GetFullName()) == FileExtManager::TypeCSS);
}

void WebTools::OnFindSymbol(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor && m_jsCodeComplete && IsJavaScriptFile(editor) && !InsideJSComment(editor)) {
        event.Skip(false);
        m_jsCodeComplete->FindDefinition(editor);
    }
}

void WebTools::OnFileLoaded(clCommandEvent& event)
{
    event.Skip();
    DoRefreshColours(event.GetFileName());
}

void WebTools::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    DoRefreshColours(event.GetFileName());
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor && m_jsCodeComplete && IsJavaScriptFile(editor) && !InsideJSComment(editor)) {
        // m_jsCodeComplete->ReparseFile(editor);
        m_jsCodeComplete->ResetTern();
    }
}

void WebTools::OnEditorContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor && m_jsCodeComplete && IsJavaScriptFile(editor) && !InsideJSComment(editor)) {
        m_jsCodeComplete->AddContextMenu(event.GetMenu(), editor);
    }
}

void WebTools::OnIsDebugger(clDebugEvent& event)
{
    event.Skip(); // always call skip
    // event.GetStrings().Add("NodeJS Debugger");
}
