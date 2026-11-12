#pragma once
#include "project_settings_base_dlg.hpp"

class CopyCompilerSettingsDlg : public CopyCompilerSettingsDlgBase
{
protected:
    void DoUpdateConfigurations(const wxString& configToSelect = wxEmptyString);

public:
    CopyCompilerSettingsDlg(wxWindow* parent);
    ~CopyCompilerSettingsDlg() override;

    void GetCopyFrom(wxString& project, wxString& config);
};
