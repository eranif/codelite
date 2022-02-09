#include "main.h"

#include "ColoursAndFontsManager.h"
#include "bitmap_loader.h"
#include "cl_aui_dock_art.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "plugin.h"
#include "windowattrmanager.h"
#include "wxc_aui_tool_stickiness.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_project_metadata.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_defs.h"
#include "wxguicraft_main_view.h"

#include <wx/dcmemory.h>
#include <wx/ffile.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/iconbndl.h>
#include <wx/msgdlg.h>
#include <wx/socket.h>
#include <wx/sysopt.h>
#include <wx/toplevel.h>

#define CHECK_IF_FOCUS_ON_READONLY_STC()    \
    wxStyledTextCtrl* stc = GetActiveSTC(); \
    if(stc) {                               \
        return;                             \
    }

#if STANDALONE_BUILD

static const wxCmdLineEntryDesc cmdLineDesc[] = {
    { wxCMD_LINE_SWITCH, "v", "version", "Print current version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "h", "help", "Print usage", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "s", "server", "Start in server mode (hidden)", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_PARAM, NULL, NULL, "Input file", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

IMPLEMENT_APP(wxcApp)

#ifdef __WXMSW__
typedef BOOL WINAPI (*SetProcessDPIAwareFunc)();
#endif

wxcApp::wxcApp()
    : m_hiddenMainFrame(false)
{
}

wxcApp::~wxcApp() {}

bool wxcApp::OnInit()
{
    // parse command line
    wxCmdLineParser parser;
    parser.SetDesc(cmdLineDesc);
    parser.SetCmdLine(wxAppBase::argc, wxAppBase::argv);
    if(parser.Parse() != 0) {
        return false;
    }

    if(parser.Found("h")) {
        // print usage
        parser.Usage();
        return false;
    }
    m_hiddenMainFrame = parser.Found("s");

    wxSystemOptions::SetOption(_T("msw.remap"), 0);
    wxSystemOptions::SetOption("msw.notebook.themed-background", 0);
    wxXmlResource::Get()->InitAllHandlers();
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxCURHandler);
    wxImage::AddHandler(new wxICOHandler);
    wxImage::AddHandler(new wxXPMHandler);
    wxImage::AddHandler(new wxGIFHandler);
    // InitXmlResource();

    wxLog::EnableLogging(false);

#ifdef __WXGTK__
    // Redirect stdout/error to a file
    wxFileName stdout_err(wxStandardPaths::Get().GetUserDataDir(), "wxcrafter-stdout-stderr.log");
#ifndef NDEBUG
    stdout_err.SetPath(stdout_err.GetPath() + "-dbg");
#endif
    FILE* new_stdout = ::freopen(stdout_err.GetFullPath().mb_str(wxConvISO8859_1).data(), "a+b", stdout);
    FILE* new_stderr = ::freopen(stdout_err.GetFullPath().mb_str(wxConvISO8859_1).data(), "a+b", stderr);
    wxUnusedVar(new_stderr);
    wxUnusedVar(new_stdout);
#endif

#ifdef __WXGTK__
    wxString installPrefix = INSTALL_PREFIX;
    wxStandardPaths::Get().SetInstallPrefix(installPrefix);
    EditorConfigST::Get()->SetInstallDir(wxString() << INSTALL_PREFIX << "/share/wxcrafter");

#elif defined(__WXMSW__)
    HINSTANCE m_user32Dll = LoadLibrary(L"User32.dll");
    if(m_user32Dll) {
        SetProcessDPIAwareFunc pFunc = (SetProcessDPIAwareFunc)GetProcAddress(m_user32Dll, "SetProcessDPIAware");
        if(pFunc) {
            pFunc();
        }
        FreeLibrary(m_user32Dll);
        m_user32Dll = NULL;
    }

    EditorConfigST::Get()->SetInstallDir(wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath());
#endif
    EditorConfigST::Get()->Init("", "2.0.2");
    EditorConfigST::Get()->Load();

    for(size_t i = 0; i < parser.GetParamCount(); i++) {
        wxString argument = parser.GetParam(i);

        // convert to full path and open it
        wxFileName fn(argument);
        fn.MakeAbsolute(wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath());

        if(fn.GetExt() == "wxcp") {
            wxCommandEvent evtOpen(wxEVT_WXC_OPEN_PROJECT);
            evtOpen.SetString(fn.GetFullPath());
            EventNotifier::Get()->AddPendingEvent(evtOpen);
        }
    }

    // initialize the socket library
    wxSocketBase::Initialize();

    // Initialize the edit manager
    wxcEditManager::Get();

    // Initialize the colours and font manager
    ColoursAndFontsManager::Get().Load();
    ColoursAndFontsManager::Get().RestoreDefaults();

    // Open log file
    FileLogger::OpenLog("wxcrafter.log", FileLogger::Dbg);
    clDEBUG() << "wxCrafter started" << endl;

    m_wxcPlugin = new wxCrafterPlugin(NULL, false);
    SetTopWindow(m_wxcPlugin->GetMainFrame());
    if(false) {
        // Don't show the top window
        return true;

    } else {
        return GetTopWindow()->Show();
    }
}

int wxcApp::OnExit()
{
    wxDELETE(m_wxcPlugin);
    return TRUE;
}

bool wxcApp::OnCmdLineParsed(wxCmdLineParser& parser) { return true; }

#endif

// --------------------------------------------------------------------------------------------------
// Main Frame
// --------------------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_FIND_CLOSE(wxID_ANY, MainFrame::OnFindDlgClose)
EVT_FIND(wxID_ANY, MainFrame::OnFindFirst)
EVT_FIND_NEXT(wxID_ANY, MainFrame::OnFindNext)
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent, bool hidden)
    : MainFrameBase(parent, wxID_ANY, "wxCrafter", wxDefaultPosition, wxDefaultSize,
                    wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL)
    , m_wxcView(NULL)
    , m_treeView(NULL)
    , m_findReplaceDialog(NULL)
    , m_exiting(false)
{
    m_mainToolbar->SetMiniToolBar(false);
    auto images = m_mainToolbar->GetBitmapsCreateIfNeeded();

#if !STANDALONE_BUILD
    m_mainToolbar->AddTool(wxID_BACKWARD, _("Back to CodeLite"), images->Add("back"));
#endif
    m_mainToolbar->AddTool(wxID_NEW, _("New Project"), images->Add("file_new"));
    m_mainToolbar->AddTool(wxID_OPEN, _("Open Project"), images->Add("file_open"), "", wxITEM_DROPDOWN);
    m_mainToolbar->AddTool(wxID_CLOSE, _("Close"), images->Add("file_close"));
    m_mainToolbar->AddTool(wxID_SAVE, _("Save"), images->Add("file_save"));
    m_mainToolbar->AddSeparator();
    m_mainToolbar->AddTool(wxID_UNDO, _("Undo"), images->Add("undo"));
    m_mainToolbar->AddTool(wxID_REDO, _("Redo"), images->Add("redo"));
    m_mainToolbar->AddSeparator();
    m_mainToolbar->AddTool(wxID_FIND, _("Find"), images->Add("find"));
    m_mainToolbar->AddSeparator();
    m_mainToolbar->AddTool(XRCID("generate-code"), _("Generate Code"), images->Add("execute"));
    m_mainToolbar->Realize();

#if !STANDALONE_BUILD
    m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnSwitchToCodelite, this, wxID_BACKWARD);
    m_mainToolbar->Bind(wxEVT_UPDATE_UI, &MainFrame::OnSwitchToCodeliteUI, this, wxID_BACKWARD);
#endif

    m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnNewProject, this, wxID_NEW);
    m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnOpen, this, wxID_OPEN);
    m_mainToolbar->Bind(wxEVT_TOOL_DROPDOWN, &MainFrame::OnOpenMenu, this, wxID_OPEN);
    m_mainToolbar->Bind(wxEVT_UPDATE_UI, &MainFrame::OnOpenUI, this, wxID_OPEN);

    m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnClose, this, wxID_CLOSE);
    m_mainToolbar->Bind(wxEVT_UPDATE_UI, &MainFrame::OnCloseUI, this, wxID_CLOSE);

    m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnSave, this, wxID_SAVE);
    m_mainToolbar->Bind(wxEVT_UPDATE_UI, &MainFrame::OnSaveUI, this, wxID_SAVE);

    m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnUndo, this, wxID_UNDO);
    m_mainToolbar->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUndoUI, this, wxID_UNDO);

    m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnRedo, this, wxID_REDO);
    m_mainToolbar->Bind(wxEVT_UPDATE_UI, &MainFrame::OnRedoUI, this, wxID_REDO);

    m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnOpenFindDialog, this, wxID_FIND);
    m_mainToolbar->Bind(wxEVT_UPDATE_UI, &MainFrame::OnFindUI, this, wxID_FIND);

    m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnGenerateCode, this, XRCID("generate-code"));
    m_mainToolbar->Bind(wxEVT_UPDATE_UI, &MainFrame::OnGenerateCodeUI, this, XRCID("generate-code"));

    wxIconBundle icons;
    wxCrafter::ResourceLoader rl;

    {
        wxBitmap bmp = rl.Bitmap("wxc-logo-16");
        wxIcon icn;
        icn.CopyFromBitmap(bmp);
        icons.AddIcon(icn);
    }

    {
        wxBitmap bmp = rl.Bitmap("wxc-logo-32");
        wxIcon icn;
        icn.CopyFromBitmap(bmp);
        icons.AddIcon(icn);
    }

    {
        wxBitmap bmp = rl.Bitmap("wxc-logo-64");
        wxIcon icn;
        icn.CopyFromBitmap(bmp);
        icons.AddIcon(icn);
    }

    {
        wxBitmap bmp = rl.Bitmap("wxc-logo-128");
        wxIcon icn;
        icn.CopyFromBitmap(bmp);
        icons.AddIcon(icn);
    }

    SetIcons(icons);

    EventNotifier::Get()->Connect(wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED,
                                  wxCommandEventHandler(MainFrame::OnProjectModified), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED,
                                  wxCommandEventHandler(MainFrame::OnProjectSynched), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WXC_PROJECT_LOADED, wxCommandEventHandler(MainFrame::OnProjectLoaded), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WXC_CLOSE_PROJECT, wxCommandEventHandler(MainFrame::OnProjectClosed), NULL,
                                  this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &MainFrame::OnWorkspaceClosed, this);
    EventNotifier::Get()->Connect(wxEVT_CODELITE_MAINFRAME_GOT_FOCUS,
                                  wxCommandEventHandler(MainFrame::OnCodeLiteGotFocus), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WXC_CODE_PREVIEW_PAGE_CHANGED,
                                  wxCommandEventHandler(MainFrame::OnCodeEditorSelected), NULL, this);

#if !STANDALONE_BUILD
    Hide();
    SetCanFocus(true);
    SetName("MainFrame");
    WindowAttrManager::Load(this);
#else
    SetName("MainFrame");
    WindowAttrManager::Load(this);
#endif
}

MainFrame::~MainFrame()
{
    EventNotifier::Get()->Disconnect(wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED,
                                     wxCommandEventHandler(MainFrame::OnProjectModified), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED,
                                     wxCommandEventHandler(MainFrame::OnProjectSynched), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WXC_PROJECT_LOADED, wxCommandEventHandler(MainFrame::OnProjectLoaded), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_WXC_CLOSE_PROJECT, wxCommandEventHandler(MainFrame::OnProjectClosed), NULL,
                                     this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &MainFrame::OnWorkspaceClosed, this);
    EventNotifier::Get()->Disconnect(wxEVT_CODELITE_MAINFRAME_GOT_FOCUS,
                                     wxCommandEventHandler(MainFrame::OnCodeLiteGotFocus), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WXC_CODE_PREVIEW_PAGE_CHANGED,
                                     wxCommandEventHandler(MainFrame::OnCodeEditorSelected), NULL, this);

#if STANDALONE_BUILD
    if(m_findReplaceDialog) {
        m_findReplaceDialog->Destroy();
        m_findReplaceDialog = NULL;
    }
#endif
}

void MainFrame::OnCloseFrame(wxCloseEvent& event)
{
#if STANDALONE_BUILD
#ifndef __WXMSW__
    // We support task bar icon on Windows only
    event.Skip();
    wxcCodeGeneratorHelper::Get().UnInitialize();
#else
    /// In a standalone mode, close the frame
    event.Skip();
    wxcCodeGeneratorHelper::Get().UnInitialize();
#endif

#else
    wxUnusedVar(event);
    HideDesigner();
#endif
}

void MainFrame::Add(GUICraftMainPanel* view)
{
    m_wxcView = view;
    m_splitterPageDesigner->GetSizer()->Add(view, 1, wxEXPAND);
    m_splitterPageDesigner->GetSizer()->Layout();
}

void MainFrame::Add(wxcTreeView* tree)
{
    m_treeView = tree;
    m_splitterPageTreeView->GetSizer()->Add(tree, 1, wxEXPAND);
    m_splitterPageTreeView->GetSizer()->Layout();
}

void MainFrame::OnClose(wxCommandEvent& event)
{
    wxCommandEvent evtClose(wxEVT_WXC_CLOSE_PROJECT);
    EventNotifier::Get()->AddPendingEvent(evtClose);
}

void MainFrame::OnSave(wxCommandEvent& event) { m_treeView->SaveProject(); }

void MainFrame::OnProjectModified(wxCommandEvent& e)
{
    e.Skip();
    wxString title = GetTitle();
    if(!title.StartsWith("*")) {
        title.Prepend("*");
        SetTitle(title);
    }
}

void MainFrame::OnProjectSynched(wxCommandEvent& e)
{
    e.Skip();
    wxString title = GetTitle();
    if(title.StartsWith("*")) {
        title.Remove(0, 1);
        SetTitle(title);
    }
}

void MainFrame::OnProjectLoaded(wxCommandEvent& e)
{
    e.Skip();
    SetTitle("wxCrafter - " + e.GetString());
}

void MainFrame::OnDeleteItem(wxCommandEvent& event)
{
    wxTextCtrl* text = GetActiveTextCtrl();
    CHECK_IF_FOCUS_ON_READONLY_STC();

    if(text) {
        event.StopPropagation();

        long from, to;
        text->GetSelection(&from, &to);
        text->Replace(from, to, "");

    } else {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_DELETE_NODE);
        m_wxcView->GetEventHandler()->AddPendingEvent(evt);
    }
}

void MainFrame::OnDeleteItemUI(wxUpdateUIEvent& event)
{
    GUICraftItemData* itemData = m_wxcView->GetSelItemData();
    event.Enable(itemData);
}

void MainFrame::OnPreview(wxCommandEvent& event)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_SHOW_PREVIEW);
    m_wxcView->GetEventHandler()->AddPendingEvent(evt);
}

void MainFrame::OnPreviewUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_wxcView->IsPreviewAlive() && wxcProjectMetadata::Get().IsLoaded());
}

void MainFrame::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    SetTitle("wxCrafter");
}

void MainFrame::OnCopy(wxCommandEvent& event)
{
    wxTextCtrl* text = GetActiveTextCtrl();
    wxStyledTextCtrl* stc = GetActiveSTC();
    if(text) {
        event.StopPropagation();
        if(text->CanCopy()) {
            text->Copy();
        }

    } else if(stc) {
        event.StopPropagation();
        if(stc->CanCopy()) {
            stc->Copy();
        }

    } else {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_COPY);
        m_wxcView->GetEventHandler()->AddPendingEvent(evt);
    }
}

void MainFrame::OnCut(wxCommandEvent& event)
{
    CHECK_IF_FOCUS_ON_READONLY_STC();
    wxTextCtrl* text = GetActiveTextCtrl();

    if(text) {

        event.StopPropagation();
        if(text->CanCut()) {
            text->Cut();
        }
    } else {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_CUT);
        m_wxcView->GetEventHandler()->AddPendingEvent(evt);
    }
}

void MainFrame::OnPaste(wxCommandEvent& event)
{
    CHECK_IF_FOCUS_ON_READONLY_STC();

    wxTextCtrl* text = GetActiveTextCtrl();
    if(text) {
        event.StopPropagation();
        if(text->CanPaste()) {
            text->Paste();
        }

    } else {

        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_PASTE);
        m_wxcView->GetEventHandler()->AddPendingEvent(evt);
    }
}

void MainFrame::OnRedo(wxCommandEvent& event)
{
    CHECK_IF_FOCUS_ON_READONLY_STC();
    wxTextCtrl* text = GetActiveTextCtrl();
    if(text) {
        event.StopPropagation();
        if(text->CanRedo()) {
            text->Redo();
        }

    } else {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REDO);
        wxTheApp->AddPendingEvent(evt);
    }
}

void MainFrame::OnRedoUI(wxUpdateUIEvent& event)
{
    wxTextCtrl* text = GetActiveTextCtrl();
    if(text) {
        event.Enable(text->CanRedo());

    } else {
        event.Enable(wxcEditManager::Get().CanRedo());
    }
}

void MainFrame::OnSelectionUI(wxUpdateUIEvent& event)
{
    GUICraftItemData* itemData = m_wxcView->GetSelItemData();
    event.Enable(itemData && itemData->m_wxcWidget);
}

void MainFrame::OnUndo(wxCommandEvent& event)
{
    CHECK_IF_FOCUS_ON_READONLY_STC();
    wxTextCtrl* text = GetActiveTextCtrl();
    if(text) {
        event.StopPropagation();
        if(text->CanUndo()) {
            text->Undo();
        }

    } else {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_UNDO);
        wxTheApp->AddPendingEvent(evt);
    }
}

void MainFrame::OnUndoUI(wxUpdateUIEvent& event)
{
    wxTextCtrl* text = GetActiveTextCtrl();
    if(text) {
        event.Enable(text->CanUndo());
    } else {
        event.Enable(wxcEditManager::Get().CanUndo());
    }
}

void MainFrame::OnBuild(wxCommandEvent& event)
{
    wxCommandEvent evtBuild(wxEVT_COMMAND_MENU_SELECTED, XRCID("build_active_project"));
    evtBuild.SetEventObject(this);
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evtBuild);

    // Also, notify to self to switch codelite
    wxCommandEvent evtSwitch(wxEVT_COMMAND_MENU_SELECTED, wxID_BACKWARD);
    evtSwitch.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(evtSwitch);
}

void MainFrame::OnBuildUI(wxUpdateUIEvent& event)
{
#if STANDALONE_BUILD
    event.Enable(false);
#endif
}

void MainFrame::OnSwitchToCodelite(wxCommandEvent& event)
{
    wxUnusedVar(event);
    MinimizeDesigner();
}

void MainFrame::OnRename(wxCommandEvent& event)
{
    wxCommandEvent evtRename(wxEVT_COMMAND_MENU_SELECTED, ID_RENAME);
    m_wxcView->GetEventHandler()->AddPendingEvent(evtRename);
}

wxTextCtrl* MainFrame::GetActiveTextCtrl()
{
    wxWindow* focusWin = wxWindow::FindFocus();
    if(focusWin) {
        return dynamic_cast<wxTextCtrl*>(focusWin);
    }
    return NULL;
}
void MainFrame::OnImportFB(wxCommandEvent& event)
{
    wxCommandEvent evtImportFB(wxEVT_COMMAND_MENU_SELECTED, XRCID("import_wxFB_project"));
    wxTheApp->AddPendingEvent(evtImportFB);
}

void MainFrame::OnImportSmith(wxCommandEvent& event)
{
    wxCommandEvent evtImportFB(wxEVT_COMMAND_MENU_SELECTED, XRCID("import_wxSmith_project"));
    wxTheApp->AddPendingEvent(evtImportFB);
}

void MainFrame::OnImportXRC(wxCommandEvent& event)
{
    wxCommandEvent evtImportFB(wxEVT_COMMAND_MENU_SELECTED, XRCID("import_XRC_project"));
    wxTheApp->AddPendingEvent(evtImportFB);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent evtAbout(wxEVT_COMMAND_MENU_SELECTED, XRCID("wxcp_about"));
    wxTheApp->AddPendingEvent(evtAbout);
}

void MainFrame::OnHide(wxCommandEvent& event)
{
    wxUnusedVar(event);
#if STANDALONE_BUILD
    m_exiting = true;
    Close();
#else
    HideDesigner();
#endif
}

void MainFrame::OnSettings(wxCommandEvent& event)
{
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, event.GetId());
    wxTheApp->AddPendingEvent(e);
}

void MainFrame::DisplayDesigner()
{
    if(!IsShown()) {
        Show();
    }
    if(IsIconized()) {
        Restore();
    }
    Raise();
    SetFocus();
}

void MainFrame::MinimizeDesigner()
{
    if(IsShown() && !IsIconized()) {
        // minimize to the task bar
        this->Iconize();

        wxFrame* mainFrame = EventNotifier::Get()->TopFrame();
        if(mainFrame) {
            mainFrame->Raise();
        }
    }
}

void MainFrame::HideDesigner()
{
    if(IsShown()) {
        // hide the designer
        this->Hide();

        wxFrame* mainFrame = EventNotifier::Get()->TopFrame();
        if(mainFrame) {
            mainFrame->Raise();
        }
    }
}

void MainFrame::OnProjectClosed(wxCommandEvent& event)
{
    event.Skip();
    SetTitle("wxCrafter");
}

void MainFrame::OnSwitchToCodeliteUI(wxUpdateUIEvent& event)
{
#if STANDALONE_BUILD
    event.Enable(false);
#else
    event.Enable(true);
#endif
}

void MainFrame::OnCodeLiteGotFocus(wxCommandEvent& e) { e.Skip(); }

void MainFrame::OnPasteUI(wxUpdateUIEvent& event) { event.Enable(true); }

void MainFrame::OnSaveUI(wxUpdateUIEvent& event) { event.Enable(wxcEditManager::Get().IsDirty()); }

void MainFrame::OnCloseUI(wxUpdateUIEvent& event) { event.Enable(wxcProjectMetadata::Get().IsLoaded()); }

void MainFrame::OnOpenUI(wxUpdateUIEvent& event) { event.Enable(true); }

void MainFrame::OnNewProject(wxCommandEvent& event)
{
    static size_t Counter = 0;
    wxString title;
    title << "UntitledProject" << ++Counter << ".wxcp";

    wxString wxcpFile = ::wxFileSelector(_("Create an empty wxCrafter project"), wxEmptyString, title, wxEmptyString,
                                         "wxCrafter Project (*.wxcp)|*.wxcp", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(wxcpFile.IsEmpty()) {
        return;
    }

    wxFFile fp(wxcpFile, "w+b");
    if(fp.IsOpened()) {
        fp.Close();

    } else {
        wxString msg;
        msg << _("Could not create resource file '") << wxcpFile << "'";
        ::wxMessageBox(msg, _("wxCrafter"), wxOK | wxICON_WARNING | wxCENTRE);
        return;
    }

    wxCommandEvent evtOpen(wxEVT_WXC_OPEN_PROJECT);
    evtOpen.SetString(wxcpFile);
    EventNotifier::Get()->ProcessEvent(evtOpen);
}

#define ID_RECENT_DOC_FIRST 2000

void MainFrame::OnOpen(wxCommandEvent& event) { DoOpenWxcpProject(); }

void MainFrame::OnOpenMenu(wxCommandEvent& event)
{
    wxMenu menu;
    wxArrayString history;
    DoCreateRecentMenu(menu, history);

    int selection = m_mainToolbar->GetMenuSelectionFromUser(event.GetId(), &menu);
    if(selection == wxID_NONE) {
        /// user cancelled
        return;
    }

    size_t idx = selection - ID_RECENT_DOC_FIRST;
    wxString file_name = history.Item(idx);
    wxFileName fn(file_name);

    wxCommandEvent evtClose(wxEVT_WXC_CLOSE_PROJECT);
    EventNotifier::Get()->ProcessEvent(evtClose);

    wxCommandEvent evtOpen(wxEVT_WXC_OPEN_PROJECT);
    evtOpen.SetString(fn.GetFullPath());
    EventNotifier::Get()->ProcessEvent(evtOpen);
}

void MainFrame::OnDeleteCustomControl(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("delete_custom_controls"));
    wxTheApp->AddPendingEvent(e);
}
void MainFrame::OnEditCustomControl(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("edit_custom_controls"));
    wxTheApp->AddPendingEvent(e);
}

void MainFrame::OnNewCustomControl(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("define_custom_controls"));
    wxTheApp->AddPendingEvent(e);
}

wxStyledTextCtrl* MainFrame::GetActiveSTC()
{
    wxWindow* focusWin = wxWindow::FindFocus();
    if(focusWin) {
        return dynamic_cast<wxStyledTextCtrl*>(focusWin);
    }
    return NULL;
}

void MainFrame::OnGenerateCode(wxCommandEvent& event)
{
    wxCommandEvent evtGenCode(wxEVT_WXC_CMD_GENERATE_CODE);
    EventNotifier::Get()->ProcessEvent(evtGenCode);
}

void MainFrame::OnGenerateCodeUI(wxUpdateUIEvent& event) { event.Enable(wxcProjectMetadata::Get().IsLoaded()); }

void MainFrame::OnOpenFindDialog(wxCommandEvent& event)
{
#if STANDALONE_BUILD
    if(m_findReplaceDialog) {
        m_findReplaceDialog->Raise();
        return;
    }

    wxStyledTextCtrl* stc = m_wxcView->GetPreviewEditor();
    if(stc) {
        m_findReplaceDialog = new wxFindReplaceDialog(stc, &m_findData, _("Find"), wxFR_NOUPDOWN);
        m_findReplaceDialog->Show();
    }
#endif
}

void MainFrame::OnCodeEditorSelected(wxCommandEvent& e)
{
    e.Skip();
#if STANDALONE_BUILD
    bool needToDisplayAgain = false;
    // if the dialog was show, close it
    if(m_findReplaceDialog) {
        m_findReplaceDialog->Destroy();
        m_findReplaceDialog = NULL;
        needToDisplayAgain = true;
    }

    if(needToDisplayAgain && e.GetClientData()) {
        // Reshow the dialog, this time for the proper parent
        wxStyledTextCtrl* parent = reinterpret_cast<wxStyledTextCtrl*>(e.GetClientData());
        if(parent) {
            m_findReplaceDialog = new wxFindReplaceDialog(parent, &m_findData, _("Find"), wxFR_NOUPDOWN);
            m_findReplaceDialog->Show();
        }
    }
#endif
}

void MainFrame::OnFindDlgClose(wxFindDialogEvent& event)
{
#if STANDALONE_BUILD
    m_findReplaceDialog = NULL;
#endif
}

void MainFrame::OnFindFirst(wxFindDialogEvent& event)
{
#if STANDALONE_BUILD
    wxStyledTextCtrl* stc = dynamic_cast<wxStyledTextCtrl*>(m_findReplaceDialog->GetParent());
    if(stc) {
        int curpos = stc->GetCurrentPos();
        if(!DoFindText(stc, *(m_findReplaceDialog->GetData()), false)) {
            // Try from start
            stc->SetCurrentPos(0);
            if(!DoFindText(stc, *(m_findReplaceDialog->GetData()), false)) {
                // Could not find from the start as well, restore the caret position
                stc->SetCurrentPos(curpos);
                stc->EnsureCaretVisible();
                ::wxMessageBox(wxString()
                                   << "Could not find string: " << m_findReplaceDialog->GetData()->GetFindString(),
                               "wxCrafter", wxOK | wxICON_WARNING | wxOK_DEFAULT | wxCENTER, this);
                m_findReplaceDialog->Raise();
            }
        }
    }
#endif
}

void MainFrame::OnFindNext(wxFindDialogEvent& event)
{
#if STANDALONE_BUILD
    wxStyledTextCtrl* stc = dynamic_cast<wxStyledTextCtrl*>(m_findReplaceDialog->GetParent());
    if(stc) {
        int curpos = stc->GetCurrentPos();
        if(!DoFindText(stc, *(m_findReplaceDialog->GetData()), true)) {
            // Try from start
            stc->SetCurrentPos(0);
            if(!DoFindText(stc, *(m_findReplaceDialog->GetData()), false)) {
                // Could not find from the start as well, restore the caret position
                stc->SetCurrentPos(curpos);
                stc->EnsureCaretVisible();
            }
        }
    }
#endif
}

bool MainFrame::DoFindText(wxStyledTextCtrl* stc, const wxFindReplaceData& frd, bool findNext)
{
    int endPos = stc->GetLastPosition();
    int startPos;

    if(findNext) {
        wxString selectedText = stc->GetSelectedText();
        if(selectedText == frd.GetFindString()) {
            startPos = stc->GetSelectionEnd();
        } else {
            startPos = stc->GetCurrentPos();
        }

    } else {
        startPos = stc->GetCurrentPos();
    }

    int flags = frd.GetFlags();
    int stcSearchFlags = 0;

    if(flags & wxFR_MATCHCASE) {
        stcSearchFlags |= wxSTC_FIND_MATCHCASE;
    }

    if(flags & wxFR_WHOLEWORD) {
        stcSearchFlags |= wxSTC_FIND_WHOLEWORD;
    }

    int where = stc->FindText(startPos, endPos, frd.GetFindString(), stcSearchFlags);
    if(where != wxNOT_FOUND) {
        stc->SelectNone();
        stc->SetSelection(where, where + frd.GetFindString().length());
        stc->EnsureCaretVisible();
        return true;

    } else {
        return false;
    }
}

void MainFrame::OnFindUI(wxUpdateUIEvent& event)
{
    wxStyledTextCtrl* stc = m_wxcView->GetPreviewEditor();
    event.Enable(stc != NULL);
}

void MainFrame::EnsureVisibile() { DisplayDesigner(); }

void MainFrame::OnBatchGenerateCode(wxCommandEvent& event)
{
    wxCommandEvent evtGenCode(wxEVT_WXC_CMD_BATCH_GENERATE_CODE);
    EventNotifier::Get()->ProcessEvent(evtGenCode);
}

void MainFrame::OnBatchGenerateCodeUI(wxUpdateUIEvent& event)
{
#ifdef STANDALONE_BUILD
    event.Enable(!wxcProjectMetadata::Get().IsLoaded());
#else
    event.Enable(false);
#endif
}

void MainFrame::DoOpenWxcpProject()
{
    wxString file_name = ::wxFileSelector(_("Open a wxCrafter file"), wxEmptyString, wxEmptyString, wxEmptyString,
                                          "wxCrafter Project (*.wxcp)|*.wxcp");

    if(!file_name.IsEmpty()) {
        wxFileName fn(file_name);
        wxCommandEvent evtOpen(wxEVT_WXC_OPEN_PROJECT);
        evtOpen.SetString(fn.GetFullPath());
        EventNotifier::Get()->ProcessEvent(evtOpen);
    }
}

void MainFrame::OnFileOpen(wxCommandEvent& event) { DoOpenWxcpProject(); }

void MainFrame::DoCreateRecentMenu(wxMenu& menu, wxArrayString& history)
{
    history = wxcSettings::Get().GetHistory();
    if(history.IsEmpty()) {
        return;
    }

    wxArrayString tmpHistory;
    for(size_t i = 0; i < history.GetCount(); ++i) {
        if(wxFileName(history.Item(i)).Exists()) {
            tmpHistory.Add(history.Item(i));
        }
    }

    history.swap(tmpHistory);
    history.Sort();

    // Update the history
    wxcSettings::Get().SetHistory(history);
    wxcSettings::Get().Save();

    for(size_t i = 0; i < history.GetCount(); ++i) {
        menu.Append(ID_RECENT_DOC_FIRST + i, history.Item(i));
    }
}

void MainFrame::SetStatusMessage(const wxString& message)
{
    if(GetStatusBar()) {
        GetStatusBar()->SetStatusText(message, 0);
    }
}
