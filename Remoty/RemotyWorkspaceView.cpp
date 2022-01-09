#include "RemotyWorkspaceView.hpp"

#include "RemotyWorkspace.hpp"
#include "asyncprocess.h"
#include "clFileSystemWorkspaceConfig.hpp"
#include "clFileSystemWorkspaceDlg.h"
#include "clRemoteFindDialog.h"
#include "clSFTPManager.hpp"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "processreaderthread.h"
#include "search_thread.h"
#include "ssh_account_info.h"
#include "wx/event.h"
#include "wx/tokenzr.h"
#include "wx/wxcrt.h"

#include <wx/arrstr.h>
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>

RemotyWorkspaceView::RemotyWorkspaceView(wxWindow* parent, RemotyWorkspace* workspace)
    : RemotyWorkspaceViewBase(parent)
    , m_workspace(workspace)
{
    m_tree = new clRemoteDirCtrl(this);
    GetSizer()->Add(m_tree, 1, wxEXPAND);
    GetSizer()->Fit(this);

    m_tree->Bind(wxEVT_REMOTEDIR_DIR_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnDirContextMenu, this);
    m_tree->Bind(wxEVT_REMOTEDIR_FILE_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnFileContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_FINDINFILES_DLG_SHOWING, &RemotyWorkspaceView::OnFindInFilesShowing, this);
    EventNotifier::Get()->Bind(wxEVT_FINDINFILES_OPEN_MATCH, &RemotyWorkspaceView::OnOpenFindInFilesMatch, this);
}

RemotyWorkspaceView::~RemotyWorkspaceView()
{
    m_tree->Unbind(wxEVT_REMOTEDIR_DIR_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnDirContextMenu, this);
    m_tree->Unbind(wxEVT_REMOTEDIR_FILE_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnFileContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_FINDINFILES_DLG_SHOWING, &RemotyWorkspaceView::OnFindInFilesShowing, this);
    EventNotifier::Get()->Unbind(wxEVT_FINDINFILES_OPEN_MATCH, &RemotyWorkspaceView::OnOpenFindInFilesMatch, this);
}

void RemotyWorkspaceView::OpenWorkspace(const wxString& path, const wxString& accountName)
{
    auto account = SSHAccountInfo::LoadAccount(accountName);
    if(account.GetAccountName().empty()) {
        clWARNING() << "Failed to open workspace at:" << path << "for account" << accountName << endl;
        clWARNING() << "No such account exist" << endl;
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
    if(isRootItem && m_workspace->GetSettings().GetSelectedConfig()) {
        auto& settings = m_workspace->GetSettings();
        auto conf = m_workspace->GetSettings().GetSelectedConfig();
        menu->AppendSeparator();
        // ===---
        // add "build" menu entry
        // ===---
        const auto& targets = settings.GetSelectedConfig()->GetBuildTargets();
        wxMenu* build_menu = new wxMenu;
        for(const auto& vt : targets) {
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
        for(auto config : configs) {
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
    menu->Append(XRCID("remoty-wps-settings"), _("Workspace settings..."));
    menu->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& e) {
            // load the remote workspace settings
            clFileSystemWorkspaceDlg dlg(EventNotifier::Get()->TopFrame(), &m_workspace->GetSettings());
            dlg.SetUseRemoteBrowsing(true, m_workspace->GetAccount().GetAccountName());
            if(dlg.ShowModal() != wxID_OK) {
                return;
            }
            // save workspace settings to the remote server
            m_workspace->CallAfter(&RemotyWorkspace::SaveSettings);
        },
        XRCID("remoty-wps-settings"));
}

void RemotyWorkspaceView::OnFileContextMenu(clContextMenuEvent& event) { event.Skip(); }

void RemotyWorkspaceView::OnFindInFilesShowing(clFindInFilesEvent& event)
{
    event.Skip();
    if(!m_workspace->IsOpened())
        return;

    // override the default find in files dialog
    event.Skip(false);
    wxString rootpath = m_workspace->GetRemoteWorkspaceFile();
    rootpath = rootpath.BeforeLast('/'); // only the path

    clRemoteFindDialog dlg(nullptr, m_workspace->GetAccount().GetAccountName(), rootpath);
    auto editor = ::clGetManager()->GetActiveEditor();
    if(editor && (editor->GetSelectionStart() != editor->GetSelectionEnd())) {
        dlg.SetFindWhat(editor->GetCtrl()->GetSelectedText());
    }

    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    // start the search
    m_workspace->FindInFiles(dlg.GetWhere(), dlg.GetFileExtensions(), dlg.GetFindWhat(), dlg.IsWholeWord(),
                             dlg.IsIcase());
}

void RemotyWorkspaceView::OnOpenFindInFilesMatch(clFindInFilesEvent& event)
{
    event.Skip();
    if(!m_workspace->IsOpened() || event.GetMatches().empty())
        return;

    // this is ours to handle
    event.Skip(false);
    const auto& match = event.GetMatches()[0];
    const auto& loc = match.locations[0];

    // if the file is already opened, just show it
    IEditor* editor = clGetManager()->FindEditor(match.file);
    if(!editor) {
        editor = clSFTPManager::Get().OpenFile(match.file, m_workspace->GetAccount().GetAccountName());
    }

    if(editor) {
        // sci is 0 based line numbers
        auto callback = [=](IEditor* peditor) {
            peditor->GetCtrl()->ClearSelections();
            int pos_start = peditor->PosFromLine(loc.line - 1) + loc.column_start;
            int pos_end = peditor->PosFromLine(loc.line - 1) + loc.column_end;
            peditor->GetCtrl()->SetSelection(pos_start, pos_end);
            peditor->CenterLinePreserveSelection(loc.line);
        };
        // load the local file (or make it selected) and then focus on the matched line
        clGetManager()->OpenFileAndAsyncExecute(editor->GetFileName().GetFullPath(), callback);
    }
}

void RemotyWorkspaceView::SetBuildConfiguration(const wxString& config)
{
    m_workspace->GetSettings().SetSelectedConfig(config);
}

void RemotyWorkspaceView::BuildTarget(const wxString& name) { m_workspace->BuildTarget(name); }
