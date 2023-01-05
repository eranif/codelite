#include "webtools.h"

#include "NodeDebuggerPane.h"
#include "NodeJSEvents.h"
#include "NodeJSWorkspaceView.h"
#include "NoteJSWorkspace.h"
#include "PHPSourceFile.h"
#include "PhpLexerAPI.h"
#include "WebToolsBase.h"
#include "WebToolsConfig.h"
#include "WebToolsSettings.h"
#include "bitmap_loader.h"
#include "clNodeJS.h"
#include "clWorkspaceManager.h"
#include "clWorkspaceView.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "globals.h"
#include "tags_options_data.h"

#include <wx/dirdlg.h>
#include <wx/menu.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

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
    , m_clangOldFlag(false)
{
    m_longName = _("Support for JavaScript, CSS/SCSS, HTML, XML and other web development tools");
    m_shortName = wxT("WebTools");

    // Initialise NodeJS
    WebToolsConfig& conf = WebToolsConfig::Get().Load();
    wxFileName fnNodeJS(conf.GetNodejs());
    wxArrayString hints;
    if(fnNodeJS.FileExists()) {
        hints.Add(fnNodeJS.GetPath());
    }
    clNodeJS::Get().Initialise(hints);

    // Update the configuration with the findings
    conf.SetNodejs(clNodeJS::Get().GetNode().GetFullPath());
    conf.SetNpm(clNodeJS::Get().GetNpm().GetFullPath());

    // Register our new workspace type
    NodeJSWorkspace::Get(); // Instantiate the singleton by faking a call
    clWorkspaceManager::Get().RegisterWorkspace(new NodeJSWorkspace(true));

    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &WebTools::OnFileLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &WebTools::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &WebTools::OnThemeChanged, this);

    // Context menu
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &WebTools::OnEditorContextMenu, this);

    // Workspace related events
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &WebTools::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &WebTools::OnWorkspaceLoaded, this);

    // Theme management
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &WebTools::OnEditorChanged, this);

    // Debugger related
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STARTED, &WebTools::OnNodeJSCliDebuggerStarted, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STOPPED, &WebTools::OnNodeJSDebuggerStopped, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_PLUGIN_DEBUGGER, &WebTools::OnIsDebugger, this);

    Bind(wxEVT_MENU, &WebTools::OnSettings, this, XRCID("webtools_settings"));
    Bind(wxEVT_NODE_COMMAND_TERMINATED, &WebTools::OnNodeCommandCompleted, this);

    m_xmlCodeComplete.Reset(new XMLCodeCompletion(this));
    m_cssCodeComplete.Reset(new CSSCodeCompletion(this));

    // Connect the timer
    m_timer = new wxTimer(this);
    m_timer->Start(3000);
    Bind(wxEVT_TIMER, &WebTools::OnTimer, this, m_timer->GetId());
    wxTheApp->Bind(wxEVT_MENU, &WebTools::OnCommentLine, this, XRCID("comment_line"));
    wxTheApp->Bind(wxEVT_MENU, &WebTools::OnCommentSelection, this, XRCID("comment_selection"));
}

WebTools::~WebTools() { NodeJSWorkspace::Free(); }

void WebTools::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

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
    // Store the configuration changes
    WebToolsConfig::Get().SaveConfig();
    Unbind(wxEVT_NODE_COMMAND_TERMINATED, &WebTools::OnNodeCommandCompleted, this);

    // Stop the debugger
    if(NodeJSWorkspace::Get()->IsOpen() && NodeJSWorkspace::Get()->GetDebugger() &&
       NodeJSWorkspace::Get()->GetDebugger()->IsRunning()) {
        NodeJSWorkspace::Get()->GetDebugger()->StopDebugger();
    }

    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &WebTools::OnEditorContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &WebTools::OnFileLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &WebTools::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &WebTools::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &WebTools::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &WebTools::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &WebTools::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_STARTED, &WebTools::OnNodeJSCliDebuggerStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_STOPPED, &WebTools::OnNodeJSDebuggerStopped, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_IS_PLUGIN_DEBUGGER, &WebTools::OnIsDebugger, this);

    wxTheApp->Unbind(wxEVT_MENU, &WebTools::OnCommentLine, this, XRCID("comment_line"));
    wxTheApp->Unbind(wxEVT_MENU, &WebTools::OnCommentSelection, this, XRCID("comment_selection"));

    // Disconnect the timer events
    Unbind(wxEVT_TIMER, &WebTools::OnTimer, this, m_timer->GetId());
    m_timer->Stop();
    wxDELETE(m_timer);
}

void WebTools::DoRefreshColours(const wxString& filename) { wxUnusedVar(filename); }

void WebTools::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    IEditor::List_t editors;
    m_mgr->GetAllEditors(editors);
}

bool WebTools::IsJavaScriptFile(const wxFileName& filename)
{
    return FileExtManager::GetType(filename.GetFullName()) == FileExtManager::TypeJS;
}

bool WebTools::IsJavaScriptFile(const wxString& filename)
{
    return FileExtManager::GetType(filename) == FileExtManager::TypeJS;
}

void WebTools::OnWorkspaceClosed(clWorkspaceEvent& event) { event.Skip(); }

void WebTools::OnEditorChanged(wxCommandEvent& event)
{
    // If we have no JS files opened, cleanup the resources
    event.Skip();
}

void WebTools::OnSettings(wxCommandEvent& event)
{
    WebToolsSettings settings(m_mgr->GetTheApp()->GetTopWindow());
    if(settings.ShowModal() == wxID_OK) {
        NodeJSWorkspace::Get()->AllocateDebugger();
        if(m_xmlCodeComplete) {
            m_xmlCodeComplete->Reload();
        }
    }
}

void WebTools::OnTimer(wxTimerEvent& event) { event.Skip(); }

bool WebTools::IsJavaScriptFile(IEditor* editor)
{
    CHECK_PTR_RET_FALSE(editor);
    if(FileExtManager::IsJavascriptFile(editor->GetFileName()))
        return true;

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
    if(FileExtManager::GetType(editor->GetFileName().GetFullName()) == FileExtManager::TypeHtml)
        return true;

    // We should also support Code Completion when inside a mixed PHP and HTML file
    if(FileExtManager::IsPHPFile(editor->GetFileName())) {

        // Check to see if we are inside a PHP section or not
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        wxString buffer = ctrl->GetTextRange(0, ctrl->GetCurrentPos());
        return !PHPSourceFile::IsInPHPSection(buffer);
    }
    return false;
}

void WebTools::OnNodeJSDebuggerStopped(clDebugEvent& event)
{
    event.Skip();

    LOG_IF_TRACE { clDEBUG1() << "Saving NodeJS debugger perspective"; }

    wxString layoutFileName = "nodejs.layout";
    if(event.GetEventType() == wxEVT_NODEJS_DEBUGGER_STOPPED) {
        layoutFileName = "nodejs_cli.layout";
    }
    wxFileName fnNodeJSLayout(clStandardPaths::Get().GetUserDataDir(), layoutFileName);
    fnNodeJSLayout.AppendDir("config");
    FileUtils::WriteFileContent(fnNodeJSLayout, m_mgr->GetDockingManager()->SavePerspective());

    if(!m_savePerspective.IsEmpty()) {
        m_mgr->GetDockingManager()->LoadPerspective(m_savePerspective);
        m_savePerspective.clear();
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

void WebTools::OnWorkspaceLoaded(clWorkspaceEvent& event)
{
    event.Skip();
    wxFileName workspaceFile = event.GetString();
    if(!workspaceFile.IsOk()) {
        return;
    }
}

bool WebTools::IsCSSFile(IEditor* editor)
{
    return (FileExtManager::GetType(editor->GetFileName().GetFullName()) == FileExtManager::TypeCSS);
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
    if(editor && IsJavaScriptFile(editor) && !InsideJSComment(editor)) {
        // Remove all compiler markers
        editor->DelAllCompilerMarkers();
        if(WebToolsConfig::Get().IsLintOnSave()) {
            clNodeJS::Get().LintFile(event.GetFileName());
        }
    }
}

void WebTools::OnEditorContextMenu(clContextMenuEvent& event) { event.Skip(); }

void WebTools::OnIsDebugger(clDebugEvent& event)
{
    event.Skip(); // always call skip
    // event.GetStrings().Add("NodeJS Debugger");
}

void WebTools::OnNodeJSCliDebuggerStarted(clDebugEvent& event)
{
    event.Skip();
    m_savePerspective = clGetManager()->GetDockingManager()->SavePerspective();

    wxWindow* parent = m_mgr->GetDockingManager()->GetManagedWindow();
    // Show the debugger pane
    if(!m_nodejsCliDebuggerPane) {
        m_nodejsCliDebuggerPane = new NodeDebuggerPane(parent);
        // Let the pane process the 'startup' event as well
        m_nodejsCliDebuggerPane->GetEventHandler()->ProcessEvent(event);
        clGetManager()->GetDockingManager()->AddPane(m_nodejsCliDebuggerPane, wxAuiPaneInfo()
                                                                                  .MinSize(wxSize(-1, 300))
                                                                                  .Layer(5)
                                                                                  .Name("nodejs_cli_debugger")
                                                                                  .Caption("Node.js Debugger")
                                                                                  .CloseButton(false)
                                                                                  .MaximizeButton()
                                                                                  .Bottom()
                                                                                  .Position(0));
    }

    wxString layout;
    wxFileName fnNodeJSLayout(clStandardPaths::Get().GetUserDataDir(), "nodejs_cli.layout");
    fnNodeJSLayout.AppendDir("config");
    if(FileUtils::ReadFileContent(fnNodeJSLayout, layout)) {
        m_mgr->GetDockingManager()->LoadPerspective(layout);
    }
    EnsureAuiPaneIsVisible("nodejs_cli_debugger", true);
}

void WebTools::OnNodeCommandCompleted(clProcessEvent& event) { event.Skip(); }
