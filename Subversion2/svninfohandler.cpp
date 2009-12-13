#include "svninfohandler.h"
#include "subversion_view.h"
#include "svnxml.h"
#include "svninfo.h"
#include "subversion2.h"

SvnInfoHandler::SvnInfoHandler(Subversion2 *plugin, int reason)
		: SvnCommandHandler(plugin)
		, m_reason(reason)
{
}

SvnInfoHandler::~SvnInfoHandler()
{
}

void SvnInfoHandler::Process(const wxString& output)
{
	SvnInfo svninfo;
	SvnXML::GetSvnInfo(output, svninfo);
	m_plugin->GetSvnPage()->OnSvnInfo(svninfo, m_reason);
}
