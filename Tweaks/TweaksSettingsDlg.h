#ifndef TWEAKSSETTINGSDLG_H
#define TWEAKSSETTINGSDLG_H
#include "wxcrafter.h"

class TweaksSettingsDlg : public TweaksSettingsDlgBase
{
public:
    TweaksSettingsDlg(wxWindow* parent);
    virtual ~TweaksSettingsDlg();

protected:
    virtual void OnDelete(wxCommandEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnEditUI(wxUpdateUIEvent& event);
    virtual void OnEdit(wxCommandEvent& event);
    virtual void OnAddProject(wxCommandEvent& event);
    virtual void OnWorkspaceOpenUI(wxUpdateUIEvent& event);

protected:
    bool IsExists(const wxString &projectName) const;
    void DoPopulateList();
};
#endif // TWEAKSSETTINGSDLG_H
