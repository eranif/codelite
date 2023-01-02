//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : memcheck.h
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

/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#ifndef _MEMCHECK_H_
#define _MEMCHECK_H_

#include <wx/process.h>

#include "plugin.h"

#include "TerminalEmulator.h"
#include "clTabTogglerHelper.h"
#include "imemcheckprocessor.h"
#include "memcheckui.h"

class MemCheckOutputView;

class MemCheckPlugin : public IPlugin
{
public:
    MemCheckPlugin(IManager* manager);
    virtual ~MemCheckPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------

    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    MemCheckSettings* const GetSettings() { return m_settings; };

    virtual IMemCheckProcessor* GetProcessor() { return m_memcheckProcessor; }

    /**
     * @brief true if test is not runnging and GUI can respond, otherwise if test is runnign user can't listing errors
     * and managing supp file
     * @param event
     * @return Plugin status.
     */
    bool IsReady(wxUpdateUIEvent& event);
    /**
     * @brief stop the current running process using SIGTERM
     */
    void StopProcess();

    /**
     * @brief return true if a test is currently running
     * @return
     */
    //    bool IsRunning() const { return m_process != NULL; }
    bool IsRunning() const { return m_terminal.IsRunning(); }

protected:
    MemCheckIcons16 m_icons16;
    MemCheckIcons24 m_icons24;

    IMemCheckProcessor* m_memcheckProcessor;
    MemCheckSettings* m_settings;
    TerminalEmulator m_terminal;
    MemCheckOutputView* m_outputView; ///< Main plugin UI pane.
    clTabTogglerHelper::Ptr_t m_tabHelper;

protected:
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);

    wxString PrepareCommand(const wxString& projectName, wxString& wd);

    /**
     * @brief After settings dialogue is closed, settings are reapplied in plugin.
     */
    void ApplySettings(bool loadLastErrors = true);

    /**
     * @brief After test ends Output notebook is opened. This opens MemCheck notebook.
     */
    void SwitchToMyPage();

    /**
     * @brief User wants test active project.
     * @param event
     */
    void OnCheckAtiveProject(wxCommandEvent& event);

    /**
     * @brief stop the currently running process
     * @param event
     */
    void OnStopProcess(wxCommandEvent& event);

    void OnStopProcessUI(wxUpdateUIEvent& event);
    /**
     * @brief User wants test some project in workspace tree.
     * @param event
     */
    void OnCheckPopupProject(wxCommandEvent& event);

    /**
     * @brief User wants test project to which belongs file in active editor.
     * @param event
     */
    void OnCheckPopupEditor(wxCommandEvent& event);

    /**
     * @brief Executes the test.
     * @param projectName
     */
    void CheckProject(const wxString& projectName);

    void OnProcessOutput(clCommandEvent& event);
    void OnProcessTerminated(clCommandEvent& event);

    /**
     * @brief Analyse can be made independent of CodeLite and log can be load from file.
     * @param event
     */
    void OnImportLog(wxCommandEvent& event);

    /**
     * @brief Settings dialog invoked.
     * @param event
     */
    void OnSettings(wxCommandEvent& event);
    void OnMemCheckUI(wxUpdateUIEvent& event);
};

#endif // memcheck
