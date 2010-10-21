#ifndef __MacBundler__
#define __MacBundler__

#include "plugin.h"
#include "workspace.h"

class wxCommandEvent;

class MacBundler : public IPlugin
{
    int m_popup_id;
    void showSettingsDialogFor(ProjectPtr project);

public:
	MacBundler(IManager *manager);
	~MacBundler();

    void onBundleInvoked_active(wxCommandEvent& evt);
    void onBundleInvoked_selected(wxCommandEvent& evt);

	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual clToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
};

#endif //MacBundler

