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
    virtual void OnDelete(wxCommandEvent& event);
    virtual void OnDeleteUI(wxUpdateUIEvent& event);
    virtual void OnEditTarget(wxCommandEvent& event);
    virtual void OnEditTargetUI(wxUpdateUIEvent& event);
    virtual void OnNewTarget(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
};
#endif // CLFILESYSTEMWORKSPACEDLG_H
