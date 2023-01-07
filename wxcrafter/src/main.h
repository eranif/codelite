#ifndef __main__
#define __main__

// main wxWidgets header file
#include "gui.h"
#include "wxcTreeView.h"
#include "wxguicraft_main_view.h"

#include <wx/cmdline.h>
#include <wx/fdrepdlg.h>
#include <wx/stc/stc.h>
#include <wx/wx.h>

class MainFrame : public MainFrameBase
{
protected:
    GUICraftMainPanel* m_wxcView = nullptr;
    wxcTreeView* m_treeView = nullptr;
    wxFindReplaceDialog* m_findReplaceDialog = nullptr;
    wxFindReplaceData m_findData;
    bool m_exiting = false;

protected:
    virtual void OnFileOpen(wxCommandEvent& event);
    virtual void OnBatchGenerateCode(wxCommandEvent& event);
    virtual void OnBatchGenerateCodeUI(wxUpdateUIEvent& event);

protected:
    virtual void OnFindUI(wxUpdateUIEvent& event);
    virtual void OnOpenFindDialog(wxCommandEvent& event);
    virtual void OnGenerateCodeUI(wxUpdateUIEvent& event);
    virtual void OnGenerateCode(wxCommandEvent& event);
    virtual void OnDeleteCustomControl(wxCommandEvent& event);
    virtual void OnEditCustomControl(wxCommandEvent& event);
    virtual void OnNewCustomControl(wxCommandEvent& event);
    virtual void OnOpen(wxCommandEvent& event);
    virtual void OnOpenMenu(wxCommandEvent& event);
    virtual void OnNewProject(wxCommandEvent& event);
    virtual void OnOpenUI(wxUpdateUIEvent& event);
    virtual void OnCloseUI(wxUpdateUIEvent& event);
    virtual void OnSaveUI(wxUpdateUIEvent& event);
    virtual void OnPasteUI(wxUpdateUIEvent& event);
    virtual void OnSwitchToCodeliteUI(wxUpdateUIEvent& event);
    virtual void OnSettings(wxCommandEvent& event);
    virtual void OnProjectClosed(wxCommandEvent& event);
    virtual void OnHide(wxCommandEvent& event);
    virtual void OnAbout(wxCommandEvent& event);
    virtual void OnImportFB(wxCommandEvent& event);
    virtual void OnImportSmith(wxCommandEvent& event);
    virtual void OnImportXRC(wxCommandEvent& event);
    virtual void OnRename(wxCommandEvent& event);
    virtual void OnSwitchToCodelite(wxCommandEvent& event);
    virtual void OnBuild(wxCommandEvent& event);
    virtual void OnBuildUI(wxUpdateUIEvent& event);
    virtual void OnCopy(wxCommandEvent& event);
    virtual void OnCut(wxCommandEvent& event);
    virtual void OnPaste(wxCommandEvent& event);
    virtual void OnRedo(wxCommandEvent& event);
    virtual void OnRedoUI(wxUpdateUIEvent& event);
    virtual void OnSelectionUI(wxUpdateUIEvent& event);
    virtual void OnUndo(wxCommandEvent& event);
    virtual void OnUndoUI(wxUpdateUIEvent& event);
    virtual void OnDeleteItem(wxCommandEvent& event);
    virtual void OnDeleteItemUI(wxUpdateUIEvent& event);
    virtual void OnPreview(wxCommandEvent& event);
    virtual void OnPreviewUI(wxUpdateUIEvent& event);
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnSave(wxCommandEvent& event);
    virtual void OnProjectModified(wxCommandEvent& e);
    virtual void OnProjectSynched(wxCommandEvent& e);
    virtual void OnProjectLoaded(wxCommandEvent& e);
    virtual void OnWorkspaceClosed(clWorkspaceEvent& e);
    virtual void OnCodeLiteGotFocus(wxCommandEvent& e);
    void OnCodeEditorSelected(wxCommandEvent& e);

protected:
    wxTextCtrl* GetActiveTextCtrl();
    wxStyledTextCtrl* GetActiveSTC();
    bool DoFindText(wxStyledTextCtrl* stc, const wxFindReplaceData& frd, bool findNext);
    void EnsureVisibile();
    void DoOpenWxcpProject();
    void DoCreateRecentMenu(wxMenu& menu, wxArrayString& history);

public:
    MainFrame(wxWindow* parent, bool hidden);
    virtual ~MainFrame();

    void Add(wxcTreeView* tree);
    void Add(GUICraftMainPanel* view);

    wxPanel* GetMainPanel() { return m_MainPanel; }

    wxWindow* GetTreeParent() { return m_splitterPageTreeView; }

    wxWindow* GetDesignerParent() { return m_splitterPageDesigner; }

    void DisplayDesigner();
    void MinimizeDesigner();
    void HideDesigner();

    void SetStatusMessage(const wxString& message);

protected:
    // protected event handlers
    virtual void OnCloseFrame(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
    void OnFindDlgClose(wxFindDialogEvent& event);
    void OnFindFirst(wxFindDialogEvent& event);
    void OnFindNext(wxFindDialogEvent& event);
};

#if STANDALONE_BUILD

#include "wxcrafter_plugin.h"

/// Build wxCrafter as a standalone executable
/// instead of a plugin
class wxcApp : public wxApp
{
    wxCrafterPlugin* m_wxcPlugin;
    bool m_hiddenMainFrame;

public:
    wxcApp();
    virtual ~wxcApp();
    virtual bool OnInit();
    virtual int OnExit();
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
};

DECLARE_APP(wxcApp)

#endif

#endif //__main__
