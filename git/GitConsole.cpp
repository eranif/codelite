#include "GitConsole.h"
#include "git.h"
#include "cl_config.h"
#include "gitentry.h"
#include <wx/datetime.h>
#include "event_notifier.h"
#include "bitmap_loader.h"
#include <algorithm>
#include "fileextmanager.h"
#include <wx/icon.h>
#include <wx/tokenzr.h>

class GitClientData : public wxClientData
{
    wxString m_path;
public:
    GitClientData(const wxString &path) : m_path(path) {}
    virtual ~GitClientData() {}

    void SetPath(const wxString& path) {
        this->m_path = path;
    }
    const wxString& GetPath() const {
        return m_path;
    }
};
// we use a custom column randerer so can have a better control over the font
class GitMyTextRenderer : public wxDataViewCustomRenderer
{
    wxDataViewListCtrl *m_listctrl;
    wxVariant           m_value;
    wxFont              m_font;
public:
    GitMyTextRenderer(wxDataViewListCtrl *listctrl) : m_listctrl(listctrl) {
        m_font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_font.SetFamily(wxFONTFAMILY_TELETYPE);

        wxBitmap bmp(1, 1);
        wxMemoryDC dc;
        dc.SelectObject(bmp);
        int xx, yy;
        wxFont f = m_font;
        dc.GetTextExtent("Tp", &xx, &yy, NULL, NULL, &f);
        m_listctrl->SetRowHeight( yy );
    }

    virtual ~GitMyTextRenderer() {}

    virtual wxSize GetSize() const {
        int xx, yy;
        wxBitmap bmp(1, 1);
        wxMemoryDC dc;
        dc.SelectObject(bmp);

        wxString s = m_value.GetString();
        wxFont f = m_font;
        dc.GetTextExtent(s, &xx, &yy, NULL, NULL, &f);
        return wxSize(xx, yy);
    }

    virtual bool SetValue(const wxVariant& value) {
        m_value = value;
        return true;
    }

    virtual bool GetValue(wxVariant& value) const {
        value = m_value;
        return true;
    }

    virtual bool Render(wxRect cell, wxDC *dc, int state) {
        wxVariant v;
        GetValue(v);
        wxString str = v.GetString();
        str.Trim();
        wxPoint pt = cell.GetTopLeft();
        wxFont f = m_font;
        dc->SetFont(f);
        dc->DrawText(str, pt);
        return true;
    }
};

// ---------------------------------------------------------------------

GitConsole::GitConsole(wxWindow* parent, GitPlugin* git)
    : GitConsoleBase(parent)
    , m_git(git)
{
    m_bitmapLoader = new BitmapLoader();
    GitImages m_images;
    m_bitmaps = m_bitmapLoader->MakeStandardMimeMap();
    m_modifiedBmp  = m_bitmapLoader->LoadBitmap("subversion/16/modified");
    m_untrackedBmp = m_bitmapLoader->LoadBitmap("subversion/16/unversioned");
    m_folderBmp    = m_bitmapLoader->LoadBitmap("mime/16/folder");
    m_newBmp    = m_images.Bitmap("gitFileAdd");

    EventNotifier::Get()->Connect(wxEVT_GIT_CONFIG_CHANGED, wxCommandEventHandler(GitConsole::OnConfigurationChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(GitConsole::OnWorkspaceClosed), NULL, this);

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_isVerbose = (data.GetFlags() & GitEntry::Git_Verbose_Log);

    m_splitter->SetSashPosition(data.GetGitConsoleSashPos());

    int screenWidth = 1000;// use a long screen width to allow long lines
    m_dvListCtrl->AppendColumn(new wxDataViewColumn(_("Git Log"), new GitMyTextRenderer(m_dvListCtrl), 0, screenWidth, wxALIGN_LEFT));

    m_auibar->AddTool(XRCID("git_refresh"), wxT("Refresh"), m_images.Bitmap("gitRefresh"), wxT("Refresh tracked file list"));
    m_auibar->AddTool(XRCID("git_reset_repository"), wxT("Reset"), m_images.Bitmap("gitResetRepo"), wxT("Reset repository"));
    m_auibar->AddSeparator();

    m_auibar->AddTool(XRCID("git_pull"), wxT("Pull"), m_images.Bitmap("gitPull"), wxT("Pull remote changes"));
    m_auibar->AddTool(XRCID("git_commit"), wxT("Commit"), m_images.Bitmap("gitCommitLocal"), wxT("Commit local changes"));
    m_auibar->AddTool(XRCID("git_push"), wxT("Push"), m_images.Bitmap("gitPush"), wxT("Push local changes"));
    m_auibar->AddSeparator();
    
    m_auibar->AddTool(XRCID("git_create_branch"), wxT("Create branch"), m_images.Bitmap("gitNewBranch"), wxT("Create local branch"));
    m_auibar->AddTool(XRCID("git_switch_branch"), wxT("Local branch"), m_images.Bitmap("gitSwitchLocalBranch"), wxT("Switch to local branch"));
    //m_auibar->AddTool(XRCID("git_switch_to_remote_branch"), wxT("Remote branch"), XPM_BITMAP(menuexport), wxT("Init and switch to remote branch"));
    m_auibar->AddSeparator();
#if 0
    //m_auibar->AddTool(XRCID("git_bisect_start"), wxT("Bisect"), XPM_BITMAP(menu_start_bisect), wxT("Start bisect"));
    //m_auibar->EnableTool(XRCID("git_bisect_start"),false);
    //m_auibar->AddTool(XRCID("git_bisect_good"), wxT("Good commit"), wxArtProvider::GetIcon(wxART_TICK_MARK), wxT("Mark commit as good"));
    //m_auibar->EnableTool(XRCID("git_bisect_good"),false);
    //m_auibar->AddTool(XRCID("git_bisect_bad"), wxT("Bad commit"), wxArtProvider::GetIcon(wxART_ERROR), wxT("Mark commit as bad"));
    //m_auibar->EnableTool(XRCID("git_bisect_bad"),false);
    //m_auibar->AddTool(XRCID("git_bisect_reset"), wxT("End bisect"), wxArtProvider::GetIcon(wxART_QUIT), wxT("Quit bisect"));
    //m_auibar->EnableTool(XRCID("git_bisect_reset"),false);
    //m_auibar->AddSeparator();
#endif
    m_auibar->AddTool(XRCID("git_commit_diff"), wxT("Diffs"), m_images.Bitmap("gitDiffs"), wxT("Show current diffs"));
    m_auibar->AddTool(XRCID("git_browse_commit_list"), wxT("Log"), m_images.Bitmap("gitCommitedFiles"), wxT("Browse commit history"));
    m_auibar->AddTool(XRCID("git_start_gitk"), wxT("gitk"), m_images.Bitmap("gitStart"), wxT("Start gitk"));
    m_auibar->Realize();
}

GitConsole::~GitConsole()
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    data.SetGitConsoleSashPos(m_splitter->GetSashPosition());
    conf.WriteItem(&data);

    wxDELETE(m_bitmapLoader);
    EventNotifier::Get()->Disconnect(wxEVT_GIT_CONFIG_CHANGED, wxCommandEventHandler(GitConsole::OnConfigurationChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(GitConsole::OnWorkspaceClosed), NULL, this);
}

void GitConsole::OnClearGitLog(wxCommandEvent& event)
{
    m_dvListCtrl->DeleteAllItems();
}

void GitConsole::OnStopGitProcess(wxCommandEvent& event)
{
    if ( m_git->GetProcess() ) {
        m_git->GetProcess()->Terminate();
    }
}

void GitConsole::OnStopGitProcessUI(wxUpdateUIEvent& event)
{
    event.Enable(m_git->GetProcess());
}

void GitConsole::OnClearGitLogUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dvListCtrl->GetItemCount() );
}

void GitConsole::AddText(const wxString& text)
{
    wxString tmp = text;
    tmp.Trim().Trim(false);

    if ( tmp.IsEmpty() )
        return;

    wxVector<wxVariant> cols;
    cols.push_back(tmp);
    m_dvListCtrl->AppendItem(cols, (wxUIntPtr)NULL);
    wxDataViewItem item = m_dvListCtrl->GetStore()->GetItem(m_dvListCtrl->GetItemCount() - 1);
    if ( item.IsOk() ) {
        m_dvListCtrl->EnsureVisible( item );
    }

}

void GitConsole::AddRawText(const wxString& text)
{
    wxString tmp = text;
    tmp.Trim().Trim(false);

    if ( tmp.IsEmpty() )
        return;

    wxArrayString lines = ::wxStringTokenize(tmp, "\n\r", wxTOKEN_STRTOK);
    for(size_t i=0; i<lines.GetCount(); ++i) {
        wxVector<wxVariant> cols;
        cols.push_back(lines.Item(i));
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)NULL);
    }

    wxDataViewItem item = m_dvListCtrl->GetStore()->GetItem(m_dvListCtrl->GetItemCount() - 1);
    if ( item.IsOk() ) {
        m_dvListCtrl->EnsureVisible( item );
    }
}


bool GitConsole::IsVerbose() const
{
    return m_isVerbose;
}

void GitConsole::OnConfigurationChanged(wxCommandEvent& e)
{
    e.Skip();
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_isVerbose = (data.GetFlags() & GitEntry::Git_Verbose_Log);
}

static wxVariant MakeIconText(const wxString& text, const wxBitmap& bmp)
{
    wxIcon icn;
    icn.CopyFromBitmap( bmp);
    wxDataViewIconText ict(text, icn);
    wxVariant v;
    v << ict;
    return v;
}

void GitConsole::UpdateTreeView(const wxString& output)
{
    m_dvFilesModel->Clear();
    wxVector<wxVariant> cols;

    cols.clear();
    cols.push_back(MakeIconText(_("Modified"), m_modifiedBmp));
    m_itemModified  = m_dvFilesModel->AppendItem(wxDataViewItem(0), cols, new wxStringClientData("Modified"));

    cols.clear();
    cols.push_back(MakeIconText(_("New Files"), m_newBmp));
    m_itemNew  = m_dvFilesModel->AppendItem(wxDataViewItem(0), cols, new wxStringClientData("New Files"));

    cols.clear();
    cols.push_back(MakeIconText(_("Untracked"), m_untrackedBmp));
    m_itemUntracked = m_dvFilesModel->AppendItem(wxDataViewItem(0), cols, new wxStringClientData("Untracked"));

    wxArrayString files = ::wxStringTokenize(output, "\n\r", wxTOKEN_STRTOK);
    std::sort(files.begin(), files.end());

    for(size_t i=0; i<files.GetCount(); ++i) {
        wxString filename = files.Item(i);
        filename.Trim().Trim(false);
        filename.Replace("\t", " ");
        wxString prefix = filename.BeforeFirst(' ');
        filename = filename.AfterFirst(' ');

        prefix.Trim().Trim(false);
        filename.Trim().Trim(false);

        wxBitmap bmp;
        if ( filename.EndsWith("/") ) {
            bmp = m_folderBmp;
        } else if ( m_bitmaps.count(FileExtManager::GetType(filename)) ) {
            bmp = m_bitmaps[FileExtManager::GetType(filename)];
        } else {
            bmp = m_bitmaps[FileExtManager::TypeText];
        }

        cols.clear();
        cols.push_back(MakeIconText(filename, bmp));

        if ( (prefix == "M") ) {
            m_dvFilesModel->AppendItem(m_itemModified, cols, new GitClientData(filename));

        } else if ( prefix == "A" ) {
            m_dvFilesModel->AppendItem(m_itemNew, cols, new GitClientData(filename));

        } else {
            m_dvFilesModel->AppendItem(m_itemUntracked, cols, new GitClientData(filename));
        }
    }
#ifndef __WXMAC__
    if ( !m_dvFilesModel->HasChildren(m_itemModified) )
        m_dvFilesModel->DeleteItem(m_itemModified);
    else
        m_dvFiles->Expand(m_itemModified);
        
    if ( !m_dvFilesModel->HasChildren(m_itemUntracked) )
        m_dvFilesModel->DeleteItem(m_itemUntracked);

    if ( !m_dvFilesModel->HasChildren(m_itemNew) )
        m_dvFilesModel->DeleteItem(m_itemNew);
    else
        m_dvFiles->Expand(m_itemNew);
#endif        
}

void GitConsole::OnContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("git_console_add_file"), _("Add file"));
    menu.Append(XRCID("git_console_reset_file"), _("Reset file"));
    m_dvFiles->PopupMenu( &menu );
}

void GitConsole::OnAddFile(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvFiles->GetSelections(items);
    wxArrayString files;
    for(size_t i=0; i<items.GetCount(); ++i) {
        GitClientData* gcd = dynamic_cast<GitClientData*>(m_dvFilesModel->GetClientObject( items.Item(i) ));
        if ( gcd ) {
            files.push_back( gcd->GetPath() );
        }
    }

    if ( !files.IsEmpty() ) {
        m_git->AddFiles( files );
    }
}

void GitConsole::OnResetFile(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvFiles->GetSelections(items);
    wxArrayString filesToRemove, filesToRevert;
    for(size_t i=0; i<items.GetCount(); ++i) {
        wxString parentNodeName;
        wxDataViewItem parent = m_dvFilesModel->GetParent(items.Item(i));
        if ( parent.IsOk() ) {
            wxStringClientData* gcd = dynamic_cast<wxStringClientData*>(m_dvFilesModel->GetClientObject(parent ));
            parentNodeName = gcd->GetData();
        }

        GitClientData* gcd = dynamic_cast<GitClientData*>(m_dvFilesModel->GetClientObject( items.Item(i) ));
        if ( gcd ) {
            if ( parentNodeName == "New Files" ) {
                filesToRemove.push_back( gcd->GetPath() );

            } else if ( parentNodeName == "Modified" ) {
                filesToRevert.push_back( gcd->GetPath() );
            }
        }
    }

    if ( !filesToRevert.IsEmpty() ) {
        m_git->ResetFiles( filesToRevert );
    }

    if ( !filesToRemove.IsEmpty() ) {
        m_git->UndoAddFiles( filesToRemove );
    }
}
void GitConsole::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_dvFilesModel->Clear();
}
void GitConsole::OnItemSelectedUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dvFiles->GetSelectedItemsCount() );
}
