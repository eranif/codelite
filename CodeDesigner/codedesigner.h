#ifndef __CodeDesigner__
#define __CodeDesigner__

#include "plugin.h"
#include "codedesigneritemdlg.h"
#include <wx/ipc.h>
#include <wx/list.h>

class CDServer;
class CDConnection;

class CodeDesigner : public IPlugin
{
public:
	CodeDesigner(IManager *manager);
	~CodeDesigner();
	
	IManager* GetManager() { return m_mgr; }
	
	void RetagWorkspace();
	bool AddFilesToWorkspace(wxArrayString& paths, CDConnection *connectio );

	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual clToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();

protected:
	bool m_addFileMenu;
	wxMenuItem*   m_separatorItem;
	CDServer *m_Server;

	wxMenu *CreatePopupMenu();
	void DoLaunchCD(const wxString &file);
	void DoCreateCDProject(CDItemInfo &info);
	wxString GetCDPath();
	void StartServer();
	
	void OnSettings(wxCommandEvent &e);
	void OnOpenFile(wxCommandEvent &e);
	void OnOpenWithCD(wxCommandEvent &e);
	void OnUpdateOpenWithCD(wxUpdateUIEvent &e);
	void OnCDTerminated(wxCommandEvent &e);
	void OnNewSCH(wxCommandEvent &e);
	void OnNewHSCH(wxCommandEvent &e);
	void OnNewCD(wxCommandEvent &e);
	void OnPokeData(wxCommandEvent &e);
};

class CDServer;

class CDConnection : public wxConnection
{
public:
	CDConnection(CDServer *server);
	virtual ~CDConnection();
	
	virtual bool OnPoke(const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
	virtual bool OnDisconnect();
	
	void SetTreeFolder(const wxString& TreeFolder) {this->m_TreeFolder = TreeFolder;}
	const wxString& GetTreeFolder() const {return m_TreeFolder;}
	
protected:
	CDServer *m_Server;
	wxString m_TreeFolder;
};

WX_DECLARE_LIST(CDConnection, ConnectionList);

class CDServer : public wxServer
{
public:
	CDServer();
	virtual ~CDServer();
	
	void Disconnect(CDConnection *connection);
	
	virtual wxConnectionBase *OnAcceptConnection(const wxString& topic);
	
protected:
	ConnectionList m_Connections;
};

#endif //CodeDesigner
