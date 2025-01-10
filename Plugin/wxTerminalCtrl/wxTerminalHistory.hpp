#pragma once

#include "codelite_exports.h"

#include <wx/arrstr.h>

struct WXDLLIMPEXP_SDK wxTerminalHistory {
    wxArrayString m_commands;
    int m_current = wxNOT_FOUND;
    void Add(const wxString& command);
    void Set(const wxArrayString& commands);
    void Up();
    void Down();
    wxString Get() const;
    void Clear();
    void Store();
    void Load();
    wxArrayString ForCompletion(const wxString& filter) const;
};
