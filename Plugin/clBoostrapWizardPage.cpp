#include "clBoostrapWizardPage.h"
#include "clBoostrapWizard.h"

clBoostrapWizardPageWelcome::clBoostrapWizardPageWelcome(wxWizard* parent,
                                                         wxWizardPage* prev,
                                                         wxWizardPage* next,
                                                         const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBoostrapWizard::GenerateBitmap(0));
}

clBoostrapWizardPageCompilers::clBoostrapWizardPageCompilers(wxWizard* parent,
                                                             wxWizardPage* prev,
                                                             wxWizardPage* next,
                                                             const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBoostrapWizard::GenerateBitmap(1));
}

clBoostrapWizardPageColours::clBoostrapWizardPageColours(wxWizard* parent,
                                                         wxWizardPage* prev,
                                                         wxWizardPage* next,
                                                         const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBoostrapWizard::GenerateBitmap(2));
}

clBoostrapWizardPageWhitespace::clBoostrapWizardPageWhitespace(wxWizard* parent,
                                                               wxWizardPage* prev,
                                                               wxWizardPage* next,
                                                               const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBoostrapWizard::GenerateBitmap(3));
}

