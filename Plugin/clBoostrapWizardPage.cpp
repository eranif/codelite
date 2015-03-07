#include "clBoostrapWizardPage.h"
#include "clBoostrapWizard.h"

clBoostrapWizardPageCompilers::clBoostrapWizardPageCompilers(wxWizard* parent,
                                                             wxWizardPage* prev,
                                                             wxWizardPage* next,
                                                             const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBoostrapWizard::GenerateBitmap(0));
}

clBoostrapWizardPageColours::clBoostrapWizardPageColours(wxWizard* parent,
                                                         wxWizardPage* prev,
                                                         wxWizardPage* next,
                                                         const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBoostrapWizard::GenerateBitmap(1));
}

clBoostrapWizardPageWhitespace::clBoostrapWizardPageWhitespace(wxWizard* parent,
                                                               wxWizardPage* prev,
                                                               wxWizardPage* next,
                                                               const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next, clBoostrapWizard::GenerateBitmap(2));
}
