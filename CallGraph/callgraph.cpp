//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : callgraph.cpp
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

#include "callgraph.h"
#include "macromanager.h"
#include "string.h"
#include "toolbaricons.h"
#include "uicallgraphpanel.h"
#include "uisettingsdlg.h"
#include "workspace.h"
#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/datetime.h>
#include <wx/image.h>
#include <wx/xrc/xmlres.h>

#include <wx/mstream.h>
#include <wx/sstream.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

#include "file_logger.h"
#include "fileutils.h"
#include <wx/msgdlg.h>

/*!
 * brief Class for include plugin to CodeLite.
 */

#ifndef nil
#define nil 0
#endif

#define myLog(...) LogFn(wxString::Format(__VA_ARGS__))

CallGraph* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new CallGraph(manager); }
    return thePlugin;
}

wxString wxbuildinfo()
{
    wxString wxbuild(wxVERSION_STRING);

#if defined(__WXMSW__)
    wxbuild << wxT("-Windows");
#elif defined(__UNIX__)
    wxbuild << wxT("-Linux");
#endif

#if wxUSE_UNICODE
    wxbuild << wxT("-Unicode build");
#else
    wxbuild << wxT("-ANSI build");
#endif

    return wxbuild;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Václav Špruček, Michal Bližňák, Tomas Bata University in Zlin, www.fai.utb.cz"));
    info.SetName(wxT("CallGraph"));
    info.SetDescription(_("Create application call graph from profiling information provided by gprof tool."));
    info.SetVersion(wxT("v1.1.0"));
    info.EnableFlag(PluginInfo::kNone, true);
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

//---- CTOR -------------------------------------------------------------------

CallGraph::CallGraph(IManager* manager)
    : IPlugin(manager)
{
    // will be created on-demand
    m_LogFile = nil;

    m_longName = _("Create application call graph from profiling information provided by gprof tool.");
    m_shortName = wxT("CallGraph");

    m_mgr->GetTheApp()->Connect(XRCID("cg_settings"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CallGraph::OnSettings), NULL, this);
    m_mgr->GetTheApp()->Connect(XRCID("cg_about"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CallGraph::OnAbout), NULL, this);

    m_mgr->GetTheApp()->Connect(XRCID("cg_show_callgraph"), wxEVT_COMMAND_TOOL_CLICKED,
                                wxCommandEventHandler(CallGraph::OnShowCallGraph), NULL, this);
}

//---- DTOR -------------------------------------------------------------------

CallGraph::~CallGraph()
{
    m_mgr->GetTheApp()->Disconnect(XRCID("cg_settings"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CallGraph::OnSettings), NULL, this);
    m_mgr->GetTheApp()->Disconnect(XRCID("cg_about"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CallGraph::OnAbout), NULL, this);

    m_mgr->GetTheApp()->Disconnect(XRCID("cg_show_callgraph"), wxEVT_COMMAND_TOOL_CLICKED,
                                   wxCommandEventHandler(CallGraph::OnShowCallGraph), NULL, this);

    wxDELETE(m_LogFile);
}

//-----------------------------------------------------------------------------

void CallGraph::LogFn(wxString s)
{
    return; // (log disabled)
}

//-----------------------------------------------------------------------------

void CallGraph::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

//-----------------------------------------------------------------------------
void CallGraph::CreatePluginMenu(wxMenu* pluginsMenu)
{
    // You can use the below code a snippet:
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, XRCID("cg_show_callgraph"), _("Show call graph"),
                          _("Show call graph for selected/active project"), wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("cg_settings"), _("Settings..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    item = new wxMenuItem(menu, XRCID("cg_about"), _("About..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    //
    pluginsMenu->Append(wxID_ANY, _("Call Graph"), menu);
}

//-----------------------------------------------------------------------------

wxMenu* CallGraph::CreateProjectPopMenu()
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("cg_show_callgraph"), _("Show call graph"),
                          _("Show call graph for selected project"), wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

//-----------------------------------------------------------------------------

void CallGraph::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeEditor) {
        // TODO::Append items for the editor context menu
    } else if(type == MenuTypeFileExplorer) {
        // TODO::Append items for the file explorer context menu
    } else if(type == MenuTypeFileView_Workspace) {
        // TODO::Append items for the file view / workspace context menu
    } else if(type == MenuTypeFileView_Project) {
        // TODO::Append items for the file view/Project context menu
        if(!menu->FindItem(XRCID("cg_show_callgraph_popup"))) {
            menu->PrependSeparator();
            menu->Prepend(XRCID("cg_show_callgraph_popup"), _("Call Graph"), CreateProjectPopMenu());
        }
    } else if(type == MenuTypeFileView_Folder) {
        // TODO::Append items for the file view/Virtual folder context menu
    } else if(type == MenuTypeFileView_File) {
        // TODO::Append items for the file view/file context menu
    }
}

//-----------------------------------------------------------------------------

void CallGraph::UnPlug()
{
    // TODO:: perform the unplug action for this plugin
}

//---- About ------------------------------------------------------------------

void CallGraph::OnAbout(wxCommandEvent& event)
{
    // wxString version = wxString::Format(DBE_VERSION);

    wxString desc =
        _("Create application call graph from profiling information provided by gprof tool.   \n\n"); // zapsat vice
                                                                                                      // info - neco o
                                                                                                      // aplikaci dot
    desc << wxbuildinfo() << wxT("\n\n");

    wxAboutDialogInfo info;
    info.SetName(_("Call Graph"));
    info.SetVersion(_("v1.1.1"));
    info.SetDescription(desc);
    info.SetCopyright(_("2012 - 2015 (C) Tomas Bata University, Zlin, Czech Republic"));
    info.SetWebSite(_("http://www.fai.utb.cz"));
    info.AddDeveloper(wxT("Václav Špruček"));
    info.AddDeveloper(wxT("Michal Bližňák"));

    wxAboutBox(info);
}

//---- Test wxProcess ---------------------------------------------------------

wxString CallGraph::LocateApp(const wxString& app_name)
{
    wxArrayString out;
    wxExecute("which " + app_name, out);
    if(out.GetCount() == 1)
        return out[0];
    else
        return "";

    //    // myLog("LocateApp(\"%s\")", app_name);
    //
    //    wxProcess* proc = new wxProcess(wxPROCESS_REDIRECT);
    //
    //    wxString cmd = "which " + app_name;
    //
    //    // Q: HOW BIG IS INTERNAL BUFFER ???
    //    int err = wxExecute(cmd, wxEXEC_SYNC, proc);
    //    // ignore -1 error due to CL signal handler overload
    //
    //    /*int	pid = proc->GetPid();
    //
    //    myLog("  wxExecute(\"%s\") returned err %d, had pid %d", cmd, err, pid);
    //    */
    //
    //    // get process output
    //    wxInputStream* pis = proc->GetInputStream();
    //    if(!pis || !pis->CanRead()) {
    //        delete proc;
    //        return "<ERROR>";
    //    }
    //
    //    // read from it
    //    wxTextInputStream tis(*pis);
    //
    //    wxString out_str = tis.ReadLine();
    //
    //    delete proc;
    //
    //    // myLog("  returned \"%s\"", out_str);
    //
    //    return out_str;
}

//---- Get Gprof Path ---------------------------------------------------------

wxString CallGraph::GetGprofPath()
{
    ConfCallGraph confData;

    m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);

    wxString gprofPath = confData.GetGprofPath();

    if(!gprofPath.IsEmpty()) return gprofPath;

#ifdef __WXMSW__
    return wxEmptyString;
#else
    gprofPath = LocateApp(GPROF_FILENAME_EXE);
#endif

    confData.SetGprofPath(gprofPath);
    m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);
    return gprofPath;
}

//---- Get Dot Path -----------------------------------------------------------

wxString CallGraph::GetDotPath()
{
    ConfCallGraph confData;

    m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);

    wxString dotPath = confData.GetDotPath();

    if(!dotPath.IsEmpty()) return dotPath;

#ifdef __WXMSW__
    return wxEmptyString;
#else
    dotPath = LocateApp(DOT_FILENAME_EXE);
#endif

    confData.SetDotPath(dotPath);

    m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);

    return dotPath;
}

//---- Show CallGraph event ---------------------------------------------------

void CallGraph::OnShowCallGraph(wxCommandEvent& event)
{
    // myLog("wxThread::IsMain(%d)", (int)wxThread::IsMain());

    IConfigTool* config_tool = m_mgr->GetConfigTool();
    MacroManager* macro = MacroManager::Instance();
    wxASSERT(macro);

    config_tool->ReadObject(wxT("CallGraph"), &confData);

    if(!wxFileExists(GetGprofPath()) || !wxFileExists(GetDotPath()))
        return MessageBox(_T("Failed to locate required tools (gprof, dot). Please check the plugin settings."),
                          wxICON_ERROR);

    clCxxWorkspace* ws = m_mgr->GetWorkspace();
    if(!ws) return MessageBox(_("Unable to get opened workspace."), wxICON_ERROR);

    wxFileName ws_cfn = ws->GetWorkspaceFileName();

    wxString projectName = ws->GetActiveProjectName();

    wxString errMsg;
    ProjectPtr proj = ws->FindProjectByName(projectName, errMsg);
    wxString projPath = proj->GetProjectPath();

    BuildMatrixPtr mtx = ws->GetBuildMatrix();
    if(!mtx) return MessageBox(_("Unable to get current build matrix."), wxICON_ERROR);

    wxString build_config_name = mtx->GetSelectedConfigurationName();

    BuildConfigPtr bldConf = ws->GetProjBuildConf(projectName, build_config_name);
    if(!bldConf) return MessageBox(_("Unable to get opened workspace."), wxICON_ERROR);

    wxString projOutputFn = macro->Expand(bldConf->GetOutputFileName(), m_mgr, projectName, build_config_name);
#ifdef __WXMSW__
    if(!projOutputFn.Lower().EndsWith(wxT(".exe"))) projOutputFn += wxT(".exe");
#endif //__WXMSW__

    //    myLog("WorkspaceFileName = \"%s\"", ws_cfn.GetFullPath());
    //    myLog("projectName \"%s\"", projectName);
    //    myLog("build_config_name = \"%s\"", build_config_name);
    //    myLog("projOutputFn = \"%s\"", projOutputFn);
    //    myLog("projPath = \"%s\"", projPath);

    wxFileName cfn(projPath + wxFileName::GetPathSeparator() + projOutputFn);
    cfn.Normalize();

    // base path
    const wxString base_path = ws_cfn.GetPath();

    // check source binary exists
    wxString bin_fpath = cfn.GetFullPath();
    if(!cfn.Exists()) {
        bin_fpath = wxFileSelector(_("Please select the binary to analyze"), base_path, "", "");
        if(bin_fpath.IsEmpty()) return MessageBox(_("selected binary was canceled"), wxICON_ERROR);

        cfn.Assign(bin_fpath, wxPATH_NATIVE);
    }
    if(!cfn.IsFileExecutable()) return MessageBox(_("bin/exe isn't executable"), wxICON_ERROR);

    // check 'gmon.out' file exists
    wxFileName gmon_cfn(cfn.GetPath() + wxFileName::GetPathSeparator() + GMON_FILENAME_OUT);
    gmon_cfn.Normalize();

    wxString gmonfn = gmon_cfn.GetFullPath();
    if(!gmon_cfn.Exists()) {
        gmonfn = wxFileSelector(_("Please select the gprof file"), gmon_cfn.GetPath(), "gmon", "out");
        if(gmonfn.IsEmpty()) return MessageBox(_("selected gprof was canceled"), wxICON_ERROR);

        gmon_cfn.Assign(gmonfn, wxPATH_NATIVE);
    }

    wxString bin, arg1, arg2;

    bin = GetGprofPath();
    arg1 = bin_fpath;
    arg2 = gmonfn;

    wxString cmdgprof = wxString::Format("%s %s %s", bin, arg1, arg2);

    // myLog("about to wxExecute(\"%s\")", cmdgprof);

    wxProcess* proc = new wxProcess(wxPROCESS_REDIRECT);

    // wxStopWatch	sw;

    const int err = ::wxExecute(cmdgprof, wxEXEC_SYNC, proc);
    wxUnusedVar(err);
    // on sync returns 0 (success), -1 (failure / "couldn't be started")

    // myLog("wxExecute() returned err %d, had pid %d", err, (int)proc->GetPid());

    wxInputStream* process_is = proc->GetInputStream();
    if(!process_is || !process_is->CanRead())
        return MessageBox(_("wxProcess::GetInputStream() can't be opened, aborting"), wxICON_ERROR);

    // start parsing and writing to dot language file
    GprofParser pgp;

    pgp.GprofParserStream(process_is);

    // myLog("gprof done (read %d lines)", (int) pgp.lines.GetCount());

    delete proc;

    ConfCallGraph conf;

    config_tool->ReadObject(wxT("CallGraph"), &conf);

    DotWriter dotWriter;

    // DotWriter
    dotWriter.SetLineParser(&(pgp.lines));

    int suggestedThreshold = pgp.GetSuggestedNodeThreshold();

    if(suggestedThreshold <= conf.GetTresholdNode()) {
        suggestedThreshold = conf.GetTresholdNode();

        dotWriter.SetDotWriterFromDialogSettings(m_mgr);

    } else {
        dotWriter.SetDotWriterFromDetails(conf.GetColorsNode(), conf.GetColorsEdge(), suggestedThreshold,
                                          conf.GetTresholdEdge(), conf.GetHideParams(), conf.GetStripParams(),
                                          conf.GetHideNamespaces());

        wxString suggest_msg = wxString::Format(_("The CallGraph plugin has suggested node threshold %d to speed-up "
                                                  "the call graph creation. You can alter it on the call graph panel."),
                                                suggestedThreshold);

        MessageBox(suggest_msg, wxICON_INFORMATION);
    }

    dotWriter.WriteToDotLanguage();

    // build output dir
    cfn.Assign(base_path, "");
    cfn.AppendDir(CALLGRAPH_DIR);
    cfn.Normalize();

    if(!cfn.DirExists()) cfn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    cfn.SetFullName(DOT_FILENAME_TXT);
    wxString dot_fn = cfn.GetFullPath();

    dotWriter.SendToDotAppOutputDirectory(dot_fn);

    cfn.SetFullName(DOT_FILENAME_PNG);
    wxString output_png_fn = cfn.GetFullPath();

    // delete any existing PNG
    if(wxFileExists(output_png_fn)) clRemoveFile(output_png_fn);

    wxString cmddot_ln;

    cmddot_ln << GetDotPath() << " -Tpng -o" << output_png_fn << " " << dot_fn;

    // myLog("wxExecute(\"%s\")", cmddot_ln);

    wxExecute(cmddot_ln, wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE);

    // myLog("dot done");

    if(!wxFileExists(output_png_fn))
        return MessageBox(_("Failed to open file CallGraph.png. Please check the project settings, rebuild the project "
                            "and try again."),
                          wxICON_INFORMATION);

    // show image and create table in the editor tab page
    uicallgraphpanel* panel = new uicallgraphpanel(m_mgr->GetEditorPaneNotebook(), m_mgr, output_png_fn, base_path,
                                                   suggestedThreshold, &(pgp.lines));

    wxString tstamp = wxDateTime::Now().Format(wxT(" %Y-%m-%d %H:%M:%S"));

    wxString title = wxT("Call graph for \"") + output_png_fn + wxT("\" " + tstamp);

    m_mgr->AddEditorPage(panel, title);
}

//---- Show Settings Dialog ---------------------------------------------------

void CallGraph::OnSettings(wxCommandEvent& event)
{
    // open the settings dialog
    wxWindow* win = m_mgr->GetTheApp()->GetTopWindow();

    uisettingsdlg uisdlg(win, m_mgr);

    uisdlg.ShowModal();
}

//---- Message Box (combo) ----------------------------------------------------

void CallGraph::MessageBox(const wxString& msg, unsigned long icon_mask)
{
    wxWindow* win = m_mgr->GetTheApp()->GetTopWindow();

    ::wxMessageBox(msg, wxT("CallGraph"), wxOK | icon_mask, win);
}
