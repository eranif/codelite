#include <wx/app.h>
#include <wx/regex.h>
#include "svn_console.h"
#include <wx/file.h>
#include "svnsettingsdata.h"
#include "svn_command_handlers.h"
#include "subversion_view.h"
#include <wx/xrc/xmlres.h>
#include "subversion2.h"

void SvnCommitHandler::Process(const wxString& output)
{
	wxString svnOutput( output );
	svnOutput.MakeLower();
	if (svnOutput.Contains(wxT("could not authenticate to server"))) {
		// failed to login...
		wxCommandEvent event(XRCID("svn_commit2"));
		m_owner->AddPendingEvent( event );

	} else {
		SvnDefaultCommandHandler::Process(output);
	}
}

void SvnUpdateHandler::Process(const wxString& output)
{
	bool conflictFound ( false  );
	wxString svnOutput ( output );

	svnOutput.MakeLower();
	if (svnOutput.Contains(wxT("summary of conflicts:"))) {
		// A conflict was found
		conflictFound = true;
	}

	// After 'Update' we usually want to do the following:
	// Reload workspace (if a project file or the workspace were modified)
	// or retag the workspace
	if ( !conflictFound ) {

		// Retag workspace only if no conflict were found
		// send an event to the main frame indicating that a re-tag is required
		if( GetPlugin()->GetSettings().GetFlags() & SvnRetagWorkspace ) {
			wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
			GetPlugin()->GetManager()->GetTheApp()->GetTopWindow()->AddPendingEvent(e);
		}
	}

	// And finally, update the Subversion view
	SvnDefaultCommandHandler::Process(output);
}

void SvnDiffHandler::Process(const wxString& output)
{
	// Open an editor with the .diff file
	wxFile fp;
	wxString fileName = wxFileName::CreateTempFileName( wxT("svnDiff"), (wxFile*)&fp );

	if(fileName.IsEmpty() == false && fp.IsOpened()) {
		wxString modOutput (output);
		modOutput.Replace(wxT("\r\n"), wxT("\n"));
		fp.Write(modOutput);
		fp.Close();
	}

	wxRenameFile(fileName, fileName + wxT(".diff"));
	GetPlugin()->GetManager()->OpenFile(fileName + wxT(".diff"));
}

void SvnPatchHandler::Process(const wxString& output)
{
	// Retag workspace only if no conflict were found
	// send an event to the main frame indicating that a re-tag is required
	if( GetPlugin()->GetSettings().GetFlags() & SvnRetagWorkspace ) {
		wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
		GetPlugin()->GetManager()->GetTheApp()->GetTopWindow()->AddPendingEvent(e);
	}

	// And finally, update the Subversion view
	SvnDefaultCommandHandler::Process(output);
}

void SvnPatchDryRunHandler::Process(const wxString& output)
{
	GetPlugin()->GetConsole()->AppendText(wxT("===== APPLYING PATCH - DRY RUN =====\n"));
	GetPlugin()->GetConsole()->AppendText(output);
	GetPlugin()->GetConsole()->AppendText(wxT("===== OUTPUT END =====\n"));
}

void SvnVersionHandler::Process(const wxString& output)
{
	//GetPlugin()->GetShell()->AppendText(output);
	wxRegEx reVersion(wxT("svn, version ([0-9]\\.[0-9])(\\.[0-9])"));
	if(reVersion.Matches(output)) {
		wxString strVersion = reVersion.GetMatch(output, 1);

		double version(0.0);
		strVersion.ToDouble(&version);

		GetPlugin()->GetConsole()->AppendText(wxString::Format(wxT("== Svn client version: %s ==\n"), strVersion.c_str()));
		GetPlugin()->SetSvnClientVersion(version);
	}
}
