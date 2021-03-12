#ifndef CLFILESYSTEMWORKSPACEDLG_H
#define CLFILESYSTEMWORKSPACEDLG_H
#include "clFileSystemWorkspaceConfig.hpp"
#include "clFileSystemWorkspaceDlgBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clFileSystemWorkspaceDlg : public clFileSystemWorkspaceDlgBase
{
    clFileSystemWorkspaceSettings* m_settings = nullptr;
    bool m_usingGlobalSettings = true;

public:
    clFileSystemWorkspaceDlg(wxWindow* parent, clFileSystemWorkspaceSettings* settings = nullptr);
    virtual ~clFileSystemWorkspaceDlg();

protected:
    virtual void OnDeleteConfig(wxCommandEvent& event);
    virtual void OnDeleteConfigUI(wxUpdateUIEvent& event);
    virtual void OnNewConfig(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
};
#endif // CLFILESYSTEMWORKSPACEDLG_H
