#include "GitConsole.h"
#include "git.h"

GitConsole::GitConsole(wxWindow* parent, GitPlugin* git)
    : GitConsoleBase(parent)
    , m_git(git)
{
    m_stc->SetReadOnly(true);
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
}

void GitConsole::OnClearGitLog(wxCommandEvent& event)
{
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
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
    event.Enable(!m_stc->IsEmpty());
}

void GitConsole::AddText(const wxString& text)
{
    m_stc->SetReadOnly(false);
    m_stc->AppendText(text + "\n");
    m_stc->SetReadOnly(true);
    
    m_stc->SetSelectionEnd(m_stc->GetLength());
    m_stc->SetSelectionStart(m_stc->GetLength());
    m_stc->SetCurrentPos(m_stc->GetLength());

    m_stc->EnsureCaretVisible();
}

void GitConsole::AddRawText(const wxString& text)
{
    m_stc->SetReadOnly(false);
    m_stc->AppendText(text);
    m_stc->SetReadOnly(true);
    m_stc->SetSelectionEnd(m_stc->GetLength());
    m_stc->SetSelectionStart(m_stc->GetLength());
    m_stc->SetCurrentPos(m_stc->GetLength());

    m_stc->EnsureCaretVisible();
}

void GitConsole::EnsureVisible()
{
    Notebook* book = m_git->GetManager()->GetOutputPaneNotebook();
    for(size_t i=0; i<book->GetPageCount(); i++) {
        if(this == book->GetPage(i)) {
            book->SetSelection(i);
            break;
        }
    }
}

bool GitConsole::IsVerbose() const
{
    return m_checkBoxVerbose->IsChecked();
}
