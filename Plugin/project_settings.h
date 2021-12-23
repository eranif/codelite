//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : project_settings.h
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
#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include "build_config.h"
#include "codelite_exports.h"
#include "wx/string.h"

#include <map>

/**
 * \ingroup SDK
 * A cookie class to allow the enumration operation to be reentrants by multiple threads
 *
 * \version 1.0
 * first version
 *
 * \date 05-15-2007
 *
 * \author Eran
 *
 */
class ProjectSettingsCookie
{
public:
    std::map<wxString, BuildConfigPtr>::const_iterator iter;
};

/**
 * \ingroup SDK
 * Representation class for the project settings
 *
 * \version 1.0
 * first version
 *
 * \date 05-15-2007
 *
 * \author Eran
 *
 */
class WXDLLIMPEXP_SDK ProjectSettings : public ConfObject
{
    std::map<wxString, BuildConfigPtr> m_configs;
    BuildConfigCommonPtr m_globalSettings;
    wxString m_projectType;

public:
    /**
     * Clone this project settings, the cloned object is allocated on the heap
     * and it is the responsibility of the caller to call delete when there are no
     * more refreneces to it
     * \return
     */
    ProjectSettings* Clone() const;

    /**
     * Construct ProjectSettings object from its XML representation
     * \param node XML node of the settings object
     */
    ProjectSettings(wxXmlNode* node);

    /**
     * Destructor
     */
    virtual ~ProjectSettings();

    /**
     * Serialize the object to XML representation
     */
    wxXmlNode* ToXml() const;

    // enumaration
    /**
     * Find the first build configuration by name
     * \param configName build configuration name to find
     * \param merge merge with global settings or not
     * \return build configuration smart pointer, will be deleted automatically when no more refrences to it
     */
    BuildConfigPtr GetBuildConfiguration(const wxString& configName, bool merge = false) const;

    /**
     * enumerate through all the build build configuration, this function returns the first build configuration found
     * \param cookie an object to allow this call to be reentrant by multiple threads
     * \return build configuration smart pointer, will be deleted automatically when no more refrences to it
     */
    BuildConfigPtr GetFirstBuildConfiguration(ProjectSettingsCookie& cookie) const;
    /**
     * enumerate through all the build build configuration, this function returns the next build configuration found
     * cookie should be the same as passed to previous calls to GetFirstBuildConfiguration or
     * \param cookie an object to allow this call to be reentrant by multiple threads GetNextBuildConfiguration
     * \return build configuration smart pointer, will be deleted automatically when no more refrences to it
     */
    BuildConfigPtr GetNextBuildConfiguration(ProjectSettingsCookie& cookie) const;

    /**
     * Insert or Update BuildConfigPtr, if a build configuration with this name already exist,
     * it is replaced
     * \param bc build configuration object
     */
    void SetBuildConfiguration(const BuildConfigPtr bc);

    /**
     * Remove build configuration from the project settings. The build configuration objects stored under
     * this object are stored using smart pointers, thus if there are no more refrences to the removed build
     * configuration it will be deleted automatically
     * \param configName configuration name to remove.
     */
    void RemoveConfiguration(const wxString& configName);

    /**
     * \brief Return the global settings, common to all configurations
     */
    BuildConfigCommonPtr GetGlobalSettings() const { return m_globalSettings; }

    /**
     * \brief Set the global settings
     * \param global settings
     */
    void SetGlobalSettings(BuildConfigCommonPtr globalSettings) { m_globalSettings = globalSettings; }

    // setters/getters
    wxString GetProjectType(const wxString& confName);
    void SetProjectType(const wxString& type) { m_projectType = type; }
};

typedef SmartPtr<ProjectSettings> ProjectSettingsPtr;

#endif // PROJECT_SETTINGS_H
