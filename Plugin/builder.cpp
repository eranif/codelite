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
#include "macros.h"
#include "build_settings_config.h"
#include "workspace.h"

Builder::Builder(const wxString &name, const wxString &buildTool, const wxString &buildToolOptions)
		: m_name(name)
		, m_buildTool(buildTool)
		, m_buildToolOptions(buildToolOptions)

{
	//override values from configuration file
	m_buildTool        = GetBuildToolFromConfig();
	m_buildToolOptions = GetBuildToolOptionsFromConfig();
}

Builder::~Builder()
{
}

wxString Builder::NormalizeConfigName(const wxString &confgName)
{
	wxString normalized(confgName);
	TrimString(normalized);
	normalized.Replace(wxT(" "), wxT("_"));
	return normalized;
}

wxString Builder::GetBuildToolFromConfig() const
{
	BuilderConfigPtr bs = BuildSettingsConfigST::Get()->GetBuilderConfig(m_name);
	if ( !bs ) {
		return m_buildTool;
	}

	return bs->GetToolPath();
}

wxString Builder::GetBuildToolOptionsFromConfig() const
{
	BuilderConfigPtr bs = BuildSettingsConfigST::Get()->GetBuilderConfig(m_name);
	if ( !bs ) {
		return m_buildToolOptions;
	}

	return bs->GetToolOptions();
}

wxString Builder::GetBuildToolJobsFromConfig() const
{
	BuilderConfigPtr bs = BuildSettingsConfigST::Get()->GetBuilderConfig(m_name);
	if ( !bs ) {
		return m_buildToolJobs;
	}

	return bs->GetToolJobs();
}
