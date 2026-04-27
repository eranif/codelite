#include "RemotyWorkspaceView.hpp"

#include "AsyncProcess/asyncprocess.h"
#include "AsyncProcess/processreaderthread.h"
#include "FileSystemWorkspace/clFileSystemWorkspaceConfig.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspaceDlg.h"
#include "RemotyWorkspace.hpp"
#include "SFTPClientData.hpp"
#include "clAuiToolBarArt.h"
#include "clRemoteFindDialog.h"
#include "clSFTPManager.hpp"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "ssh/ssh_account_info.h"

#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
#include <wx/wxcrt.h>
#include <wx/xrc/xmlres.h>

RemotyWorkspaceView::RemotyWorkspaceView(wxWindow* parent, RemotyWorkspace* workspace)
    : RemotyWorkspaceViewBase(parent)
    , m_workspace(workspace)
{
    m_tree = new clRemoteDirCtrl(this);
    m_toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
    auto icons = clGetManager()->GetStdIcons();

    clAuiToolBarArt::AddTool(
        m_toolbar, XRCID("remoty-wsp-settings"), _("Workspace Settings"), icons->LoadBitmap("cog"));
    clAuiToolBarArt::AddTool(
        m_toolbar, XRCID("edit-codelite-remote"), _("Editor codelite-remote.json"), icons->LoadBitmap("json"));
    clAuiToolBarArt::AddTool(
        m_toolbar, XRCID("build-menu"), _("Build"), icons->LoadBitmap("build"), _("Build"), wxITEM_DROPDOWN);
    m_configs =
        new wxChoice(m_toolbar, wxID_ANY, wxDefaultPosition, wxSize{GetTextExtent("RELEASE_RELEASE").GetWidth(), -1});

#ifdef __WXMAC__
    m_configs->SetWindowVariant(wxWINDOW_VARIANT_MINI);
#else
    m_configs->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif

    m_toolbar->AddControl(m_configs);
    m_toolbar->Realize();

    m_configs->Bind(wxEVT_CHOICE, &RemotyWorkspaceView::OnConfigSelected, this);
    m_toolbar->Bind(wxEVT_TOOL, &RemotyWorkspaceView::OnEditWorkspaceSettings, this, XRCID("remoty-wsp-settings"));
    m_toolbar->Bind(
        wxEVT_TOOL, &RemotyWorkspaceView::OnEditCodeLiteRemoteJsonFile, this, XRCID("edit-codelite-remote"));
    m_toolbar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &RemotyWorkspaceView::OnBuildMenu, this, XRCID("build-menu"));
    GetSizer()->Add(m_toolbar, 0, wxEXPAND);
    GetSizer()->Add(m_tree, 1, wxEXPAND);

    GetSizer()->Fit(this);

    m_tree->Bind(wxEVT_REMOTEDIR_DIR_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnDirContextMenu, this);
    m_tree->Bind(wxEVT_REMOTEDIR_FILE_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnFileContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_FINDINFILES_DLG_SHOWING, &RemotyWorkspaceView::OnFindInFilesShowing, this);
    EventNotifier::Get()->Bind(wxEVT_FINDINFILES_OPEN_MATCH, &RemotyWorkspaceView::OnOpenFindInFilesMatch, this);
    EventNotifier::Get()->Bind(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, &RemotyWorkspaceView::OnRemoteFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &RemotyWorkspaceView::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &RemotyWorkspaceView::OnWorkspaceClosed, this);
}

RemotyWorkspaceView::~RemotyWorkspaceView()
{
    m_tree->Unbind(wxEVT_REMOTEDIR_DIR_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnDirContextMenu, this);
    m_tree->Unbind(wxEVT_REMOTEDIR_FILE_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnFileContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_FINDINFILES_DLG_SHOWING, &RemotyWorkspaceView::OnFindInFilesShowing, this);
    EventNotifier::Get()->Unbind(wxEVT_FINDINFILES_OPEN_MATCH, &RemotyWorkspaceView::OnOpenFindInFilesMatch, this);
    EventNotifier::Get()->Unbind(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, &RemotyWorkspaceView::OnRemoteFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &RemotyWorkspaceView::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &RemotyWorkspaceView::OnWorkspaceClosed, this);
}

void RemotyWorkspaceView::OpenWorkspace(const wxString& path, const wxString& accountName)
{
    auto account = SSHAccountInfo::LoadAccount(accountName);
    if (account.GetAccountName().empty()) {
        clWARNING() << "Failed to open workspace at:" << path << "for account" << accountName << endl;
        clWARNING() << "Account does not exist" << endl;
    }
    m_tree->Close(false);
    m_tree->Open(path, account);
}

void RemotyWorkspaceView::CloseWorkspace() { m_tree->Close(false); }

void RemotyWorkspaceView::OnDirContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    wxMenu* menu = event.GetMenu();
    auto item = m_tree->GetTree()->GetSelection();
    CHECK_ITEM_RET(item);

    bool isRootItem = (item == m_tree->GetTree()->GetRootItem());
    if (isRootItem && m_workspace->GetSettings().GetSelectedConfig()) {
        auto& settings = m_workspace->GetSettings();
        auto conf = m_workspace->GetSettings().GetSelectedConfig();
        menu->AppendSeparator();
        // ===---
        // add "build" menu entry
        // ===---
        const auto& targets = settings.GetSelectedConfig()->GetBuildTargets();
        wxMenu* build_menu = new wxMenu;
        for (const auto& vt : targets) {
            const wxString& name = vt.first;
            wxString xrcid_str;
            xrcid_str << "wsp-build-" << name;
            int xrcid = wxXmlResource::GetXRCID(xrcid_str);
            build_menu->Append(xrcid, name, wxEmptyString, wxITEM_NORMAL);
            build_menu->Bind(
                wxEVT_MENU,
                [this, name](wxCommandEvent& event) {
                    wxUnusedVar(event);
                    CallAfter(&RemotyWorkspaceView::BuildTarget, name);
                },
                xrcid);
        }
        menu->AppendSubMenu(build_menu, _("Build"));
        // ===---
        // add "select configuration" menu entry
        // ===---
        wxMenu* select_config_menu = new wxMenu;
        wxArrayString configs = settings.GetConfigs();
        wxString activeConfig = settings.GetSelectedConfig()->GetName();
        for (auto config : configs) {
            wxString xrcid_str;
            xrcid_str << "wsp-config-" << config;
            int xrcid = wxXmlResource::GetXRCID(xrcid_str);
            select_config_menu->Append(xrcid, config, wxEmptyString, wxITEM_CHECK);
            select_config_menu->Check(xrcid, config == activeConfig);
            select_config_menu->Bind(
                wxEVT_MENU,
                [this, config](wxCommandEvent& event) {
                    wxUnusedVar(event);
                    CallAfter(&RemotyWorkspaceView::SetBuildConfiguration, config);
                },
                xrcid);
        }
        menu->AppendSubMenu(select_config_menu, _("Configuration"));
    }

    menu->AppendSeparator();
    menu->Append(XRCID("remoty-wsp-settings"), _("Workspace settings..."));
    menu->Append(XRCID("edit-codelite-remote"), _("Edit codelite-remote.json..."));

    menu->Bind(wxEVT_MENU, &RemotyWorkspaceView::OnEditWorkspaceSettings, this, XRCID("remoty-wsp-settings"));
    menu->Bind(wxEVT_MENU, &RemotyWorkspaceView::OnEditCodeLiteRemoteJsonFile, this, XRCID("edit-codelite-remote"));

    menu->AppendSeparator();
    menu->Append(XRCID("reload-workspace"), _("Reload workspace"));
    menu->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& e) {
            wxUnusedVar(e);
            CallAfter(&RemotyWorkspaceView::DoReloadWorkspace);
        },
        XRCID("reload-workspace"));
    menu->AppendSeparator();
    menu->Append(wxID_CLOSE, _("Close workspace"));
    menu->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& e) {
            wxUnusedVar(e);
            CallAfter(&RemotyWorkspaceView::DoCloseWorkspace);
        },
        wxID_CLOSE);
}

void RemotyWorkspaceView::OnFileContextMenu(clContextMenuEvent& event) { event.Skip(); }

void RemotyWorkspaceView::OnFindInFilesShowing(clFindInFilesEvent& event)
{
    event.Skip();
    if (!m_workspace->IsOpened())
        return;

    // override the default find in files dialog
    event.Skip(false);
    wxString rootpath = m_workspace->GetRemoteWorkspaceFile();
    rootpath = rootpath.BeforeLast('/'); // only the path

    clRemoteFindDialog dlg(EventNotifier::Get()->TopFrame(), m_workspace->GetAccount().GetAccountName(), rootpath);
    auto editor = ::clGetManager()->GetActiveEditor();
    if (editor && (editor->GetSelectionStart() != editor->GetSelectionEnd())) {
        dlg.SetFindWhat(editor->GetCtrl()->GetSelectedText());
    }

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    // start the search
    if (dlg.IsReplaceAction()) {
        m_workspace->ReplaceInFiles(dlg.GetWhere(),
                                    dlg.GetFileExtensions(),
                                    dlg.GetExcludePatterns(),
                                    dlg.GetFindWhat(),
                                    dlg.GetReplaceWith(),
                                    dlg.IsWholeWord(),
                                    dlg.IsIcase());
    } else {
        m_workspace->FindInFiles(dlg.GetWhere(),
                                 dlg.GetFileExtensions(),
                                 dlg.GetExcludePatterns(),
                                 dlg.GetFindWhat(),
                                 dlg.IsWholeWord(),
                                 dlg.IsIcase());
    }
}

void RemotyWorkspaceView::OnOpenFindInFilesMatch(clFindInFilesEvent& event)
{
    event.Skip();
    if (!m_workspace->IsOpened() || event.GetMatches().empty())
        return;

    // this is ours to handle
    event.Skip(false);
    const auto& match = event.GetMatches()[0];
    const auto& loc = match.locations[0];

    // if the file is already opened, just show it
    IEditor* editor = clGetManager()->FindEditor(match.file);
    if (!editor) {
        editor = clSFTPManager::Get().OpenFile(match.file, m_workspace->GetAccount().GetAccountName());
    }

    if (editor) {
        // sci is 0 based line numbers
        auto callback = [=](IEditor* peditor) {
            int editor_line = loc.line - 1;
            peditor->GetCtrl()->ClearSelections();
            int pos = peditor->PosFromLine(editor_line);
            peditor->SetActive();
            peditor->SetCaretAt(pos);
            peditor->CenterLinePreserveSelection(editor_line);
        };
        // load the local file (or make it selected) and then focus on the matched line
        clGetManager()->OpenFileAndAsyncExecute(editor->GetFileName().GetFullPath(), callback);
    }
}

void RemotyWorkspaceView::SetBuildConfiguration(const wxString& config)
{
    m_workspace->GetSettings().SetSelectedConfig(config);
    wxString current_sel = m_configs->GetStringSelection();
    if (current_sel != config) {
        m_configs->SetStringSelection(config);
    }
}

void RemotyWorkspaceView::BuildTarget(const wxString& name) { m_workspace->BuildTarget(name); }

void RemotyWorkspaceView::DoReloadWorkspace()
{
    // Async call to re-open the workspace
    clCommandEvent e(wxEVT_CMD_RELOAD_WORKSPACE, GetId());
    e.SetEventObject(this);
    EventNotifier::Get()->AddPendingEvent(e);
}

void RemotyWorkspaceView::DoCloseWorkspace()
{
    wxBusyCursor bc;
    // let the plugins close any custom workspace
    clCommandEvent e(wxEVT_CMD_CLOSE_WORKSPACE, GetId());
    e.SetEventObject(this);
    EventNotifier::Get()->ProcessEvent(e);
}

void RemotyWorkspaceView::OnRemoteFileSaved(clCommandEvent& event)
{
    event.Skip();
    if (!m_workspace->IsOpened())
        return;

    clGetManager()->SetStatusMessage(_("Remote file: ") + event.GetFileName() + _(" saved!"));

    const wxString& filename = event.GetFileName();
    bool is_codelite_remote_json = filename.EndsWith("codelite-remote.json");
    IEditor* editor = clGetManager()->FindEditor(filename);

    auto cd = editor->GetRemoteData();
    CHECK_PTR_RET(cd);

    if (is_codelite_remote_json && cd->GetAccountName() == m_workspace->GetAccount().GetAccountName()) {
        clGetManager()->SetStatusMessage(
            _("NOTICE: a workspace reload is required in order for the changes to take place"), 3);
    }
}

wxString RemotyWorkspaceView::GetRemotePathIsOwnedByWorkspace(IEditor* editor) const
{
    if (!m_workspace->IsOpened())
        return wxEmptyString;

    auto cd = editor->GetRemoteData();
    if (!cd) {
        return wxEmptyString;
    }

    if (cd->GetAccountName() == m_workspace->GetAccount().GetAccountName()) {
        return cd->GetRemotePath();
    }
    return wxEmptyString;
}

size_t RemotyWorkspaceView::GetWorkspaceRemoteFilesOpened(wxArrayString* paths) const
{
    if (!paths) {
        return 0;
    }

    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    for (auto editor : editors) {
        wxString remote_path = GetRemotePathIsOwnedByWorkspace(editor);
        if (remote_path.empty())
            continue;
        paths->Add(remote_path);
    }
    return paths->size();
}

void RemotyWorkspaceView::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    UpdateConfigChoices();
}

void RemotyWorkspaceView::OnWorkspaceLoaded(clWorkspaceEvent& event)
{
    event.Skip();
    UpdateConfigChoices();
}

void RemotyWorkspaceView::OnConfigSelected(wxCommandEvent& event)
{
    CallAfter(&RemotyWorkspaceView::SetBuildConfiguration, event.GetString());
}

void RemotyWorkspaceView::OnEditWorkspaceSettings(wxCommandEvent& event)
{
    CHECK_PTR_RET(m_workspace);
    wxUnusedVar(event); // load the remote workspace settings
    clFileSystemWorkspaceDlg dlg(EventNotifier::Get()->TopFrame(), &m_workspace->GetSettings());
    dlg.SetUseRemoteBrowsing(true, m_workspace->GetAccount().GetAccountName());
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }
    // save workspace settings to the remote server
    m_workspace->CallAfter(&RemotyWorkspace::SaveSettings);
}

void RemotyWorkspaceView::OnEditCodeLiteRemoteJsonFile(wxCommandEvent& event)
{
    CHECK_PTR_RET(m_workspace);
    wxUnusedVar(event);
    m_workspace->CallAfter(&RemotyWorkspace::OpenAndEditCodeLiteRemoteJson);
}

void RemotyWorkspaceView::OnBuildMenu(wxAuiToolBarEvent& event)
{
    CHECK_PTR_RET(m_workspace);
    if (!event.IsDropDownClicked()) {
        CallAfter(&RemotyWorkspaceView::BuildTarget, "build");
        return;
    }

    auto id = event.GetId();
    // Show the menu
    wxMenu menu;
    auto& settings = m_workspace->GetSettings();
    auto conf = m_workspace->GetSettings().GetSelectedConfig();
    const auto& targets = settings.GetSelectedConfig()->GetBuildTargets();
    for (const auto& [target_name, _] : targets) {
        wxString xrcid_str;
        xrcid_str << "wsp-build-" << target_name;
        int xrcid = wxXmlResource::GetXRCID(xrcid_str);
        menu.Append(xrcid, target_name, wxEmptyString, wxITEM_NORMAL);
        menu.Bind(
            wxEVT_MENU,
            [this, target_name](wxCommandEvent& event) {
                wxUnusedVar(event);
                CallAfter(&RemotyWorkspaceView::BuildTarget, target_name);
            },
            xrcid);
    }

    m_toolbar->SetToolSticky(id, true);

    // line up our menu with the button
    wxRect rect = m_toolbar->GetToolRect(id);
    wxPoint pt = m_toolbar->ClientToScreen(rect.GetBottomLeft());
    pt = ScreenToClient(pt);

    PopupMenu(&menu, pt);

    // make sure the button is "un-stuck"
    m_toolbar->SetToolSticky(id, false);
}

void RemotyWorkspaceView::UpdateConfigChoices()
{
    m_configs->Clear();
    CHECK_PTR_RET(m_workspace);

    auto& settings = m_workspace->GetSettings();
    auto conf = m_workspace->GetSettings().GetSelectedConfig();

    wxArrayString configs = settings.GetConfigs();
    m_configs->Append(configs);
    m_configs->SetStringSelection(conf->GetName());
}
