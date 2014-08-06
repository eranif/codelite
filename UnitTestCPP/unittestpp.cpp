//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : unittestpp.cpp
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
#include <wx/app.h>
#include "bitmap_loader.h"
#include "unittestspage.h"
#include "macros.h"
#include "unittestdata.h"
#include "event_notifier.h"
#include "asyncprocess.h"
#include "processreaderthread.h"
#include "ctags_manager.h"
#include "unittestcppoutputparser.h"
#include <wx/tokenzr.h>
#include "environmentconfig.h"
#include "pipedprocess.h"
#include "dirsaver.h"
#include "procutils.h"
#include "workspace.h"
#include "project.h"
#include "testclassdlg.h"
#include "newunittestdlg.h"
#include <wx/ffile.h>
#include <wx/msgdlg.h>
#include "unittestpp.h"
#include <wx/xrc/xmlres.h>
#include <wx/menuitem.h>
#include <wx/menu.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#include "evnvarlist.h"
#endif

static UnitTestPP* thePlugin = NULL;

// Define the plugin entry point
extern "C" EXPORT IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new UnitTestPP(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("UnitTestPP"));
    info.SetDescription(_("A Unit test plugin based on the UnitTest++ framework"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

BEGIN_EVENT_TABLE(UnitTestPP, IPlugin)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, UnitTestPP::OnProcessTerminated)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ, UnitTestPP::OnProcessRead)
END_EVENT_TABLE()

UnitTestPP::UnitTestPP(IManager* manager)
    : IPlugin(manager)
    , m_proc(NULL)
{
    // Connect the events to us
    wxTheApp->Connect(XRCID("run_unit_tests"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(UnitTestPP::OnRunUnitTests),
                      NULL,
                      (wxEvtHandler*)this);
    wxTheApp->Connect(XRCID("run_unit_tests"),
                      wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(UnitTestPP::OnRunUnitTestsUI),
                      NULL,
                      (wxEvtHandler*)this);

    m_outputPage = new UnitTestsPage(m_mgr->GetOutputPaneNotebook(), m_mgr);
    m_mgr->GetOutputPaneNotebook()->AddPage(m_outputPage,
                                            _("UnitTest++"),
                                            false,
                                            m_mgr->GetStdIcons()->LoadBitmap("toolbars/16/unittest++/run_as_unittest"));

    m_longName = _("A Unit test plugin based on the UnitTest++ framework");
    m_shortName = wxT("UnitTestPP");
    m_topWindow = m_mgr->GetTheApp();
}

UnitTestPP::~UnitTestPP() {}

clToolBar* UnitTestPP::CreateToolBar(wxWindow* parent)
{
    // support both toolbars icon size
    clToolBar* tb(NULL);

    if(m_mgr->AllowToolbar()) {
        int size = m_mgr->GetToolbarIconSize();

        tb = new clToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
        tb->SetToolBitmapSize(wxSize(size, size));

        BitmapLoader& bitmapLoader = *m_mgr->GetStdIcons();

        if(size == 24) {
            tb->AddTool(XRCID("run_unit_tests"),
                        _("Run Unit tests..."),
                        bitmapLoader.LoadBitmap(wxT("toolbars/24/unittest++/run_as_unittest")),
                        _("Run project as unit test project..."));

        } else {
            tb->AddTool(XRCID("run_unit_tests"),
                        _("Run Unit tests..."),
                        bitmapLoader.LoadBitmap(wxT("toolbars/16/unittest++/run_as_unittest")),
                        _("Run project as unit test project..."));
        }
        tb->Realize();
    }
    return tb;
}

void UnitTestPP::CreatePluginMenu(wxMenu* pluginsMenu)
{
    // Create the popup menu for the file explorer
    // The only menu that we are interseted is the file explorer menu
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(
        menu, XRCID("unittestpp_new_simple_test"), _("Create new &test..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(
        menu, XRCID("unittestpp_new_class_test"), _("Create tests for &class..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();

    item = new wxMenuItem(menu, XRCID("mark_project_as_ut"), _("Mark this project as UnitTest++ project"));
    menu->Append(item);

    item = new wxMenuItem(
        menu, XRCID("run_unit_tests"), _("Run Project as UnitTest++ and report"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, wxT("UnitTest++"), menu);

    // connect the events
    wxTheApp->Connect(XRCID("unittestpp_new_simple_test"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(UnitTestPP::OnNewSimpleTest),
                      NULL,
                      (wxEvtHandler*)this);
    wxTheApp->Connect(XRCID("unittestpp_new_class_test"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(UnitTestPP::OnNewClassTest),
                      NULL,
                      (wxEvtHandler*)this);
    wxTheApp->Connect(XRCID("mark_project_as_ut"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(UnitTestPP::OnMarkProjectAsUT),
                      NULL,
                      (wxEvtHandler*)this);
}

void UnitTestPP::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeEditor) {
        menu->Append(XRCID("UNITTESTPP_EDITOR_POPUP"), _("UnitTest++"), CreateEditorPopMenu());
    }
}

void UnitTestPP::UnPlug()
{
    wxDELETE(m_proc);
    m_output.Clear();
}

wxMenu* UnitTestPP::CreateEditorPopMenu()
{
    // Create the popup menu for the file explorer
    // The only menu that we are interseted is the file explorer menu
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(
        menu, XRCID("unittestpp_new_simple_test"), _("Create new &test..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(
        menu, XRCID("unittestpp_new_class_test"), _("Create tests for &class..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

void UnitTestPP::OnNewClassTest(wxCommandEvent& e)
{
    wxUnusedVar(e);

    if(GetUnitTestProjects().empty()) {
        if(wxMessageBox(
               wxString::Format(
                   _("There are currently no UnitTest project in your workspace\nWould you like to create one now?")),
               _("CodeLite"),
               wxYES_NO | wxCANCEL) == wxYES) {
            // add new UnitTest project
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("new_project"));
            m_mgr->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
        }
        return;
    }

    // position has changed, compare line numbers
    wxString clsName;
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor) {
        int line = editor->GetCurrentLine();
        TagEntryPtr tag = m_mgr->GetTagsManager()->FunctionFromFileLine(editor->GetFileName(), line + 1);
        if(tag && tag->GetScope().IsEmpty() == false && tag->GetScope() != wxT("<global>")) {
            clsName = tag->GetScope();
        }
    }

    TestClassDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr, this);
    dlg.SetClassName(clsName);

    if(dlg.ShowModal() == wxID_OK) {
        wxArrayString arr = dlg.GetTestsList();
        wxString fixture = dlg.GetFixtureName();
        wxString filename = dlg.GetFileName();
        wxString projectName = dlg.GetProjectName();

        wxFileName fn(filename);
        wxString err_msg;

        fixture.Trim().Trim(false);
        ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(projectName, err_msg);
        if(p) {
            // incase a relative path was given, use the selected project path
            fn = FindBestSourceFile(p, fn);
            for(size_t i = 0; i < arr.GetCount(); i++) {

                // Construct the test name in the format of:
                // Test<FuncName>
                wxString name = arr.Item(i);
                wxString prefix = name.Mid(0, 1);

                name = name.Mid(1);
                prefix.MakeUpper();
                prefix << name;

                wxString testName;
                testName << wxT("Test") << prefix;

                if(!fixture.IsEmpty()) {
                    DoCreateFixtureTest(testName, fixture, projectName, fn.GetFullPath());
                } else {
                    DoCreateSimpleTest(testName, projectName, fn.GetFullPath());
                }
            }
        }
    }
}

void UnitTestPP::OnNewSimpleTest(wxCommandEvent& e)
{
    wxUnusedVar(e);

    if(GetUnitTestProjects().empty()) {
        if(wxMessageBox(
               wxString::Format(
                   _("There are currently no UnitTest project in your workspace\nWould you like to create one now?")),
               _("CodeLite"),
               wxYES_NO | wxCANCEL) == wxYES) {
            // add new UnitTest project
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("new_project"));
            m_mgr->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
        }
        return;
    }

    NewUnitTestDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), this, m_mgr->GetConfigTool());
    if(dlg.ShowModal() == wxID_OK) {
        // create the unit test
        wxString testName = dlg.GetTestName();
        wxString fixture = dlg.GetFixtureName();
        wxString projectName = dlg.GetProjectName();
        wxString filename = dlg.GetFilename();

        // incase a relative path was given, use the selected project path
        wxFileName fn(filename);
        wxString err_msg;
        ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(projectName, err_msg);
        if(p) {
            fn = FindBestSourceFile(p, fn);
            fixture.Trim().Trim(false);
            if(!fixture.IsEmpty()) {
                DoCreateFixtureTest(testName, fixture, projectName, fn.GetFullPath());
            } else {
                DoCreateSimpleTest(testName, projectName, fn.GetFullPath());
            }
        }
    }
}

void UnitTestPP::DoCreateFixtureTest(const wxString& name,
                                     const wxString& fixture,
                                     const wxString& projectName,
                                     const wxString& filename)
{
    wxString text;

    text << wxT("\nTEST_FIXTURE(") << fixture << wxT(", ") << name << wxT(")\n");
    text << wxT("{\n");
    text << wxT("}\n");

    IEditor* editor = DoAddTestFile(filename, projectName);
    if(editor) {
        editor->AppendText(text);
    }
}

void UnitTestPP::DoCreateSimpleTest(const wxString& name, const wxString& projectName, const wxString& filename)
{
    // try to locate the file
    wxString errMsg;
    ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projectName, errMsg);
    if(!proj) {
        // no such project!
        wxMessageBox(_("Could not find the target project"), _("CodeLite"), wxOK | wxICON_ERROR);
        return;
    }

    IEditor* editor = DoAddTestFile(filename, projectName);
    wxString text;

    text << wxT("\nTEST(") << name << wxT(")\n");
    text << wxT("{\n");
    text << wxT("}\n");

    if(editor) {
        editor->AppendText(text);
    }
}

void UnitTestPP::OnRunUnitTests(wxCommandEvent& e)
{
    ProjectPtr p = m_mgr->GetSelectedProject();
    if(!p) {
        return;
    }
    wxString wd;
    wxString cmd = m_mgr->GetProjectExecutionCommand(p->GetName(), wd);
    DirSaver ds;

    // first we need to CD to the project directory
    ::wxSetWorkingDirectory(p->GetFileName().GetPath());

    // now change the directory
    ::wxSetWorkingDirectory(wd);

    EnvSetter es;
    // m_proc will be deleted upon termination
    m_output.Clear();
    m_proc = ::CreateAsyncProcess(this, cmd);
}

void UnitTestPP::OnRunUnitTestsUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    if(m_proc) {
        e.Enable(false);

    } else {
        ProjectPtr p = m_mgr->GetSelectedProject();
        e.Enable(p && p->GetProjectInternalType() == wxT("UnitTest++"));
    }
}

std::vector<ProjectPtr> UnitTestPP::GetUnitTestProjects()
{
    std::vector<ProjectPtr> ut_projects;
    wxArrayString projects;
    m_mgr->GetWorkspace()->GetProjectList(projects);
    for(size_t i = 0; i < projects.GetCount(); i++) {
        wxString err_msg;
        ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), err_msg);
        if(proj && IsUnitTestProject(proj)) {
            ut_projects.push_back(proj);
        }
    }
    return ut_projects;
}

bool UnitTestPP::IsUnitTestProject(ProjectPtr p)
{
    if(!p) {
        return false;
    }
    return p->GetProjectInternalType() == wxT("UnitTest++");
}

IEditor* UnitTestPP::DoAddTestFile(const wxString& filename, const wxString& projectName)
{
    // first open / create the target file
    wxFileName fn(filename);
    if(wxFileName::FileExists(filename) == false) {
        // the file does not exist!
        wxFFile file(filename, wxT("wb"));
        if(!file.IsOpened()) {
            wxMessageBox(wxString::Format(_("Could not create target file '%s'"), filename.c_str()),
                         _("CodeLite"),
                         wxICON_WARNING | wxOK);
            return NULL;
        }

        // since this is a new file, it will most probably will need the include file
        file.Write(wxT("#include <UnitTest++.h>\n"));
        file.Close();
    }

    // locate the project
    wxString errMsg;
    IEditor* editor(NULL);

    ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projectName, errMsg);
    if(proj) {
        std::vector<wxFileName> files;
        proj->GetFiles(files, true);

        // Search the target file, if it is already exist in the project, open the file
        // and return
        for(size_t i = 0; i < files.size(); i++) {
            if(files.at(i) == fn) {
                m_mgr->OpenFile(fn.GetFullPath());
                editor = m_mgr->GetActiveEditor();
                if(editor && editor->GetFileName() == fn) {
                    return editor;
                } else {
                    return NULL;
                }
            }
        }

        // add it to the project
        wxArrayString paths;
        paths.Add(filename);
        m_mgr->CreateVirtualDirectory(proj->GetName(), wxT("tests"));
        m_mgr->AddFilesToVirtualFolder(proj->GetName() + wxT(":tests"), paths);

        // open the file
        m_mgr->OpenFile(fn.GetFullPath());
        editor = m_mgr->GetActiveEditor();
        if(editor && editor->GetFileName() == fn) {
            return editor;
        }
    }
    return NULL;
}

wxFileName UnitTestPP::FindBestSourceFile(ProjectPtr proj, const wxFileName& filename)
{
    if(filename.IsOk() == false) {
        // no such file
        std::vector<wxFileName> files;
        proj->GetFiles(files, true);

        // Search the target file, if it is already exist in the project, open the file
        // and return
        for(size_t i = 0; i < files.size(); i++) {
            wxFileName fn = files.at(i);
            if(IsSourceFile(fn.GetExt())) {
                return fn;
            }
        }
        // no source file were found in the project
        // create a path name of the file which will be located
        // under the selected project path (we dont create it here)
        wxFileName fn(proj->GetFileName());
        fn.SetFullName(wxT("unit_tests.cpp"));
        return fn;
    } else if(filename.IsAbsolute() == false) {
        // relative path was given, set the path to the project path
        wxFileName fn(filename);
        fn.SetPath(proj->GetFileName().GetPath());
        return fn;
    } else {
        return filename;
    }
}

void UnitTestPP::OnMarkProjectAsUT(wxCommandEvent& e)
{
    ProjectPtr p = m_mgr->GetSelectedProject();
    if(!p) {
        return;
    }

    p->SetProjectInternalType(wxT("UnitTest++"));
    p->Save();
}

void UnitTestPP::OnProcessRead(wxCommandEvent& e)
{
    ProcessEventData* ped = (ProcessEventData*)e.GetClientData();
    m_output << ped->GetData();
    delete ped;
}

void UnitTestPP::OnProcessTerminated(wxCommandEvent& e)
{
    ProcessEventData* ped = (ProcessEventData*)e.GetClientData();
    delete ped;

    wxDELETE(m_proc);

    wxArrayString arr = wxStringTokenize(m_output, wxT("\r\n"));
    UnitTestCppOutputParser parser(arr);

    // parse the results
    TestSummary summary;
    parser.Parse(&summary);

    if(summary.totalTests == 0) {
        return;
    }

    m_outputPage->Initialize(&summary);

    wxString msg;
    double errCount = summary.errorCount;
    double totalTests = summary.totalTests;

    double err_percent = (errCount / totalTests) * 100;
    double pass_percent = ((totalTests - errCount) / totalTests) * 100;
    msg << err_percent << wxT("%");
    m_outputPage->UpdateFailedBar((size_t)summary.errorCount, msg);

    msg.clear();
    msg << pass_percent << wxT("%");
    m_outputPage->UpdatePassedBar((size_t)(summary.totalTests - summary.errorCount), msg);

    SelectUTPage();
}

void UnitTestPP::SelectUTPage()
{
    size_t pageCount = m_mgr->GetOutputPaneNotebook()->GetPageCount();
    for(size_t i = 0; i < pageCount; ++i) {
        if(m_mgr->GetOutputPaneNotebook()->GetPage(i) == m_outputPage) {
            m_mgr->GetOutputPaneNotebook()->SetSelection(i);
            break;
        }
    }
}
