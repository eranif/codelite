#ifndef GITBLAMEDLG_H
#define GITBLAMEDLG_H
#include "clEditorEditEventsHandler.h"
#include "cl_command_event.h"
#include "gitui.h"
#include "macros.h"
#include <map>
#include <wx/arrstr.h>
#include <wx/choice.h>
#include <wx/stc/stc.h>

class IProcess;
class GitPlugin;

class CommitStore
{
public:
    CommitStore()
        : m_index(-1)
    {
    }

    void Clear()
    {
        m_visitedCommits.Clear();
        m_index = -1;
        m_revlistOutput.Clear();
    }

    void AddCommit(const wxString& commit)
    {
        wxCHECK_RET(!commit.empty(), "Passed an empty commit");
        m_index = m_visitedCommits.Index(commit); // See if it's already here
        if(m_index == wxNOT_FOUND) {
            m_index = m_visitedCommits.Add(commit);
        }
    }
    wxString GetPreviousCommit() { return CanGoBack() ? m_visitedCommits.Item(++m_index) : ""; }
    wxString GetNextCommit() { return CanGoForward() ? m_visitedCommits.Item(--m_index) : ""; }
    wxString GetCurrentlyDisplayedCommit() const { return (m_index < 0) ? "" : m_visitedCommits.Item(m_index); }
    void SetCurrentlyDisplayedCommit(const wxString& commit)
    {
        wxCHECK_RET(!commit.empty(), "Passed an empty commit");
        m_index = m_visitedCommits.Index(commit);
        wxASSERT(m_index != wxNOT_FOUND);
    }
    wxString GetCommit(size_t n) const { return (n < m_visitedCommits.GetCount()) ? m_visitedCommits.Item(n) : ""; }
    bool CanGoBack() const { return m_index >= 0 && m_index < (int)(m_visitedCommits.GetCount() - 1); }
    bool CanGoForward() const { return m_index > 0; }
    void LoadChoice(wxChoice* choice) const
    {
        choice->Clear();
        choice->Append(m_visitedCommits);
        choice->SetSelection(m_index);
    }
    void SetRevlistOutput(const wxArrayString& commits) { m_revlistOutput = commits; }
    const wxArrayString& GetRevlistOutput() const { return m_revlistOutput; }
    wxString GetCommitParent(const wxString& commit)
    {
        wxString parent;
        for(size_t n = 0; n < m_revlistOutput.GetCount(); ++n) {
            if(m_revlistOutput.Item(n).Left(8) == commit) {
                wxString parentHash = m_revlistOutput.Item(n).AfterFirst(' ');
                if(!parentHash.empty()) {
                    parent = parentHash.Left(8);
                    break;
                }
            }
        }
        return parent;
    }

protected:
    wxArrayString m_visitedCommits;
    int m_index;
    wxArrayString m_revlistOutput;
};

class GitBlameDlg : public GitBlameDlgBase
{
public:
    GitBlameDlg(wxWindow* parent, GitPlugin* plugin);
    virtual ~GitBlameDlg();

    void SetBlame(const wxString& blame, const wxString& args);
    void OnRevListOutput(const wxString& output, const wxString& Arguments);
    void Show();

protected:
    virtual void OnExtraArgsTextEnter(wxCommandEvent& event);
    virtual void OnRefreshBlame(wxCommandEvent& event);
    virtual void OnHistoryItemSelected(wxCommandEvent& event);
    virtual void OnBackUpdateUI(wxUpdateUIEvent& event);
    virtual void OnForwardUpdateUI(wxUpdateUIEvent& event);
    virtual void OnNextBlame(wxCommandEvent& event);
    virtual void OnPreviousBlame(wxCommandEvent& event);
    virtual void OnSettings(wxCommandEvent& event);
    virtual void OnStcblameLeftDclick(wxMouseEvent& event);
    virtual void OnCloseDialog(wxCommandEvent& event);
    virtual void OnCharHook(wxKeyEvent& event);
    void GetNewCommitBlame(const wxString& commit);
    void ClearLogControls();
    void UpdateLogControls(const wxString& commit);

    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnChangeFile(wxCommandEvent& event);

    void DoCloseDialog();

    GitPlugin* m_plugin;
    clEditEventsHandler::Ptr_t m_editEventsHandler;

    CommitStore m_commitStore;
    bool m_showParentCommit;
    bool m_showLogControls;
    wxStringMap_t m_diffMap;
    int m_sashPositionMain;
    int m_sashPositionV;
    int m_sashPositionH;

    wxString m_commandOutput;
    IProcess* m_process;
};

class GitBlameSettingsDlg : public GitBlameSettingsDlgBase
{
public:
    GitBlameSettingsDlg(wxWindow* parent, bool showParentCommit, bool showLogControls);
    virtual ~GitBlameSettingsDlg() {}

protected:
};
#endif // GITBLAMEDLG_H
