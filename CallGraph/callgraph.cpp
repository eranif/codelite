#include "callgraph.h"
#include "uisettingsdlg.h"
#include "uicallgraphpanel.h"
#include "toolbaricons.h"
#include <wx/xrc/xmlres.h>
#include <wx/artprov.h>
#include "workspace.h"
#include "string.h"
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/aboutdlg.h>
#include <wx/datetime.h>
#include "imacromanager.h"

/*!
  * \brief Class for include plugin to CodeLite.
  */

CallGraph* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == 0) {
        thePlugin = new CallGraph(manager);
    }
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

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Václav Špruček, Michal Bližňák, Tomas Bata University in Zlin, www.fai.utb.cz"));
    info.SetName(wxT("CallGraph"));
    info.SetDescription(_("Create application call graph from profiling information provided by gprof tool."));
    info.SetVersion(wxT("v1.1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

CallGraph::CallGraph(IManager *manager)
    : IPlugin(manager)
{
    m_longName = _("Create application call graph from profiling information provided by gprof tool.");
    m_shortName = wxT("CallGraph");

    m_mgr->GetTheApp()->Connect( XRCID("cg_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CallGraph::OnSettings ), NULL, this );
    m_mgr->GetTheApp()->Connect( XRCID("cg_about"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CallGraph::OnAbout ), NULL, this );

    m_mgr->GetTheApp()->Connect( XRCID("cg_show_callgraph"), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CallGraph::OnShowCallGraph ), NULL, this );

    stvariables::InicializeStatic();
    //inicialize paths for standard and stored paths for this plugin
    GetDotPath();
    GetGprofPath();
}

CallGraph::~CallGraph()
{
    m_mgr->GetTheApp()->Disconnect( XRCID("cg_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CallGraph::OnSettings ), NULL, this );
    m_mgr->GetTheApp()->Disconnect( XRCID("cg_about"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CallGraph::OnAbout ), NULL, this );

    m_mgr->GetTheApp()->Disconnect( XRCID("cg_show_callgraph"), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CallGraph::OnShowCallGraph ), NULL, this );
}

clToolBar *CallGraph::CreateToolBar(wxWindow *parent)
{
    //Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);

    // First, check that CodeLite allows plugin to register plugins
    if (m_mgr->AllowToolbar()) {
        // Support both toolbars icon size
        int size = m_mgr->GetToolbarIconSize();

        // Allocate new toolbar, which will be freed later by CodeLite
        tb = new clToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);

        // Set the toolbar size
        tb->SetToolBitmapSize(wxSize(size, size));

        // Add tools to the plugins toolbar. You must provide 2 sets of icons: 24x24 and 16x16
		BitmapLoader *bmpLoader = m_mgr->GetStdIcons();
		
        if (size == 24) {
            tb->AddTool(XRCID("cg_show_callgraph"), 
						_("Show call graph"), 
						bmpLoader->LoadBitmap(wxT("callgraph/24/cg")), 
						_("Show call graph for selected/active project"), 
						wxITEM_NORMAL);
        } else {
            tb->AddTool(XRCID("cg_show_callgraph"), 
						_("Show call graph"), 
						bmpLoader->LoadBitmap(wxT("callgraph/16/cg")), 
						_("Show call graph for selected/active project"), 
						wxITEM_NORMAL);
        }
        tb->Realize();
    }
    return tb;
}

void CallGraph::CreatePluginMenu(wxMenu *pluginsMenu)
{

    // You can use the below code a snippet:
    wxMenu *menu = new wxMenu();
    wxMenuItem *item(NULL);
    item = new wxMenuItem(menu, XRCID("cg_show_callgraph"), _("Show call graph"), _("Show call graph for selected/active project"), wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("cg_settings"), _("Settings..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    item = new wxMenuItem(menu, XRCID("cg_about"), _("About..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    //
    pluginsMenu->Append(wxID_ANY, wxT("Call Graph"), menu);

}

wxMenu* CallGraph::CreateProjectPopMenu()
{
    wxMenu* menu = new wxMenu();
    wxMenuItem *item(NULL);

    item = new wxMenuItem(menu, XRCID("cg_show_callgraph"), _("Show call graph"), _("Show call graph for selected project"), wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

void CallGraph::HookPopupMenu(wxMenu *menu, MenuType type)
{
    if (type == MenuTypeEditor) {
        //TODO::Append items for the editor context menu
    } else if (type == MenuTypeFileExplorer) {
        //TODO::Append items for the file explorer context menu
    } else if (type == MenuTypeFileView_Workspace) {
        //TODO::Append items for the file view / workspace context menu
    } else if (type == MenuTypeFileView_Project) {
        //TODO::Append items for the file view/Project context menu
        if ( !menu->FindItem( XRCID("cg_show_callgraph_popup") ) ) {
            menu->PrependSeparator();
            menu->Prepend( XRCID("cg_show_callgraph_popup"), _("Call Graph"), CreateProjectPopMenu() );
        }
    } else if (type == MenuTypeFileView_Folder) {
        //TODO::Append items for the file view/Virtual folder context menu
    } else if (type == MenuTypeFileView_File) {
        //TODO::Append items for the file view/file context menu
    }
}

/*void CallGraph::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		//TODO::Unhook items for the editor context menu
	} else if (type == MenuTypeFileExplorer) {
		//TODO::Unhook  items for the file explorer context menu
	} else if (type == MenuTypeFileView_Workspace) {
		//TODO::Unhook  items for the file view / workspace context menu
	} else if (type == MenuTypeFileView_Project) {
		//TODO::Unhook  items for the file view/Project context menu
	} else if (type == MenuTypeFileView_Folder) {
		//TODO::Unhook  items for the file view/Virtual folder context menu
	} else if (type == MenuTypeFileView_File) {
		//TODO::Unhook  items for the file view/file context menu
	}
}*/


void CallGraph::UnPlug()
{
    //TODO:: perform the unplug action for this plugin
}

void CallGraph::OnAbout(wxCommandEvent& event)
{
    //wxString version = wxString::Format(DBE_VERSION);

    wxString desc = _("Create application call graph from profiling information provided by gprof tool.   \n\n"); // zapsat vice info - neco o aplikaci dot
    desc << wxbuildinfo() << wxT("\n\n");

    wxAboutDialogInfo info;
    info.SetName(_("Call Graph"));
    info.SetVersion(_("v1.1.0"));
    info.SetDescription(desc);
    info.SetCopyright(_("2012 (C) Tomas Bata University, Zlin, Czech Republic"));
    info.SetWebSite(_("http://www.fai.utb.cz"));
    info.AddDeveloper(wxT("Václav Špruček"));
    info.AddDeveloper(wxT("Michal Bližňák"));

    wxAboutBox(info);
}

wxString CallGraph::GetGprofPath()
{
    //ConfCallGraph confData;
    m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);
    wxString gprofPath = confData.GetGprofPath();

    if(gprofPath.IsEmpty()) {
#ifdef __WXMSW__
        //gprof
        if (wxFileExists(wxT("C:") + stvariables::sd + wxT("MinGW-4.4.1") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::gprofname)) {
            gprofPath = wxT("C:") + stvariables::sd + wxT("MinGW-4.4.1") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::gprofname;
        } else if (wxFileExists(wxT("C:") + stvariables::sd + wxT("MinGW") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::gprofname)) {
            gprofPath = wxT("C:") + stvariables::sd + wxT("MinGW") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::gprofname;
        } else gprofPath = wxT("");

#else
        if (wxFileExists(stvariables::sd + wxT("usr") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::gprofname)) {
            gprofPath = stvariables::sd + wxT("usr") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::gprofname;
        } else if (wxFileExists(stvariables::sd + wxT("usr") + stvariables::sd + wxT("local") + stvariables::sd  + wxT("bin") + stvariables::gprofname)) {
            gprofPath = stvariables::sd + wxT("usr") + stvariables::sd + wxT("local") + stvariables::sd  + wxT("bin") + stvariables::sd  + stvariables::gprofname;
        } else gprofPath = wxT("");
#endif

        confData.SetGprofPath(gprofPath);
        m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);
    }

    return gprofPath;
}

wxString CallGraph::GetDotPath()
{
    //ConfCallGraph confData;
    m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);
    wxString dotPath = confData.GetDotPath();

    if(dotPath.IsEmpty()) {
#ifdef __WXMSW__
        // DOT
        if (wxFileExists(wxT("C:") + stvariables::sd + wxT("Graphviz 2.28") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::dotname)) {
            dotPath = wxT("C:") + stvariables::sd + wxT("Graphviz 2.28") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::dotname;
        } else if (wxFileExists(wxT("C:") + stvariables::sd + wxT("Graphviz") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::dotname)) {
            dotPath = wxT("C:") + stvariables::sd + wxT("Graphviz") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::dotname;
        } else dotPath = wxT("");
#else
        if (wxFileExists(stvariables::sd + wxT("usr") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::dotname)) {
            dotPath = stvariables::sd + wxT("usr") + stvariables::sd + wxT("bin") + stvariables::sd + stvariables::dotname;
        } else if (wxFileExists(stvariables::sd + wxT("usr") + stvariables::sd + wxT("local") + stvariables::sd  + wxT("bin") + stvariables::sd + stvariables::dotname)) {
            dotPath = stvariables::sd + wxT("usr") + stvariables::sd + wxT("local") + stvariables::sd  + wxT("bin") + stvariables::sd + stvariables::dotname;
        } else dotPath = wxT("");
#endif

        confData.SetDotPath(dotPath);
        m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);
    }

    return dotPath;
}

void CallGraph::OnShowCallGraph(wxCommandEvent& event)
{
    wxString errMsg, projectPath, projectPathActive, projectName, workingDirectory, outFile;
    wxFileName outFn;
    m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);

    if (m_mgr->GetWorkspace()) {
        TreeItemInfo info = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
        if( info.m_itemType == ProjectItem::TypeProject) projectName = info.m_text;
        else projectName = m_mgr->GetWorkspace()->GetActiveProjectName(); // get project name for active project

        ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName( projectName, errMsg );
        if (proj) {
            BuildConfigPtr bldConf = m_mgr->GetWorkspace()->GetProjBuildConf(proj->GetName(), wxT(""));
            if(bldConf) {
                outFile =  m_mgr->GetMacrosManager()->Expand( bldConf->GetCommand(), m_mgr, proj->GetName() );
                outFn = wxFileName(outFile);
                if (outFn.IsRelative()) {
                    workingDirectory = m_mgr->GetMacrosManager()->Expand( bldConf->GetWorkingDirectory(), m_mgr, proj->GetName() );
                }
            }
            projectPath = proj->GetFileName().GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR ) + workingDirectory + stvariables::sd;; //path for active project

            if (outFn.IsRelative()) {
                projectPathActive = projectPath;
            }
        } else {
            wxMessageBox(errMsg, wxT("CallGraph"), wxOK | wxICON_ERROR, m_mgr->GetTheApp()->GetTopWindow());
            return;
        }
    } else {
        wxMessageBox(_("Unable to get opened workspace."), wxT("CallGraph"), wxOK | wxICON_ERROR, m_mgr->GetTheApp()->GetTopWindow());
        return;
    }

    bool gmonfound = true;

    if (!wxFileExists(projectPath + stvariables::gmonfile)) {
        gmonfound = false;

        if (outFn.IsAbsolute() && outFn.GetPath(wxPATH_GET_SEPARATOR) != projectPath) {
            // If it's not in the workspace (and for a custom makefile it probably won't be) try in the binary's dir
            if (wxFileExists(outFn.GetPath(wxPATH_GET_SEPARATOR) + stvariables::gmonfile)) {
                projectPath = outFn.GetPath(wxPATH_GET_SEPARATOR);
                gmonfound = true;
            }
        } else { // If all else fails, throw the problem back at the user
            wxString gmonFp = wxFileSelector(_("Please select the gprof file to analyse (it's probably called 'gmon.out')."));
            wxFileName gmonFn(gmonFp);
            if (wxFileExists(gmonFn.GetPath(wxPATH_GET_SEPARATOR) + stvariables::gmonfile)) {
                projectPath = gmonFn.GetPath(wxPATH_GET_SEPARATOR);
                gmonfound = true;
            }
        }
    }

    if( !gmonfound ) {
        wxString msg;
        msg << _("Failed to locate gmon.out file. Please check the following:\n")
            << _("1. Make sure that your project is compiled AND linked with the '-pg' flag\n")
            << _("2. You need to RUN your project at least once to be able to view the call-graph\n");
        wxMessageBox(msg, wxT("CallGraph"), wxOK | wxICON_WARNING, m_mgr->GetTheApp()->GetTopWindow());
        return;
    }

    if ( !wxFileExists(GetGprofPath()) || !wxFileExists(GetDotPath()) ) {
        wxMessageBox(_T("Failed to locate required tools (gprof, dot). Please check the plugin settings."), wxT("CallGraph"), wxOK | wxICON_ERROR, m_mgr->GetTheApp()->GetTopWindow());
        return;
    }

    // start for parsing and writing to the dot language file
    int suggestedThreshold = -1;
    GprofParser pgp;
    DotWriter pdw;

    wxProcess gprofProcess;
    gprofProcess.Redirect();
    if( !stvariables::filetype.IsEmpty() && outFile.Contains( stvariables::filetype ) ) outFile.Replace( stvariables::filetype, wxEmptyString );
    wxString cmdgprof = GetGprofPath() + stvariables::sw + stvariables::sq + projectPathActive + outFile + stvariables::filetype +
                        stvariables::sq + stvariables::sw + stvariables::sq + projectPath + stvariables::gmonfile + stvariables::sq;
    wxExecute(cmdgprof, wxEXEC_SYNC, &gprofProcess);
    m_pInputStream = gprofProcess.GetInputStream();

    if(m_pInputStream->CanRead()) {
        pgp.GprofParserStream(m_pInputStream);
    } else {
        wxMessageBox(_("CallGraph failed to get profiling data. Please check the project settings and build it again."),
                     wxT("CallGraph"), wxOK | wxICON_ERROR, m_mgr->GetTheApp()->GetTopWindow());
        return;
    }

    ConfCallGraph conf;
    m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &conf);
    //DotWriter to output png file
    pdw.SetLineParser(&(pgp.lines));
    suggestedThreshold = pgp.GetSuggestedNodeThreshold();
    if( suggestedThreshold <= conf.GetTresholdNode() ) {
        suggestedThreshold = conf.GetTresholdNode();
        pdw.SetDotWriterFromDialogSettings(m_mgr);
    } else {
        pdw.SetDotWriterFromDetails(conf.GetColorsNode(),
                                    conf.GetColorsEdge(),
                                    suggestedThreshold,
                                    conf.GetTresholdEdge(),
                                    conf.GetHideParams(),
                                    conf.GetStripParams(),
                                    conf.GetHideNamespaces());

        wxMessageBox(wxString::Format(_("The CallGraph plugin has suggested node threshold %d to speed-up the call graph creation. You can alter it on the call graph panel."),
                                      suggestedThreshold), wxT("CallGraph"), wxOK | wxICON_INFORMATION, m_mgr->GetTheApp()->GetTopWindow());
    }

    pdw.WriteToDotLanguage();
    pdw.SendToDotAppOutputDirectory(projectPathActive);

    if (pdw.DotFileExist(projectPathActive)) {
        //-Gcharset=Latin1		//UTF-8
        wxString cmddot = GetDotPath() + stvariables::sw + wxT("-Tpng -o") + stvariables::sw + projectPathActive + stvariables::dotfilesdir + stvariables::sd + stvariables::dotpngname  + stvariables::sw + projectPathActive + stvariables::dotfilesdir + stvariables::sd + stvariables::dottxtname;
        wxProcess cmddotProcess;
        cmddotProcess.Redirect();
        wxExecute(cmddot, wxEXEC_SYNC, &cmddotProcess);
    } else {
        wxMessageBox(_("CallGraph failed to save file with DOT language, please build the project again."),
                     wxT("CallGraph"), wxOK | wxICON_INFORMATION, m_mgr->GetTheApp()->GetTopWindow());
        return;
    }

    //show image and greate table in the editor tab page
    if(wxFileExists(projectPathActive + stvariables::dotfilesdir + stvariables::sd + stvariables::dotpngname)) {
        m_mgr->AddEditorPage( new uicallgraphpanel( m_mgr->GetEditorPaneNotebook(), m_mgr, projectPathActive + stvariables::dotfilesdir + stvariables::sd + stvariables::dotpngname, projectPathActive, suggestedThreshold, &(pgp.lines)), wxT("Call graph for \"") + projectName +  wxT("\" ") + wxDateTime::Now().Format(wxT("%Y-%m-%d %H:%M:%S")));
    } else {
        //wxMessageBox(wxT("File CallGraph.png is not exist and can not be open in page."));
        wxMessageBox(_("Failed to open file CallGraph.png. Please check the project settings, rebuild the project and try again."),
                     wxT("CallGraph"), wxOK | wxICON_INFORMATION, m_mgr->GetTheApp()->GetTopWindow());
        return;
    }
}

void CallGraph::OnSettings(wxCommandEvent& event)
{
    // open the settings dialog
    uisettingsdlg uisdlg(m_mgr->GetTheApp()->GetTopWindow(),m_mgr);//,this);
    uisdlg.ShowModal();
}
