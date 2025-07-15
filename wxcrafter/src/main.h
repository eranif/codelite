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

protected:
    void OnFileOpen(wxCommandEvent& event) override;
    void OnBatchGenerateCode(wxCommandEvent& event) override;
    void OnBatchGenerateCodeUI(wxUpdateUIEvent& event) override;

protected:
    virtual void OnFindUI(wxUpdateUIEvent& event);
    void OnOpenFindDialog(wxCommandEvent& event) override;
    void OnGenerateCodeUI(wxUpdateUIEvent& event) override;
    void OnGenerateCode(wxCommandEvent& event) override;
    void OnDeleteCustomControl(wxCommandEvent& event) override;
    void OnEditCustomControl(wxCommandEvent& event) override;
    void OnNewCustomControl(wxCommandEvent& event) override;
    virtual void OnOpen(wxCommandEvent& event);
    virtual void OnOpenMenu(wxCommandEvent& event);
    virtual void OnNewProject(wxCommandEvent& event);
    virtual void OnOpenUI(wxUpdateUIEvent& event);
    virtual void OnCloseUI(wxUpdateUIEvent& event);
    void OnSaveUI(wxUpdateUIEvent& event) override;
    void OnPasteUI(wxUpdateUIEvent& event) override;
    virtual void OnSwitchToCodeliteUI(wxUpdateUIEvent& event);
    void OnSettings(wxCommandEvent& event) override;
    virtual void OnProjectClosed(wxCommandEvent& event);
    void OnHide(wxCommandEvent& event) override;
    void OnAbout(wxCommandEvent& event) override;
    void OnImportFB(wxCommandEvent& event) override;
    void OnImportSmith(wxCommandEvent& event) override;
    void OnImportXRC(wxCommandEvent& event) override;
    void OnRename(wxCommandEvent& event) override;
    virtual void OnSwitchToCodelite(wxCommandEvent& event);
    void OnBuild(wxCommandEvent& event) override;
    void OnBuildUI(wxUpdateUIEvent& event) override;
    void OnCopy(wxCommandEvent& event) override;
    void OnCut(wxCommandEvent& event) override;
    void OnPaste(wxCommandEvent& event) override;
    void OnRedo(wxCommandEvent& event) override;
    void OnRedoUI(wxUpdateUIEvent& event) override;
    void OnSelectionUI(wxUpdateUIEvent& event) override;
    void OnUndo(wxCommandEvent& event) override;
    void OnUndoUI(wxUpdateUIEvent& event) override;
    void OnDeleteItem(wxCommandEvent& event) override;
    void OnDeleteItemUI(wxUpdateUIEvent& event) override;
    void OnPreview(wxCommandEvent& event) override;
    void OnPreviewUI(wxUpdateUIEvent& event) override;
    virtual void OnClose(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event) override;
    virtual void OnProjectModified(wxCommandEvent& e);
    virtual void OnProjectSynched(wxCommandEvent& e);
    virtual void OnProjectLoaded(wxCommandEvent& e);
    virtual void OnWorkspaceClosed(clWorkspaceEvent& e);
    virtual void OnCodeLiteGotFocus(wxCommandEvent& e);
    virtual void OnCodeEditorSelected(wxCommandEvent& e);

protected:
    wxTextCtrl* GetActiveTextCtrl();
    wxStyledTextCtrl* GetActiveSTC();
    bool DoFindText(wxStyledTextCtrl* stc, const wxFindReplaceData& frd, bool findNext);
    void EnsureVisibile();
    void DoOpenWxcpProject();
    void DoCreateRecentMenu(wxMenu& menu, wxArrayString& history);

public:
    MainFrame(wxWindow* parent, bool hidden);
    ~MainFrame() override;

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
    void OnCloseFrame(wxCloseEvent& event) override;

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
    ~wxcApp() override = default;
    bool OnInit() override;
    int OnExit() override;
    bool OnCmdLineParsed(wxCmdLineParser& parser) override;
};

DECLARE_APP(wxcApp)

#endif

#endif //__main__
