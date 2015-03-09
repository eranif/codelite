#ifndef CLBOOSTRAPWIZARDPAGE_H
#define CLBOOSTRAPWIZARDPAGE_H

#include <wx/wizard.h>
#include <wx/bitmap.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clBoostrapWizardPageCompilers : public wxWizardPageSimple
{
public:
    clBoostrapWizardPageCompilers(wxWizard* parent,
                                  wxWizardPage* prev = NULL,
                                  wxWizardPage* next = NULL,
                                  const wxBitmap& bitmap = wxNullBitmap);
    virtual ~clBoostrapWizardPageCompilers() {}
};

class WXDLLIMPEXP_SDK clBoostrapWizardPageColours : public wxWizardPageSimple
{
public:
    clBoostrapWizardPageColours(wxWizard* parent,
                                wxWizardPage* prev = NULL,
                                wxWizardPage* next = NULL,
                                const wxBitmap& bitmap = wxNullBitmap);
    virtual ~clBoostrapWizardPageColours() {}
};

class WXDLLIMPEXP_SDK clBoostrapWizardPageWhitespace : public wxWizardPageSimple
{
public:
    clBoostrapWizardPageWhitespace(wxWizard* parent,
                                   wxWizardPage* prev = NULL,
                                   wxWizardPage* next = NULL,
                                   const wxBitmap& bitmap = wxNullBitmap);
    virtual ~clBoostrapWizardPageWhitespace() {}
};

class WXDLLIMPEXP_SDK clBoostrapWizardPageWelcome : public wxWizardPageSimple
{
public:
    clBoostrapWizardPageWelcome(wxWizard* parent,
                                wxWizardPage* prev = NULL,
                                wxWizardPage* next = NULL,
                                const wxBitmap& bitmap = wxNullBitmap);
    virtual ~clBoostrapWizardPageWelcome() {}
};

class WXDLLIMPEXP_SDK clBoostrapWizardPagePlugins : public wxWizardPageSimple
{
public:
    clBoostrapWizardPagePlugins(wxWizard* parent,
                                wxWizardPage* prev = NULL,
                                wxWizardPage* next = NULL,
                                const wxBitmap& bitmap = wxNullBitmap);
    virtual ~clBoostrapWizardPagePlugins() {}
};

#endif // CLBOOSTRAPWIZARDPAGE_H
