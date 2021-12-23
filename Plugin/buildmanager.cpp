//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : buildmanager.cpp
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
#include "buildmanager.h"

#include "BuilderGnuMakeMSYS.hpp"
#include "builder.h"
#include "builder_NMake.h"
#include "builder_gnumake.h"
#include "builder_gnumake_default.h"
#include "builder_gnumake_onestep.h"

BuildManager::BuildManager()
{
    // register all builders here
    AddBuilder(new BuilderGnuMake());
    AddBuilder(new BuilderGNUMakeClassic());
    AddBuilder(new BuilderGnuMakeOneStep());
#ifdef __WXMSW__
    AddBuilder(new BuilderNMake());
    AddBuilder(new BuilderGnuMakeMSYS());
#endif
}

BuildManager::~BuildManager() { m_builders.clear(); }

void BuildManager::AddBuilder(BuilderPtr builder)
{
    if(!builder) {
        return;
    }
    m_builders[builder->GetName()] = builder;
}

void BuildManager::RemoveBuilder(const wxString& name)
{
    auto iter = m_builders.find(name);
    if(iter != m_builders.end()) {
        m_builders.erase(iter);
    }
}

void BuildManager::GetBuilders(std::list<wxString>& list)
{
    auto iter = m_builders.begin();
    for(; iter != m_builders.end(); ++iter) {
        list.push_back(iter->second->GetName());
    }
}

BuilderPtr BuildManager::GetBuilder(const wxString& name)
{
    auto iter = m_builders.begin();
    for(; iter != m_builders.end(); ++iter) {
        if(iter->first == name) {
            return iter->second;
        }
    }

    // return the default builder
    return m_builders.begin()->second;
}

static BuildManager* gs_BuildManager = nullptr;
void BuildManagerST::Free()
{
    if(gs_BuildManager) {
        delete gs_BuildManager;
        gs_BuildManager = nullptr;
    }
}

BuildManager* BuildManagerST::Get()
{
    if(gs_BuildManager == nullptr)
        gs_BuildManager = new BuildManager;
    return gs_BuildManager;
}
