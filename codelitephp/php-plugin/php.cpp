#include "php.h"
#include "php_utils.h"
#include "php_open_resource_dlg.h"
#include <wx/xrc/xmlres.h>
#include "editor_config.h"
#include "php_settings_dlg.h"
#include <detachedpanesinfo.h>
#include <dockablepane.h>
#include "php_code_completion.h"
#include "quick_outline_dlg.h"
#include <wx/app.h>
#include <wx/filedlg.h>
#include "new_workspace_selection_dlg.h"
#include "new_php_workspace_dlg.h"
#include "php_workspace.h"
#include "php_workspace_view.h"
#include "php_strings.h"
#include "php_editor_context_menu.h"
#include <event_notifier.h>
#include <plugin.h>
#include <wx/richmsgdlg.h>
#include <cl_config.h>
#include <ctags_manager.h>
#include "ssh_workspace_settings.h"
#include "php_parser_thread.h"
#include <file_logger.h>
#include "PHPDebugPane.h"
#include <cl_standard_paths.h>
#include "xdebugevent.h"
#include "localsview.h"
#include "evalpane.h"
#include "XDebugTester.h"
#include "clZipReader.h"
#include "cl_standard_paths.h"
#include "php_configuration_data.h"
#include "PHPLocator.h"
#include <wx/regex.h>
#include "bookmark_manager.h"
#include "NewPHPProjectWizard.h"
#include "PHPXDebugSetupWizard.h"
#include "globals.h"

static PhpPlugin* thePlugin = NULL;

// Define the plugin entry point
extern "C" EXPORT IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new PhpPlugin(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("The CodeLite Team"));
    info.SetName(wxT("PHP"));
    info.SetDescription(wxT("Enable PHP support for codelite IDE"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

PhpPlugin::PhpPlugin(IManager* manager)
    : IPlugin(manager)
    , m_clangOldFlag(false)
    , m_browser(NULL)
    , m_debuggerPane(NULL)
    , m_xdebugLocalsView(NULL)
    , m_xdebugEvalPane(NULL)
    , m_showWelcomePage(false)
{
    m_lint.Reset(new PHPLint(this));

    m_longName = wxT("PHP Plugin for the codelite IDE");
    m_shortName = wxT("PHP");

    // Instantiate the bitmaps, we do this so they will be populated in wxXmlResource
    // Sigleton class
    PHPImages images;
    PHPWorkspace::Get()->SetPluginManager(m_mgr);
    XDebugManager::Initialize(this);

    // Add our UI
    // create tab (possibly detached)
    Notebook* book = m_mgr->GetWorkspacePaneNotebook();
    if(IsWorkspaceViewDetached()) {
        // Make the window child of the main panel (which is the grand parent of the notebook)
        DockablePane* cp = new DockablePane(
            book->GetParent()->GetParent(), book, PHPStrings::PHP_WORKSPACE_VIEW_TITLE, wxNullBitmap, wxSize(200, 200));
        m_workspaceView = new PHPWorkspaceView(cp, m_mgr);
        cp->SetChildNoReparent(m_workspaceView);

    } else {
        m_workspaceView = new PHPWorkspaceView(book, m_mgr);
        book->InsertPage(0, m_workspaceView, PHPStrings::PHP_WORKSPACE_VIEW_TITLE, true);
    }

    PHPCodeCompletion::Instance()->SetManager(m_mgr);

    PHPEditorContextMenu::Instance()->ConnectEvents();
    PHPParserThread::Instance()->Start();

    // Pass the manager class to the context menu manager
    PHPEditorContextMenu::Instance()->SetManager(m_mgr);

    // Connect events
    EventNotifier::Get()->Connect(
        wxEVT_CC_SHOW_QUICK_OUTLINE, clCodeCompletionEventHandler(PhpPlugin::OnShowQuickOutline), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_DBG_UI_DELTE_ALL_BREAKPOINTS, clDebugEventHandler(PhpPlugin::OnXDebugDeleteAllBreakpoints), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_CREATE_NEW_WORKSPACE, wxCommandEventHandler(PhpPlugin::OnNewWorkspace), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_NEW_PROJECT_WIZARD_SHOWING, clNewProjectEventHandler(PhpPlugin::OnNewProject), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_NEW_PROJECT_WIZARD_FINISHED, clNewProjectEventHandler(PhpPlugin::OnNewProjectFinish), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_IS_WORKSPACE_OPEN, clCommandEventHandler(PhpPlugin::OnIsWorkspaceOpen), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_CLOSE_WORKSPACE, clCommandEventHandler(PhpPlugin::OnCloseWorkspace), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_OPEN_WORKSPACE, clCommandEventHandler(PhpPlugin::OnOpenWorkspace), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_RELOAD_WORKSPACE, clCommandEventHandler(PhpPlugin::OnReloadWorkspace), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_OPEN_RESOURCE, wxCommandEventHandler(PhpPlugin::OnOpenResource), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_GET_WORKSPACE_FILES, wxCommandEventHandler(PhpPlugin::OnGetWorkspaceFiles), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_GET_CURRENT_FILE_PROJECT_FILES,
                                  wxCommandEventHandler(PhpPlugin::OnGetCurrentFileProjectFiles),
                                  NULL,
                                  this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_GET_ACTIVE_PROJECT_FILES, wxCommandEventHandler(PhpPlugin::OnGetActiveProjectFiles), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_GET_FIND_IN_FILES_MASK, clCommandEventHandler(PhpPlugin::OnGetFiFMask), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_FIND_IN_FILES_DISMISSED, clCommandEventHandler(PhpPlugin::OnFindInFilesDismissed), NULL, this);

    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(PhpPlugin::OnFileSaved), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PHP_LOAD_URL, PHPEventHandler(PhpPlugin::OnLoadURL), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(PhpPlugin::OnAllEditorsClosed), NULL, this);

    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_STARTED, &PhpPlugin::OnDebugStarted, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_ENDED, &PhpPlugin::OnDebugEnded, this);

    EventNotifier::Get()->Connect(wxEVT_GOING_DOWN, clCommandEventHandler(PhpPlugin::OnGoingDown), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SYSTEM_UPDATED, &PhpPlugin::OnFileSysetmUpdated, this);
    EventNotifier::Get()->Bind(wxEVT_SAVE_SESSION_NEEDED, &PhpPlugin::OnSaveSession, this);
    CallAfter(&PhpPlugin::FinalizeStartup);

    // Extract all CC files from PHP.zip into the folder ~/.codelite/php-plugin/cc
    wxFileName phpResources(clStandardPaths::Get().GetDataDir(), "PHP.zip");
    if(phpResources.Exists()) {

        clZipReader zipReader(phpResources);
        wxFileName targetDir(clStandardPaths::Get().GetUserDataDir(), "");
        targetDir.AppendDir("php-plugin");

        // Don't extract the zip if one of the files on disk is newer or equal to the zip timestamp
        wxFileName fnSampleFile(targetDir.GetPath(), "basic.php");
        fnSampleFile.AppendDir("cc");
        PHPConfigurationData config;
        if(!fnSampleFile.Exists() || // the sample file does not exists
                                     // Or the resource file (PHP.zip) is newer than the sample file
           (phpResources.GetModificationTime().GetTicks() > fnSampleFile.GetModificationTime().GetTicks())) {

            targetDir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            zipReader.Extract("*", targetDir.GetPath());

            // Make sure we add this path to the general PHP settings
            targetDir.AppendDir("cc"); // the CC files are located under an internal folder named "cc" (lowercase)
            
            if(config.Load().GetCcIncludePath().Index(targetDir.GetPath()) == wxNOT_FOUND) {
                config.Load().GetCcIncludePath().Add(targetDir.GetPath());
                config.Save();
            }
        } else if(fnSampleFile.Exists()) {
            // Ensure that we have the core PHP code completion methods
            if(config.Load().GetCcIncludePath().Index(fnSampleFile.GetPath()) == wxNOT_FOUND) {
                config.Load().GetCcIncludePath().Add(fnSampleFile.GetPath());
                config.Save();
            }
        }
    } else {
        CL_WARNING("PHP: Could not locate PHP resources 'PHP.zip' => '%s'", phpResources.GetFullPath());
    }
}

PhpPlugin::~PhpPlugin() {}

bool PhpPlugin::IsWorkspaceViewDetached()
{
    DetachedPanesInfo dpi;
    m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
    wxArrayString detachedPanes = dpi.GetPanes();
    return detachedPanes.Index(PHPStrings::PHP_WORKSPACE_VIEW_TITLE) != wxNOT_FOUND;
}

clToolBar* PhpPlugin::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void PhpPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    if(pluginsMenu->GetMenuBar()) {
        DoPlaceMenuBar(pluginsMenu->GetMenuBar());
    }
}

void PhpPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void PhpPlugin::UnHookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void PhpPlugin::UnPlug()
{
    XDebugManager::Free();
    EventNotifier::Get()->Disconnect(
        wxEVT_DBG_UI_DELTE_ALL_BREAKPOINTS, clDebugEventHandler(PhpPlugin::OnXDebugDeleteAllBreakpoints), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CC_SHOW_QUICK_OUTLINE, clCodeCompletionEventHandler(PhpPlugin::OnShowQuickOutline), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_CREATE_NEW_WORKSPACE, wxCommandEventHandler(PhpPlugin::OnNewWorkspace), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_NEW_PROJECT_WIZARD_SHOWING, clNewProjectEventHandler(PhpPlugin::OnNewProject), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_NEW_PROJECT_WIZARD_FINISHED, clNewProjectEventHandler(PhpPlugin::OnNewProjectFinish), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_IS_WORKSPACE_OPEN, clCommandEventHandler(PhpPlugin::OnIsWorkspaceOpen), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_CLOSE_WORKSPACE, clCommandEventHandler(PhpPlugin::OnCloseWorkspace), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_OPEN_WORKSPACE, clCommandEventHandler(PhpPlugin::OnOpenWorkspace), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_RELOAD_WORKSPACE, clCommandEventHandler(PhpPlugin::OnReloadWorkspace), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_OPEN_RESOURCE, wxCommandEventHandler(PhpPlugin::OnOpenResource), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_GET_WORKSPACE_FILES, wxCommandEventHandler(PhpPlugin::OnGetWorkspaceFiles), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_FIND_IN_FILES_DISMISSED, clCommandEventHandler(PhpPlugin::OnFindInFilesDismissed), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_GET_CURRENT_FILE_PROJECT_FILES,
                                     wxCommandEventHandler(PhpPlugin::OnGetCurrentFileProjectFiles),
                                     NULL,
                                     this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_GET_ACTIVE_PROJECT_FILES, wxCommandEventHandler(PhpPlugin::OnGetActiveProjectFiles), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_GET_FIND_IN_FILES_MASK, clCommandEventHandler(PhpPlugin::OnGetFiFMask), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(PhpPlugin::OnFileSaved), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PHP_LOAD_URL, PHPEventHandler(PhpPlugin::OnLoadURL), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(PhpPlugin::OnAllEditorsClosed), NULL, this);

    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_STARTED, &PhpPlugin::OnDebugStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_ENDED, &PhpPlugin::OnDebugEnded, this);
    EventNotifier::Get()->Disconnect(wxEVT_GOING_DOWN, clCommandEventHandler(PhpPlugin::OnGoingDown), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SYSTEM_UPDATED, &PhpPlugin::OnFileSysetmUpdated, this);
    EventNotifier::Get()->Unbind(wxEVT_SAVE_SESSION_NEEDED, &PhpPlugin::OnSaveSession, this);
    
    SafelyDetachAndDestroyPane(m_debuggerPane, "XDebug");
    SafelyDetachAndDestroyPane(m_xdebugLocalsView, "XDebugLocals");
    SafelyDetachAndDestroyPane(m_xdebugEvalPane, "XDebugEval");

    // Remove the PHP tab
    size_t index = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_workspaceView);
    if(index != Notebook::npos) {
        m_mgr->GetWorkspacePaneNotebook()->RemovePage(index, false);
    }

    // Close any open workspace
    if(PHPWorkspace::Get()->IsOpen()) {
        PHPWorkspace::Get()->Close(true, false);
        m_workspaceView->UnLoadWorkspace();
    }

    m_workspaceView->Destroy();
    m_workspaceView = NULL;

    PHPParserThread::Release();
    PHPWorkspace::Release();
    PHPCodeCompletion::Release();
    PHPEditorContextMenu::Release();
}

void PhpPlugin::OnShowQuickOutline(clCodeCompletionEvent& e)
{
    IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
    if(editor) {
        // we handle only .php files
        if(!IsPHPFile(editor)) {
            // get the position
            e.Skip();
            return;
        }
        PHPQuickOutlineDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), editor, m_mgr);
        dlg.ShowModal();
        CallAfter(&PhpPlugin::SetEditorActive, editor);
    }
}

void PhpPlugin::OnNewWorkspace(wxCommandEvent& e)
{
    NewWorkspaceSelectionDlg dlg(FRAME);
    if(dlg.ShowModal() == wxID_OK) {
        if(dlg.GetIsPHPWorkspace()) {

            // Create a PHP workspace
            NewPHPWorkspaceDlg newWspDlg(m_mgr->GetTheApp()->GetTopWindow());
            if(newWspDlg.ShowModal() == wxID_OK) {
                PHPWorkspace::Get()->Create(newWspDlg.GetWorkspacePath());
                DoOpenWorkspace(newWspDlg.GetWorkspacePath());
            }

        } else {
            // Call skip so the normal new workspace dialog will popup
            e.Skip();
        }
    }
}

void PhpPlugin::OnIsWorkspaceOpen(clCommandEvent& e)
{
    e.Skip();
    bool isOpen = PHPWorkspace::Get()->IsOpen();
    e.SetAnswer(isOpen);
    if(isOpen) {
        e.SetFileName(PHPWorkspace::Get()->GetFilename().GetFullPath());
        e.SetString(e.GetFileName());
    }
}

void PhpPlugin::OnCloseWorkspace(clCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {

        m_mgr->EnableClangCodeCompletion(m_clangOldFlag);
        PHPWorkspace::Get()->Close(true, true);
        m_workspaceView->UnLoadWorkspace();

        // notify codelite to close the currently opened workspace
        wxCommandEvent eventClose(wxEVT_COMMAND_MENU_SELECTED, wxID_CLOSE_ALL);
        eventClose.SetEventObject(FRAME);
        FRAME->GetEventHandler()->ProcessEvent(eventClose);

        wxCommandEvent eventCloseWsp(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
        eventCloseWsp.SetEventObject(FRAME);
        FRAME->GetEventHandler()->ProcessEvent(eventCloseWsp);

        // Show the 'Workspace' tab by sending the main frame a "Hide workspace tab" event
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("show_workspace_tab"));
        event.SetEventObject(FRAME);
        event.SetInt(1);
        FRAME->GetEventHandler()->AddPendingEvent(event);

        /// The 'wxID_CLOSE_ALL' is done async (i.e. it will take place in the next event loop)
        /// So we mark ourself that we should display the welcome page next time we capture
        /// the 'All Editors Closed' event
        m_showWelcomePage = true;

    } else {
        e.Skip();
    }
}

void PhpPlugin::OnOpenWorkspace(clCommandEvent& e)
{
    e.Skip();
    wxFileName workspaceFile(e.GetFileName());
    JSONRoot root(workspaceFile);
    if(!root.isOk()) return;

    wxString type = root.toElement().namedObject("metadata").namedObject("type").toString();
    bool hasProjects = root.toElement().hasNamedObject("projects");
    if(type == "php" || hasProjects) {
        // this is our to handle
        e.Skip(false);
    } else {
        return;
    }

    // Check if this is a PHP workspace
    if(PHPWorkspace::Get()->IsOpen()) {
        PHPWorkspace::Get()->Close(true, true);
    }
    DoOpenWorkspace(workspaceFile.GetFullPath());
}

void PhpPlugin::DoOpenWorkspace(const wxString& filename, bool createIfMissing)
{
    // notify codelite to close the currently opened workspace
    wxCommandEvent eventClose(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_workspace"));
    eventClose.SetEventObject(FRAME);
    FRAME->GetEventHandler()->ProcessEvent(eventClose);

    // Open the PHP workspace
    if(!PHPWorkspace::Get()->Open(filename, createIfMissing)) {
        wxMessageBox(_("Failed to open workspace: corrupted workspace file"),
                     wxT("CodeLite"),
                     wxOK | wxICON_WARNING | wxCENTER,
                     FRAME);
        return;
    }

    // Keep the old clang state before we disable it
    const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();
    m_clangOldFlag = (options.GetClangOptions() & CC_CLANG_ENABLED);

    m_mgr->EnableClangCodeCompletion(false);
    m_workspaceView->LoadWorkspace();

    // Hide the 'Workspace' tab by sending the main frame a "Hide workspace tab" event
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("show_workspace_tab"));
    event.SetEventObject(FRAME);
    event.SetInt(0);
    FRAME->GetEventHandler()->AddPendingEvent(event);

    // Select the 'PHP' tab
    size_t index = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_workspaceView);
    if(index != Notebook::npos) {
        m_mgr->GetWorkspacePaneNotebook()->SetSelection(index);
    }

    // and finally, request codelite to keep this workspace in the recently opened workspace list
    m_mgr->AddWorkspaceToRecentlyUsedList(filename);
}

void PhpPlugin::OnOpenResource(wxCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        OpenResourceDlg dlg(FRAME, m_mgr);
        if(dlg.ShowModal() == wxID_OK) {
            ResourceItem* itemData = dlg.GetSelectedItem();
            if(itemData) {
                if(m_mgr->OpenFile(itemData->filename.GetFullPath())) {
                    IEditor* editor = m_mgr->GetActiveEditor();
                    if(editor && itemData->line != wxNOT_FOUND) {
                        editor->FindAndSelect(
                            itemData->displayName, itemData->displayName, editor->PosFromLine(itemData->line), NULL);
                    }
                }
            }
        }
    } else {
        e.Skip();
    }
}

void PhpPlugin::OnGetActiveProjectFiles(wxCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        wxArrayString* pfiles = (wxArrayString*)e.GetClientData();
        if(pfiles) {
            wxString activeProjectName = PHPWorkspace::Get()->GetActiveProjectName();
            PHPProject::Ptr_t proj = PHPWorkspace::Get()->GetProject(activeProjectName);
            CHECK_PTR_RET(proj);
            const wxArrayString& projfiles = proj->GetFiles();
            pfiles->insert(pfiles->end(), projfiles.begin(), projfiles.end());
        }
    } else {
        e.Skip();
    }
}

void PhpPlugin::OnGetCurrentFileProjectFiles(wxCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        IEditor* editor = m_mgr->GetActiveEditor();
        wxArrayString* pfiles = (wxArrayString*)e.GetClientData();
        if(editor && pfiles) {
            ::wxMessageBox("Not implemented for PHP!");
        }
    } else {
        e.Skip();
    }
}

void PhpPlugin::OnGetWorkspaceFiles(wxCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        wxArrayString* pfiles = (wxArrayString*)e.GetClientData();
        if(pfiles) {
            std::set<wxString> files;
            PHPWorkspace::Get()->GetWorkspaceFiles(files);
            std::set<wxString>::iterator iter = files.begin();
            for(; iter != files.end(); iter++) {
                pfiles->Add(*iter);
            }
        }

    } else
        e.Skip();
}

void PhpPlugin::OnNewProject(clNewProjectEvent& e)
{
    if(!PHPWorkspace::Get()->IsOpen()) {
        e.Skip();
#if 0
        // No workspace is opened yet, let codelite process this event normally
        e.Skip();
        clNewProjectEvent::Template phpTemplate;
        phpTemplate.m_category = "PHP";
        phpTemplate.m_categoryPng = "m_bmpElephant";
        phpTemplate.m_template = "PHP Project";
        phpTemplate.m_templatePng = "m_bmpPhpFile";
        phpTemplate.m_debugger = "XDebug";
        phpTemplate.m_toolchain = "PHP Tools";
        phpTemplate.m_allowSeparateFolder = true;
        e.GetTemplates().push_back(phpTemplate);
#endif
    } else {
        // we have a PHP workspace opened - handle it ourself
        NewPHPProjectWizard wiz(EventNotifier::Get()->TopFrame());
        if(wiz.RunWizard(wiz.GetFirstPage())) {
            m_workspaceView->CallAfter(&PHPWorkspaceView::CreateNewProject, wiz.GetCreateData());
        }
    }
}

void PhpPlugin::OnGetFiFMask(clCommandEvent& e)
{
    // always skip this event so other plugins could modify it as well
    e.Skip();
    if(PHPWorkspace::Get()->IsOpen()) {
        // set the default find in files mask
        PHPConfigurationData conf;
        e.SetString(conf.Load().GetFindInFilesMask());
    }
}

void PhpPlugin::DoPlaceMenuBar(wxMenuBar* menuBar)
{
    // Add our menu bar
    wxMenu* phpMenuBarMenu = new wxMenu();
    phpMenuBarMenu->Append(wxID_PHP_SETTINGS, _("Settings..."), _("Settings..."));
    phpMenuBarMenu->Append(
        wxID_PHP_RUN_XDEBUG_DIAGNOSTICS, _("Run XDebug Setup Wizard..."), _("Run XDebug Setup Wizard..."));

    int helpLoc = menuBar->FindMenu(_("Help"));
    if(helpLoc != wxNOT_FOUND) {
        menuBar->Insert(helpLoc, phpMenuBarMenu, _("P&HP"));
    }

    phpMenuBarMenu->Connect(wxID_PHP_SETTINGS,
                            wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(PhpPlugin::OnMenuCommand),
                            NULL,
                            (wxEvtHandler*)this);
    phpMenuBarMenu->Connect(wxID_PHP_RUN_XDEBUG_DIAGNOSTICS,
                            wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(PhpPlugin::OnRunXDebugDiagnostics),
                            NULL,
                            (wxEvtHandler*)this);
}

void PhpPlugin::OnMenuCommand(wxCommandEvent& e)
{
    switch(e.GetId()) {
    case wxID_PHP_SETTINGS: {
        PHPSettingsDlg dlg(FRAME);
        dlg.ShowModal();
    } break;
    default:
        e.Skip();
        break;
    }
}

void PhpPlugin::OnFileSaved(clCommandEvent& e)
{
    e.Skip();

    if(PHPWorkspace::Get()->IsOpen()) {
        // Check to see if we got a remote-upload setup
        SSHWorkspaceSettings settings;
        settings.Load();

        if(settings.IsRemoteUploadSet() && settings.IsRemoteUploadEnabled()) {
            // Post an event to the SFTP plugin and ask it to save our file
            wxFileName fnLocalFile(e.GetString());

            fnLocalFile.MakeRelativeTo(PHPWorkspace::Get()->GetFilename().GetPath());
            fnLocalFile.MakeAbsolute(wxFileName(settings.GetRemoteFolder(), "", wxPATH_UNIX).GetPath());
            wxString remoteFile = fnLocalFile.GetFullPath(wxPATH_UNIX);
            wxString localFile = e.GetString();

            JSONRoot root(cJSON_Object);
            root.toElement().addProperty("account", settings.GetAccount());
            root.toElement().addProperty("local_file", localFile);
            root.toElement().addProperty("remote_file", remoteFile);

            clCommandEvent eventSave(XRCID("wxEVT_SFTP_SAVE_FILE"));
            eventSave.SetString(root.toElement().format());
            EventNotifier::Get()->AddPendingEvent(eventSave);
        }
    }

    // Run php lint
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);
    
    PHPConfigurationData conf;
    conf.Load();
    if(::IsPHPFile(editor) && conf.IsRunLint()) {
        if(m_mgr->GetActiveEditor()) {
            m_mgr->GetActiveEditor()->DelAllCompilerMarkers();
        }
        m_lint->CheckCode(e.GetFileName());
    }
}

void PhpPlugin::OnReloadWorkspace(clCommandEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        m_workspaceView->CallAfter(&PHPWorkspaceView::ReloadWorkspace, false);

    } else {
        e.Skip();
    }
}

void PhpPlugin::OnLoadURL(PHPEvent& e)
{
    e.Skip();
    CL_DEBUG("Loading URL: " + e.GetUrl());
    ::wxLaunchDefaultBrowser(e.GetUrl());
}

// Debugger events
void PhpPlugin::OnDebugEnded(XDebugEvent& e)
{
    e.Skip();

    // Save the layout
    wxFileName fnConfig(clStandardPaths::Get().GetUserDataDir(), "xdebug-perspective");
    fnConfig.AppendDir("config");

    wxFFile fp(fnConfig.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(m_mgr->GetDockingManager()->SavePerspective());
        fp.Close();
    }

    if(!m_savedPerspective.IsEmpty()) {
        m_mgr->GetDockingManager()->LoadPerspective(m_savedPerspective);
        m_savedPerspective.Clear();
    }
}

void PhpPlugin::OnDebugStarted(XDebugEvent& e)
{
    e.Skip();
    DoEnsureXDebugPanesVisible();
}

void PhpPlugin::OnXDebugDeleteAllBreakpoints(clDebugEvent& e)
{
    e.Skip();
    PHPEvent eventDelAllBP(wxEVT_PHP_DELETE_ALL_BREAKPOINTS);
    EventNotifier::Get()->AddPendingEvent(eventDelAllBP);
}

void PhpPlugin::OnXDebugShowBreakpointsWindow(wxCommandEvent& e) { DoEnsureXDebugPanesVisible(_("Breakpoints")); }

void PhpPlugin::DoEnsureXDebugPanesVisible(const wxString& selectWindow)
{
    // Save the current layout to be the normal layout
    m_savedPerspective = m_mgr->GetDockingManager()->SavePerspective();
    m_debuggerPane->SelectTab(selectWindow);

    // If we have an old perspective, load it
    wxFileName fnConfig(clStandardPaths::Get().GetUserDataDir(), "xdebug-perspective");
    fnConfig.AppendDir("config");

    if(fnConfig.Exists()) {
        wxFFile fp(fnConfig.GetFullPath(), "rb");
        if(fp.IsOpened()) {
            wxString perspective;
            fp.ReadAll(&perspective);

            m_mgr->GetDockingManager()->LoadPerspective(perspective, false);
        }
    }

    EnsureAuiPaneIsVisible("XDebug");
    EnsureAuiPaneIsVisible("XDebugEval");
    EnsureAuiPaneIsVisible("XDebugLocals", true);
}

void PhpPlugin::SafelyDetachAndDestroyPane(wxWindow* pane, const wxString& name)
{
    if(pane) {
        wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(name);
        if(pi.IsOk()) {
            m_mgr->GetDockingManager()->DetachPane(pane);
            pane->Destroy();
        }
    }
}

void PhpPlugin::EnsureAuiPaneIsVisible(const wxString& paneName, bool update)
{
    wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(paneName);
    if(pi.IsOk() && !pi.IsShown()) {
        pi.Show();
    }
    if(update) {
        m_mgr->GetDockingManager()->Update();
    }
}

void PhpPlugin::OnNewProjectFinish(clNewProjectEvent& e)
{
    if(e.GetTemplateName() != "PHP Project") {
        e.Skip();
        return;
    }

    if(m_mgr->IsWorkspaceOpen()) {
        ::wxMessageBox(
            _("Can't create PHP project. Close your current workspace first"), "PHP", wxOK | wxICON_ERROR | wxCENTER);
        return;
    }

    if(!PHPWorkspace::Get()->IsOpen()) {
        // No PHP workspace is open, create a new one
        wxFileName workspacePath(e.GetProjectFolder(), e.GetProjectName());
        workspacePath.SetExt(PHPStrings::PHP_WORKSPACE_EXT);
        DoOpenWorkspace(workspacePath.GetFullPath(), true);
    }

    if(PHPWorkspace::Get()->IsOpen()) {
        PHPProject::CreateData cd;
        cd.importFilesUnderPath = true;
        cd.name = e.GetProjectName();
        cd.path = e.GetProjectFolder();
        m_workspaceView->CallAfter(&PHPWorkspaceView::CreateNewProject, cd);
    }
}

void PhpPlugin::OnXDebugSettings(wxCommandEvent& e) {}

void PhpPlugin::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();

    /// If all editors closed event was triggered due to workspace close
    /// show the welcome page

    if(m_showWelcomePage) {
        m_showWelcomePage = false;
        // Show the 'Welcome Page'
        wxCommandEvent eventShowWelcomePage(wxEVT_COMMAND_MENU_SELECTED, XRCID("view_welcome_page"));
        eventShowWelcomePage.SetEventObject(FRAME);
        FRAME->GetEventHandler()->AddPendingEvent(eventShowWelcomePage);
    }
}

void PhpPlugin::SetEditorActive(IEditor* editor) { editor->SetActive(); }

void PhpPlugin::RunXDebugDiagnostics()
{
    PHPXDebugSetupWizard wiz(EventNotifier::Get()->TopFrame());
    if(wiz.RunWizard(wiz.GetFirstPage())) {
        
    }
#if 0
    XDebugTester xdebugTester;
    if(xdebugTester.RunTest()) {
        // Display the result
        wxString html;
        html << "<html><body>";
        html << "<table>";

        html << "<tr valign=\"top\" align=\"left\"><th>What?</th><th>Result</th><th>Description</th></tr>";
        const XDebugTester::ResultMap_t& result = xdebugTester.GetResults();
        XDebugTester::ResultMap_t::const_iterator iter = result.begin();
        for(; iter != result.end(); ++iter) {
            html << "<tr valign=\"top\" align=\"left\">";
            html << "<td>" << iter->first << "</td>";
            html << "<td>" << iter->second.first << "</td>";
            html << "<td>" << iter->second.second << "</td>";
            html << "</tr>";
        }

        html << "</table></body></html>";

        XDebugDiagDlg dlg(EventNotifier::Get()->TopFrame());
        dlg.Load(html);
        dlg.ShowModal();
    }
#endif
}

void PhpPlugin::OnRunXDebugDiagnostics(wxCommandEvent& e)
{
    wxUnusedVar(e);
    RunXDebugDiagnostics();
}

void PhpPlugin::FinalizeStartup()
{
    // Create the debugger windows (hidden)
    m_debuggerPane = new PHPDebugPane(EventNotifier::Get()->TopFrame());
    m_mgr->GetDockingManager()->AddPane(m_debuggerPane,
                                        wxAuiPaneInfo()
                                            .Name("XDebug")
                                            .Caption("Call Stack & Breakpoints")
                                            .Hide()
                                            .CloseButton()
                                            .MaximizeButton()
                                            .Bottom()
                                            .Position(3));

    m_xdebugLocalsView = new LocalsView(EventNotifier::Get()->TopFrame());
    m_mgr->GetDockingManager()->AddPane(
        m_xdebugLocalsView,
        wxAuiPaneInfo().Name("XDebugLocals").Caption("Locals").Hide().CloseButton().MaximizeButton().Bottom());

    m_xdebugEvalPane = new EvalPane(EventNotifier::Get()->TopFrame());
    m_mgr->GetDockingManager()->AddPane(
        m_xdebugEvalPane,
        wxAuiPaneInfo().Name("XDebugEval").Caption("PHP").Hide().CloseButton().MaximizeButton().Bottom().Position(2));

    // Check to see if the have a PHP executable setup
    // if not - update it
    PHPConfigurationData data;
    data.Load();

    PHPLocator locator;
    if(locator.Locate()) {
        // update a PHP executable in the settings
        if(data.GetPhpExe().IsEmpty()) {
            data.SetPhpExe(locator.GetPhpExe().GetFullPath());
        }
        data.Save();
    }
}

void PhpPlugin::OnGoingDown(clCommandEvent& event)
{
    event.Skip();
}

void PhpPlugin::PhpLintDone(const wxString& lintOutput, const wxString& filename)
{
    // Find the editor
    CL_DEBUG("PHPLint: searching editor for file: %s", filename);
    IEditor* editor = m_mgr->FindEditor(filename);
    CHECK_PTR_RET(editor);

    wxRegEx reLine("[ \t]*on line ([0-9]+)");
    wxArrayString lines = ::wxStringTokenize(lintOutput, "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.GetCount(); ++i) {
        wxString errorString = lines.Item(i);
        errorString.Trim().Trim(false);
        if(errorString.Contains("syntax error")) {
            // get the line number
            if(reLine.Matches(errorString)) {
                wxString strLine = reLine.GetMatch(errorString, 1);
                int where = errorString.Find(" in ");
                if(where != wxNOT_FOUND) {
                    errorString.Truncate(where);
                }
                long nLine(wxNOT_FOUND);
                if(strLine.ToCLong(&nLine)) {
                    CL_DEBUG("PHPLint: adding error marker @%d", (int)nLine - 1);
                    editor->SetErrorMarker(nLine - 1, errorString);
                }
            }
        }
    }
}

void PhpPlugin::OnFindInFilesDismissed(clCommandEvent& e)
{
    e.Skip();
    if(PHPWorkspace::Get()->IsOpen()) {
        // store the find in files mask
        PHPConfigurationData conf;
        conf.Load().SetFindInFilesMask(e.GetString()).Save();
    }
}

void PhpPlugin::OnFileSysetmUpdated(clFileSystemEvent& event)
{
    event.Skip();
    if(PHPWorkspace::Get()->IsOpen()) {
        PHPWorkspace::Get()->SyncWithFileSystem();
        m_workspaceView->ReloadWorkspace(true);
    }
}

void PhpPlugin::OnSaveSession(clCommandEvent& event)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        // CodeLite requires us to store the session, do it
        m_mgr->StoreWorkspaceSession(PHPWorkspace::Get()->GetFilename());
    } else {
        event.Skip();
    }
}
