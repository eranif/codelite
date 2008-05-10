#ifndef __refactorindexbuildjob__
#define __refactorindexbuildjob__

#include "cpptoken.h"
#include <vector>
#include <wx/filename.h>

class RefactorIndexBuildJob
{
	std::vector<wxFileName> m_files;
	
public:
	RefactorIndexBuildJob(const std::vector<wxFileName> &files);
	virtual ~RefactorIndexBuildJob();

public:
	void Parse(CppTokensMap &l);
};
#endif // __refactorindexbuildjob__
