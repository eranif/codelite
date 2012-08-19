#ifndef SVNCOMMITDIALOG_H
#define SVNCOMMITDIALOG_H

#include "wxcrafter.h"
class Subversion2;

class SvnCommitDialog : public SvnCommitDialogBaseClass
{
    Subversion2 *m_plugin;
    wxString     m_url;
    
public:
	static wxString NormalizeMessage(const wxString &message);
	void OnChoiceMessage(wxCommandEvent &e);
    
public:
    SvnCommitDialog(wxWindow* parent, Subversion2* plugin);
    SvnCommitDialog(wxWindow* parent, const wxArrayString &paths, const wxString &url, Subversion2 *plugin);
    virtual ~SvnCommitDialog();
    
    wxString GetMesasge();
	wxArrayString GetPaths();
};

#endif // SVNCOMMITDIALOG_H
