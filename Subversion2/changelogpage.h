//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : changelogpage.h
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

#ifndef CHANGELOGPAGE_H
#define CHANGELOGPAGE_H

#include "subversion2_ui.h" // Base class: ChangeLogPageBase

#include <map>
#include <wx/regex.h>

class Subversion2;
class ChangeLogPage : public ChangeLogPageBase
{
    wxString m_url;
    std::map<wxString, wxString> m_macrosMap;
    std::map<wxString, wxString> m_rmacrosMap;

protected:
    wxString DoFormatLinesToUrl(const wxString& text, const wxString& pattern, const wxString& urlPattern);
    void DoMakeRegexFromPattern(const wxString& pattern, wxRegEx& re);
    wxArrayString DoMakeBugFrIdToUrl(const wxString& bugFrId, const wxString& urlPattern);

public:
    ChangeLogPage(wxWindow* parent, Subversion2* plugin);
    virtual ~ChangeLogPage() = default;

    void AppendText(const wxString& text);
    void OnURL(wxTextUrlEvent& event);

    void SetUrl(const wxString& url) { this->m_url = url; }
    const wxString& GetUrl() const { return m_url; }
};

#endif // CHANGELOGPAGE_H
