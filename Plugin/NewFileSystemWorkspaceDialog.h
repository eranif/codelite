#ifndef NEWFILESYSTEMWORKSPACEDIALOG_H
#define NEWFILESYSTEMWORKSPACEDIALOG_H

#include "clFileSystemWorkspaceDlgBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK NewFileSystemWorkspaceDialog : public NewFileSystemWorkspaceDialogBase
{
    bool m_autoSetNameFromPath = true;

public:
    NewFileSystemWorkspaceDialog(wxWindow* parent, bool autoSetNameFromPath = true);
    virtual ~NewFileSystemWorkspaceDialog();

    wxString GetWorkspaceName() const;
    wxString GetWorkspacePath() const;

protected:
    virtual void OnDirSelected(wxFileDirPickerEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NEWFILESYSTEMWORKSPACEDIALOG_H
