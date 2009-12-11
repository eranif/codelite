#ifndef SVNCOMMANDHANDLER_H
#define SVNCOMMANDHANDLER_H

#include <wx/string.h>

class Subversion2;
class IManager;

class SvnCommandHandler
{
protected:
	Subversion2 *m_plugin;

public:
	SvnCommandHandler(Subversion2 *plugin)
			: m_plugin(plugin)
	{
	}

	virtual ~SvnCommandHandler() {
	}

	Subversion2* GetPlugin() {
		return m_plugin;
	}

	virtual void Process(const wxString &output) = 0;
};

#endif // SVNCOMMANDHANDLER_H
