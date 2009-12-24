#ifndef __CppChecker__
#define __CppChecker__

#include "plugin.h"
#include "cppcheckjob.h"
#include <wx/process.h>

class clProcess;
class wxMenuItem;
class CppCheckReportPage;

extern const wxEventType wxEVT_CPPCHECK_START_DAEMON;

class CppCheckPlugin : public IPlugin
{
	wxString             m_cppcheckPath;
	clProcess *          m_cppcheckProcess;
	bool                 m_canRestart;
	wxArrayString        m_filelist;
	wxMenuItem*          m_explorerSepItem;
	wxMenuItem*          m_workspaceSepItem;
	wxMenuItem*          m_projectSepItem;
	CppCheckReportPage * m_view;
	bool                 m_analysisInProgress;
	size_t               m_fileCount;
	CppCheckSettings     m_settings;
#ifdef __WXMSW__
	bool                 m_restartRequired;
#endif // __WXMSW__
	size_t               m_fileProcessed;

protected:
	wxMenu *         CreateFileExplorerPopMenu();
	wxMenu *         CreateWorkspacePopMenu();
	wxMenu *         CreateProjectPopMenu();

protected:
	void             GetFileListFromDir( const wxString &root );
	void             ProcessNextFromList();
	void             RemoveExcludedFiles();
	void             SetTabVisible(bool clearContent);
	void             DoProcess(size_t count);
	void             DoStartTest();

protected:
	// Events
	void OnStartDaemon          (wxCommandEvent &e);
	/**
	 * @brief handle the context meun activation from the file explorer
	 * @param e
	 */
	void OnCheckFileExplorerItem(wxCommandEvent &e);
	/**
	 * @brief handle the context meun activation from the workspace
	 * @param e
	 */
	void OnCheckWorkspaceItem(wxCommandEvent &e);
	/**
	 * @brief handle the context meun activation from the project
	 * @param e
	 */
	void OnCheckProjectItem(wxCommandEvent &e);
	/**
	 * @brief handles the cppcheck process termination
	 * @param e
	 */
	void OnCppCheckTerminated   (wxProcessEvent &e);
	/**
	 * @brief called by the worker thread when it wants to report a message (informative one)
	 * @param e
	 */
	void OnStatusMessage        (wxCommandEvent &e);
	/**
	 * @brief Send by the worker thread when it has done compelted processing a file
	 * the current behavior is to to process the next file on the list
	 * @param e
	 */
	void OnCheckCompleted       (wxCommandEvent &e);
	/**
	 * @brief called by the worker thread, when a complete report for a file
	 * is retrieved
	 * @param e event
	 */
	void OnReport               (wxCommandEvent &e);

	/**
	 * @brief handle the workspace closed event and clear the view
	 * @param e
	 */
	void OnWorkspaceClosed      (wxCommandEvent &e);
	/**
	 * @brief handle the settings item
	 * @param e event
	 */
	void OnSettingsItem         (wxCommandEvent &e);

	/**
	 * @brief launch the daemon
	 */
	bool StartCppCheckDaemon();

public:
	CppCheckPlugin(IManager *manager);
	~CppCheckPlugin();

	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();

	/**
	 * @brief stop the analysis of the current file
	 * and clear the queue
	 */
	void StopAnalysis();
	/**
	 * @brief stop the analysis of the current file
	 * this method does not clear the queue
	 */
	void SkipCurrentFile();
	/**
	 * @brief return true if analysis currently running
	 */
	bool AnalysisInProgress() const {
		return m_analysisInProgress;
	}

	/**
	 * @brief return the progress
	 * @return value between 0-100
	 */
	size_t GetProgress();
};

#endif //CppChecker
