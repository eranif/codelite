#ifndef __PHP__
#define __PHP__

#include "plugin.h"
#include <wx/filename.h>
#include <cl_command_event.h>
#include "plugin_settings.h"
#include "php_event.h"
#include <wx/sharedptr.h>
#include "XDebugManager.h"

class EvalPane;
class LocalsView;
class PHPDebugPane;
class BrowserPanel;
class PHPWorkspaceView;
class wxMenuBar;

class PhpPlugin : public IPlugin
{
protected:
    PHPWorkspaceView*    m_workspaceView;
    bool                 m_clangOldFlag;
    BrowserPanel*        m_browser;
    wxString             m_savedPerspective;
    
    /// Debugger panes
    PHPDebugPane*        m_debuggerPane;
    LocalsView*          m_xdebugLocalsView;
    EvalPane*            m_xdebugEvalPane;
    bool                 m_showWelcomePage;
public:
    enum {
        wxID_PHP_SETTINGS = 2000,
        wxID_XDEBUG_SETTING,
        wxID_XDEBUG_DELETE_ALL_BREAKPOINTS,
        wxID_XDEBUG_SHOW_BREAKPOINTS_WINDOW,
    };

public:
    PhpPlugin(IManager *manager);
    ~PhpPlugin();
    void SafelyDetachAndDestroyPane(wxWindow* pane, const wxString &name);
    void EnsureAuiPaneIsVisible(const wxString &paneName, bool update = false);
    
protected:
    bool IsWorkspaceViewDetached();
    void DoOpenWorkspace(const wxString &filename, bool createIfMissing = false);
    void DoPlaceMenuBar(wxMenuBar *menuBar);
    void DoEnsureXDebugPanesVisible(const wxString &selectWindow = "");
    
public:
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void HookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnPlug();
    
    IManager *GetManager() {
        return m_mgr;
    }
    // Event handlers

    //////////////////////////////////////////////
    // Code completion related events
    //////////////////////////////////////////////
    void OnShowQuickOutline(clCodeCompletionEvent &e);

    //////////////////////////////////////////////
    // Other common codelite events
    //////////////////////////////////////////////
    void OnNewWorkspace              (wxCommandEvent &e);
    void OnIsWorkspaceOpen           (clCommandEvent &e);
    void OnCloseWorkspace            (clCommandEvent &e);
    void OnOpenWorkspace             (clCommandEvent &e);
    void OnReloadWorkspace           (clCommandEvent &e);
    void OnOpenResource              (wxCommandEvent &e);
    void OnGetWorkspaceFiles         (wxCommandEvent &e);
    void OnGetCurrentFileProjectFiles(wxCommandEvent &e);
    void OnGetActiveProjectFiles     (wxCommandEvent &e);
    void OnNewProject                (clNewProjectEvent &e);
    void OnNewProjectFinish          (clNewProjectEvent &e);
    void OnGetFiFMask                (clCommandEvent &e);
    void OnMenuCommand               (wxCommandEvent &e);
    void OnFileSaved                 (clCommandEvent &e);
    void OnXDebugShowBreakpointsWindow(wxCommandEvent &e);
    void OnXDebugDeleteAllBreakpoints(clDebugEvent &e);
    void OnXDebugSettings            (wxCommandEvent &e);
    void OnLoadURL                   (PHPEvent& e);
    void OnAllEditorsClosed           (wxCommandEvent& e);
    
    void OnDebugSatrted(XDebugEvent &e);
    void OnDebugEnded  (XDebugEvent &e);
    
};

#endif //PHP
