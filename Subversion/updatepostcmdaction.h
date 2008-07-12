#ifndef __updatepostcmdaction__
#define __updatepostcmdaction__

#include "svniconrefreshhandler.h"

class IManager;
class SubversionPlugin;

class UpdatePostCmdAction : public SvnIconRefreshHandler {

public:
	UpdatePostCmdAction(IManager *mgr, SubversionPlugin *plugin);
	virtual ~UpdatePostCmdAction();

public:
	virtual void DoCommand();
};
#endif // __updatepostcmdaction__
