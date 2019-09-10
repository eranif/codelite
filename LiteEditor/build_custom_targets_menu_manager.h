//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : build_custom_targets_menu_manager.h
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

#ifndef BUILDCUSTOMTARGETSMENUMANAGER_H
#define BUILDCUSTOMTARGETSMENUMANAGER_H

#include <wx/string.h>
#include <map>

class CustomTargetsMgr
{
public:
    typedef std::pair<wxString, wxString> Pair_t;
    typedef std::map<int, Pair_t> Map_t;

protected:
    Map_t m_targets;
    wxString m_projectName;

public:
    CustomTargetsMgr();
    virtual ~CustomTargetsMgr();
    void Clear();

    static CustomTargetsMgr& Get();
    void SetTargets(const wxString& projectName, const std::map<wxString, wxString>& targets);
    CustomTargetsMgr::Pair_t GetTarget(int menuId) const;
    const CustomTargetsMgr::Map_t& GetTargets() const { return m_targets; }
    const wxString& GetProjectName() const { return m_projectName; }
};

#endif // BUILDCUSTOMTARGETSMENUMANAGER_H
