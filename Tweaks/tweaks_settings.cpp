//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : tweaks_settings.cpp
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

#include "tweaks_settings.h"
#include <wx/settings.h>
#include "workspace.h"

#define TWEAKS_CONF_FILE "tweaks-settings"

// --------------------------------------------------------
// TweaksSettings
// --------------------------------------------------------

TweaksSettings::TweaksSettings()
    : clConfigItem(TWEAKS_CONF_FILE)
    , m_enableTweaks(false)
    , m_flags(0)
{
}

TweaksSettings::~TweaksSettings()
{
}

void TweaksSettings::FromJSON(const JSONItem& json)
{
    m_projects.clear();
    m_globalBgColour = json.namedObject("m_globalBgColour").toColour();
    m_globalFgColour = json.namedObject("m_globalFgColour").toColour();
    m_enableTweaks = json.namedObject("m_enableTweaks").toBool(false);
    m_flags = json.namedObject("m_flags").toSize_t();
    
    JSONItem arr = json.namedObject("projects");
    int size = arr.arraySize();
    for(int i=0; i<size; ++i) {
        ProjectTweaks tw;
        tw.FromJSON( arr.arrayItem(i) );
        m_projects.insert( std::make_pair(tw.GetProjectName(), tw) );
    }
}

JSONItem TweaksSettings::ToJSON() const
{
    JSONItem e = JSONItem::createObject(GetName());
    
    e.addProperty("m_globalBgColour", m_globalBgColour);
    e.addProperty("m_globalFgColour", m_globalFgColour);
    e.addProperty("m_enableTweaks", m_enableTweaks);
    e.addProperty("m_flags", m_flags);
    JSONItem arr = JSONItem::createArray("projects");
    e.append( arr );
    
    ProjectTweaks::Map_t::const_iterator iter = m_projects.begin();
    for( ; iter != m_projects.end(); ++iter ) {
        arr.arrayAppend( iter->second.ToJSON() );
    }
    return e;
}

TweaksSettings& TweaksSettings::Load()
{
    // Get the file name
    if(!clCxxWorkspaceST::Get()->IsOpen()) {
        return *this;
    }
    
    wxFileName fn(clCxxWorkspaceST::Get()->GetPrivateFolder(), "tweaks.conf");
    clConfig conf( fn.GetFullPath() );
    conf.ReadItem( this );
    return *this;
}

void TweaksSettings::Save()
{
    if(!clCxxWorkspaceST::Get()->IsOpen()) {
        return;
    }
    wxFileName fn(clCxxWorkspaceST::Get()->GetPrivateFolder(), "tweaks.conf");
    clConfig conf( fn.GetFullPath() );
    conf.WriteItem( this );
}

const ProjectTweaks& TweaksSettings::GetProjectTweaks(const wxString& project) const
{
    if ( m_projects.count(project) ) {
        return m_projects.find(project)->second;
    }
    static ProjectTweaks DUMMY;
    return DUMMY;
}

ProjectTweaks& TweaksSettings::GetProjectTweaks(const wxString& project)
{
    if ( m_projects.count(project) == 0 ) {
        // add an entry
        ProjectTweaks pt;
        pt.SetProjectName(project);
        m_projects.insert( std::make_pair(project, pt) );
    }
    return m_projects.find( project )->second;
}

void TweaksSettings::UpdateProject(const ProjectTweaks& pt)
{
    DeleteProject( pt.GetProjectName() );
    m_projects.insert( std::make_pair(pt.GetProjectName(), pt) );
}

void TweaksSettings::Clear()
{
    m_projects.clear();
    ResetColours();
    m_enableTweaks = false;
    m_flags = 0x0;
}

void TweaksSettings::DeleteProject(const wxString& name)
{
    if ( m_projects.count(name) ) {
        m_projects.erase(name);
    }
}

void TweaksSettings::ResetColours()
{
    m_globalBgColour = wxColour();
    m_globalFgColour = wxColour();
    ProjectTweaks::Map_t::iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter ) {
        iter->second.ResetColours();
    }
}

// --------------------------------------------------------
// ProjectTweaks
// --------------------------------------------------------

ProjectTweaks::ProjectTweaks()
    : clConfigItem("")
{
}

ProjectTweaks::~ProjectTweaks()
{
}

void ProjectTweaks::FromJSON(const JSONItem& json)
{
    m_tabBgColour = json.namedObject("m_tabBgColour").toColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    m_tabFgColour = json.namedObject("m_tabFgColour").toColour(*wxBLACK);
    m_projectName = json.namedObject("m_projectName").toString();
    m_bitmapFilename = json.namedObject("m_bitmapFilename").toString();
}

JSONItem ProjectTweaks::ToJSON() const
{
    JSONItem e = JSONItem::createObject();
    e.addProperty("m_tabFgColour",      m_tabFgColour);
    e.addProperty("m_tabBgColour",      m_tabBgColour);
    e.addProperty("m_projectName",      m_projectName);
    e.addProperty("m_bitmapFilename",   m_bitmapFilename);
    return e;
}

void ProjectTweaks::ResetColours()
{
    m_tabBgColour = wxColour();
    m_tabFgColour = wxColour();
}
