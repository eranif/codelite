#include "RemoteWorkspace.hpp"
#include "RemotyWorkspaceView.hpp"
#include "clFileSystemWorkspaceConfig.hpp"
#include "clFileSystemWorkspaceDlg.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "ssh_account_info.h"
#include <wx/msgdlg.h>

RemotyWorkspaceView::RemotyWorkspaceView(wxWindow* parent, RemoteWorkspace* workspace)
    : RemotyWorkspaceViewBase(parent)
    , m_workspace(workspace)
{
    m_tree = new clRemoteDirCtrl(this);
    GetSizer()->Add(m_tree, 1, wxEXPAND);
    GetSizer()->Fit(this);

    m_tree->Bind(wxEVT_REMOTEDIR_DIR_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnDirContextMenu, this);
    m_tree->Bind(wxEVT_REMOTEDIR_FILE_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnFileContextMenu, this);
}

RemotyWorkspaceView::~RemotyWorkspaceView()
{
    m_tree->Unbind(wxEVT_REMOTEDIR_DIR_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnDirContextMenu, this);
    m_tree->Unbind(wxEVT_REMOTEDIR_FILE_CONTEXT_MENU_SHOWING, &RemotyWorkspaceView::OnFileContextMenu, this);
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
    if(isRootItem) {
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
                m_workspace->CallAfter(&RemoteWorkspace::SaveSettings);
            },
            XRCID("remoty-wps-settings"));
    }
}

void RemotyWorkspaceView::OnFileContextMenu(clContextMenuEvent& event) { event.Skip(); }
