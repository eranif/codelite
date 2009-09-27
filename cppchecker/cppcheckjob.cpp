#include <wx/xrc/xmlres.h>
#include <memory>
#include <wx/stdpaths.h>
#include "procutils.h"
#include "network/cppchecker_request.h"
#include "cppcheckjob.h"
#include "plugin.h"
#include "network/cppchecker_request.h"
#include "network/cppchecker_protocol.h"
#include "network/named_pipe_client.h"
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
		options << wxT("--all ");
	}
	if (Force()) {
		options << wxT("--force ");
	}
	if (Style()) {
		options << wxT("--style ");
	}
	if (UnusedFunctions()) {
		options << wxT("--unused-functions ");
	}
	options << wxT("--xml ");
	return options;
}

//------------------------------------------------------
CppCheckJob::CppCheckJob(IPlugin *plugin, const CppCheckSettings* settings)
		: m_plugin(plugin)
		, m_settings(settings)
{
}

CppCheckJob::~CppCheckJob()
{
}

void CppCheckJob::Process(wxThread* thread)
{
#ifdef __WXMSW__
	// Windows way: the hard way, connect to the named pipe and read
	// the output from it

	// build the channel name
	char channel_name[256];

	sprintf(channel_name, PIPE_NAME, GetProcessId().mb_str(wxConvUTF8).data());

	clNamedPipeClient client(channel_name);

	CPPCheckerRequest req;
	wxCharBuffer buff = GetFilename().mb_str(wxConvUTF8);
	req.setFile( buff.data() ); //set the file to be parsed here

	// Establish connection
	if ( client.connect() == false ) {
		SendStatusMessage( wxT("Failed to connect to codelite_cppcheck daemon" ) );
		SendCompleteEvent();
		return;
	}

	if ( CPPCheckerProtocol::SendRequest(&client, req) == false ) {
		SendStatusMessage( wxT("Failed to send request to codelite_cppcheck daemon" ) );
		SendCompleteEvent();
		return;
	}

	while ( thread && !thread->TestDestroy() ) {

		CPPCheckerReply reply;

		//SendStatusMessage( wxT("Reading reply...") );
		if ( CPPCheckerProtocol::ReadReply(&client, reply) == false ) {
			SendStatusMessage( wxT("Failed to read reply from codelite_cppcheck daemon" ) );
			SendCompleteEvent();
			return;
		}

		//SendStatusMessage( wxT("Received reply...") );
		switch ( reply.getCompletionCode() ) {
		case CPPCheckerReply::StatusMessage:
			// Informative message
			SendStatusMessage( wxString::From8BitData(reply.getReport().c_str()) );
			break;
		case CPPCheckerReply::CheckingCompleted:
			// Done, send complete event + the report
			SendFileReport   ();
			SendCompleteEvent();
			return;
		case CPPCheckerReply::CheckingInProgress:
			// Concatentate the report
			m_reply << wxString::From8BitData(reply.getReport().c_str());
			break;
		}
	}
	client.disconnect();
#else
	// Linux / Mac way: spawn the process and execute the command
	wxString cmd, path;
#if defined (__WXMAC__)
	path = wxStandardPaths::Get().GetDataDir();
#else
	wxFileName exePath( wxStandardPaths::Get().GetExecutablePath() );
	path = exePath.GetPath();
#endif

	path << wxFileName::GetPathSeparator() << wxT("codelite_cppcheck");

	// build the command
	cmd << wxT("\"") << path << wxT("\" ");
	cmd << m_settings->GetOptions();
	cmd << m_filename;

	wxArrayString out_put;
	ProcUtils::SafeExecuteCommand(cmd, out_put);
	for( size_t i=0; i<out_put.GetCount(); i++ ) {
		m_reply << out_put.Item(i);
	}
	SendFileReport   ();
	SendCompleteEvent();
#endif
}

wxString CppCheckJob::GetProcessId()
{
	wxString pid;
	pid << wxGetProcessId();
	return pid;
}

void CppCheckJob::SendStatusMessage(const wxString& msg)
{
	wxCommandEvent e( wxEVT_CPPCHECKJOB_STATUS_MESSAGE );
	e.SetString(msg);
	m_plugin->AddPendingEvent( e );
}

void CppCheckJob::SendCompleteEvent()
{
	wxCommandEvent e( wxEVT_CPPCHECKJOB_CHECK_COMPLETED );
	m_plugin->AddPendingEvent( e );
}

void CppCheckJob::SendFileReport()
{
	wxCommandEvent e( wxEVT_CPPCHECKJOB_REPORT );
	e.SetString(m_reply);
	m_plugin->AddPendingEvent( e );
}
