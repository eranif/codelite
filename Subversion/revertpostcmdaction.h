#ifndef __revertpostcmdaction__
#define __revertpostcmdaction__

#include "svnpostcmdaction.h"

class IManager;

class RevertPostCmdAction : public SvnPostCmdAction {
	IManager *m_mgr;
public:
	RevertPostCmdAction(IManager *mgr);
	virtual ~RevertPostCmdAction();

public:
	virtual void DoCommand();
};
#endif // __revertpostcmdaction__
