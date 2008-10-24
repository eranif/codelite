#ifndef __applypatchpostcmdaction__
#define __applypatchpostcmdaction__

#include "svniconrefreshhandler.h"

class ApplyPatchPostCmdAction : public SvnIconRefreshHandler {

public:
	ApplyPatchPostCmdAction(IManager *mgr, SubversionPlugin *plugin);
	virtual ~ApplyPatchPostCmdAction();

public:
	virtual void DoCommand();
};
#endif // __applypatchpostcmdaction__
