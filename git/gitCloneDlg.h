#ifndef GITCLONEDLG_H
#define GITCLONEDLG_H

#include "gitui.h"

class gitCloneDlg : public gitCloneDlgBaseClass
{
public:
    gitCloneDlg(wxWindow* parent);
    virtual ~gitCloneDlg();

protected:
    virtual void OnUseCredentialsUI(wxUpdateUIEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);

public:
    wxString GetCloneURL() const ;
    wxString GetTargetDirectory() const {
        return m_dirPickerTargetDir->GetPath();
    }
};
#endif // GITCLONEDLG_H
