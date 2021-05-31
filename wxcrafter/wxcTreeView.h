#ifndef __wxcTreeView__
#define __wxcTreeView__

#include "NewFormWizard.h"
#include "wxcrafter.h"

#include "wx/defs.h"
#ifdef __WINDOWS__          // __WINDOWS__ defined by wx/defs.h
#include "wx/msw/wrapwin.h" // includes windows.h

// macro and function name crash fix
#if defined(AddForm)
#undef AddForm
#endif
#endif

class EventsEditorPane;
extern const wxEventType wxEVT_SHOW_WXCRAFTER_DESIGNER;

class wxAuiToolBar;
class wxCrafterPlugin;

class wxcTreeView : public wxcTreeViewBaseClass
{
    wxCrafterPlugin* m_plugin;
    EventsEditorPane* m_eventsPane;
    wxArrayString m_fileList;
    bool m_loadingProject;

protected:
    virtual void OnWxcpComboxTextEnter(wxCommandEvent& event);
    virtual void OnItemLabelEditEnd(wxTreeEvent& event);
    virtual void OnWorkspaceOpenUI(wxUpdateUIEvent& event);
    virtual void OnSashPositionChanged(wxSplitterEvent& event);
    virtual void OnChar(wxKeyEvent& event);
    void DoRefreshFileList(bool reloadFileList = true);

public:
    wxcTreeView(wxWindow* parent, wxCrafterPlugin* plugin);
    virtual ~wxcTreeView();

    clTreeCtrl* GetTree() { return m_treeControls; }
    virtual void OnItemSelected(wxTreeEvent& event);
    void LoadProject(const wxFileName& filname);
    void SaveProject();
    void CloseProject(bool saveBeforeClose);
    void OnProjectClosed(wxCommandEvent& event);

    void AddForm(const NewFormDetails& fd);
    DECLARE_EVENT_TABLE()
    void OnOpen(wxCommandEvent& e);
    void OnProjectSaved(wxCommandEvent& e);
    void OnOpenUI(wxUpdateUIEvent& e);
    void OnWorkspaceLoaded(clWorkspaceEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& e);

protected:
    virtual void OnRefreshWxcpFiles(wxCommandEvent& event);
    virtual void OnWxcpFileSelected(wxCommandEvent& event);
};

#endif // __wxcTreeView__
