#ifndef BUILDORDERDIALOG_H
#define BUILDORDERDIALOG_H

#include "buildorderbasepage.h"
#include <wx/arrstr.h>

class BuildOrderDialog : public BuildOrderDialogBase
{
    wxString m_projectName;
    wxString m_current_configuration;
    bool m_dirty = false;

public:
    BuildOrderDialog(wxWindow* parent, const wxString& projectName);
    virtual ~BuildOrderDialog();

protected:
    void Initialise(const wxString& config);
    void DoPopulateControl(const wxString& configuration);
    void Save(const wxString& config_name);
    void DoMoveUp(int index, const wxString& projectName);
    wxArrayString GetBuildOrderProjects() const;

protected:
    void OnButtonOK(wxCommandEvent& event) override;
    void OnApplyButton(wxCommandEvent& event) override;
    void OnApplyButtonUI(wxUpdateUIEvent& event) override;
    void OnConfigChanged(wxCommandEvent& event) override;
    void OnMoveDown(wxCommandEvent& event) override;
    void OnMoveLeft(wxCommandEvent& event) override;
    void OnMoveLeftUI(wxUpdateUIEvent& event) override;
    void OnMoveRight(wxCommandEvent& event) override;
    void OnMoveRightUI(wxUpdateUIEvent& event) override;
    void OnMoveUp(wxCommandEvent& event) override;
};
#endif // BUILDORDERDIALOG_H
