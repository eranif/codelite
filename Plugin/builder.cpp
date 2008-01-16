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
	m_buildTool = GetBuildToolFromConfig();
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
	BuildSystemPtr bs = BuildSettingsConfigST::Get()->GetBuildSystem(m_name);
	if( !bs ){
		return m_buildTool;
	}

	return bs->GetToolPath();
}

wxString Builder::GetBuildToolOptionsFromConfig() const
{
	BuildSystemPtr bs = BuildSettingsConfigST::Get()->GetBuildSystem(m_name);
	if( !bs ){
		return m_buildToolOptions;
	}

	return bs->GetToolOptions();
}

wxString Builder::GetBuildToolCommand() const
{
	//enclose the tool path in quatation marks
	return wxT("\"") + GetBuildToolFromConfig() + wxT("\" ") + GetBuildToolOptionsFromConfig();
}

wxString Builder::GetBuildToolName() const
{
	return GetBuildToolFromConfig();
}

wxString Builder::GetBuildToolOptions() const
{
	return GetBuildToolOptionsFromConfig();
}
