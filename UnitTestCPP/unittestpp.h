//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : unittestpp.h
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

#ifndef __UnitTestPP__
#define __UnitTestPP__

#include "plugin.h"
#include <wx/process.h>
#include "project.h"
#include <vector>
#include "cl_command_event.h"

class wxMenuItem;
class IProcess;
class UnitTestsPage;

class UnitTestPP : public IPlugin
{
    wxEvtHandler* m_topWindow;
    IProcess* m_proc;
    wxString m_output;
    UnitTestsPage* m_outputPage;

public:
    UnitTestPP(IManager* manager);
    ~UnitTestPP();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual clToolBar* CreateToolBar(wxWindow* parent);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
    bool IsUnitTestProject(ProjectPtr p);
    /**
     * @brief return list of the projects which are identified as UnitTests project
     * @return
     */
    std::vector<ProjectPtr> GetUnitTestProjects();
    
    /**
     * @brief execute a project for unit tests
     */
    void DoRunProject(ProjectPtr project);
    
protected:
    DECLARE_EVENT_TABLE()
    void OnNewSimpleTest(wxCommandEvent& e);
    void OnNewClassTest(wxCommandEvent& e);
    void OnRunUnitTests(wxCommandEvent& e);
    void OnRunProject(clExecuteEvent& e);
    void OnMarkProjectAsUT(wxCommandEvent& e);
    void OnRunUnitTestsUI(wxUpdateUIEvent& e);
    void OnProcessTerminated(wxCommandEvent& e);
    void OnProcessRead(wxCommandEvent& e);
    void DoCreateSimpleTest(const wxString& name, const wxString& projectName, const wxString& filename);
    void DoCreateFixtureTest(const wxString& name,
                             const wxString& fixture,
                             const wxString& projectName,
                             const wxString& filename);
    IEditor* DoAddTestFile(const wxString& filename, const wxString& projectName);
    wxFileName FindBestSourceFile(ProjectPtr proj, const wxFileName& filename);
    void SelectUTPage();

private:
    wxMenu* CreateEditorPopMenu();
};

#endif // UnitTestPP
