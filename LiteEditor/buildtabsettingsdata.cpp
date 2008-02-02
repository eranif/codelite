#include "buildtabsettingsdata.h"
#include "wx/wxFlatNotebook/renderer.h"
#include "wx/settings.h"

BuildTabSettingsData::BuildTabSettingsData()
: m_skipWarnings(false)
, m_boldErrFont(true)
, m_boldWarnFont(true)
{
	wxColour errColour(wxT("RED"));
	wxColour wrnColour(wxT("GOLD"));
	
	m_errorColour = errColour.GetAsString(wxC2S_HTML_SYNTAX);
	m_warnColour  = wrnColour.GetAsString(wxC2S_HTML_SYNTAX);
	
	m_warnColourBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString(wxC2S_HTML_SYNTAX);
	m_errorColourBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString(wxC2S_HTML_SYNTAX);
}

BuildTabSettingsData::~BuildTabSettingsData()
{
}

void BuildTabSettingsData::Serialize(Archive &arch)
{
	arch.Write(wxT("m_errorColour"), m_errorColour);
	arch.Write(wxT("m_warnColour"), m_warnColour);
	arch.Write(wxT("m_errorColourBg"), m_errorColourBg);
	arch.Write(wxT("m_warnColourBg"), m_warnColourBg);
	arch.Write(wxT("m_skipWarnings"), m_skipWarnings);
	arch.Write(wxT("m_boldErrFont"), m_boldErrFont);
	arch.Write(wxT("m_boldWarnFont"), m_boldWarnFont);
}

void BuildTabSettingsData::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_errorColour"), m_errorColour);
	arch.Read(wxT("m_warnColour"), m_warnColour);
	arch.Read(wxT("m_errorColourBg"), m_errorColourBg);
	arch.Read(wxT("m_warnColourBg"), m_warnColourBg);
	arch.Read(wxT("m_skipWarnings"), m_skipWarnings);
	arch.Read(wxT("m_boldErrFont"), m_boldErrFont);
	arch.Read(wxT("m_boldWarnFont"), m_boldWarnFont);
}
