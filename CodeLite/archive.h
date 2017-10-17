//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : archive.h
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
#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "wx/string.h"
#include "wx/hashmap.h"
#include <wx/arrstr.h>
#include "wx/filename.h"
#include <wx/gdicmn.h>
#include <vector>
#include <map>
#include <set>
#include "codelite_exports.h"
#include <wx/font.h>
#include "macros.h"

class wxXmlNode;
class SerializedObject;
class TabInfo;
class BreakpointInfo;

WX_DECLARE_STRING_HASH_MAP(wxString, StringMap);

/**
 * \class Archive
 * \brief an auxulariy class which serializes variables into XML format
 * \author Eran
 * \date 07/20/07
 */
class WXDLLIMPEXP_CL Archive
{
    wxXmlNode* m_root;

public:
    Archive();
    virtual ~Archive();

    /**
     * \brief set a root node for this Archive object, all Write operations will append their nodes
     * to this node
     * \param node wxXmlNode to act as the root
     */
    void SetXmlNode(wxXmlNode* node);

    //--------------------
    // Write API
    //--------------------
    bool Write(const wxString& name, SerializedObject* obj);
    bool Write(const wxString& name, int value);
    bool Write(const wxString& name, bool value);
    bool Write(const wxString& name, long value);
    bool Write(const wxString& name, const wxString& str);
    bool Write(const wxString& name, const wxArrayString& arr);
    bool Write(const wxString& name, const wxFileName& fileName);
    bool Write(const wxString& name, size_t value);
    bool Write(const wxString& name, wxSize size);
    bool Write(const wxString& name, wxPoint pt);
    bool Write(const wxString& name, const StringMap& str_map);
    bool Write(const wxString& name, const wxColour& colour);
    bool Write(const wxString& name, std::vector<TabInfo>& _vTabInfoArr);
    bool Write(const wxString& name, std::vector<int>& _vInt);
    bool Write(const wxString& name, const wxStringMap_t& strinMap);
    bool Write(const wxString& name, const wxStringSet_t& s);
    bool Write(const wxString& name, const wxFont& font);
    bool WriteCData(const wxString& name, const wxString& value);

    //--------------------
    // Read API
    //--------------------
    bool Read(const wxString& name, int& value);
    bool Read(const wxString& name, bool& value);
    bool Read(const wxString& name, long& value);
    bool Read(const wxString& name, wxString& value);
    bool Read(const wxString& name, wxArrayString& arr);
    bool Read(const wxString& name, wxFileName& fileName);
    bool Read(const wxString& name, size_t& value);
    bool Read(const wxString& name, wxSize& size);
    bool Read(const wxString& name, wxPoint& pt);
    bool Read(const wxString& name, StringMap& str_map);
    bool Read(const wxString& name, SerializedObject* obj);
    bool Read(const wxString& name, wxColour& colour);
    bool Read(const wxString& name, std::vector<TabInfo>& _vTabInfoArr);
    bool Read(const wxString& name, std::vector<int>& _vInt);
    bool Read(const wxString& name, wxStringMap_t& strinMap);
    bool Read(const wxString& name, wxStringSet_t& s);
    bool ReadCData(const wxString& name, wxString& value);
    bool Read(const wxString& name, wxFont& font, const wxFont& defaultFont = wxNullFont);

private:
    bool WriteSimple(long value, const wxString& typeName, const wxString& name);
    bool ReadSimple(long& value, const wxString& typeName, const wxString& name);
};

#endif // ARCHIVE_H
