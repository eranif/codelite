#ifndef DBCONTENTCACHER_H
#define DBCONTENTCACHER_H

#include "job.h" // Base class: Job

extern const wxEventType wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED;

class DbContentCacher : public Job 
{
	wxString m_filename;
public:
	DbContentCacher(wxEvtHandler* parent, const wxChar* dbfilename);
	virtual ~DbContentCacher();

public:
	virtual void Process(wxThread* thread);
};

#endif // DBCONTENTCACHER_H
