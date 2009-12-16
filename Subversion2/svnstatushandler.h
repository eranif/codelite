#ifndef SVNSTATUSHANDLER_H
#define SVNSTATUSHANDLER_H

#include "svncommandhandler.h" // Base class

class SubversionView;
class Subversion2;

class SvnStatusHandler : public SvnCommandHandler {
public:
	SvnStatusHandler(Subversion2 *plugin, int commandId, wxEvtHandler *owner);
	virtual ~SvnStatusHandler();

public:
	virtual void Process(const wxString &output);
};

#endif // SVNSTATUSHANDLER_H
