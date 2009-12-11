#ifndef SVNCOMMITHANDLER_H
#define SVNCOMMITHANDLER_H

#include "svn_default_command_handler.h" // Base class

class SvnCommitHandler : public SvnDefaultCommandHandler {

public:
	SvnCommitHandler(Subversion2 *plugin);
	virtual ~SvnCommitHandler();

public:
	virtual void Process(const wxString &output);
};

#endif // SVNCOMMITHANDLER_H
