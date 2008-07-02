#ifndef __webupdatethread__
#define __webupdatethread__

#include "job.h"

extern const wxEventType wxEVT_CMD_NEW_VERSION_AVAILABLE;
extern const wxEventType wxEVT_CMD_VERSION_UPTODATE;

class WebUpdateJobData
{

	wxString m_url;
	long m_curVersion;
	long m_newVersion;
	bool m_upToDate;
	
public:
	WebUpdateJobData(const wxString &url, long curVersion, long newVersion, bool upToDate)
			: m_url(url.c_str())
			, m_curVersion(curVersion) 
			, m_newVersion(newVersion) 
			, m_upToDate(upToDate)
			{}
			
	~WebUpdateJobData() {}

	const wxString& GetUrl() const {
		return m_url;
	}
	long GetCurrentVersion() const {
		return m_curVersion;
	}
	long GetNewVersion() const {
		return m_newVersion;
	}
	
	bool IsUpToDate() const {
		return m_upToDate;
	}
	
};

class WebUpdateJob : public Job
{
	wxString m_dataRead;
public:
	WebUpdateJob(wxEvtHandler *parent);
	virtual ~WebUpdateJob();
	void ParseFile();

	static size_t WriteData(void *buffer, size_t size, size_t nmemb, void *obj);

public:
	virtual void Process(wxThread *thread);
};
#endif // __webupdatethread__


