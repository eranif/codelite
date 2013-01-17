#ifndef SVNCOMMITDIALOG_H
#define SVNCOMMITDIALOG_H

#include "wxcrafter.h"
#include "macros.h"

class Subversion2;
class IProcess;
class SvnCommitDialog : public SvnCommitDialogBaseClass
{
    Subversion2 * m_plugin;
    wxString      m_url;
    wxString      m_repoPath;
    IProcess*     m_process;
    wxString      m_output;
    wxStringMap_t m_cache;
    wxString      m_currentFile;
    
public:
    static wxString NormalizeMessage(const wxString &message);
    void OnChoiceMessage(wxCommandEvent &e);

public:
    SvnCommitDialog(wxWindow* parent, Subversion2* plugin);
    SvnCommitDialog(wxWindow* parent, const wxArrayString &paths, const wxString &url, Subversion2 *plugin, const wxString &repoPath);
    virtual ~SvnCommitDialog();

    wxString GetMesasge();
    wxArrayString GetPaths();
    
protected:  
    void DoShowDiff(int selection);
    
protected:
    DECLARE_EVENT_TABLE()
    
    virtual void OnFileSelected(wxCommandEvent& event);
    void OnProcessOutput(wxCommandEvent &e);
    void OnProcessTerminatd(wxCommandEvent &e);
    
};

#endif // SVNCOMMITDIALOG_H
