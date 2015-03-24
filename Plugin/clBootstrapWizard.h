#ifndef CLBOOSTRAPWIZARD_H
#define CLBOOSTRAPWIZARD_H
#include "clBoostrapWizardBase.h"
#include "codelite_exports.h"
#include <wx/bitmap.h>
#include "ICompilerLocator.h"

class WXDLLIMPEXP_SDK clBootstrapData 
{
public:
    wxString selectedTheme;
    int whitespaceVisibility;
    bool useTabs;
    ICompilerLocator::CompilerVec_t compilers;
};

class WXDLLIMPEXP_SDK clBootstrapWizard : public clBoostrapWizardBase
{
    ICompilerLocator::CompilerVec_t m_compilers;
public:
    static wxBitmap GenerateBitmap(size_t labelIndex);

public:
    clBootstrapWizard(wxWindow* parent);
    virtual ~clBootstrapWizard();
    clBootstrapData GetData();
    wxArrayString GetSelectedPlugins();
    wxArrayString GetUnSelectedPlugins();
        
    bool IsRestartRequired();
    
protected:
    virtual void OnCancelWizard(wxCommandEvent& event);
    virtual void OnFinish(wxWizardEvent& event);
    virtual void OnCheckAllPlugins(wxCommandEvent& event);
    virtual void OnCheckAllPluginsUI(wxUpdateUIEvent& event);
    virtual void OnToggleCxxPlugins(wxCommandEvent& event);
    virtual void OnUnCheckAllPlugins(wxCommandEvent& event);
    virtual void OnUnCheckAllPluginsUI(wxUpdateUIEvent& event);
    virtual void OnInstallCompiler(wxCommandEvent& event);
    virtual void OnInstallCompilerUI(wxUpdateUIEvent& event);
    virtual void OnScanForCompilers(wxCommandEvent& event);
    virtual void OnThemeSelected(wxCommandEvent& event);
};
#endif // CLBOOSTRAPWIZARD_H
