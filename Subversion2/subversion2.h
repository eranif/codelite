#ifndef __Subversion2__
#define __Subversion2__

#include "plugin.h"
#include "svnsettingsdata.h"

class SubversionPage;
class SvnConsole;

class Subversion2 : public IPlugin
{
private:
	SubversionPage *m_subversionPage;
	SvnConsole       *m_subversionShell;

protected:
	void OnSettings(wxCommandEvent &event);

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

	SubversionPage *GetSvnPage() {
		return m_subversionPage;
	}

	SvnSettingsData GetSettings();
	void            SetSettings(SvnSettingsData& ssd);

protected:
	void DoInitialize();
};

#endif //Subversion2

