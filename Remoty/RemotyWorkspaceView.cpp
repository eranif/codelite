#include "RemotyWorkspace.hpp"
#include "RemotyWorkspaceView.hpp"
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
#include <wx/msgdlg.h>

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

void RemotyWorkspaceView::CloseWorkspace()
{
    m_tree->Close(false);
}

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
                m_workspace->CallAfter(&RemotyWorkspace::SaveSettings);
            },
            XRCID("remoty-wps-settings"));
    }
}

void RemotyWorkspaceView::OnFileContextMenu(clContextMenuEvent& event)
{
    event.Skip();
}

void RemotyWorkspaceView::OnFindInFilesShowing(clFindInFilesEvent& event)
{
    event.Skip();
    if(!m_workspace->IsOpened())
        return;

    // override the default find in files dialog
    event.Skip(false);
    clRemoteFindDialog dlg(nullptr, m_workspace->GetAccount().GetAccountName());
    auto editor = ::clGetManager()->GetActiveEditor();
    if(editor && (editor->GetSelectionStart() != editor->GetSelectionEnd())) {
        dlg.SetFindWhat(editor->GetCtrl()->GetSelectedText());
    }

    wxArrayString folders;
    m_tree->GetSelectedFolders(folders);
    if(!folders.empty()) {
        // use the selected folder
        dlg.SetWhere(folders[0]);
    } else if(editor) {
        // use the current file's path
        wxString remote_path = editor->GetRemotePath();
        if(!remote_path.empty()) {
            remote_path = remote_path.BeforeLast('/');
            dlg.SetWhere(remote_path);
        }
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
        int pos_start = editor->PosFromLine(loc.line - 1) + loc.column_start;
        int pos_end = editor->PosFromLine(loc.line - 1) + loc.column_end;
        editor->GetCtrl()->GotoLine(loc.line - 1);
        editor->GetCtrl()->SetSelection(pos_start, pos_end);
    }
}
