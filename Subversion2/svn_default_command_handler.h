#ifndef SVN_DEFAULT_COMMAND_HANDLER_H
#define SVN_DEFAULT_COMMAND_HANDLER_H

#include "svncommandhandler.h" // Base class

class SubversionView;

class SvnDefaultCommandHandler : public SvnCommandHandler {
public:
	SvnDefaultCommandHandler(Subversion2 *plugin);
	virtual ~SvnDefaultCommandHandler();

public:
	virtual void Process(const wxString &output);
};

#endif // SVN_DEFAULT_COMMAND_HANDLER_H
