//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cppchecker.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef __CppChecker__
#define __CppChecker__

#include "plugin.h"
#include "asyncprocess.h"
#include "cppcheck_settings.h"
#include "clTabTogglerHelper.h"

class wxMenuItem;
class CppCheckReportPage;

class CppCheckPlugin : public IPlugin
{
    wxString m_cppcheckPath;
    IProcess* m_cppcheckProcess;
    bool m_canRestart;
    wxArrayString m_filelist;
    wxMenuItem* m_explorerSepItem;
    wxMenuItem* m_workspaceSepItem;
    wxMenuItem* m_projectSepItem;
    CppCheckReportPage* m_view;
    bool m_analysisInProgress;
    size_t m_fileCount;
    CppCheckSettings m_settings;
    size_t m_fileProcessed;
    clTabTogglerHelper::Ptr_t m_tabHelper;

protected:
    wxString DoGetCommand(ProjectPtr proj);
    wxString DoGenerateFileList();

protected:
    wxMenu* CreateEditorPopMenu();
    wxMenu* CreateFileExplorerPopMenu();
    wxMenu* CreateWorkspacePopMenu();
    wxMenu* CreateProjectPopMenu();

protected:
    void GetFileListFromDir(const wxString& root);
    void RemoveExcludedFiles();
    void SetTabVisible(bool clearContent);
    void DoProcess(ProjectPtr proj);
    void DoStartTest(ProjectPtr proj = NULL);
    ProjectPtr FindSelectedProject();
    void DoSettingsItem(ProjectPtr project = NULL);

protected:
    /**
     * @brief handle the context meun activation from within the editor context menu
     * @param e
     */
    void OnCheckFileEditorItem(wxCommandEvent& e);

    /**
     * @brief handle the context meun activation from within the file explorer
     * @param e
     */
    void OnCheckFileExplorerItem(wxCommandEvent& e);

    /**
     * @brief handle the context meun activation from the workspace
     * @param e
     */
    void OnCheckWorkspaceItem(wxCommandEvent& e);

    /**
     * @brief handle the context meun activation from the project
     * @param e
     */
    void OnCheckProjectItem(wxCommandEvent& e);

    /**
     * @brief handles the cppcheck process termination
     * @param e
     */
    void OnCppCheckTerminated(clProcessEvent& e);

    /**
     * @brief there is data to read from the process
     * @param e
     */
    void OnCppCheckReadData(clProcessEvent& e);

    /**
     * @brief handle the workspace closed event and clear the view
     * @param e
     */
    void OnWorkspaceClosed(wxCommandEvent& e);
    /**
     * @brief handle the settings item
     * @param e event
     */
    void OnSettingsItem(wxCommandEvent& e);
    /**
     * @brief handle the settings item from a project context menu
     * @param e event
     */
    void OnSettingsItemProject(wxCommandEvent& e);

    /**
     * @brief editor context menu is about to be shown.
     * Append our content if the active editor is a Cxx file
     */
    void OnEditorContextMenu(clContextMenuEvent& event);

public:
    CppCheckPlugin(IManager* manager);
    ~CppCheckPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    /**
     * @brief stop the analysis of the current file
     * and clear the queue
     */
    void StopAnalysis();
    /**
     * @brief return true if analysis currently running
     */
    bool AnalysisInProgress() const { return m_cppcheckProcess != NULL; }

    /**
     * @brief return the progress
     * @return value between 0-100
     */
    size_t GetProgress();
};

#endif // CppChecker
