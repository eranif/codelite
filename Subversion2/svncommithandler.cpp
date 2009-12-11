#include "svncommithandler.h"
#include "subversion_page.h"
#include <wx/xrc/xmlres.h>
#include "subversion2.h"
SvnCommitHandler::SvnCommitHandler(Subversion2 *plugin)
		: SvnDefaultCommandHandler(plugin)
{
}

SvnCommitHandler::~SvnCommitHandler()
{
}

void SvnCommitHandler::Process(const wxString& output)
{
	wxString svnOutput( output );
	svnOutput.MakeLower();
	if (svnOutput.Contains(wxT("could not authenticate to server"))) {
		// failed to login...
		wxCommandEvent event(XRCID("svn_commit2"));
		GetPlugin()->GetSvnPage()->AddPendingEvent( event );

	} else {
		SvnDefaultCommandHandler::Process(output);
	}
}
