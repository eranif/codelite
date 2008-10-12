#ifndef __custombuildrequest__
#define __custombuildrequest__
#include "compiler_action.h"

class CustomBuildRequest : public ShellCommand
{
public:
	CustomBuildRequest(wxEvtHandler *owner, const QueueCommand &buildInfo);
	virtual ~CustomBuildRequest();

public:
	virtual void Process();
};
#endif // __custombuildrequest__
