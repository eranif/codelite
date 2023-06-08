#ifndef CLREMOTEFINDDIALOG_H
#define CLREMOTEFINDDIALOG_H

#include "clThemedComboBox.hpp"
#include "codelite_exports.h"
#include "sessionmanager.h"
#include "sftp_ui.h"

#include <wx/arrstr.h>

class WXDLLIMPEXP_SDK clRemoteFindDialog : public clRemoteFindDialogBase
{
    wxString m_root_path;
    FindInFilesSession m_data;
    bool m_isReplace = false;

protected:
    virtual void OnTool(wxCommandEvent& event);
    virtual void OnReplaceUI(wxUpdateUIEvent& event);
    virtual void OnFindUI(wxUpdateUIEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnReplace(wxCommandEvent& event);

    void DoShowSearchControls(bool show);
    void DoShowReplaceControls(bool show);
    void DoShowControls();

public:
    clRemoteFindDialog(wxWindow* parent, const wxString& account_name, const wxString& rootpath);
    virtual ~clRemoteFindDialog();

    void SetFileTypes(const wxString& filetypes);
    void SetFindWhat(const wxString& findWhat);

    wxString GetWhere() const;
    wxString GetFindWhat() const;
    wxString GetReplaceWith() const;
    wxString GetFileExtensions() const;
    bool IsIcase() const;
    bool IsWholeWord() const;
    bool IsReplaceAction() const { return m_isReplace; }
};

#endif // CLREMOTEFINDDIALOG_H
