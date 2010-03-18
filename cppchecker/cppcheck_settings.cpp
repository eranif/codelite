#include <wx/xrc/xmlres.h>
#include <memory>
#include <wx/stdpaths.h>
#include "cppcheck_settings.h"
#include "plugin.h"
#include <stdio.h>

// Some macros needed
#ifdef __WXMSW__
#define PIPE_NAME "\\\\.\\pipe\\cppchecker_%s"
#else
#define PIPE_NAME "/tmp/cppchecker.%s.sock"
#endif

//------------------------------------------------------

const wxEventType wxEVT_CPPCHECKJOB_STATUS_MESSAGE  = XRCID("cppcheck_status_message");
const wxEventType wxEVT_CPPCHECKJOB_CHECK_COMPLETED = XRCID("cppcheck_check_completed");
const wxEventType wxEVT_CPPCHECKJOB_REPORT          = XRCID("cppcheck_report");



//------------------------------------------------------

CppCheckSettings::CppCheckSettings()
: m_bAll(true)
, m_bForce(false)
, m_bStyle(true)
, m_bUnusedFunctions(false)
{
}

void CppCheckSettings::Serialize(Archive &arch)
{
	arch.Write(wxT("option.all"),             m_bAll);
	arch.Write(wxT("option.force"),           m_bForce);
	arch.Write(wxT("option.style"),           m_bStyle);
	arch.Write(wxT("option.unusedFunctions"), m_bUnusedFunctions);
	arch.Write(wxT("m_excludeFiles"),         m_excludeFiles);
}

void CppCheckSettings::DeSerialize(Archive &arch)
{
	arch.Read(wxT("option.all"),             m_bAll);
	arch.Read(wxT("option.force"),           m_bForce);
	arch.Read(wxT("option.style"),           m_bStyle);
	arch.Read(wxT("option.unusedFunctions"), m_bUnusedFunctions);
	arch.Read(wxT("m_excludeFiles"),         m_excludeFiles);
}

wxString CppCheckSettings::GetOptions() const
{
	wxString options;
	if (All()) {
		options << wxT(" --enable=all ");
	}
	if (Force()) {
		options << wxT("--force ");
	}
	if (Style()) {
		options << wxT(" --enable=style ");
	}
	if (UnusedFunctions()) {
		options << wxT(" --enable=unusedFunctions ");
	}
	return options;
}
