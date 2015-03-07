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

class WXDLLIMPEXP_SDK clBoostrapWizard : public clBoostrapWizardBase
{
    ICompilerLocator::CompilerVec_t m_compilers;
public:
    static wxBitmap GenerateBitmap(size_t labelIndex);

public:
    clBoostrapWizard(wxWindow* parent);
    virtual ~clBoostrapWizard();
    clBootstrapData GetData();
protected:
    virtual void OnInstallCompiler(wxCommandEvent& event);
    virtual void OnInstallCompilerUI(wxUpdateUIEvent& event);
    virtual void OnScanForCompilers(wxCommandEvent& event);
    virtual void OnThemeSelected(wxCommandEvent& event);
};
#endif // CLBOOSTRAPWIZARD_H
