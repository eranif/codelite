#ifndef __custombuildrequest__
#define __custombuildrequest__

#include "compiler_action.h"
#include "build_config.h"

class CustomBuildRequest : public ShellCommand
{
	wxString m_fileName;
	
protected:
	void DoUpdateCommand(wxString &cmd, ProjectPtr proj, BuildConfigPtr bldConf);
	
public:
	CustomBuildRequest(wxEvtHandler *owner, const QueueCommand &buildInfo, const wxString &fileName);
	virtual ~CustomBuildRequest();
	
public:
	virtual void Process();
};
#endif // __custombuildrequest__
