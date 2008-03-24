#ifndef __svnreportgeneratoraction__
#define __svnreportgeneratoraction__

#include "svnpostcmdaction.h"

class SubversionPlugin;

class SvnReportGeneratorAction : public SvnPostCmdAction {
	int m_eventId;
	SubversionPlugin *m_plugin;
	
public:
	SvnReportGeneratorAction(SubversionPlugin *plugin, int eventId);
	virtual ~SvnReportGeneratorAction();

public:
	virtual void DoCommand();
};
#endif // __svnreportgeneratoraction__
