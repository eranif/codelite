#include "frame.h"
#include "buidltab.h"
#include "buildsettingstab.h"
#include "buildtabsettingsdata.h"
#include "editor_config.h"

BuildTabSetting::BuildTabSetting( wxWindow* parent )
:
BuildTabSettingsBase( parent )
{
	BuildTabSettingsData options;
	EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);
	m_checkBoxSkipeWarnings->SetValue( options.GetSkipWarnings() );
	m_colourPickerError->SetColour(options.GetErrorColourBg());
	m_colourPickerWarnings->SetColour(options.GetWarnColourBg());
//	m_colourPickerErrorBg->SetColour(options.GetErrorColourBg());
//	m_colourPickerWarningsBg->SetColour(options.GetWarnColourBg());
}

void BuildTabSetting::Save()
{
	BuildTabSettingsData options;
	options.SetErrorColourBg(m_colourPickerError->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
	options.SetWarnColourBg(m_colourPickerWarnings->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
	
//	options.SetErrorColourBg(m_colourPickerErrorBg->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
//	options.SetWarnColourBg(m_colourPickerWarningsBg->GetColour().GetAsString(wxC2S_HTML_SYNTAX));

	options.SetSkipWarnings(m_checkBoxSkipeWarnings->IsChecked());
	
	EditorConfigST::Get()->WriteObject(wxT("build_tab_settings"), &options);
	//reload the settings
	Frame::Get()->GetOutputPane()->GetBuildTab()->ReloadSettings();
}
