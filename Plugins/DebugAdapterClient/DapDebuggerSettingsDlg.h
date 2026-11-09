#pragma once
#include "UI.hpp"
#include "clDapSettingsStore.hpp"

class clPropertiesPage;
class DapDebuggerSettingsDlg : public DapDebuggerSettingsDlgBase
{
    clDapSettingsStore& m_store;

protected:
    void OnScan(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnOpenHelpDocs(wxCommandEvent& event);
    void OnItemsUI(wxUpdateUIEvent& event);

private:
    void Initialise();

public:
    DapDebuggerSettingsDlg(wxWindow* parent, clDapSettingsStore& store);
    ~DapDebuggerSettingsDlg() override = default;
};
