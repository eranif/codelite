//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : tweaks_settings.h
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

#ifndef TWEAKSSETTINGS_H
#define TWEAKSSETTINGS_H

#include "cl_config.h"
#include <wx/colour.h>
#include <map>

class ProjectTweaks : public clConfigItem
{
    wxColour m_tabFgColour;
    wxColour m_tabBgColour;
    wxString m_projectName;
    wxString m_bitmapFilename;

public:
    typedef std::map<wxString, ProjectTweaks> Map_t;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;
    ProjectTweaks();
    virtual ~ ProjectTweaks();
    
    void ResetColours();
    void SetBitmapFilename(const wxString& bitmapFilename) {
        this->m_bitmapFilename = bitmapFilename;
    }
    const wxString& GetBitmapFilename() const {
        return m_bitmapFilename;
    }
    bool IsOk() const {
        return !m_projectName.IsEmpty() && m_tabBgColour.IsOk() && m_tabFgColour.IsOk();
    }
    void SetTabBgColour(const wxColour& tabBgColour) {
        this->m_tabBgColour = tabBgColour;
    }
    void SetTabFgColour(const wxColour& tabFgColour) {
        this->m_tabFgColour = tabFgColour;
    }
    const wxColour& GetTabBgColour() const {
        return m_tabBgColour;
    }
    const wxColour& GetTabFgColour() const {
        return m_tabFgColour;
    }
    void SetProjectName(const wxString& projectName) {
        this->m_projectName = projectName;
    }
    const wxString& GetProjectName() const {
        return m_projectName;
    }
};

class TweaksSettings : public clConfigItem
{
    ProjectTweaks::Map_t m_projects;
    wxColour             m_globalBgColour;
    wxColour             m_globalFgColour;
    bool                 m_enableTweaks;
    size_t               m_flags;
    
public:
    enum {
        kDontPromptForProjectReload = 0x00000001
    };
    
public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    TweaksSettings();
    virtual ~TweaksSettings();

    TweaksSettings& Load();
    void Clear();
    void ResetColours();
    void Save();
    
    bool HasFlag(int flag) const {
        return m_flags & kDontPromptForProjectReload;
    }
    void EnableFlag(int flag, bool b) {
        if ( b ) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }
    
    const ProjectTweaks::Map_t& GetProjects() const {
        return m_projects;
    }

    void SetGlobalBgColour(const wxColour& globalBgColour) {
        this->m_globalBgColour = globalBgColour;
    }
    void SetGlobalFgColour(const wxColour& globalFgColour) {
        this->m_globalFgColour = globalFgColour;
    }
    void SetProjects(const ProjectTweaks::Map_t& projects) {
        this->m_projects = projects;
    }
    const wxColour& GetGlobalBgColour() const {
        return m_globalBgColour;
    }
    const wxColour& GetGlobalFgColour() const {
        return m_globalFgColour;
    }
    void SetEnableTweaks(bool enableTweaks) {
        this->m_enableTweaks = enableTweaks;
    }
    bool IsEnableTweaks() const {
        return m_enableTweaks;
    }
    const ProjectTweaks& GetProjectTweaks(const wxString &project) const;
    ProjectTweaks& GetProjectTweaks(const wxString &project);
    void UpdateProject(const ProjectTweaks& pt);
    void DeleteProject(const wxString& pt);
};

#endif // TWEAKSSETTINGS_H
