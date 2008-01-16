#include "newwxprojectinfo.h"
NewWxProjectInfo::NewWxProjectInfo()
{
	size_t flags(0);
#if defined (__WXMSW__)	
	flags |= wxWidgetsSetMWindows;
#endif
	flags |= wxWidgetsUnicode;
	SetFlags(flags);
}

NewWxProjectInfo::~NewWxProjectInfo()
{
}

