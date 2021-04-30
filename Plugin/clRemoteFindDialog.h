#ifndef CLREMOTEFINDDIALOG_H
#define CLREMOTEFINDDIALOG_H

#include "clThemedComboBox.hpp"
#include "codelite_exports.h"
#include "sftp_ui.h"
#include "wx/arrstr.h"

class WXDLLIMPEXP_SDK clRemoteFindDialog : public clRemoteFindDialogBase
{
protected:
    virtual void OnOK_UI(wxUpdateUIEvent& event);
    void UpdateCombo(clThemedComboBox* cb, const wxArrayString& options, const wxString& lastSelection);

public:
    clRemoteFindDialog(wxWindow* parent, const wxString& account_name);
    virtual ~clRemoteFindDialog();

    void SetWhere(const wxString& where);
    void SetFileTypes(const wxString& filetypes);
    void SetFindWhat(const wxString& findWhat);

    wxString GetWhere() const;
    wxString GetFindWhat() const;
    wxString GetFileExtensions() const;
    bool IsIcase() const;
    bool IsWholeWord() const;

    /**
     * @brief build grep command from the input provided
     */
    wxArrayString GetGrepCommand() const;
};

#endif // CLREMOTEFINDDIALOG_H
