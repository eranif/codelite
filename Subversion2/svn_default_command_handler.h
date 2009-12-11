#ifndef SVNCOMMITHANDLER_H
#define SVNCOMMITHANDLER_H

#include "svncommandhandler.h" // Base class

class SubversionPage;

class SvnDefaultCommandHandler : public SvnCommandHandler {
	SubversionPage *m_ui;

public:
	SvnDefaultCommandHandler(IManager *manager, SubversionPage *ui);
	virtual ~SvnDefaultCommandHandler();

public:
	virtual void Process(const wxString &output);
};

#endif // SVNCOMMITHANDLER_H
