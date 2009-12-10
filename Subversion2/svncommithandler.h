#ifndef SVNCOMMITHANDLER_H
#define SVNCOMMITHANDLER_H

#include "svncommandhandler.h" // Base class

class SubversionPage;

class SvnCommitHandler : public SvnCommandHandler {
	SubversionPage *m_ui;

public:
	SvnCommitHandler(IManager *manager, SubversionPage *ui);
	virtual ~SvnCommitHandler();

public:
	virtual void Process(const wxString &output);
};

#endif // SVNCOMMITHANDLER_H
