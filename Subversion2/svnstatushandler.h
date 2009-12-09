#ifndef SVNSTATUSHANDLER_H
#define SVNSTATUSHANDLER_H

#include "svncommandhandler.h" // Base class

class SubversionPage;

class SvnStatusHandler : public SvnCommandHandler {
	SubversionPage *m_ui;
public:
	SvnStatusHandler(IManager *manager, SubversionPage *ui);
	virtual ~SvnStatusHandler();

public:
	virtual void Process(const wxString &output);
};

#endif // SVNSTATUSHANDLER_H
