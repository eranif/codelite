#ifndef __custombuildrequest__
#define __custombuildrequest__

#include "compiler_action.h"

class CustomBuildRequest : public ShellCommand
{
	wxString m_fileName;
	
public:
	CustomBuildRequest(wxEvtHandler *owner, const QueueCommand &buildInfo, const wxString &fileName);
	virtual ~CustomBuildRequest();
	
public:
	virtual void Process();
};
#endif // __custombuildrequest__
