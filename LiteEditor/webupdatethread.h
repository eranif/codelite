// temporarly this is supported only under Windows & GTK
#if defined (__WXGTK__) || defined (__WXMSW__)

#ifndef __webupdatethread__
#define __webupdatethread__

#include "job.h"

class WebUpdateThread : public Job {

public:
	WebUpdateThread();
	virtual ~WebUpdateThread();

public:
	virtual void Process(wxThread *thread);
};
#endif // __webupdatethread__
#endif
