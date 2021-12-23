#include "clBoostrapWizardPage.h"

#include "clBootstrapWizard.h"

clBoostrapWizardPageWelcome::clBoostrapWizardPageWelcome(wxWizard* parent, wxWizardPage* prev, wxWizardPage* next,
                                                         const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next);
}

clBoostrapWizardPagePlugins::clBoostrapWizardPagePlugins(wxWizard* parent, wxWizardPage* prev, wxWizardPage* next,
                                                         const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next);
}

clBoostrapWizardPageCompilers::clBoostrapWizardPageCompilers(wxWizard* parent, wxWizardPage* prev, wxWizardPage* next,
                                                             const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next);
}

clBoostrapWizardPageColours::clBoostrapWizardPageColours(wxWizard* parent, wxWizardPage* prev, wxWizardPage* next,
                                                         const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next);
}

clBoostrapWizardPageWhitespace::clBoostrapWizardPageWhitespace(wxWizard* parent, wxWizardPage* prev, wxWizardPage* next,
                                                               const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
    Create(parent, prev, next);
}
