//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : build_custom_targets_menu_manager.cpp
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

#include "build_custom_targets_menu_manager.h"
#include "macros.h"

CustomTargetsMgr::CustomTargetsMgr()
{
}

CustomTargetsMgr::~CustomTargetsMgr()
{
}

CustomTargetsMgr& CustomTargetsMgr::Get()
{
    static CustomTargetsMgr theMgr;
    return theMgr;
}

void CustomTargetsMgr::SetTargets(const wxString& projectName, const BuildConfig::StringMap_t& targets)
{
    Clear();
    
    m_projectName = projectName;
    BuildConfig::StringMap_t::const_iterator iter = targets.begin();
    
    int id = ID_MENU_CUSTOM_TARGET_FIRST;
    for(; iter != targets.end(); ++iter) {
        m_targets.insert( std::make_pair( id, std::make_pair(iter->first, iter->second) ) );
        ++id;
    }
}

CustomTargetsMgr::Pair_t CustomTargetsMgr::GetTarget(int menuId) const
{
    if ( m_targets.count(menuId) ) {
        return m_targets.find(menuId)->second;
    }
    return Pair_t();
}

void CustomTargetsMgr::Clear()
{
    m_projectName.Clear();
    m_targets.clear();
}

