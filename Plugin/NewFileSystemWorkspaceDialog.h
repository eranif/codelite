#ifndef NEWFILESYSTEMWORKSPACEDIALOG_H
#define NEWFILESYSTEMWORKSPACEDIALOG_H
#include "clFileSystemWorkspaceDlgBase.h"

class NewFileSystemWorkspaceDialog : public NewFileSystemWorkspaceDialogBase
{
public:
    NewFileSystemWorkspaceDialog(wxWindow* parent);
    virtual ~NewFileSystemWorkspaceDialog();

    wxString GetWorkspaceName() const;
    wxString GetWorkspacePath() const;

protected:
    virtual void OnDirSelected(wxFileDirPickerEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NEWFILESYSTEMWORKSPACEDIALOG_H
