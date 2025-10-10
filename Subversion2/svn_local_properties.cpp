//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_local_properties.cpp
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

#include "svn_local_properties.h"
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include "cl_standard_paths.h"
#include "fileutils.h"

wxString SubversionLocalProperties::BUG_TRACKER_URL = wxT("bug_tracker_url");
wxString SubversionLocalProperties::BUG_TRACKER_MESSAGE = wxT("bug_tracker_message");
wxString SubversionLocalProperties::FR_TRACKER_URL = wxT("fr_tracker_url");
wxString SubversionLocalProperties::FR_TRACKER_MESSAGE = wxT("fr_tracker_message");

SubversionLocalProperties::SubversionLocalProperties(const wxString& url)
    : m_url(url)
{
}

wxString SubversionLocalProperties::ReadProperty(const wxString& propName)
{
    ReadProperties();

    // find the relevant group
    GroupTable::const_iterator iter = m_values.find(m_url);
    if(iter == m_values.end()) return wxT("");

    SimpleTable::const_iterator it = iter->second.find(propName);
    if(it == iter->second.end()) return wxT("");

    return it->second;
}

void SubversionLocalProperties::WriteProperty(const wxString& name, const wxString& val)
{
    ReadProperties();

    GroupTable::iterator iter = m_values.find(m_url);
    if(iter == m_values.end()) {
        SimpleTable tb;
        tb[name] = val;
        m_values[m_url] = tb;
    } else {
        m_values[m_url][name] = val;
    }

    // Update the properties
    WriteProperties();
}

wxString SubversionLocalProperties::GetConfigFile()
{
    wxFileName fnConfig(clStandardPaths::Get().GetUserDataDir(), "codelite-properties.ini");
    fnConfig.AppendDir("subversion");
    
    fnConfig.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    if(!fnConfig.Exists()) {
        // Create an empty file
        FileUtils::WriteFileContent(fnConfig, "");
    }
    return fnConfig.GetFullPath();
}

void SubversionLocalProperties::ReadProperties()
{
    m_values.clear();
    wxString group;
    wxFFile fp(GetConfigFile(), wxT("rb"));
    if(fp.IsOpened()) {
        wxString content;
        fp.ReadAll(&content);

        wxArrayString lines = wxStringTokenize(content, wxT("\n"), wxTOKEN_STRTOK);
        for (auto entry : lines) {
            // remove the comment part
            entry = entry.BeforeFirst(wxT(';'));

            // trim the string
            entry.Trim().Trim(false);

            if(entry.IsEmpty()) continue;

            if(entry.StartsWith(wxT("["))) {
                // found new group
                entry = entry.AfterFirst(wxT('['));
                group = entry.BeforeFirst(wxT(']'));
                group.Trim().Trim(false);
                continue;
            }

            wxString key = entry.BeforeFirst(wxT('='));
            wxString value = entry.AfterFirst(wxT('='));

            key.Trim().Trim(false);
            value.Trim().Trim(false);

            if(group.IsEmpty()) {
                // we don't have group yet - discard this entry
                continue;
            }

            GroupTable::iterator iter = m_values.find(group);
            if(iter == m_values.end()) {
                // create new table and the value
                SimpleTable tb;
                tb[key] = value;
                m_values[group] = tb;
            } else {
                m_values[group][key] = value;
            }
        }
    }
}

void SubversionLocalProperties::WriteProperties()
{
    wxFFile fp(GetConfigFile(), wxT("wb"));
    if (fp.IsOpened()) {
        for (const auto& [sectionName, tb] : m_values) {
            fp.Write(wxString::Format(wxT("[%s]\n"), sectionName.c_str()));

            for (const auto& [key, value] : tb) {
                fp.Write(wxString::Format(wxT("%s=%s\n"), key.c_str(), value.c_str()));
            }
        }
    }
}
