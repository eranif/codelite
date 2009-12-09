#ifndef SVNCOMMANDHANDLER_H
#define SVNCOMMANDHANDLER_H

#include <wx/string.h>
#include "imanager.h"

class SvnCommandHandler
{
	IManager *m_manager;

public:
	SvnCommandHandler(IManager *manager)
			: m_manager(manager) {
	}

	virtual ~SvnCommandHandler() {
	}

	IManager* GetManager() {
		return m_manager;
	}

	virtual void Process(const wxString &output) = 0;
};

#endif // SVNCOMMANDHANDLER_H
