//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : builder.cpp
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

#include "builder.h"

#include "build_settings_config.h"
#include "buildmanager.h"
#include "macros.h"
#include "project.h"
#include "workspace.h"

Builder::Builder(const wxString& name)
    : m_name(name)
    , m_isActive(false)
{
    // Override values from configuration file
    BuilderConfigPtr config = BuildSettingsConfigST::Get()->GetBuilderConfig(m_name);
    if(config) {
        m_isActive = config->GetIsActive();

    } else {
        m_isActive = (m_name == "Default");
    }
}

Builder::~Builder() {}

wxString Builder::NormalizeConfigName(const wxString& confgName)
{
    wxString normalized(confgName);
    TrimString(normalized);
    normalized.Replace(wxT(" "), wxT("_"));
    return normalized;
}

void Builder::SetActive()
{
    std::list<wxString> builders;
    BuildManagerST::Get()->GetBuilders(builders);
    std::list<wxString>::iterator iter = builders.begin();
    for(; iter != builders.end(); iter++) {

        wxString builderName = *iter;
        BuilderPtr builder = BuildManagerST::Get()->GetBuilder(builderName);

        if(builder && builder->m_name == m_name)
            builder->m_isActive = true;

        else if(builder)
            builder->m_isActive = false;
    }
}

Builder::OptimalBuildConfig Builder::GetOptimalBuildConfig(const wxString& projectType) const
{
    OptimalBuildConfig conf;
    conf.intermediateDirectory = "$(ConfigurationName)";
    conf.outputFile = "$(IntermediateDirectory)/";
    conf.command = "$(OutputFile)";

    if(projectType == PROJECT_TYPE_STATIC_LIBRARY || projectType == PROJECT_TYPE_DYNAMIC_LIBRARY) {
        conf.outputFile << "lib";
    }
    conf.outputFile << "$(ProjectName)" << GetOutputFileSuffix(projectType);

    return conf;
}

wxString Builder::GetOutputFileSuffix(const wxString& projectType) const
{
    if(projectType == PROJECT_TYPE_EXECUTABLE) {
#ifdef __WXMSW__
        return ".exe";
#endif
    } else if(projectType == PROJECT_TYPE_STATIC_LIBRARY) {
        return GetStaticLibSuffix();
    } else if(projectType == PROJECT_TYPE_DYNAMIC_LIBRARY) {
#ifdef __WXMSW__
        return ".dll";
#elif defined(__WXGTK__)
        return ".so";
#elif defined(__WXOSX__)
        return ".dylib";
#endif
    }
    return wxEmptyString;
}
