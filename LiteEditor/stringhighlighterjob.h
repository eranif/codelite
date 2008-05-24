#ifndef __stringhighlighterjob__
#define __stringhighlighterjob__

#include "job.h"

class StringHighlighterJob : public Job {
	
	wxString m_str;
	wxString m_word;
	
public:
	StringHighlighterJob(wxEvtHandler *parent, const wxChar *str, const wxChar *word);
	virtual ~StringHighlighterJob();

public:
	virtual void Process(wxThread *thread);
};
#endif // __stringhighlighterjob__
