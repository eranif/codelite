#include "svninfohandler.h"
#include "subversion_page.h"
#include "svnxml.h"
#include "svninfo.h"

SvnInfoHandler::SvnInfoHandler(IManager *manager, int reason, SubversionPage *ui)
		: SvnCommandHandler(manager)
		, m_reason(reason)
		, m_ui    (ui)
{
}

SvnInfoHandler::~SvnInfoHandler()
{
}

void SvnInfoHandler::Process(const wxString& output)
{
	SvnInfo svninfo;
	SvnXML::GetSvnInfo(output, svninfo);
	m_ui->OnSvnInfo(svninfo, m_reason);
}
