#ifndef GITCONSOLE_H
#define GITCONSOLE_H
#include "gitui.h"

class GitPlugin;
class GitConsole : public GitConsoleBase
{
    GitPlugin* m_git;
    bool       m_isVerbose;
    
public:
    GitConsole(wxWindow* parent, GitPlugin* git);
    virtual ~GitConsole();
    void AddText(const wxString &text);
    void AddRawText(const wxString &text);
    bool IsVerbose() const;
    
protected:
    virtual void OnStopGitProcessUI(wxUpdateUIEvent& event);
    virtual void OnClearGitLogUI(wxUpdateUIEvent& event);
    virtual void OnClearGitLog(wxCommandEvent& event);
    virtual void OnStopGitProcess(wxCommandEvent& event);
    void OnConfigurationChanged(wxCommandEvent &e);

};
#endif // GITCONSOLE_H
