#ifndef __singleinstancethreadjob__
#define __singleinstancethreadjob__

#include <wx/event.h>
#include "job.h"

extern const wxEventType wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES;
extern const wxEventType wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP;

class SingleInstanceThreadJob : public Job {
	wxString m_path;
	
protected:	
	void ProcessFile(const wxString &fileContent);
	
public:
	SingleInstanceThreadJob(wxEvtHandler *parent, const wxString &path);
	virtual ~SingleInstanceThreadJob();
	
public:
	virtual void Process(wxThread *thread);
};
#endif // __singleinstancethreadjob__
