#ifndef CLBOOSTRAPWIZARD_H
#define CLBOOSTRAPWIZARD_H
#include "clBoostrapWizardBase.h"

class clBoostrapWizard : public clBoostrapWizardBase
{
public:
    clBoostrapWizard(wxWindow* parent);
    virtual ~clBoostrapWizard();
protected:
    virtual void OnCheckAllPlugins(wxCommandEvent& event);
    virtual void OnCheckAllPluginsUI(wxUpdateUIEvent& event);
    virtual void OnInstallCompiler(wxCommandEvent& event);
    virtual void OnInstallCompilerUI(wxUpdateUIEvent& event);
    virtual void OnScanForCompilers(wxCommandEvent& event);
    virtual void OnThemeSelected(wxCommandEvent& event);
    virtual void OnToggleCxxPlugins(wxCommandEvent& event);
    virtual void OnUnCheckAllPlugins(wxCommandEvent& event);
    virtual void OnUnCheckAllPluginsUI(wxUpdateUIEvent& event);
};
#endif // CLBOOSTRAPWIZARD_H
