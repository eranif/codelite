#ifndef COPYCOMPILERSETTINGSDLG_H
#define COPYCOMPILERSETTINGSDLG_H
#include "project_settings_base_dlg.h"

class CopyCompilerSettingsDlg : public CopyCompilerSettingsDlgBase
{
protected:
    void DoUpdateConfigurations(const wxString& configToSelect = wxEmptyString);

public:
    CopyCompilerSettingsDlg(wxWindow* parent);
    virtual ~CopyCompilerSettingsDlg();

    void GetCopyFrom(wxString& project, wxString& config);
};
#endif // COPYCOMPILERSETTINGSDLG_H
