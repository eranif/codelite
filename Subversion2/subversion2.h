#ifndef __Subversion2__
#define __Subversion2__

#include "plugin.h"
#include "svninfo.h"
#include "commitmessagescache.h"
#include "svncommand.h"
#include "svnsettingsdata.h"

class SubversionView;
class SvnConsole;
class wxMenu;
class wxMenuItem;

class Subversion2 : public IPlugin
{
private:
	SubversionView*     m_subversionView;
	SvnConsole*         m_subversionConsole;
	wxMenuItem*         m_explorerSepItem;
	SvnCommand          m_simpleCommand;
	SvnCommand          m_diffCommand;
	SvnCommand          m_blameCommand;
	double              m_svnClientVersion;
	CommitMessagesCache m_commitMessagesCache;

protected:
	void OnSettings(wxCommandEvent &event);

public:
	void EditSettings();

protected:

	///////////////////////////////////////////////////////////
	// File Explorer event handlers
	///////////////////////////////////////////////////////////
	void OnCommit           (wxCommandEvent &event);
	void OnUpdate           (wxCommandEvent &event);
	void OnAdd              (wxCommandEvent &event);
	void OnDelete           (wxCommandEvent &event);
	void OnRevert           (wxCommandEvent &event);
	void OnDiff             (wxCommandEvent &event);
	void OnPatch            (wxCommandEvent &event);
	void OnLog              (wxCommandEvent &event);
	void OnBlame            (wxCommandEvent &event);
	void OnIgnoreFile       (wxCommandEvent &event);
	void OnIgnoreFilePattern(wxCommandEvent &event);
	void OnSelectAsView     (wxCommandEvent &event);

	///////////////////////////////////////////////////////////
	// IDE events
	///////////////////////////////////////////////////////////
	void OnGetCompileLine   (wxCommandEvent &event);

	wxMenu* CreateFileExplorerPopMenu();
	bool    IsSubversionViewDetached ();
	void    DoGetSvnInfoSync         (SvnInfo& svnInfo, const wxString &workingDirectory);

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

	SvnConsole *GetConsole() {
		return m_subversionConsole;
	}

	IManager *GetManager() {
		return m_mgr;
	}

	SubversionView *GetSvnView() {
		return m_subversionView;
	}

	SvnSettingsData GetSettings();
	void            SetSettings(SvnSettingsData& ssd);
	wxString        GetSvnExeName(bool nonInteractive = true);
	wxString        GetUserConfigDir();
	void            UpdateIgnorePatterns();
	void            Patch(bool dryRun, const wxString &workingDirectory, wxEvtHandler *owner, int id);
	void            IgnoreFiles(const wxArrayString& files, bool pattern);
	void            Blame(wxCommandEvent& event, const wxArrayString &files);

	void SetSvnClientVersion(double svnClientVersion) {
		this->m_svnClientVersion = svnClientVersion;
	}

	double GetSvnClientVersion() const {
		return m_svnClientVersion;
	}
	CommitMessagesCache& GetCommitMessagesCache() {
		return m_commitMessagesCache;
	}

	bool LoginIfNeeded        (wxCommandEvent &event, const wxString &workingDirectory, wxString& loginString);
	bool GetNonInteractiveMode(wxCommandEvent &event);
	bool IsPathUnderSvn       (const wxString &path);

protected:
	void DoInitialize();
	void DoSetSSH();
	void DoGetSvnVersion();

	wxString DoGetFileExplorerItemFullPath();
	wxString DoGetFileExplorerItemPath();
};

#endif //Subversion2
