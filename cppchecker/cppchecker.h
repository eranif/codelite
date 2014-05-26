#ifndef __CppChecker__
#define __CppChecker__

#include "plugin.h"
#include "asyncprocess.h"
#include "cppcheck_settings.h"

class wxMenuItem;
class CppCheckReportPage;

class CppCheckPlugin : public IPlugin
{
	wxString             m_cppcheckPath;
	IProcess*            m_cppcheckProcess;
	bool                 m_canRestart;
	wxArrayString        m_filelist;
	wxMenuItem*          m_explorerSepItem;
	wxMenuItem*          m_workspaceSepItem;
	wxMenuItem*          m_projectSepItem;
	CppCheckReportPage * m_view;
	bool                 m_analysisInProgress;
	size_t               m_fileCount;
	CppCheckSettings     m_settings;
	size_t               m_fileProcessed;

protected:
	wxString         DoGetCommand();
	wxString         DoGenerateFileList();

protected:
	wxMenu *         CreateFileExplorerPopMenu();
	wxMenu *         CreateWorkspacePopMenu();
	wxMenu *         CreateProjectPopMenu();

protected:
	void             GetFileListFromDir( const wxString &root );
	void             RemoveExcludedFiles();
	void             SetTabVisible(bool clearContent);
	void             DoProcess();
	void             DoStartTest(ProjectPtr proj = NULL);
    ProjectPtr       FindSelectedProject();
    void             DoSettingsItem(ProjectPtr project = NULL);

protected:
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
	void OnCppCheckTerminated   (wxCommandEvent &e);

	/**
	 * @brief there is data to read from the process
	 * @param e
	 */
	void OnCppCheckReadData     (wxCommandEvent &e);

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
	 * @brief handle the settings item from a project context menu
	 * @param e event
	 */
	void OnSettingsItemProject  (wxCommandEvent &e);
public:
	CppCheckPlugin(IManager *manager);
	~CppCheckPlugin();

	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual clToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
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
	bool AnalysisInProgress() const { return m_cppcheckProcess != NULL;}

	/**
	 * @brief return the progress
	 * @return value between 0-100
	 */
	size_t GetProgress();

	DECLARE_EVENT_TABLE()
};

#endif //CppChecker
