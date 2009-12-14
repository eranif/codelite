#ifndef SVNCOMMANDHANDLER_H
#define SVNCOMMANDHANDLER_H

#include <wx/string.h>

class Subversion2;
class IManager;
class IProcess;

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

	/**
	 * @brief handle here process output (e.g. interactive commands that needs response)
	 * @param process the process
	 * @param output  the process output
	 */
	virtual void OnProcessOutput(IProcess *process, const wxString &output);
	virtual void Process(const wxString &output) = 0;
};

#endif // SVNCOMMANDHANDLER_H
