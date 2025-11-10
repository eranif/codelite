#ifndef CLFILESYSTEMWORKSPACEVIEW_HPP
#define CLFILESYSTEMWORKSPACEVIEW_HPP

#include "clFileSystemWorkspace.hpp"
#include "clFileSystemWorkspaceConfig.hpp"
#include "clTreeCtrlPanel.h"
#include "cl_command_event.h"
#include "cl_config.h"

#include <wx/choice.h>

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

class clThemedButton;
class WXDLLIMPEXP_SDK clFileSystemWorkspaceView : public clTreeCtrlPanel
{
    clConfig m_config;
    wxChoice* m_choiceConfigs = nullptr;
    wxArrayString m_configs;
    bool m_buildInProgress = false;
    bool m_runInProgress = false;
    wxArrayString m_selectedFolders;

protected:
    void OnFolderDropped(clCommandEvent& event);
    void OnContextMenu(clContextMenuEvent& event);
    void OnCloseFolder(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnAddIncludePath(wxCommandEvent& event);
    void OnCreateCompileFlagsFile(wxCommandEvent& event);
    void OnChoiceConfigSelected(wxCommandEvent& event);
    void OnRefreshView(wxCommandEvent& event);
    void OnRefreshViewUI(wxUpdateUIEvent& event);
    void OnBuildStarted(clBuildEvent& event);
    void OnBuildEnded(clBuildEvent& event);
    void OnProgramStarted(clExecuteEvent& event);
    void OnProgramStopped(clExecuteEvent& event);
    void OnBuildActiveProjectDropdown(wxAuiToolBarEvent& event);
    void OnFindInFilesDismissed(clFindInFilesEvent& event);
    void OnFindInFilesShowing(clFindInFilesEvent& event);
    void OnExcludePath(wxCommandEvent& event);
    void OnThemeChanged(clCommandEvent& event);

protected:
    void DoAddIncludePathsToConfig(clFileSystemWorkspaceConfig::Ptr_t config, const wxArrayString& paths);

public:
    clFileSystemWorkspaceView(wxWindow* parent, const wxString& viewName);
    virtual ~clFileSystemWorkspaceView();
    void UpdateConfigs(const wxArrayString& configs, const wxString& selectedConfig);
};

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif // CLFILESYSTEMWORKSPACEVIEW_HPP
