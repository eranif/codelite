#include "buildtabsettingsdata.h"
#include "wx/wxFlatNotebook/renderer.h"

BuildTabSettingsData::BuildTabSettingsData()
: m_skipWarnings(false)
{
	wxColour errColour = wxFNBRenderer::LightColour(wxT("RED"), 50);
	wxColour wrnColour = wxFNBRenderer::LightColour(wxT("YELLOW"), 70);
	m_errorColour = errColour.GetAsString(wxC2S_HTML_SYNTAX);
	m_warnColour  = wrnColour.GetAsString(wxC2S_HTML_SYNTAX);
}

BuildTabSettingsData::~BuildTabSettingsData()
{
}

void BuildTabSettingsData::Serialize(Archive &arch)
{
	arch.Write(wxT("m_errorColour"), m_errorColour);
	arch.Write(wxT("m_warnColour"), m_warnColour);
	arch.Write(wxT("m_skipWarnings"), m_skipWarnings);
}

void BuildTabSettingsData::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_errorColour"), m_errorColour);
	arch.Read(wxT("m_warnColour"), m_warnColour);
	arch.Read(wxT("m_skipWarnings"), m_skipWarnings);
}
