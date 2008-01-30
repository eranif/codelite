#ifndef __MakefileImporter__
#define __MakefileImporter__

#include "plugin.h"

class MakefileImporter : public IPlugin
{
public:
	MakefileImporter(IManager *manager);
	~MakefileImporter();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
	void OnImportMakefile(wxCommandEvent &event);
	void OnImportMakefileUI(wxUpdateUIEvent &event);
	void OnImporterOptions(wxCommandEvent &e);
	void OnImporterOptionsUI(wxUpdateUIEvent &e);
	
private:
	void ImportFromMakefile(const wxString &path);

	void LogMessage(const wxString& message);
	void ClearMessagePane();
};

#endif //MakefileImporter

