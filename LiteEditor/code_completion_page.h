//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : code_completion_page.h
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

#ifndef CODECOMPLETIONPAGE_H
#define CODECOMPLETIONPAGE_H

#include "workspacesettingsbase.h" // Base class: CodeCompletionBasePage

class CodeCompletionPage : public CodeCompletionBasePage
{
public:
    enum { TypeWorkspace, TypeProject };

protected:
    int m_type;
    bool m_ccChanged;

protected:
    // Event handlers
    void OnCCContentModified(wxStyledTextEvent& event);

public:
    CodeCompletionPage(wxWindow* parent, int type);
    virtual ~CodeCompletionPage();

    void Save();

    wxArrayString GetIncludePaths() const;
    wxString GetMacros() const;
    wxString GetIncludePathsAsString() const;
    bool IsCpp11Enabled() const;
};

#endif // CODECOMPLETIONPAGE_H
