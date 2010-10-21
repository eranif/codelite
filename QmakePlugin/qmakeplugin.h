#ifndef __QMakePlugin__
#define __QMakePlugin__

#include "plugin.h"
#include "qmakeplugindata.h"

class QMakeTab;
class QmakeConf;

class QMakePlugin : public IPlugin
{
	std::map<wxString, QMakeTab*> m_pages;
	QmakeConf *                   m_conf;

protected:
	QMakeTab * DoGetQmakeTab     (const wxString &config);
	void       DoUnHookAllTabs   (wxNotebook *book);
	bool       DoGetData         (const wxString &project, const wxString &conf, QmakePluginData::BuildConfPluginData &bcpd);
	wxString   DoGetBuildCommand (const wxString &project, const wxString &config, bool projectOnly);

public:
	QMakePlugin(IManager *manager);
	~QMakePlugin();

	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual clToolBar *CreateToolBar     (wxWindow *parent);
	virtual void CreatePluginMenu        (wxMenu *pluginsMenu);
	virtual void HookPopupMenu           (wxMenu *menu, MenuType type);
	virtual void HookProjectSettingsTab  (wxNotebook *book, const wxString &projectName, const wxString &configName);
	virtual void UnHookProjectSettingsTab(wxNotebook *book, const wxString &projectName, const wxString &configName);
	virtual void UnPlug                  ();

	// event handlers
	void OnSaveConfig          (wxCommandEvent &event);
	void OnBuildStarting       (wxCommandEvent &event);
	void OnSettings            (wxCommandEvent &event);
	void OnGetCleanCommand     (wxCommandEvent &event);
	void OnGetBuildCommand     (wxCommandEvent &event);
	void OnGetIsPluginMakefile (wxCommandEvent &event);
	void OnNewQmakeBasedProject(wxCommandEvent &event);
	void OnOpenFile            (wxCommandEvent &event);
	void OnExportMakefile      (wxCommandEvent &event);
};

#endif //QMakePlugin

