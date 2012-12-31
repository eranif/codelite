#include "GitConsole.h"
#include "git.h"
#include "cl_config.h"
#include "gitentry.h"
#include <wx/datetime.h>
#include "event_notifier.h"

GitConsole::GitConsole(wxWindow* parent, GitPlugin* git)
    : GitConsoleBase(parent)
    , m_git(git)
{
    EventNotifier::Get()->Connect(wxEVT_GIT_CONFIG_CHANGED, wxCommandEventHandler(GitConsole::OnConfigurationChanged), NULL, this);
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_isVerbose = (data.GetFlags() & GitEntry::Git_Verbose_Log);

    GitImages m_images;
    m_auibar->AddTool(XRCID("git_settings"), wxT("GIT plugin settings"), m_images.Bitmap("gitSettings"), wxT("GIT plugin settings"));
    m_auibar->AddSeparator();
    m_auibar->AddTool(XRCID("git_set_repository"), wxT("Set GIT repository path"), m_images.Bitmap("gitPath"), wxT("Set GIT repository path"));
    m_auibar->AddTool(XRCID("git_clone"), wxT("Clone"), m_images.Bitmap("gitClone"), wxT("Clone git URL"));
    m_auibar->AddSeparator();
    m_auibar->AddTool(XRCID("git_pull"), wxT("Pull"), m_images.Bitmap("gitPull"), wxT("Pull remote changes"));
    m_auibar->AddTool(XRCID("git_commit"), wxT("Commit"), m_images.Bitmap("gitCommitLocal"), wxT("Commit local changes"));
    m_auibar->AddTool(XRCID("git_push"), wxT("Push"), m_images.Bitmap("gitPush"), wxT("Push local changes"));
    m_auibar->AddSeparator();
    m_auibar->AddTool(XRCID("git_refresh"), wxT("Refresh"), m_images.Bitmap("gitRefresh"), wxT("Refresh tracked file list"));
    m_auibar->AddTool(XRCID("git_reset_repository"), wxT("Reset"), m_images.Bitmap("gitReset"), wxT("Reset repository"));
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
    EventNotifier::Get()->Disconnect(wxEVT_GIT_CONFIG_CHANGED, wxCommandEventHandler(GitConsole::OnConfigurationChanged), NULL, this);
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
    cols.push_back(wxDateTime::Now().FormatISOTime());
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
        cols.push_back(i == 0 ? wxDateTime::Now().FormatISOTime() : "...");
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

