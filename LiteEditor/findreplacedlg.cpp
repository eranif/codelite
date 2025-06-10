//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findreplacedlg.cpp
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
#include "findreplacedlg.h"

#include "sessionmanager.h"

//---------------------------------------------------------------
// FindReplaceData
//---------------------------------------------------------------

void FindReplaceData::SetReplaceString(const wxString& str)
{
    int where = m_replaceString.Index(str);
    if(where != wxNOT_FOUND) {
        m_replaceString.RemoveAt(where);
    }
    m_replaceString.Insert(str, 0);

    long max_value = clConfig::Get().Read(kConfigMaxItemsInFindReplaceDialog, 15);
    TruncateArray(m_replaceString, (size_t)max_value);
}

void FindReplaceData::SetFindString(const wxString& str)
{
    int where = m_findString.Index(str);
    if(where != wxNOT_FOUND) {
        m_findString.RemoveAt(where);
    }
    m_findString.Insert(str, 0);

    long max_value = clConfig::Get().Read(kConfigMaxItemsInFindReplaceDialog, 15);
    TruncateArray(m_findString, (size_t)max_value);
}

wxString FindReplaceData::GetReplaceString() const
{
    if(m_replaceString.IsEmpty()) {
        return wxEmptyString;
    } else {
        return m_replaceString.Item(0);
    }
}

wxString FindReplaceData::GetFindString() const
{
    if(m_findString.IsEmpty()) {
        return wxEmptyString;

    } else {
        return m_findString.Item(0);
    }
}

void FindReplaceData::TruncateArray(wxArrayString& arr, size_t maxSize)
{
    while(arr.GetCount() > maxSize && arr.GetCount() > 0) {
        arr.RemoveAt(arr.GetCount() - 1);
    }
}
void FindReplaceData::FromJSON(const JSONItem& json)
{
    m_findString = json.namedObject("m_findString").toArrayString();
    m_replaceString = json.namedObject("m_replaceString").toArrayString();
    m_flags = json.namedObject("m_flags").toSize_t(m_flags);
    m_findWhere = json.namedObject("m_findWhere").toArrayString(m_findWhere);
    m_encoding = json.namedObject("m_encoding").toString(m_encoding);
    m_fileMask = json.namedObject("m_fileMask").toArrayString();
    m_selectedMask = json.namedObject("m_selectedMask").toString(m_selectedMask);
    m_file_scanner_flags = json.namedObject("m_file_scanner_flags").toSize_t(m_file_scanner_flags);

    long max_value = clConfig::Get().Read(kConfigMaxItemsInFindReplaceDialog, 15);
    TruncateArray(m_replaceString, (size_t)max_value);
    TruncateArray(m_findString, (size_t)max_value);

    if(m_fileMask.IsEmpty()) {
        m_fileMask.Add("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.inc;*.mm;*.m;*.xrc");
        m_selectedMask = m_fileMask.Item(0);
    }
}

JSONItem FindReplaceData::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("m_findString", m_findString);
    element.addProperty("m_replaceString", m_replaceString);
    element.addProperty("m_flags", m_flags);
    element.addProperty("m_findWhere", m_findWhere);
    element.addProperty("m_encoding", m_encoding);
    element.addProperty("m_fileMask", m_fileMask);
    element.addProperty("m_selectedMask", m_selectedMask);
    element.addProperty("m_file_scanner_flags", m_file_scanner_flags);
    return element;
}

wxArrayString FindReplaceData::GetFindStringArr() const
{
    wxArrayString findArr;
    for(size_t i = 0; i < m_findString.GetCount(); ++i) {
        if((findArr.Index(m_findString.Item(i)) == wxNOT_FOUND) && !m_findString.Item(i).IsEmpty()) {
            findArr.Add(m_findString.Item(i));
        }
    }
    return findArr;
}

wxArrayString FindReplaceData::GetReplaceStringArr() const
{
    wxArrayString replaceArr;
    for(size_t i = 0; i < m_replaceString.GetCount(); ++i) {
        if((replaceArr.Index(m_replaceString.Item(i)) == wxNOT_FOUND) && !m_replaceString.Item(i).IsEmpty()) {
            replaceArr.Add(m_replaceString.Item(i));
        }
    }
    return replaceArr;
}

FindReplaceData::FindReplaceData()
    : clConfigItem("FindReplaceData")
    , m_flags(wxFRD_SEPARATETAB_DISPLAY | wxFRD_MATCHCASE | wxFRD_MATCHWHOLEWORD | wxFRD_ENABLE_PIPE_SUPPORT)
    , m_selectedMask("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.inc;*.mm;*.m;*.xrc;*.plist;*.txt") // Default file mask
{
    m_findWhere.Add(SEARCH_IN_WORKSPACE);
    m_fileMask.Add("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.inc;*.mm;*.m;*.xrc;*.xml;*.json;*.sql");
}

wxString FindReplaceData::GetWhere() const
{
    if(m_findWhere.empty()) {
        return SEARCH_IN_WORKSPACE;
    }
    return m_findWhere[0];
}

void FindReplaceData::SetWhereOptions(const wxArrayString& where)
{
    std::unordered_set<wxString> visited;
    wxArrayString normalized_list;
    normalized_list.reserve(where.size());

    for(const wxString& str : where) {
        if(!visited.insert(str).second || str.empty()) {
            continue;
        }
        normalized_list.Add(str);
    }
    m_findWhere.swap(normalized_list);
}
