#ifndef __Subversion2__
#define __Subversion2__

#include "plugin.h"
#include "svnsettingsdata.h"

class SubversionView;
class SvnConsole;
class wxMenu;
class wxMenuItem;

class Subversion2 : public IPlugin
{
private:
	SubversionView*   m_subversionPage;
	SvnConsole*       m_subversionShell;
	wxMenuItem*       m_explorerSepItem;

protected:
	void OnSettings(wxCommandEvent &event);

	///////////////////////////////////////////////////////////
	// File Explorer event handlers
	///////////////////////////////////////////////////////////
	void OnCommit  (wxCommandEvent &event);
	void OnCommit2 (wxCommandEvent &event); // Commit with login prompt
	void OnUpdate  (wxCommandEvent &event);
	void OnAdd     (wxCommandEvent &event);
	void OnDelete  (wxCommandEvent &event);
	void OnRevert  (wxCommandEvent &event);

	wxMenu* CreateFileExplorerPopMenu();

public:
	Subversion2(IManager *manager);
	~Subversion2();

	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();

	SvnConsole *GetShell() {
		return m_subversionShell;
	}

	IManager *GetManager() {
		return m_mgr;
	}

	SubversionView *GetSvnPage() {
		return m_subversionPage;
	}

	SvnSettingsData GetSettings();
	void            SetSettings(SvnSettingsData& ssd);
	wxString        GetSvnExeName();
	wxString        GetUserConfigDir();
	void            UpdateIgnorePatterns();

protected:
	void DoInitialize();
	void DoSetSSH();
	wxString DoGetFileExplorerItemFullPath();
	wxString DoGetFileExplorerItemPath();
};

#endif //Subversion2

