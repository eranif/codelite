#ifndef SUBVERSION_H
#define SUBVERSION_H

#include "plugin.h"
#include "map"
#include "list"
#include "wx/timer.h"
#include "wx/treectrl.h"
#include "svnoptions.h"

class SvnDriver;
class VdtcTreeItemBase;

class SubversionPlugin : public IPlugin
{
	wxMenu *m_svnMenu;
	SvnDriver *m_svn;
	wxEvtHandler *topWin;
	wxTreeItemId m_firstVisibleItem;
	SvnOptions m_options;	
	friend class SvnDriver;
	bool m_isValid;
	bool m_initIsDone;
	wxMenuItem *m_sepItem;
	
public:
	SubversionPlugin(IManager *manager);
	virtual ~SubversionPlugin();
	const SvnOptions& GetOptions() const{return m_options;}

	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
protected:
	wxMenu *CreatePopMenu();
	wxMenu *CreateEditorPopMenu();
	void RefreshTreeStatus(wxTreeItemId *item);
	void ScanForSvnDirs(const wxTreeItemId &item, std::map<wxString, wxTreeItemId> &svnRepos, bool allVisibles = true);
	bool IsItemSvnDir(wxTreeItemId &item);
	void UpdateParent(const wxTreeItemId &child, VdtcTreeItemBase *childData, const wxString &childPath, int imgId);
	void GetAllChildren(const wxTreeItemId &item, std::list<wxTreeItemId> &children);
	bool IsSvnDirectory(const wxFileName &fn);
	
	//icons methods
	int GetOkIcon(VdtcTreeItemBase *data);
	int GetModifiedIcon(VdtcTreeItemBase *data);
	int GetConflictIcon(VdtcTreeItemBase *data);
	
	// event handlers
	///////////////////////////////////////////////////////
	void OnUpdate(wxCommandEvent &event);
	void OnCommit(wxCommandEvent &event);
	void OnDiff(wxCommandEvent &event);
	void OnCleanup(wxCommandEvent &event);
	void OnFileSaved(wxCommandEvent &event);
	void OnRefreshFolderStatus(wxCommandEvent &event);
	void OnTreeExpanded(wxTreeEvent &event);
	void OnFileExplorerInitDone(wxCommandEvent &event);
	void OnOptions(wxCommandEvent &event);
	void OnChangeLog(wxCommandEvent &event);
	void OnSvnAbort(wxCommandEvent &event);
	void OnSvnAdd(wxCommandEvent &event);
	void OnDelete(wxCommandEvent &event);
	void OnRevert(wxCommandEvent &event);
	void OnProjectFileAdded(wxCommandEvent &event);
	void OnAppInitDone(wxCommandEvent &event);
	void OnCommitFile(wxCommandEvent &e);
	void OnUpdateFile(wxCommandEvent &e);
	void OnDiffFile(wxCommandEvent &e);
	void OnRevertFile(wxCommandEvent &e);
	
public:
	static int SvnOkImageId;
	static int SvnConflictImageId;
	static int SvnModifiedImageId;
	static int CppOK;
	static int CppModified;
	static int CppConflict;
	static int CConflict;
	static int COK;
	static int CModified;
	static int TextOK;
	static int TextModified;
	static int TextConflict;
	static int HeaderOK;
	static int HeaderModified;
	static int HeaderConflict;
};

#endif //SUBVERSION_H

