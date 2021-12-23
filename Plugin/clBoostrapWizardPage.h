//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clBoostrapWizardPage.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CLBOOSTRAPWIZARDPAGE_H
#define CLBOOSTRAPWIZARDPAGE_H

#include "codelite_exports.h"

#include <wx/bitmap.h>
#include <wx/wizard.h>

class WXDLLIMPEXP_SDK clBoostrapWizardPageCompilers : public wxWizardPageSimple
{
public:
    clBoostrapWizardPageCompilers(wxWizard* parent, wxWizardPage* prev = NULL, wxWizardPage* next = NULL,
                                  const wxBitmap& bitmap = wxNullBitmap);
    virtual ~clBoostrapWizardPageCompilers() {}
};

class WXDLLIMPEXP_SDK clBoostrapWizardPageColours : public wxWizardPageSimple
{
public:
    clBoostrapWizardPageColours(wxWizard* parent, wxWizardPage* prev = NULL, wxWizardPage* next = NULL,
                                const wxBitmap& bitmap = wxNullBitmap);
    virtual ~clBoostrapWizardPageColours() {}
};

class WXDLLIMPEXP_SDK clBoostrapWizardPageWhitespace : public wxWizardPageSimple
{
public:
    clBoostrapWizardPageWhitespace(wxWizard* parent, wxWizardPage* prev = NULL, wxWizardPage* next = NULL,
                                   const wxBitmap& bitmap = wxNullBitmap);
    virtual ~clBoostrapWizardPageWhitespace() {}
};

class WXDLLIMPEXP_SDK clBoostrapWizardPageWelcome : public wxWizardPageSimple
{
public:
    clBoostrapWizardPageWelcome(wxWizard* parent, wxWizardPage* prev = NULL, wxWizardPage* next = NULL,
                                const wxBitmap& bitmap = wxNullBitmap);
    virtual ~clBoostrapWizardPageWelcome() {}
};

class WXDLLIMPEXP_SDK clBoostrapWizardPagePlugins : public wxWizardPageSimple
{
public:
    clBoostrapWizardPagePlugins(wxWizard* parent, wxWizardPage* prev = NULL, wxWizardPage* next = NULL,
                                const wxBitmap& bitmap = wxNullBitmap);
    virtual ~clBoostrapWizardPagePlugins() {}
};

#endif // CLBOOSTRAPWIZARDPAGE_H
