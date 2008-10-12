#ifndef __queuecommand__
#define __queuecommand__

#include <wx/string.h>

class QueueCommand
{
	wxString m_project;
	wxString m_configuration;
	bool m_projectOnly;
	int m_kind;
	bool m_cleanLog;
	bool m_checkBuildSuccess;
	wxString m_customBuildTarget;

public:
	// Command's kind
	enum {
		Build,
		Clean,
		CustomBuild,
		Debug,
		Cmd,
		BatchCmds
	};

public:
	QueueCommand(const wxString &project, const wxString &configuration, bool projectOnly, int kind);
	QueueCommand(int kind);
	~QueueCommand();

	//----------------------------------------
	// Setters/Getters
	//----------------------------------------

	void SetConfiguration(const wxString& configuration) ;
	void SetProject(const wxString& project) ;
	const wxString& GetConfiguration() const ;
	const wxString& GetProject() const ;
	void SetProjectOnly(const bool& projectOnly) ;
	const bool& GetProjectOnly() const ;
	void SetKind(const int& kind) ;
	const int& GetKind() const ;
	void SetCleanLog(const bool& cleanLog) ;
	const bool& GetCleanLog() const ;
	void SetCheckBuildSuccess(const bool& checkBuildSuccess) {
		this->m_checkBuildSuccess = checkBuildSuccess;
	}
	const bool& GetCheckBuildSuccess() const {
		return m_checkBuildSuccess;
	}
	void SetCustomBuildTarget(const wxString& customBuildTarget) {
		this->m_customBuildTarget = customBuildTarget;
	}
	const wxString& GetCustomBuildTarget() const {
		return m_customBuildTarget;
	}
};

#endif // __queuecommand__
