//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : findusagetab.h
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

#ifndef FINDUSAGETAB_H
#define FINDUSAGETAB_H

#include "outputtabwindow.h" // Base class OutputTabWindow
#include "cpptoken.h"
#include "wxStringHash.h"

typedef std::unordered_map<int, CppToken> UsageResultsMap;

class FindUsageTab : public OutputTabWindow
{
    UsageResultsMap m_matches;

protected:
    void DoOpenResult(const CppToken& token);

public:
    FindUsageTab(wxWindow* parent, const wxString& name);
    virtual ~FindUsageTab();

public:
    virtual void Clear();
    virtual void OnClearAllUI(wxUpdateUIEvent& e);
    virtual void OnClearAll(wxCommandEvent& e);
    virtual void OnMouseDClick(wxStyledTextEvent& e);
    virtual void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
    virtual void OnStyleNeeded(wxStyledTextEvent& e);
    virtual void OnThemeChanged(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& event);

public:
    void ShowUsage(const CppToken::Vec_t& matches, const wxString& searchWhat);
};

#endif // FINDUSAGETAB_H
