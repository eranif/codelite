#ifndef __refactorindexbuildjob__
#define __refactorindexbuildjob__

#include <vector>
#include <wx/filename.h>
#include "job.h"

class RefactorIndexBuildJob : public Job
{
	std::vector<wxFileName> m_files;
public:
	RefactorIndexBuildJob(wxEvtHandler *handler, const std::vector<wxFileName> &files);
	virtual ~RefactorIndexBuildJob();

public:
	virtual void Process();
};
#endif // __refactorindexbuildjob__
