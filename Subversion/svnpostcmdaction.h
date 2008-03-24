#ifndef __svnpostcmdaction__
#define __svnpostcmdaction__

class SvnPostCmdAction {

public:
	SvnPostCmdAction();
	virtual ~SvnPostCmdAction();
	
	virtual void DoCommand() = 0;
};
#endif // __svnpostcmdaction__
