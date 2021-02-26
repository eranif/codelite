#ifndef CLFSWNEWCONFIGDLG_H
#define CLFSWNEWCONFIGDLG_H

#include "clFileSystemWorkspaceDlgBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clFSWNewConfigDlg : public clFSWNewConfigDlgBase
{
public:
    clFSWNewConfigDlg(wxWindow* parent);
    virtual ~clFSWNewConfigDlg();

    wxString GetCopyFrom() const;
    wxString GetConfigName() const;

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // CLFSWNEWCONFIGDLG_H
