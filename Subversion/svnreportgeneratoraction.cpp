#include "svnreportgeneratoraction.h"
#include "wx/event.h"
#include "subversion.h"

SvnReportGeneratorAction::SvnReportGeneratorAction(SubversionPlugin *plugin, int eventId)
: m_eventId(eventId)
, m_plugin(plugin)
{
}

SvnReportGeneratorAction::~SvnReportGeneratorAction()
{
}

void SvnReportGeneratorAction::DoCommand()
{
	if(m_plugin) {
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, m_eventId);
		event.SetEventObject(m_plugin);
		wxPostEvent(m_plugin, event);
	}
}
