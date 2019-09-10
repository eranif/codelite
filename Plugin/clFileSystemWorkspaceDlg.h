#ifndef CLFILESYSTEMWORKSPACEDLG_H
#define CLFILESYSTEMWORKSPACEDLG_H
#include "clFileSystemWorkspaceDlgBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clFileSystemWorkspaceDlg : public clFileSystemWorkspaceDlgBase
{
public:
    clFileSystemWorkspaceDlg(wxWindow* parent);
    virtual ~clFileSystemWorkspaceDlg();

protected:
    virtual void OnDeleteConfig(wxCommandEvent& event);
    virtual void OnDeleteConfigUI(wxUpdateUIEvent& event);
    virtual void OnNewConfig(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
};
#endif // CLFILESYSTEMWORKSPACEDLG_H
