#ifndef SVNINFOHANDLER_H
#define SVNINFOHANDLER_H

#include "svncommandhandler.h" // Base class
class SubversionPage;

class SvnInfoHandler : public SvnCommandHandler {
	int             m_reason;
	SubversionPage *m_ui;

public:
	SvnInfoHandler(IManager *manager, int reason, SubversionPage *ui);
	virtual ~SvnInfoHandler();

public:
	virtual void Process(const wxString &output);
};

#endif // SVNINFOHANDLER_H
