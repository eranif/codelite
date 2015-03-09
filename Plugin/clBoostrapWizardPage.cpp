#include "clBoostrapWizardPage.h"
#include "clBootstrapWizard.h"

clBoostrapWizardPageWelcome::clBoostrapWizardPageWelcome(wxWizard* parent,
                                                         wxWizardPage* prev,
                                                         wxWizardPage* next,
                                                         const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBootstrapWizard::GenerateBitmap(0));
}

clBoostrapWizardPagePlugins::clBoostrapWizardPagePlugins(wxWizard* parent,
                                                         wxWizardPage* prev,
                                                         wxWizardPage* next,
                                                         const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBootstrapWizard::GenerateBitmap(1));
}

clBoostrapWizardPageCompilers::clBoostrapWizardPageCompilers(wxWizard* parent,
                                                             wxWizardPage* prev,
                                                             wxWizardPage* next,
                                                             const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBootstrapWizard::GenerateBitmap(2));
}

clBoostrapWizardPageColours::clBoostrapWizardPageColours(wxWizard* parent,
                                                         wxWizardPage* prev,
                                                         wxWizardPage* next,
                                                         const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBootstrapWizard::GenerateBitmap(3));
}

clBoostrapWizardPageWhitespace::clBoostrapWizardPageWhitespace(wxWizard* parent,
                                                               wxWizardPage* prev,
                                                               wxWizardPage* next,
                                                               const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBootstrapWizard::GenerateBitmap(4));
}

