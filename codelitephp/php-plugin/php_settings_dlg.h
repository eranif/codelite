#ifndef __php_settings_dlg__
#define __php_settings_dlg__

#include "php_ui.h"

class PHPSettingsDlg : public PHPSettingsBaseDlg
{
protected:
    // Handlers for PHPSettingsBaseDlg events.
    void OnBrowseForIncludePath(wxCommandEvent& event);
    void OnAddCCPath(wxCommandEvent& event);
    void OnUpdateApplyUI(wxCommandEvent& event);

public:
    PHPSettingsDlg(wxWindow* parent);
    virtual ~PHPSettingsDlg();
    void OnOK(wxCommandEvent& event);
};

#endif // __php_settings_dlg__
