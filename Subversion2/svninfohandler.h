#ifndef SVNINFOHANDLER_H
#define SVNINFOHANDLER_H

#include "svncommandhandler.h" // Base class
class SubversionPage;

class SvnInfoHandler : public SvnCommandHandler {
	int             m_reason;
public:
	SvnInfoHandler(Subversion2 *plugin, int reason);
	virtual ~SvnInfoHandler();

public:
	virtual void Process(const wxString &output);
};

#endif // SVNINFOHANDLER_H
