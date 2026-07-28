#include "mainFrame.h"

#include "Importer/importer_from_wxFB.h"
#include "Importer/importer_from_wxSmith.h"
#include "Importer/importer_from_xrc.h"
#include "MyComboBoxXmlHandler.h"
#include "MyRearrangeListXmlHandler.h"
#include "UI/AboutDlg.h"
#include "UI/DefineCustomControlWizard.h"
#include "UI/DeleteCustomControlDlg.h"
#include "UI/EditCustomControlDlg.h"
#include "UI/wxcSettingsDlg.h"
#include "UI/wxcTreeView.h"
#include "UI/wxguicraft_main_view.h"
#include "allocator_mgr.h"
#include "bitmap_loader.h"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "controls/Containers/wizard_page_wrapper.h"
#include "ctags_manager.h"
#include "event_notifier.h"
#include "functions_parser.h"
#include "imanager.h"
#include "myxh_auimgr.h"
#include "myxh_auitoolb.h"
#include "myxh_cmdlinkbtn.h"
#include "myxh_dataview.h"
#include "myxh_dlg.h"
#include "myxh_dvlistctrl.h"
#include "myxh_dvtreectrl.h"
#include "myxh_frame.h"
#include "myxh_glcanvas.h"
#include "myxh_grid.h"
#include "myxh_infobar.h"
#include "myxh_listc.h"
#include "myxh_mediactrl.h"
#include "myxh_panel.h"
#include "myxh_propgrid.h"
#include "myxh_ribbon.h"
#include "myxh_richtext.h"
#include "myxh_searchctrl.h"
#include "myxh_simplebook.h"
#include "myxh_stc.h"
#include "myxh_textctrl.h"
#include "myxh_toolbk.h"
#include "myxh_treebk.h"
#include "myxh_treelist.h"
#include "myxh_webview.h"
#include "windowattrmanager.h"
#include "workspace.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_project_metadata.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_defs.h"
#include "wxgui_globals.h"
#include "wxgui_helpers.h"

#include <wx/ffile.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/iconbndl.h>
#include <wx/msgdlg.h>
#include <wx/toplevel.h>

#define CHECK_IF_FOCUS_ON_READONLY_STC()    \
    wxStyledTextCtrl* stc = GetActiveSTC(); \
    if (stc) {                              \
        return;                             \
    }

namespace
{
void InitializeAllImageHandlers()
{
    // Initialize all image handlers known to us (that aren't already loaded)
    if (wxImage::FindHandler(wxBITMAP_TYPE_PNG) == 0) {
        wxImage::AddHandler(new wxPNGHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_JPEG) == 0) {
        wxImage::AddHandler(new wxJPEGHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_GIF) == 0) {
        wxImage::AddHandler(new wxGIFHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_BMP) == 0) {
        wxImage::AddHandler(new wxBMPHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_ICO) == 0) {
        wxImage::AddHandler(new wxICOHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_ANI) == 0) {
        wxImage::AddHandler(new wxANIHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_CUR) == 0) {
        wxImage::AddHandler(new wxCURHandler);
    }
}

void AddMyXmlHandlers()
{
    wxXmlResource::Get()->ClearHandlers();

    // +++++++++++---------------------------------------------
    // Custom XML resource handlers
    // +++++++++++---------------------------------------------

    wxXmlResource::Get()->AddHandler(new MYwxTreebookXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxRichTextCtrlXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxGridXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxSearchCtrlXmlHandler);
    wxXmlResource::Get()->AddHandler(new MYwxToolbookXmlHandler);
    wxXmlResource::Get()->AddHandler(new MYwxListCtrlXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxSimplebookXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxAuiToolBarXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxStcXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxDataViewListCtrlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxDataViewTreeCtrlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxDataViewCtrlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxInfoBarCtrlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxWebViewXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxGLCanvasXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxMediaCtrlXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxMyFrameXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxPanelXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxAuiManagerXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxDialogXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxPropGridXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxRibbonXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxCommandLinkButtonXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyTreeListCtrl);
    wxXmlResource::Get()->AddHandler(new MyTextCtrlXrcHandler);
    wxXmlResource::Get()->AddHandler(new MyComboBoxXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyRearrangeListXmlHandler);
    wxXmlResource::Get()->InitAllHandlers();
}


void DoWriteFileContent(const wxFileName& fn, const wxString& content, IEditor* editor)
{
    if (editor) {
        editor->GetCtrl()->SetText(content);
    } else {
        // Write the file content to the disk
        wxCrafter::WriteFile(fn, content, true);
    }
}
void FormatString(wxString& content, const wxFileName& filename)
{
    clSourceFormatEvent event{wxEVT_FORMAT_STRING};
    event.SetFileName(filename.GetFullPath());
    event.SetInputString(content);
    EventNotifier::Get()->ProcessEvent(event);
    if (!event.GetFormattedString().IsEmpty()) {
        content = event.GetFormattedString();
    }
}
wxStringSet_t GetProjectFiles(const wxString& projectName)
{
    ProjectPtr p = clCxxWorkspaceST::Get()->GetProject(projectName);
    if (!p) {
        return {};
    }
    const Project::FilesMap_t& filesMap = p->GetFiles();
    wxStringSet_t files;
    files.reserve(filesMap.size());
    for (const auto& [filename, _] : filesMap) {
        files.insert(filename);
    }
    return files;
}
void NotifyFileSaved(const wxFileName& fn)
{
    EventNotifier::Get()->PostFileSavedEvent(fn.GetFullPath());
}
} // namespace

// --------------------------------------------------------------------------------------------------
// Main Frame
// --------------------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_FIND_CLOSE(wxID_ANY, MainFrame::OnFindDlgClose)
EVT_FIND(wxID_ANY, MainFrame::OnFindFirst)
EVT_FIND_NEXT(wxID_ANY, MainFrame::OnFindNext)
END_EVENT_TABLE()

#ifdef __WXMAC__
const wxSize BMP_SIZE{32, 32};
#else
const wxSize BMP_SIZE{16, 16};
#endif

MainFrame::MainFrame(wxWindow* parent, bool hidden, IManager* manager)
    : MainFrameBase(parent,
                    wxID_ANY,
                    "wxCrafter",
                    wxDefaultPosition,
                    wxDefaultSize,
                    wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT | wxTAB_TRAVERSAL)
    , m_mgr(manager)
    , m_wxcView(nullptr)
    , m_treeView(nullptr)
    , m_findReplaceDialog(nullptr)
{
    InitializeAllImageHandlers();
    AddMyXmlHandlers();

    if (m_mgr) {
        // Use CodeLite's images.
        auto images = m_mgr->GetStdIcons();
        m_mainToolbar->SetToolBitmapSize(BMP_SIZE);
        m_mainToolbar->AddTool(wxID_BACKWARD, _("Back to CodeLite"), images->GetBundle("back"));
        m_mainToolbar->AddTool(wxID_NEW, _("New Project"), images->GetBundle("file_new"));
        m_mainToolbar->AddTool(wxID_OPEN, _("Open Project"), images->GetBundle("file_open"));
        m_mainToolbar->SetToolDropDown(wxID_OPEN, true);
        m_mainToolbar->AddTool(wxID_CLOSE, _("Close"), images->GetBundle("file_close"));
        m_mainToolbar->AddTool(wxID_SAVE, _("Save"), images->GetBundle("file_save"));
        m_mainToolbar->AddSeparator();
        m_mainToolbar->AddTool(wxID_UNDO, _("Undo"), images->GetBundle("undo"));
        m_mainToolbar->AddTool(wxID_REDO, _("Redo"), images->GetBundle("redo"));
        m_mainToolbar->AddSeparator();
        m_mainToolbar->AddTool(wxID_FIND, _("Find"), images->GetBundle("find"));
        m_mainToolbar->AddSeparator();
        m_mainToolbar->AddTool(XRCID("generate-code"), _("Generate Code"), images->GetBundle("execute"));

        m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnSwitchToCodelite, this, wxID_BACKWARD);
        m_mainToolbar->Bind(wxEVT_UPDATE_UI, &MainFrame::OnSwitchToCodeliteUI, this, wxID_BACKWARD);
    } else {
        // Stand alone build: Use wxWidgets images.
        m_mainToolbar->SetToolBitmapSize(BMP_SIZE);
        m_mainToolbar->AddTool(
            wxID_NEW, _("New Project"), wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR, BMP_SIZE));
        m_mainToolbar->AddTool(
            wxID_OPEN, _("Open Project"), wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR, BMP_SIZE));
        m_mainToolbar->SetToolDropDown(wxID_OPEN, true);
        m_mainToolbar->AddTool(wxID_CLOSE, _("Close"), wxArtProvider::GetBitmap(wxART_CLOSE, wxART_TOOLBAR, BMP_SIZE));
        m_mainToolbar->AddTool(
            wxID_SAVE, _("Save"), wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR, BMP_SIZE));
        m_mainToolbar->AddSeparator();
        m_mainToolbar->AddTool(wxID_UNDO, _("Undo"), wxArtProvider::GetBitmap(wxART_UNDO, wxART_TOOLBAR, BMP_SIZE));
        m_mainToolbar->AddTool(wxID_REDO, _("Redo"), wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR, BMP_SIZE));
        m_mainToolbar->AddSeparator();
        m_mainToolbar->AddTool(wxID_FIND, _("Find"), wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR, BMP_SIZE));
        m_mainToolbar->AddSeparator();
        m_mainToolbar->AddTool(XRCID("generate-code"),
                               _("Generate Code"),
                               wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_TOOLBAR, BMP_SIZE));
    }

    WizardPageWrapper::isActiveWizardPage = [](const wxcWidget* page) {
        if (!GUICraftMainPanel::m_MainPanel) {
            return false;
        }
        return GUICraftMainPanel::m_MainPanel->GetActiveWizardPage() == page;
    };

    wxcWidget::placeHolderImageFullPathGetter = []() {
        wxCrafter::ResourceLoader rl;
        return rl.GetPlaceHolderImagePath().GetFullPath();
    };
    wxcWidget::placeHolder16ImageFullPathGetter = []() {
        wxCrafter::ResourceLoader rl;
        return rl.GetPlaceHolder16ImagePath().GetFullPath();
    };
    m_mainToolbar->Realize();

    m_mainToolbar->Bind(wxEVT_TOOL, &MainFrame::OnNewProject, this, wxID_NEW);
    m_mainToolbar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &MainFrame::OnOpenMenu, this, wxID_OPEN);
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

    EventNotifier::Get()->Connect(
        wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED, wxCommandEventHandler(MainFrame::OnProjectModified), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED, wxCommandEventHandler(MainFrame::OnProjectSynched), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WXC_PROJECT_LOADED, wxCommandEventHandler(MainFrame::OnProjectLoaded), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WXC_CLOSE_PROJECT, wxCommandEventHandler(MainFrame::OnProjectClosed), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &MainFrame::OnWorkspaceClosed, this);
    EventNotifier::Get()->Connect(
        wxEVT_CODELITE_MAINFRAME_GOT_FOCUS, wxCommandEventHandler(MainFrame::OnCodeLiteGotFocus), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WXC_CODE_PREVIEW_PAGE_CHANGED, wxCommandEventHandler(MainFrame::OnCodeEditorSelected), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_NOTIFY_PAGE_CLOSING, wxNotifyEventHandler(MainFrame::OnPageClosing), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BITMAP_CODE_GENERATION_DONE,
                                  wxCommandEventHandler(MainFrame::OnBitmapCodeGenerationCompleted),
                                  nullptr,
                                  this);

    wxTheApp->Connect(
        XRCID("wxc_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSettings), nullptr, this);
    wxTheApp->Connect(XRCID("import_wxFB_project"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(MainFrame::OnImportFB),
                      nullptr,
                      this);
    wxTheApp->Connect(XRCID("import_wxSmith_project"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(MainFrame::OnImportSmith),
                      nullptr,
                      this);
    wxTheApp->Connect(XRCID("import_XRC_project"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(MainFrame::OnImportXRC),
                      nullptr,
                      this);

    if (m_mgr) {
        Hide();
        SetCanFocus(true);
        SetName("MainFrame");
        if (GetParent()) {
            CenterOnParent();
        }
    } else {
        SetName("MainFrame");
        WindowAttrManager::Load(this);
    }

    m_treeView = new wxcTreeView(m_splitterPageTreeView);
    m_splitterPageTreeView->GetSizer()->Add(m_treeView, 1, wxEXPAND);
    m_splitterPageTreeView->GetSizer()->Layout();
    m_wxcView = new GUICraftMainPanel(m_splitterPageDesigner, this, m_treeView->GetTree());
    m_splitterPageDesigner->GetSizer()->Add(m_wxcView, 1, wxEXPAND);
    m_splitterPageDesigner->GetSizer()->Layout();
    Layout();
}

MainFrame::~MainFrame()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED, wxCommandEventHandler(MainFrame::OnProjectModified), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED, wxCommandEventHandler(MainFrame::OnProjectSynched), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WXC_PROJECT_LOADED, wxCommandEventHandler(MainFrame::OnProjectLoaded), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WXC_CLOSE_PROJECT, wxCommandEventHandler(MainFrame::OnProjectClosed), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &MainFrame::OnWorkspaceClosed, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CODELITE_MAINFRAME_GOT_FOCUS, wxCommandEventHandler(MainFrame::OnCodeLiteGotFocus), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WXC_CODE_PREVIEW_PAGE_CHANGED, wxCommandEventHandler(MainFrame::OnCodeEditorSelected), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_NOTIFY_PAGE_CLOSING, wxNotifyEventHandler(MainFrame::OnPageClosing), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_BITMAP_CODE_GENERATION_DONE,
                                     wxCommandEventHandler(MainFrame::OnBitmapCodeGenerationCompleted),
                                     nullptr,
                                     this);

    wxTheApp->Disconnect(XRCID("wxc_settings"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(MainFrame::OnSettings),
                         nullptr,
                         this);
    wxTheApp->Disconnect(XRCID("import_wxFB_project"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(MainFrame::OnImportFB),
                         nullptr,
                         this);
    wxTheApp->Disconnect(XRCID("import_wxSmith_project"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(MainFrame::OnImportSmith),
                         nullptr,
                         this);
    wxTheApp->Disconnect(XRCID("import_XRC_project"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(MainFrame::OnImportXRC),
                         nullptr,
                         this);
    if (!m_mgr && m_findReplaceDialog) {
        m_findReplaceDialog->Destroy();
        m_findReplaceDialog = nullptr;
    }
}

void MainFrame::OnCloseFrame(wxCloseEvent& event)
{
    if (m_mgr) {
        wxUnusedVar(event);
        HideDesigner();
    } else {
#ifndef __WXMSW__
        // We support task bar icon on Windows only
        event.Skip();
        wxcCodeGeneratorHelper::Get().UnInitialize();
#else
        /// In a standalone mode, close the frame
        event.Skip();
        wxcCodeGeneratorHelper::Get().UnInitialize();
#endif
    }
}

void MainFrame::OnClose(wxCommandEvent& event)
{
    wxCommandEvent evtClose(wxEVT_WXC_CLOSE_PROJECT);
    EventNotifier::Get()->AddPendingEvent(evtClose);
}

void MainFrame::OnPageClosing(wxNotifyEvent& e)
{
    const wxWindow* win = reinterpret_cast<wxWindow*>(e.GetClientData());
    if (win && win == GetWxcView()) {
        if (wxcEditManager::Get().IsDirty()) {

            wxString msg;
            msg << _("wxCrafter project is modified\nDo you want to save your changes?");

            const int rc = ::wxMessageBox(msg, _("wxCrafter"), wxYES_NO | wxCANCEL | wxCENTER);
            switch (rc) {
            case wxYES: {
                CloseProject(true);
                e.Skip();
                break;
            }
            case wxNO:
                CloseProject(false);
                e.Skip();
                break;

            case wxCANCEL:
                e.Veto();
                break;
            }
        } else {
            CloseProject(false);
        }

    } else {
        e.Skip();
    }
}

void MainFrame::OnSave(wxCommandEvent& event)
{
    m_treeView->SaveProject();
}

void MainFrame::OnProjectModified(wxCommandEvent& e)
{
    e.Skip();
    wxString title = GetTitle();
    if (!title.StartsWith("*")) {
        title.Prepend("*");
        SetTitle(title);
    }
}

void MainFrame::OnProjectSynched(wxCommandEvent& e)
{
    e.Skip();
    wxString title = GetTitle();
    if (title.StartsWith("*")) {
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

    if (text) {
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
    if (text) {
        event.StopPropagation();
        if (text->CanCopy()) {
            text->Copy();
        }

    } else if (stc) {
        event.StopPropagation();
        if (stc->CanCopy()) {
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

    if (text) {

        event.StopPropagation();
        if (text->CanCut()) {
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
    if (text) {
        event.StopPropagation();
        if (text->CanPaste()) {
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
    if (text) {
        event.StopPropagation();
        if (text->CanRedo()) {
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
    if (text) {
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
    if (text) {
        event.StopPropagation();
        if (text->CanUndo()) {
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
    if (text) {
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
    if (!m_mgr) {
        event.Enable(false);
    }
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
    if (focusWin) {
        return dynamic_cast<wxTextCtrl*>(focusWin);
    }
    return NULL;
}
void MainFrame::OnImportFB(wxCommandEvent&)
{
    OpenWxFormBuilderImporterDialog("");
}

void MainFrame::OnImportSmith(wxCommandEvent&)
{
    OpenWxSmithImporterDialog("");
}

void MainFrame::OnImportXRC(wxCommandEvent&)
{
    OpenXRCImporterDialog();
}

void MainFrame::OpenSettingsDialog()
{
    const bool standAlone = m_mgr == nullptr;

    wxcSettingsDlg dlg(nullptr, standAlone);
    dlg.ShowModal();

    if (dlg.IsRestartRequired()) {
        ::wxMessageBox(_("In order for the change to take place, you need to restart CodeLite"), _("wxCrafter"));
    }
}

void MainFrame::OpenAboutDialog()
{
    wxcAboutDlg dlg(nullptr);
    dlg.ShowModal();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    OpenAboutDialog();
}

void MainFrame::OnHide(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if (m_mgr) {
        HideDesigner();
    } else {
        Close();
    }
}

void MainFrame::OnSettings(wxCommandEvent&)
{
    OpenSettingsDialog();
}

void MainFrame::DisplayDesigner()
{
    if (!IsShown()) {
        Show();
        // Center on the parent.
        if (GetParent()) {
            CenterOnParent();
        }
    }
    if (IsIconized()) {
        Restore();
    }

    CallAfter(&MainFrame::Raise);
    CallAfter(&MainFrame::SetFocus);
}

void MainFrame::MinimizeDesigner()
{
    if (IsShown() && !IsIconized()) {
        // minimize to the task bar
        this->Iconize();

        wxFrame* mainFrame = EventNotifier::Get()->TopFrame();
        if (mainFrame) {
            mainFrame->CallAfter(&wxFrame::Raise);
        }
    }
}

void MainFrame::HideDesigner()
{
    if (IsShown()) {
        // hide the designer
        this->Hide();

        wxFrame* mainFrame = EventNotifier::Get()->TopFrame();
        if (mainFrame) {
            mainFrame->CallAfter(&wxFrame::Raise);
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
    if (m_mgr) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}

void MainFrame::OnCodeLiteGotFocus(wxCommandEvent& e)
{
    e.Skip();
}

void MainFrame::OnPasteUI(wxUpdateUIEvent& event)
{
    event.Enable(true);
}

void MainFrame::OnSaveUI(wxUpdateUIEvent& event)
{
    event.Enable(wxcEditManager::Get().IsDirty());
}

void MainFrame::OnCloseUI(wxUpdateUIEvent& event)
{
    event.Enable(wxcProjectMetadata::Get().IsLoaded());
}

void MainFrame::OnOpenUI(wxUpdateUIEvent& event)
{
    event.Enable(true);
}

void MainFrame::OnNewProject(wxCommandEvent& event)
{
    static size_t Counter = 0;
    wxString title;
    title << "UntitledProject" << ++Counter << ".wxcp";

    wxString wxcpFile = ::wxFileSelector(_("Create an empty wxCrafter project"),
                                         wxEmptyString,
                                         title,
                                         wxEmptyString,
                                         "wxCrafter Project (*.wxcp)|*.wxcp",
                                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (wxcpFile.IsEmpty()) {
        return;
    }

    wxFFile fp(wxcpFile, "w+b");
    if (fp.IsOpened()) {
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

void MainFrame::OnOpen(wxCommandEvent& event)
{
    DoOpenWxcpProject();
}

void MainFrame::OnOpenMenu(wxAuiToolBarEvent& event)
{
    if (event.IsDropDownClicked()) {
        wxMenu menu;
        wxArrayString history;
        auto result = std::make_shared<wxString>();
        DoCreateRecentMenu(menu, history, result);

        wxRect rect = m_mainToolbar->GetToolRect(event.GetToolId());
        wxPoint pt = m_mainToolbar->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);

        // line up our menu with the button
        m_mainToolbar->SetToolSticky(event.GetToolId(), true);

        PopupMenu(&menu, pt);

        // make sure the button is "un-stuck"
        m_mainToolbar->SetToolSticky(event.GetToolId(), false);

        if (result->empty()) {
            return;
        }

        wxFileName fn{*result};
        wxCommandEvent evtClose(wxEVT_WXC_CLOSE_PROJECT);
        EventNotifier::Get()->ProcessEvent(evtClose);

        wxCommandEvent evtOpen(wxEVT_WXC_OPEN_PROJECT);
        evtOpen.SetString(fn.GetFullPath());
        EventNotifier::Get()->ProcessEvent(evtOpen);
    } else {
        DoOpenWxcpProject();
    }
}

void MainFrame::OnDeleteCustomControl(wxCommandEvent&)
{
    OpenDeleteCustomControlDialog();
}

void MainFrame::OnEditCustomControl(wxCommandEvent& event)
{
    OpenEditCustomControlDialog();
}

void MainFrame::OnNewCustomControl(wxCommandEvent&)
{
    OpenDefineCustomControlWizard();
}

wxStyledTextCtrl* MainFrame::GetActiveSTC()
{
    wxWindow* focusWin = wxWindow::FindFocus();
    if (focusWin) {
        return dynamic_cast<wxStyledTextCtrl*>(focusWin);
    }
    return NULL;
}

void MainFrame::OnGenerateCode(wxCommandEvent& event)
{
    wxCommandEvent evtGenCode(wxEVT_WXC_CMD_GENERATE_CODE);
    EventNotifier::Get()->ProcessEvent(evtGenCode);
}

void MainFrame::OnGenerateCodeUI(wxUpdateUIEvent& event)
{
    event.Enable(wxcProjectMetadata::Get().IsLoaded());
}

void MainFrame::OnOpenFindDialog(wxCommandEvent& event)
{
    if (m_mgr) {
        // Ask CodeLite to open the find dialog
        wxCommandEvent event_find{wxEVT_MENU, XRCID("id_find")};
        event_find.SetEventObject(EventNotifier::Get()->TopFrame());
        EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(event_find);
    } else {
        if (m_findReplaceDialog) {
            m_findReplaceDialog->Raise();
            return;
        }

        wxStyledTextCtrl* stc = m_wxcView->GetPreviewEditor();
        if (stc) {
            m_findReplaceDialog = new wxFindReplaceDialog(stc, &m_findData, _("Find"), wxFR_NOUPDOWN);
            m_findReplaceDialog->Show();
        }
    }
}

void MainFrame::OnCodeEditorSelected(wxCommandEvent& e)
{
    e.Skip();
    if (!m_mgr) {
        bool needToDisplayAgain = false;
        // if the dialog was show, close it
        if (m_findReplaceDialog) {
            m_findReplaceDialog->Destroy();
            m_findReplaceDialog = NULL;
            needToDisplayAgain = true;
        }

        if (needToDisplayAgain && e.GetClientData()) {
            // Reshow the dialog, this time for the proper parent
            wxStyledTextCtrl* parent = reinterpret_cast<wxStyledTextCtrl*>(e.GetClientData());
            if (parent) {
                m_findReplaceDialog = new wxFindReplaceDialog(parent, &m_findData, _("Find"), wxFR_NOUPDOWN);
                m_findReplaceDialog->Show();
            }
        }
    }
}

void MainFrame::OnFindDlgClose(wxFindDialogEvent& event)
{
    if (!m_mgr) {
        m_findReplaceDialog = nullptr;
    }
}

void MainFrame::OnFindFirst(wxFindDialogEvent& event)
{
    if (m_mgr) {
        return;
    }
    wxStyledTextCtrl* stc = dynamic_cast<wxStyledTextCtrl*>(m_findReplaceDialog->GetParent());
    if (stc) {
        int curpos = stc->GetCurrentPos();
        if (!DoFindText(stc, *(m_findReplaceDialog->GetData()), false)) {
            // Try from start
            stc->SetCurrentPos(0);
            if (!DoFindText(stc, *(m_findReplaceDialog->GetData()), false)) {
                // Could not find from the start as well, restore the caret position
                stc->SetCurrentPos(curpos);
                stc->EnsureCaretVisible();
                ::wxMessageBox(
                    wxString() << "Could not find string: " << m_findReplaceDialog->GetData()->GetFindString(),
                    "wxCrafter",
                    wxOK | wxICON_WARNING | wxOK_DEFAULT | wxCENTER,
                    this);
                m_findReplaceDialog->Raise();
            }
        }
    }
}

void MainFrame::OnFindNext(wxFindDialogEvent& event)
{
    if (m_mgr) {
        return;
    }
    wxStyledTextCtrl* stc = dynamic_cast<wxStyledTextCtrl*>(m_findReplaceDialog->GetParent());
    if (stc) {
        int curpos = stc->GetCurrentPos();
        if (!DoFindText(stc, *(m_findReplaceDialog->GetData()), true)) {
            // Try from start
            stc->SetCurrentPos(0);
            if (!DoFindText(stc, *(m_findReplaceDialog->GetData()), false)) {
                // Could not find from the start as well, restore the caret position
                stc->SetCurrentPos(curpos);
                stc->EnsureCaretVisible();
            }
        }
    }
}

bool MainFrame::DoFindText(wxStyledTextCtrl* stc, const wxFindReplaceData& frd, bool findNext)
{
    int endPos = stc->GetLastPosition();
    int startPos;

    if (findNext) {
        wxString selectedText = stc->GetSelectedText();
        if (selectedText == frd.GetFindString()) {
            startPos = stc->GetSelectionEnd();
        } else {
            startPos = stc->GetCurrentPos();
        }

    } else {
        startPos = stc->GetCurrentPos();
    }

    int flags = frd.GetFlags();
    int stcSearchFlags = 0;

    if (flags & wxFR_MATCHCASE) {
        stcSearchFlags |= wxSTC_FIND_MATCHCASE;
    }

    if (flags & wxFR_WHOLEWORD) {
        stcSearchFlags |= wxSTC_FIND_WHOLEWORD;
    }

    int where = stc->FindText(startPos, endPos, frd.GetFindString(), stcSearchFlags);
    if (where != wxNOT_FOUND) {
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

void MainFrame::OnBatchGenerateCode(wxCommandEvent& event)
{
    wxCommandEvent evtGenCode(wxEVT_WXC_CMD_BATCH_GENERATE_CODE);
    EventNotifier::Get()->ProcessEvent(evtGenCode);
}

void MainFrame::OnBatchGenerateCodeUI(wxUpdateUIEvent& event)
{
    if (m_mgr) {
        event.Enable(false);
    } else {
        event.Enable(!wxcProjectMetadata::Get().IsLoaded());
    }
}

void MainFrame::DoOpenWxcpProject()
{
    wxString file_name = ::wxFileSelector(
        _("Open a wxCrafter file"), wxEmptyString, wxEmptyString, wxEmptyString, "wxCrafter Project (*.wxcp)|*.wxcp");

    if (!file_name.IsEmpty()) {
        wxFileName fn(file_name);
        wxCommandEvent evtOpen(wxEVT_WXC_OPEN_PROJECT);
        evtOpen.SetString(fn.GetFullPath());
        EventNotifier::Get()->ProcessEvent(evtOpen);
    }
}

void MainFrame::OnFileOpen(wxCommandEvent& event)
{
    DoOpenWxcpProject();
}

void MainFrame::DoCreateRecentMenu(wxMenu& menu, wxArrayString& history, std::shared_ptr<wxString> result)
{
    history = wxcSettings::Get().GetHistory();
    if (history.empty()) {
        return;
    }

    wxArrayString tmpHistory;
    for (size_t i = 0; i < history.GetCount(); ++i) {
        if (wxFileName(history.Item(i)).Exists()) {
            tmpHistory.Add(history.Item(i));
        }
    }

    history.swap(tmpHistory);
    history.Sort();

    // Update the history
    wxcSettings::Get().SetHistory(history);
    wxcSettings::Get().Save();

    wxString prefix = "wxc-recent-menu-item-";
    for (size_t i = 0; i < history.size(); ++i) {
        wxString label = history.Item(i);
        wxString menu_xrc_id;
        menu_xrc_id << prefix << i;
        menu.Append(wxXmlResource::GetXRCID(menu_xrc_id), label);
        menu.Bind(
            wxEVT_MENU, [=](wxCommandEvent& event) { *result.get() = label; }, wxXmlResource::GetXRCID(menu_xrc_id));
    }
}

void MainFrame::SetStatusMessage(const wxString& message)
{
    if (GetStatusBar()) {
        GetStatusBar()->SetStatusText(message, 0);
    }
}

void MainFrame::OpenDefineCustomControlWizard()
{
    DefineCustomControlWizard wiz(nullptr);

    if (wiz.RunWizard(wiz.GetFirstPage())) {
        CustomControlTemplate controlData = wiz.GetControl();
        wxcSettings::Get().RegisterCustomControl(controlData);
        wxcSettings::Get().Save();
    }
}

void MainFrame::OpenEditCustomControlDialog()
{
    EditCustomControlDlg dlg(nullptr);
    dlg.ShowModal();

    // Once this dialog is saved, we should:
    // 1) Refresh  the view
    wxCommandEvent evt(wxEVT_PROPERTIES_MODIFIED);
    EventNotifier::Get()->AddPendingEvent(evt);
}

void MainFrame::OpenDeleteCustomControlDialog()
{
    DeleteCustomControlDlg dlg(nullptr);
    dlg.ShowModal();

    // Once this dialog is saved, we should:
    // 1) Refresh  the view
    wxCommandEvent evt(wxEVT_PROPERTIES_MODIFIED);
    EventNotifier::Get()->AddPendingEvent(evt);
}

bool MainFrame::DoCreateVirtualFolder(const wxString& vdFullPath)
{
    CHECK_POINTER_RET_FALSE(m_mgr);
    wxString name = vdFullPath.AfterLast(':');
    wxString parent = vdFullPath.BeforeLast(':');
    return m_mgr->CreateVirtualDirectory(parent, name);
}

void MainFrame::DoLoadAfterImport(ImportDlg::ImportFileData& data)
{
    if (m_mgr && data.addToProject && !data.virtualFolder.IsEmpty()) {
        wxArrayString filesToAdd;
        filesToAdd.Add(data.wxcpFilename.GetFullPath());
        if (DoCreateVirtualFolder(data.virtualFolder)) {
            m_mgr->AddFilesToVirtualFolder(data.virtualFolder, filesToAdd);
        }
    }
    if (data.loadWhenDone) {
        LoadProject(data.wxcpFilename);
    }

    // do it using event, or else the main frame will steal the focus
    wxCommandEvent evtShowDesigner(wxEVT_SHOW_WXCRAFTER_DESIGNER);
    EventNotifier::Get()->AddPendingEvent(evtShowDesigner);
}

void MainFrame::OpenWxFormBuilderImporterDialog(const wxString& filename)
{
    ImportDlg::ImportFileData data;
    ImportFromwxFB::Importer import(wxCrafter::TopFrame());
    const bool showAddToProject = m_mgr != nullptr;

    if (import.ImportProject(data, filename, showAddToProject)) {
        DoLoadAfterImport(data);
    }
}

void MainFrame::OpenXRCImporterDialog()
{
    ImportDlg::ImportFileData data;
    ImportFromXrc::Importer import(wxCrafter::TopFrame());
    const bool showAddToProject = m_mgr != nullptr;

    if (import.ImportProject(data, showAddToProject)) {
        DoLoadAfterImport(data);
    }
}

void MainFrame::OpenWxSmithImporterDialog(const wxString& filename)
{
    ImportDlg::ImportFileData data;
    ImportFromwxSmith::Importer import(wxCrafter::TopFrame());
    const bool showAddToProject = m_mgr != nullptr;

    if (import.ImportProject(data, filename, showAddToProject)) {
        DoLoadAfterImport(data);
    }
}

bool MainFrame::DoReadFileContentAndPrompt(const wxFileName& fn, wxString& content, IEditor** editor)
{
    *editor = nullptr;

    // Check to see if we got the file opened in the editor
    if (m_mgr) {
        *editor = m_mgr->FindEditor(fn.GetFullPath());
        if (*editor) {
            content = (*editor)->GetTextRange(0, (*editor)->GetLength());
            return true;
        }
    }

    // Read the header file content from the disk
    if (!wxCrafter::ReadFileContent(fn.GetFullPath(), content)) {
        wxString msg;
        msg << wxString::Format(_("Error while reading file content: '%s'.\n"), fn.GetFullPath())
            << _("Could not read file content");
        ::wxMessageBox(msg, _("wxCrafter"), wxICON_WARNING | wxOK | wxCENTER, EventNotifier::Get()->TopFrame());
        return false;
    }
    return true;
}

void MainFrame::DoUpdateDerivedClassEventHandlers(const wxString& className,
                                                  const wxFileName& derivedSource,
                                                  const wxFileName& derivedHeader)
{
    wxString headerContent, sourceContent;
    IEditor* headerEditor(nullptr);
    IEditor* sourceEditor(nullptr);

    if (!DoReadFileContentAndPrompt(derivedHeader, headerContent, &headerEditor)) {
        return;
    }

    if (!DoReadFileContentAndPrompt(derivedSource, sourceContent, &sourceEditor)) {
        return;
    }

    CHECK_POINTER(GetWxcView());

    // Parse and collect all functions declared in the header file
    wxcWidget* topLevelWin = GetWxcView()->GetActiveTopLevelWin();
    CHECK_POINTER(topLevelWin);

    wxcWidget::Map_t connectedEvents = topLevelWin->GetConnectedEventsRecursively();
    FunctionsParser parser(connectedEvents, className, headerContent);
    parser.Parse();

    wxString decl, impl;
    const wxcWidget::Map_t& newEvents = parser.GetAllEvents();
    if (newEvents.empty()) {
        return;
    }

    for (const auto& [_, connectDetail] : newEvents) {
        impl << connectDetail.GetFunctionImpl(className);
        decl << connectDetail.GetFunctionDecl();
    }

    // Insert the functions declarations
    if (TagsManagerST::Get()->InsertFunctionDecl(className, decl, headerContent, 1)) {
        if (m_mgr && wxcSettings::Get().HasFlag(wxcSettings::FORMAT_INHERITED_FILES)) {
            FormatString(headerContent, derivedHeader);
        }

        // Write the resulting string
        DoWriteFileContent(derivedHeader, headerContent, headerEditor);
        if (m_mgr) {
            NotifyFileSaved(derivedHeader);
        }
    }

    // Insert the functions definitions at the end of the file buffer
    if (!sourceContent.EndsWith("\n")) {
        sourceContent << "\n";
    }
    sourceContent << impl;

    // Format the source file
    if (m_mgr && wxcSettings::Get().HasFlag(wxcSettings::FORMAT_INHERITED_FILES)) {
        FormatString(headerContent, derivedSource);
    }

    DoWriteFileContent(derivedSource, sourceContent, sourceEditor);
    if (m_mgr) {
        NotifyFileSaved(derivedSource);
    }
}

void MainFrame::OnBitmapCodeGenerationCompleted(wxCommandEvent& e)
{
    if (wxcProjectMetadata::Get().GetGenerateCPPCode()) {
        // First, notify CodeLite to reload all modified files that are opened (so CodeLite will not prompt us about
        // them)
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT);
        EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);

        // Next, compile a list of all generated files and add them to CodeLite
        wxFileName bitmapSourceFile(e.GetString());
        wxCrafter::MakeAbsToProject(bitmapSourceFile);

        // Base header/cpp
        wxFileName headerFile = wxcProjectMetadata::Get().BaseHeaderFile();
        wxCrafter::MakeAbsToProject(headerFile);

        wxFileName sourceFile = wxcProjectMetadata::Get().BaseCppFile();
        wxCrafter::MakeAbsToProject(sourceFile);

        // And the derived classes
        wxFileName derivedHeader = wxcProjectMetadata::Get().GetGeneratedHeader();
        wxCrafter::MakeAbsToProject(derivedHeader);

        wxFileName derivedSource = wxcProjectMetadata::Get().GetGeneratedSource();
        wxCrafter::MakeAbsToProject(derivedSource);

        // XRC file
        wxFileName xrcFile;
        if (wxcProjectMetadata::Get().GetGenerateXRC()) {
            xrcFile = wxcProjectMetadata::Get().GetXrcFileName();
            wxCrafter::MakeAbsToProject(xrcFile);
        }

        // Notify CodeLite to retag the workspace
        std::vector<wxFileName> filesToRetag{headerFile, sourceFile, derivedHeader, derivedSource, bitmapSourceFile};

        // Prepare a list of additional files generated by wxC (this can happen for custom model for wxDataViewCtrl)
        wxArrayString additionalFiles;
        for (const auto& p : wxcProjectMetadata::Get().GetAdditionalFiles()) {
            wxFileName f(wxcProjectMetadata::Get().GetGeneratedHeader());
            f.SetFullName(p.first);
            wxCrafter::MakeAbsToProject(f);
            additionalFiles.Add(f.GetFullPath());
            filesToRetag.push_back(f);
        }

        wxString sourceFilesVD;
        std::set<wxString> uniqueFiles;
        if (m_mgr) {
            wxArrayString filesToAdd;
            if (clCxxWorkspaceST::Get()->IsOpen()) {
                wxString vd = wxcProjectMetadata::Get().GetVirtualFolder();

                wxString projectName;
                vd.Trim().Trim(false);
                if (vd.IsEmpty()) {
                    // We got no Virtual Folder to place the files to
                    // Search for resource file
                    wxFileName projectFile(wxcProjectMetadata::Get().GetProjectFile());
                    projectName = m_mgr->GetProjectNameByFile(projectFile.GetFullPath());

                } else {
                    projectName = vd.BeforeFirst(':');
                }

                projectName.Trim().Trim(false);
                if (projectName.IsEmpty()) {
                    return;
                }

                wxString errMsg;
                ProjectPtr project = clCxxWorkspaceST::Get()->FindProjectByName(projectName, errMsg);

                if (!project) {
                    wxString msg;
                    msg << wxString::Format(_("Could not find target project: '%s'\n"), projectName);
                    msg << _("Make sure that the virtual folder entry is valid (it must exist)\n");
                    msg << wxString::Format(_("Virtual Folder provided was: '%s'"), vd);
                    ::wxMessageBox(msg, _("wxCrafter"), wxOK | wxICON_ERROR);
                    return;
                }

                std::vector<wxFileName> projectFiles;
                project->GetFilesAsVectorOfFileName(projectFiles);
                uniqueFiles = wxCrafter::VectorToSet(projectFiles);

                wxString baseFilesVD, bitmapFilesVD, xrcVD;
                sourceFilesVD << wxcProjectMetadata::Get().GetVirtualFolder();
                sourceFilesVD.Trim().Trim(false);

                if (sourceFilesVD.IsEmpty()) {
                    sourceFilesVD << projectName << ":src";
                }

                baseFilesVD << projectName << ":wxcrafter:base";
                bitmapFilesVD << projectName << ":wxcrafter:resources";
                xrcVD << projectName << ":wxcrafter:XRC";

                //////////////////////////////////////////////////////////////
                // Add the bitmap resource files
                //////////////////////////////////////////////////////////////
                if (uniqueFiles.find(bitmapSourceFile.GetFullPath()) == uniqueFiles.end()) {
                    filesToAdd.Add(bitmapSourceFile.GetFullPath());
                }

                if (filesToAdd.IsEmpty() == false) {
                    DoCreateVirtualFolder(bitmapFilesVD);
                    m_mgr->AddFilesToVirtualFolder(bitmapFilesVD, filesToAdd);
                }

                //////////////////////////////////////////////////////////////
                // Add the XRC files
                //////////////////////////////////////////////////////////////
                filesToAdd.Clear();
                if (xrcFile.IsOk() && uniqueFiles.find(xrcFile.GetFullPath()) == uniqueFiles.end()) {
                    filesToAdd.Add(xrcFile.GetFullPath());
                }

                if (filesToAdd.IsEmpty() == false) {
                    DoCreateVirtualFolder(xrcVD);
                    m_mgr->AddFilesToVirtualFolder(xrcVD, filesToAdd);
                }

                //////////////////////////////////////////////////////////////
                // Add the base classes files
                //////////////////////////////////////////////////////////////
                filesToAdd.Clear();
                if (uniqueFiles.find(headerFile.GetFullPath()) == uniqueFiles.end()) {
                    filesToAdd.Add(headerFile.GetFullPath());
                }
                if (uniqueFiles.find(sourceFile.GetFullPath()) == uniqueFiles.end()) {
                    filesToAdd.Add(sourceFile.GetFullPath());
                }

                // The additional files are added to the 'base'
                for (size_t i = 0; i < additionalFiles.GetCount(); ++i) {
                    if (uniqueFiles.find(additionalFiles.Item(i)) == uniqueFiles.end()) {
                        filesToAdd.Add(additionalFiles.Item(i));
                    }
                }

                if (filesToAdd.IsEmpty() == false) {
                    DoCreateVirtualFolder(baseFilesVD);
                    m_mgr->AddFilesToVirtualFolder(baseFilesVD, filesToAdd);
                }
            }
        }
        //////////////////////////////////////////////////////////////
        // Add the derived classes files
        //////////////////////////////////////////////////////////////

        // Not all top level windows wants subclassing..
        if (derivedHeader.IsOk() && !derivedHeader.GetName().IsEmpty()) {
            if (m_mgr && clCxxWorkspaceST::Get()->IsOpen()) {
                wxArrayString filesToAdd;
                if (uniqueFiles.find(derivedHeader.GetFullPath()) == uniqueFiles.end()) {
                    filesToAdd.Add(derivedHeader.GetFullPath());
                }

                if (uniqueFiles.find(derivedSource.GetFullPath()) == uniqueFiles.end()) {
                    filesToAdd.Add(derivedSource.GetFullPath());
                }

                if (filesToAdd.IsEmpty() == false) {
                    DoCreateVirtualFolder(sourceFilesVD);
                    m_mgr->AddFilesToVirtualFolder(sourceFilesVD, filesToAdd);
                }
            }
            DoUpdateDerivedClassEventHandlers(
                wxcProjectMetadata::Get().GetGeneratedClassName(), derivedSource, derivedHeader);
        }
    }

    wxString msg;
    msg << _("wxCrafter: code generation completed successfully!");
    SetStatusMessage(msg);
}

void MainFrame::DoGenerateCode(const NewFormDetails& fd)
{
    wxString errMsg;

    wxFileName wxcpFile = wxFileName(fd.wxcpFile);
    if (!wxcpFile.FileExists()) {
        wxFFile fp(wxcpFile.GetFullPath(), "w+b");
        if (fp.IsOpened()) {
            fp.Close();

        } else {
            wxString msg;
            msg << _("Could not create resource file '") << wxcpFile.GetFullPath() << "'";
            ::wxMessageBox(msg, _("wxCrafter"), wxOK | wxICON_WARNING | wxCENTRE);
            return;
        }
    }

    // If we have a workspace opened, handle the virtual folder thing here
    if (m_mgr && clCxxWorkspaceST::Get()->IsOpen()) {
        // Step 1:
        // Check if we already got a virtual folder named 'wxcrafter'
        wxString projectName = fd.virtualFolder.BeforeFirst(':');
        ProjectPtr project = clCxxWorkspaceST::Get()->FindProjectByName(projectName, errMsg);
        if (!project) {
            ::wxMessageBox(errMsg, _("wxCrafter"), wxOK | wxICON_WARNING | wxCENTRE);
            return;
        }

        wxString vdFullPath;
        vdFullPath << projectName;

        // Check if already got a file with this name in the project
        wxStringSet_t files = GetProjectFiles(project->GetName());

        if (!files.count(wxcpFile.GetFullPath())) {

            if (!m_mgr->CreateVirtualDirectory(vdFullPath, "wxcrafter")) {
                ::wxMessageBox(
                    _("Could not create virtual folder 'wxcrafter'"), _("wxCrafter"), wxOK | wxICON_WARNING | wxCENTRE);
                return;
            }

            // Add the resource file to the newly created virtual folder
            wxArrayString f;
            f.Add(wxcpFile.GetFullPath());
            vdFullPath << ":"
                       << "wxcrafter";
            m_mgr->AddFilesToVirtualFolder(vdFullPath, f);
        }
    }

    // At this point, wxcpFile contains the fullpath the
    // wxCrafter project, load it
    DisplayDesigner();
    LoadProject(wxcpFile.GetFullPath());
    AddForm(fd);

    // Notify about file system changes here
    clFileSystemEvent eventFilesGenerate(wxEVT_FILE_CREATED);
    eventFilesGenerate.GetPaths().Add(wxcpFile.GetFullPath());
    EventNotifier::Get()->QueueEvent(eventFilesGenerate.Clone());

    // Show the designer
    wxCommandEvent evtShowDesigner(wxEVT_SHOW_WXCRAFTER_DESIGNER);
    EventNotifier::Get()->ProcessEvent(evtShowDesigner);
}

void MainFrame::OpenNewFormWizard(int type)
{
    if (!m_mgr && !wxcProjectMetadata::Get().IsLoaded()) {
        ::wxMessageBox(_("You must create a project before you can add new forms"),
                       _("wxCrafter"),
                       wxOK | wxCENTER | wxICON_WARNING);
        return;
    }

    // Check to see if we already got a wxCrafter.wxcp file
    NewFormWizard wizard(wxCrafter::TopFrame(), m_mgr, type);
    if (wizard.RunWizard(wizard.GetFirstPage())) {
        NewFormDetails details = wizard.GetFormDetails();
        DoGenerateCode(details);

        wxString label;
        switch (type) {
        case ID_WXDIALOG:
            label << "new dialog";
            break;
        case ID_WXFRAME:
            label << "new frame";
            break;
        case ID_WXWIZARD:
            label << "new wizard";
            break;
        case ID_WXPANEL_TOPLEVEL:
            label << "new panel";
            break;
        case ID_WXPOPUPWINDOW:
            label << "new popup window";
            break;
        case ID_WXIMAGELIST:
            label << "new image list";
            break;
        default:
            label << "new top-level item";
        }
        wxcEditManager::Get().PushState(label);
    }
}
