//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_registry.cpp
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

#include "cl_registry.h"

#include <map>
#include <wx/tokenzr.h>

wxString clRegistry::m_filename;

clRegistry::clRegistry()
{
    // m_filename is set in the static method SetFilename()
    m_fp.Open(m_filename.c_str(), wxT("rb"));
    if(m_fp.IsOpened()) {
        wxString fileContent;
        m_fp.ReadAll(&fileContent, wxConvUTF8);

        wxArrayString entries = wxStringTokenize(fileContent, wxT("\n\r"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < entries.GetCount(); i++) {
            entries.Item(i).Trim().Trim(false);

            // Empty line?
            if(entries.Item(i).IsEmpty())
                continue;

            // Comment line?
            if(entries.Item(i).StartsWith(wxT(";"))) {
                continue;
            }

            if(entries.Item(i).StartsWith(wxT("["))) {
                // Section
                continue;
            }

            wxString key = entries.Item(i).BeforeFirst(wxT('='));
            wxString value = entries.Item(i).AfterFirst(wxT('='));

            m_entries[key] = value;
        }
    }
}

clRegistry::~clRegistry() {}

bool clRegistry::Read(const wxString& key, wxString& val)
{
    std::map<wxString, wxString>::iterator iter = m_entries.find(key);
    if(iter == m_entries.end()) {
        return false;
    }
    val = iter->second;
    return true;
}

void clRegistry::SetFilename(const wxString& filename) { m_filename = filename.c_str(); }
