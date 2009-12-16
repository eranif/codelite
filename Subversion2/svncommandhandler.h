#ifndef SVNCOMMANDHANDLER_H
#define SVNCOMMANDHANDLER_H

#include <wx/string.h>
#include <wx/event.h>

class Subversion2;
class IManager;
class IProcess;

#define LOGIN_REQUIRES 1253

class SvnCommandHandler
{
protected:
	Subversion2*  m_plugin;
	int           m_commandId;
	wxEvtHandler* m_owner;

public:
	SvnCommandHandler(Subversion2 *plugin, int commandId, wxEvtHandler *owner)
			: m_plugin   (plugin   )
			, m_commandId(commandId)
			, m_owner    (owner    )
			{
	}

	virtual ~SvnCommandHandler() {
	}

	Subversion2* GetPlugin() {
		return m_plugin;
	}

	void SetCommandId(int commandId) {
		this->m_commandId = commandId;
	}
	
	void SetOwner(wxEvtHandler* owner) {
		this->m_owner = owner;
	}

	int GetCommandId() const {
		return m_commandId;
	}

	wxEvtHandler* GetOwner() {
		return m_owner;
	}
	
	bool TestLoginRequired(const wxString &output) {
		
		wxString svnOutput( output );
		svnOutput.MakeLower();
		if (svnOutput.Contains(wxT("could not authenticate to server")) || svnOutput.Contains(wxT(": authorization failed"))) {
			// failed to login...
			return true;
		}
		return false;
	}
	
	/**
	 * @brief handle here process output (e.g. interactive commands that needs response)
	 * @param process the process
	 * @param output  the process output
	 */
	virtual void OnProcessOutput(IProcess *process, const wxString &output);
	
	/**
	 * @brief the svn operation failed due to login error. Retry the last command but this
	 * time, pop the login dialog
	 */
	virtual void ProcessLoginRequired() {
		if(m_commandId != wxNOT_FOUND && m_owner) {
			int eventId (m_commandId);
			wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, eventId);
			event.SetInt(LOGIN_REQUIRES);
			m_owner->AddPendingEvent(event);
		}
	}
	
	virtual void Process(const wxString &output) = 0;
};

#endif // SVNCOMMANDHANDLER_H
