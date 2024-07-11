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

#include "AsyncProcess/asyncprocess.h"
#include "clTabTogglerHelper.h"
#include "plugin.h"

class wxMenuItem;
class CppCheckReportPage;

class CppCheckPlugin : public IPlugin
{
    wxString m_cppcheckPath;
    IProcess* m_cppcheckProcess = nullptr;
    bool m_runStartedByUser = false;

protected:
    wxString DoGetCommand();

protected:
    void DoRun();
    void AddOutputLine(const wxString& message);
    void NotifyStopped();

protected:
    void OnSettings(wxCommandEvent& event);
    void OnIsBuildInProgress(clBuildEvent& event);
    void OnStopRun(clBuildEvent& event);
    void OnRun(wxCommandEvent& event);
    void OnCppCheckTerminated(clProcessEvent& e);
    void OnCppCheckReadData(clProcessEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& e);

public:
    CppCheckPlugin(IManager* manager);
    ~CppCheckPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    /**
     * @brief return true if analysis currently running
     */
    bool AnalysisInProgress() const { return m_cppcheckProcess != NULL; }
};

#endif // CppChecker
