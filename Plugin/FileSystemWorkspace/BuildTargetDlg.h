#ifndef BUILDTARGETDLG_H
#define BUILDTARGETDLG_H
#include "clFileSystemWorkspaceDlgBase.h"

class BuildTargetDlg : public BuildTargetDlgBase
{
public:
    BuildTargetDlg(wxWindow* parent, const wxString& name, const wxString& command);
    virtual ~BuildTargetDlg();

    wxString GetTargetName() const;
    wxString GetTargetCommand() const;

protected:
    virtual void OnOK_UI(wxUpdateUIEvent& event);
};
#endif // BUILDTARGETDLG_H
