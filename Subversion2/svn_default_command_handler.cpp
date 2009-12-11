#include "svn_default_command_handler.h"
#include "subversion_page.h"

SvnDefaultCommandHandler::SvnDefaultCommandHandler(IManager *manager, SubversionPage *ui)
		: SvnCommandHandler(manager)
		, m_ui(ui)
{
}

SvnDefaultCommandHandler::~SvnDefaultCommandHandler()
{
}

void SvnDefaultCommandHandler::Process(const wxString &output)
{
	// TODO:: do something with the output
	wxUnusedVar(output);

	// Refresh the SVN output page
	m_ui->BuildTree();
}
