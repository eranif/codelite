//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : gizmos.cpp
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
#include "PluginWizard.h"
#include "clFileSystemWorkspace.hpp"
#include "cl_command_event.h"
#include "cl_standard_paths.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "dirsaver.h"
#include "editor_config.h"
#include "entry.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "gizmos.h"
#include "globals.h"
#include "newclassdlg.h"
#include "newplugindata.h"
#include "newwxprojectdlg.h"
#include "workspace.h"
#include "wx/ffile.h"
#include <algorithm>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>

static wxString MI_NEW_WX_PROJECT = wxT("Create new wxWidgets project...");
static wxString MI_NEW_CODELITE_PLUGIN = wxT("Create new CodeLite plugin...");
static wxString MI_NEW_NEW_CLASS = wxT("Create new C++ class...");

enum { ID_MI_NEW_WX_PROJECT = 9000, ID_MI_NEW_CODELITE_PLUGIN, ID_MI_NEW_NEW_CLASS };

static WizardsPlugin* theGismos = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(theGismos == 0) { theGismos = new WizardsPlugin(manager); }
    return theGismos;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("Wizards"));
    info.SetDescription(_("Wizards Plugin - a collection of useful wizards for C++:\nnew Class Wizard, new wxWidgets "
                          "Wizard, new Plugin Wizard"));
    info.SetVersion(wxT("v1.1"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

/// Ascending sorting function
struct ascendingSortOp {
    bool operator()(const TagEntryPtr& rStart, const TagEntryPtr& rEnd)
    {
        return rEnd->GetName().Cmp(rStart->GetName()) > 0;
    }
};

//-------------------------------------
// helper methods
//-------------------------------------
static void ExpandVariables(wxString& content, const NewWxProjectInfo& info)
{
    content.Replace(wxT("$(ProjectName)"), info.GetName());
    wxString projname = info.GetName();
    projname.MakeLower();

    wxString appfilename = projname + wxT("_app");
    wxString framefilename = projname + wxT("_frame");

    content.Replace(wxT("$(MainFile)"), projname);
    content.Replace(wxT("$(AppFile)"), appfilename);
    content.Replace(wxT("$(MainFrameFile)"), framefilename);
    content.Replace(wxT("$(Unicode)"), info.GetFlags() & wxWidgetsUnicode ? wxT("yes") : wxT("no"));
    content.Replace(wxT("$(Static)"), info.GetFlags() & wxWidgetsStatic ? wxT("yes") : wxT("no"));
    content.Replace(wxT("$(Universal)"), info.GetFlags() & wxWidgetsUniversal ? wxT("yes") : wxT("no"));
    content.Replace(wxT("$(WinResFlag)"), info.GetFlags() & wxWidgetsWinRes ? wxT("yes") : wxT("no"));
    content.Replace(wxT("$(MWindowsFlag)"), info.GetFlags() & wxWidgetsSetMWindows ? wxT("-mwindows") : wxEmptyString);
    content.Replace(wxT("$(PCHFlag)"), info.GetFlags() & wxWidgetsPCH ? wxT("WX_PRECOMP") : wxEmptyString);
    content.Replace(wxT("$(PCHCmpOptions)"),
                    info.GetFlags() & wxWidgetsPCH ? wxT("-Winvalid-pch;-include wx_pch.h") : wxEmptyString);
    content.Replace(wxT("$(PCHFileName)"), info.GetFlags() & wxWidgetsPCH ? wxT("wx_pch.h") : wxEmptyString);

    if(info.GetFlags() & wxWidgetsWinRes) content.Replace(wxT("$(WinResFile)"), wxT("<File Name=\"resources.rc\" />"));
    if(info.GetFlags() & wxWidgetsPCH) content.Replace(wxT("$(PCHFile)"), wxT("<File Name=\"wx_pch.h\" />"));

    wxString othersettings;
    if(info.GetVersion() != wxT("Default")) othersettings << wxT("--version=") << info.GetVersion();
    if(!info.GetPrefix().IsEmpty()) othersettings << wxT(" --prefix=") << info.GetPrefix();
    content.Replace(wxT("$(Other)"), othersettings);

    // create the application class name
    wxString initial = appfilename.Mid(0, 1);
    initial.MakeUpper();
    appfilename.SetChar(0, initial.GetChar(0));

    // create the main frame class name
    wxString framename(projname);
    wxString appname(projname);

    framename << wxT("Frame");
    appname << wxT("App");

    initial = framename.Mid(0, 1);
    initial.MakeUpper();
    framename.SetChar(0, initial.GetChar(0));

    initial = appname.Mid(0, 1);
    initial.MakeUpper();
    appname.SetChar(0, initial.GetChar(0));

    content.Replace(wxT("$(AppName)"), appname);
    content.Replace(wxT("$(MainFrameName)"), framename);
}

static void WriteFile(const wxString& fileName, const wxString& content)
{
    wxFFile file;
    if(!file.Open(fileName, wxT("w+b"))) { return; }

    file.Write(content);
    file.Close();
}

static void WriteNamespacesDeclaration(const wxArrayString& namespacesList, wxString& buffer)
{
    for(unsigned int i = 0; i < namespacesList.Count(); i++) {
        buffer << wxT("namespace ") << namespacesList[i] << wxT("\n{\n\n");
    }
}

WizardsPlugin::WizardsPlugin(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Wizards Plugin - a collection of useful utils for C++");
    m_shortName = wxT("Wizards");
}

WizardsPlugin::~WizardsPlugin() {}

void WizardsPlugin::CreateToolBar(clToolBar* toolbar)
{
    wxUnusedVar(toolbar);
    // Connect the events to us
#if !USE_AUI_TOOLBAR
    m_mgr->GetTheApp()->Connect(XRCID("gizmos_options"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(WizardsPlugin::OnGizmos), NULL, (wxEvtHandler*)this);
#endif
    m_mgr->GetTheApp()->Connect(XRCID("gizmos_options"), wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(WizardsPlugin::OnGizmosUI), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(ID_MI_NEW_CODELITE_PLUGIN, wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(WizardsPlugin::OnNewPlugin), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(ID_MI_NEW_CODELITE_PLUGIN, wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(WizardsPlugin::OnNewPluginUI), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(ID_MI_NEW_NEW_CLASS, wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(WizardsPlugin::OnNewClass), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(ID_MI_NEW_NEW_CLASS, wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(WizardsPlugin::OnNewClassUI), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(ID_MI_NEW_WX_PROJECT, wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(WizardsPlugin::OnNewWxProject), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(ID_MI_NEW_WX_PROJECT, wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(WizardsPlugin::OnNewWxProjectUI), NULL, (wxEvtHandler*)this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &WizardsPlugin::OnFolderContentMenu, this);
}

void WizardsPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, ID_MI_NEW_CODELITE_PLUGIN, _("New CodeLite Plugin Wizard..."), wxEmptyString,
                          wxITEM_NORMAL);
    menu->Append(item);
    item = new wxMenuItem(menu, ID_MI_NEW_NEW_CLASS, _("New Class Wizard..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    item =
        new wxMenuItem(menu, ID_MI_NEW_WX_PROJECT, _("New wxWidgets Project Wizard..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, wxT("Wizards"), menu);
}

void WizardsPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeFileView_Folder) {
        // Create the popup menu for the virtual folders
        wxMenuItem* item(NULL);

        item = new wxMenuItem(menu, wxID_SEPARATOR);
        menu->Prepend(item);
        m_vdDynItems.push_back(item);

        item = new wxMenuItem(menu, ID_MI_NEW_NEW_CLASS, _("&New Class..."), wxEmptyString, wxITEM_NORMAL);
        menu->Prepend(item);
        m_vdDynItems.push_back(item);
    }
}

void WizardsPlugin::UnPlug()
{
    m_mgr->GetTheApp()->Disconnect(XRCID("gizmos_options"), wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(WizardsPlugin::OnGizmos), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("gizmos_options"), wxEVT_UPDATE_UI,
                                   wxUpdateUIEventHandler(WizardsPlugin::OnGizmosUI), NULL, (wxEvtHandler*)this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &WizardsPlugin::OnFolderContentMenu, this);
}

void WizardsPlugin::OnNewPlugin(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoCreateNewPlugin();
}

void WizardsPlugin::DoCreateNewPlugin()
{
    // Load the wizard
    PluginWizard wiz(wxTheApp->GetTopWindow());
    NewPluginData data;
    if(wiz.Run(data)) {
        // load the template file and replace all variables with the
        // actual values provided by user
        wxString filename(m_mgr->GetStartupDirectory() + wxT("/templates/gizmos/liteeditor-plugin.project.wizard"));
        wxString content;
        if(!ReadFileWithConversion(filename, content)) { return; }

        // Convert the paths provided by user to relative paths
        wxFileName fn(data.GetCodelitePath(), "");
        if(!fn.MakeRelativeTo(wxFileName(data.GetProjectPath()).GetPath())) {
            clLogMessage(wxT("Warning: Failed to convert paths to relative path."));
        }

#ifdef __WXMSW__
        wxString dllExt(wxT("dll"));
#else
        wxString dllExt(wxT("so"));
#endif

        wxString clpath = fn.GetFullPath();
        fn.Normalize(); // Remove all .. and . from the path

        if(clpath.EndsWith("/") || clpath.EndsWith("\\")) { clpath.RemoveLast(); }

        content.Replace(wxT("$(CodeLitePath)"), clpath);
        content.Replace(wxT("$(DllExt)"), dllExt);
        content.Replace(wxT("$(PluginName)"), data.GetPluginName());
        wxString baseFileName = data.GetPluginName();
        baseFileName.MakeLower();
        content.Replace(wxT("$(BaseFileName)"), baseFileName);
        content.Replace(wxT("$(ProjectName)"), data.GetPluginName());

        // save the file to the disk
        wxString projectFileName;
        projectFileName << data.GetProjectPath();
        {
            wxLogNull noLog;
            ::wxMkdir(wxFileName(data.GetProjectPath()).GetPath());
        }
        wxFFile file;
        if(!file.Open(projectFileName, wxT("w+b"))) { return; }

        file.Write(content);
        file.Close();

        // Create the plugin source and header files
        wxFileName srcFile(wxFileName(data.GetProjectPath()).GetPath(), baseFileName);
        srcFile.SetExt("cpp");

        wxFileName headerFile(wxFileName(data.GetProjectPath()).GetPath(), baseFileName);
        headerFile.SetExt("h");

        //---------------------------------------------------------------
        // write the content of the file based on the file template
        //---------------------------------------------------------------

        // Generate the source files
        filename = m_mgr->GetStartupDirectory() + wxT("/templates/gizmos/plugin.cpp.wizard");
        content.Clear();
        if(!ReadFileWithConversion(filename, content)) {
            wxMessageBox(_("Failed to load wizard's file 'plugin.cpp.wizard'"), _("CodeLite"), wxICON_WARNING | wxOK);
            return;
        }

        // Expand macros
        content.Replace(wxT("$(PluginName)"), data.GetPluginName());
        content.Replace(wxT("$(BaseFileName)"), baseFileName);
        content.Replace(wxT("$(PluginShortName)"), data.GetPluginName());
        content.Replace(wxT("$(PluginLongName)"), data.GetPluginDescription());
        content.Replace(wxT("$(UserName)"), wxGetUserName().c_str());

        // Notify the formatter plugin to format the plugin source files
        clSourceFormatEvent evtFormat(wxEVT_FORMAT_STRING);
        evtFormat.SetInputString(content);
        evtFormat.SetFileName(srcFile.GetFullPath());
        EventNotifier::Get()->ProcessEvent(evtFormat);
        content = evtFormat.GetFormattedString();

        // Write it down
        file.Open(srcFile.GetFullPath(), wxT("w+b"));
        file.Write(content);
        file.Close();

        // create the header file
        filename = m_mgr->GetStartupDirectory() + wxT("/templates/gizmos/plugin.h.wizard");
        content.Clear();
        if(!ReadFileWithConversion(filename, content)) {
            wxMessageBox(_("Failed to load wizard's file 'plugin.h.wizard'"), _("CodeLite"), wxICON_WARNING | wxOK);
            return;
        }

        // Expand macros
        content.Replace(wxT("$(PluginName)"), data.GetPluginName());
        content.Replace(wxT("$(BaseFileName)"), baseFileName);
        content.Replace(wxT("$(PluginShortName)"), data.GetPluginName());
        content.Replace(wxT("$(PluginLongName)"), data.GetPluginDescription());
        content.Replace(wxT("$(UserName)"), wxGetUserName().c_str());

        // format the content
        evtFormat.SetString(content);
        EventNotifier::Get()->ProcessEvent(evtFormat);
        content = evtFormat.GetString();

        // Write it down
        file.Open(headerFile.GetFullPath(), wxT("w+b"));
        file.Write(content);
        file.Close();

        // Read the CMakeLists.txt.plugin.wizard file
        wxFileName cmakeFile(clStandardPaths::Get().GetDataDir(), "CMakeLists.txt.plugin.wizard");
        cmakeFile.AppendDir("templates");
        cmakeFile.AppendDir("gizmos");
        if(cmakeFile.FileExists()) {
            wxString cmakeContent;
            if(FileUtils::ReadFileContent(cmakeFile, cmakeContent)) {
                cmakeContent.Replace("$(PluginName)", data.GetPluginName());
                cmakeFile.SetFullName("CMakeLists.txt");
                cmakeFile.SetPath(headerFile.GetPath());
                FileUtils::WriteFileContent(cmakeFile, cmakeContent);
            }
        }

        // add the new project to the workspace
        wxString errMsg;

        // convert the path to be full path as required by the
        // workspace manager
        m_mgr->AddProject(projectFileName);
    }
}

void WizardsPlugin::OnNewPluginUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    // we enable the button only when workspace is opened
    e.Enable(m_mgr->IsWorkspaceOpen());
}

void WizardsPlugin::OnNewClassUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    // we enable the button only when workspace is opened
    e.Enable(m_mgr->IsWorkspaceOpen() || clFileSystemWorkspace::Get().IsOpen());
}

void WizardsPlugin::OnNewClass(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoCreateNewClass();
}

void WizardsPlugin::DoCreateNewClass()
{
    NewClassDlg dlg(EventNotifier::Get()->TopFrame(), m_mgr);
    if(dlg.ShowModal() == wxID_OK) {
        // do something with the information here
        NewClassInfo info;
        dlg.GetNewClassInfo(info);
        CreateClass(info);
    }
}

void WizardsPlugin::CreateClass(NewClassInfo& info)
{
    // Start by finding the best choice for tabs/spaces.
    // Use the preference for the target VirtualDir, not the active project, in case the user perversely adds to an
    // inactive one.
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions(); // Globals first
    wxString TargetProj = info.virtualDirectory.BeforeFirst(wxT(':'));
    if(!TargetProj.empty()) {
        clCxxWorkspaceST::Get()->GetLocalWorkspace()->GetOptions(options,
                                                                 TargetProj); // Then override with any local ones
    }

    wxString separator(wxT("\t"));
    if(!options->GetIndentUsesTabs()) { separator = wxString(wxT(' '), wxMax(1, options->GetTabWidth())); }

    wxString blockGuard(info.blockGuard);
    if(blockGuard.IsEmpty()) {
        // use the name instead
        blockGuard = info.name;
        blockGuard.MakeUpper();
        blockGuard << (info.hppHeader ? wxT("_HPP") : wxT("_H"));
    }

    wxString headerExt = (info.hppHeader ? wxT(".hpp") : wxT(".h"));

    wxString srcFile;
    srcFile << info.path << wxFileName::GetPathSeparator() << info.fileName << wxT(".cpp");

    wxString hdrFile;
    hdrFile << info.path << wxFileName::GetPathSeparator() << info.fileName << headerExt;

    // create cpp + h file
    wxString cpp;
    wxString header;

    //----------------------------------------------------
    // header file
    //----------------------------------------------------
    if(info.usePragmaOnce) {
        header << "#pragma once\n\n";
    } else {
        header << wxT("#ifndef ") << blockGuard << wxT("\n");
        header << wxT("#define ") << blockGuard << wxT("\n");
        header << wxT("\n");
    }

    wxString closeMethod;
    if(info.isInline)
        closeMethod << wxT('\n') << separator << wxT("{\n") << separator << wxT("}\n");
    else
        closeMethod = wxT(";\n");

    // Add include for base classes
    if(!info.parents.name.empty()) {
        const ClassParentInfo& pi = info.parents;

        // Include the header name only (no paths)
        wxFileName includeFileName(pi.fileName);
        if(!pi.fileName.IsEmpty()) {
            header << wxT("#include \"") << includeFileName.GetFullName() << wxT("\" // Base class: ") << pi.name
                   << wxT("\n");
        }
        header << wxT("\n");
    }

    // Open namespace
    if(!info.namespacesList.IsEmpty()) { WriteNamespacesDeclaration(info.namespacesList, header); }

    header << wxT("class ") << info.name;

    if(!info.parents.name.empty()) {
        header << wxT(" : ");
        const ClassParentInfo& pi = info.parents;
        header << pi.access << wxT(" ") << pi.name;
    }
    header << wxT("\n{\n");
    if(info.isSingleton) { header << separator << wxT("static ") << info.name << wxT("* ms_instance;\n\n"); }

    if(info.isAssingable == false) {
        // declare copy constructor & assingment operator as private
        header << wxT("private:\n");
        header << separator << info.name << wxT("(const ") << info.name << wxT("& rhs)") << closeMethod;
        header << separator << info.name << wxT("& operator=(const ") << info.name << wxT("& rhs)") << closeMethod;
        header << wxT("\n");
    }

    if(info.isSingleton) {
        header << wxT("public:\n");
        header << separator << wxT("static ") << info.name << wxT("* Instance();\n");
        header << separator << wxT("static void Release();\n\n");

        header << wxT("private:\n");
        header << separator << info.name << wxT("();\n");

        if(info.isVirtualDtor) {
            header << separator << wxT("virtual ~") << info.name << wxT("();\n\n");
        } else {
            header << separator << wxT('~') << info.name << wxT("();\n\n");
        }
    } else {
        header << wxT("public:\n");
        header << separator << info.name << wxT("()") << closeMethod;
        if(info.isVirtualDtor) {
            header << separator << wxT("virtual ~") << info.name << wxT("()") << closeMethod << wxT("\n");
        } else {
            header << separator << wxT('~') << info.name << wxT("()") << closeMethod << wxT("\n");
        }
    }

    // add virtual function declaration
    wxString v_decl = DoGetVirtualFuncDecl(info, separator);
    if(v_decl.IsEmpty() == false) {
        header << wxT("public:\n");
        header << v_decl;
    }

    header << wxT("};\n\n");

    // Close namespaces
    for(unsigned int i = 0; i < info.namespacesList.Count(); i++) {
        header << wxT("}\n\n");
    }

    if(!info.usePragmaOnce) {
        // Close the block guard
        header << wxT("#endif // ") << blockGuard << wxT("\n");
    }

    wxFFile file;

    file.Open(hdrFile, wxT("w+b"));
    file.Write(header);
    file.Close();

    // if we have a selected virtual folder, add the files to it
    wxArrayString paths;
    paths.Add(hdrFile);

    //----------------------------------------------------
    // source file
    //----------------------------------------------------
    if(!info.isInline) {
        wxString nsPrefix;
        if(!info.namespacesList.IsEmpty()) {
            for(size_t i = 0; i < info.namespacesList.size(); ++i) {
                nsPrefix << info.namespacesList.Item(i) << "::";
            }
        }

        cpp << wxT("#include \"") << info.fileName << headerExt << wxT("\"\n");

        if(info.isSingleton) {
            cpp << "#include <cstdlib> // NULL \n\n";
            cpp << nsPrefix << info.name << wxT("* ") << nsPrefix << info.name << wxT("::ms_instance = NULL;\n\n");
        } else {
            cpp << "\n";
        }

        // ctor/dtor
        cpp << nsPrefix << info.name << wxT("::") << info.name << wxT("()\n");
        cpp << wxT("{\n}\n\n");
        cpp << nsPrefix << info.name << wxT("::~") << info.name << wxT("()\n");
        cpp << wxT("{\n}\n\n");

        // Prepend the ns to the class name (we do this after the ctor/dtor impl)
        info.name.Prepend(nsPrefix);

        if(info.isSingleton) {
            cpp << info.name << wxT("* ") << info.name << wxT("::Instance()\n");
            cpp << wxT("{\n");
            cpp << separator << wxT("if (ms_instance == NULL) {\n");
            cpp << separator << separator << wxT("ms_instance = new ") << info.name << wxT("();\n");
            cpp << separator << wxT("}\n");
            cpp << separator << wxT("return ms_instance;\n");
            cpp << wxT("}\n\n");

            cpp << wxT("void ") << info.name << wxT("::Release()\n");
            cpp << wxT("{\n");
            cpp << separator << wxT("if (ms_instance) {\n");
            cpp << separator << separator << wxT("delete ms_instance;\n");
            cpp << separator << wxT("}\n");
            cpp << separator << wxT("ms_instance = NULL;\n");
            cpp << wxT("}\n\n");
        }

        cpp << DoGetVirtualFuncImpl(info);

        file.Open(srcFile, wxT("w+b"));
        file.Write(cpp);
        file.Close();

        paths.Add(srcFile);
    }

    if(clCxxWorkspaceST::Get()->IsOpen()) {
        // We have a .cpp and an .h file, and there may well be a :src and an :include folder available
        // So try to place the files appropriately. If that fails, dump both in the selected folder
        bool smartAddFiles = EditorConfigST::Get()->GetOptions()->GetOptions() & OptionsConfig::Opt_SmartAddFiles;
        if(!smartAddFiles || !m_mgr->AddFilesToVirtualFolderIntelligently(info.virtualDirectory, paths))
            m_mgr->AddFilesToVirtualFolder(info.virtualDirectory, paths);
    }

    // Open the newly created classes in codelite
    for(const auto& file : paths) {
        m_mgr->OpenFile(file);
    }
    
    // Notify about files created on the file system
    clFileSystemEvent eventFilesCreated(wxEVT_FILE_CREATED);
    eventFilesCreated.GetPaths().swap(paths);
    EventNotifier::Get()->QueueEvent(eventFilesCreated.Clone());
    
    // Notify codelite to parse the files
    wxCommandEvent parseEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(parseEvent);
}

void WizardsPlugin::OnNewWxProject(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoCreateNewWxProject();
}

void WizardsPlugin::DoCreateNewWxProject()
{
    NewWxProjectDlg* dlg = new NewWxProjectDlg(NULL, m_mgr);
    if(dlg->ShowModal() == wxID_OK) {
        // Create the project
        NewWxProjectInfo info;
        dlg->GetProjectInfo(info);
        CreateWxProject(info);
    }
    dlg->Destroy();
}

void WizardsPlugin::OnNewWxProjectUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    // we enable the button only when workspace is opened
    e.Enable(m_mgr->IsWorkspaceOpen());
}

void WizardsPlugin::CreateWxProject(NewWxProjectInfo& info)
{
    // TODO:: Implement this ...
    wxString basedir = m_mgr->GetStartupDirectory();

    // we first create the project files
    if(info.GetType() == wxProjectTypeGUI) {

        // we are creating a project of type GUI
        wxString projectConent;
        wxString mainFrameCppContent;
        wxString mainFrameHContent;
        wxString appCppConent;
        wxString apphConent;
        wxString rcContent;
        wxString pchContent;

        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/wxproject.project.wizard"), projectConent)) {
            return;
        }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/mainframe.cpp.wizard"), mainFrameCppContent)) {
            return;
        }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/mainframe.h.wizard"), mainFrameHContent)) {
            return;
        }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/app.h.wizard"), apphConent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/app.cpp.wizard"), appCppConent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/resources.rc.wizard"), rcContent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/wx_pch.h.wizard"), pchContent)) { return; }

        ExpandVariables(projectConent, info);
        ExpandVariables(mainFrameCppContent, info);
        ExpandVariables(mainFrameHContent, info);
        ExpandVariables(apphConent, info);
        ExpandVariables(appCppConent, info);

        // Write the files content into the project directory
        DirSaver ds;
        wxSetWorkingDirectory(info.GetPath());

        wxString projname = info.GetName();
        projname.MakeLower();

        wxString appfilename = projname + wxT("_app");
        wxString framefilename = projname + wxT("_frame");

        WriteFile(framefilename + wxT(".cpp"), mainFrameCppContent);
        WriteFile(framefilename + wxT(".h"), mainFrameHContent);
        WriteFile(appfilename + wxT(".h"), apphConent);
        WriteFile(appfilename + wxT(".cpp"), appCppConent);
        if(info.GetFlags() & wxWidgetsWinRes) WriteFile(wxT("resources.rc"), rcContent);
        if(info.GetFlags() & wxWidgetsPCH) WriteFile(wxT("wx_pch.h"), pchContent);
        WriteFile(info.GetName() + wxT(".project"), projectConent);

        // If every this is OK, add the project as well
        m_mgr->AddProject(info.GetName() + wxT(".project"));

    } else if(info.GetType() == wxProjectTypeGUIFBDialog) {

        // we are creating a project of type GUI (dialog generated by wxFormBuilder)

        wxString projectContent;
        wxString mainFrameCppContent;
        wxString mainFrameHContent;
        wxString appCppContent;
        wxString apphContent;
        wxString fbContent;
        wxString rcContent;
        wxString pchContent;

        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/wxproject-fb.project.wizard"), projectContent)) {
            return;
        }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/gui-dialog.cpp.wizard"), mainFrameCppContent)) {
            return;
        }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/gui-dialog.h.wizard"), mainFrameHContent)) {
            return;
        }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/main-dialog.h.wizard"), apphContent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/main-dialog.cpp.wizard"), appCppContent)) {
            return;
        }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/gui-dialog.fbp.wizard"), fbContent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/resources.rc.wizard"), rcContent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/wx_pch.h.wizard"), pchContent)) { return; }

        ExpandVariables(projectContent, info);
        ExpandVariables(mainFrameCppContent, info);
        ExpandVariables(mainFrameHContent, info);
        ExpandVariables(apphContent, info);
        ExpandVariables(appCppContent, info);

        // Write the files content into the project directory
        DirSaver ds;
        wxSetWorkingDirectory(info.GetPath());

        wxString projname = info.GetName();
        projname.MakeLower();

        wxString appfilename = projname + wxT("_app");

        WriteFile(wxT("gui.cpp"), mainFrameCppContent);
        WriteFile(wxT("gui.h"), mainFrameHContent);
        WriteFile(appfilename + wxT(".h"), apphContent);
        WriteFile(appfilename + wxT(".cpp"), appCppContent);
        WriteFile(wxT("gui.fbp"), fbContent);
        if(info.GetFlags() & wxWidgetsWinRes) WriteFile(wxT("resources.rc"), rcContent);
        if(info.GetFlags() & wxWidgetsPCH) WriteFile(wxT("wx_pch.h"), pchContent);
        WriteFile(info.GetName() + wxT(".project"), projectContent);

        // If every this is OK, add the project as well
        m_mgr->AddProject(info.GetName() + wxT(".project"));

    } else if(info.GetType() == wxProjectTypeGUIFBFrame) {

        // we are creating a project of type GUI (dialog generated by wxFormBuilder)

        wxString projectContent;
        wxString mainFrameCppContent;
        wxString mainFrameHContent;
        wxString appCppContent;
        wxString apphContent;
        wxString fbContent;
        wxString pchContent;
        wxString rcContent;

        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/wxproject-fb.project.wizard"), projectContent)) {
            return;
        }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/gui-frame.cpp.wizard"), mainFrameCppContent)) {
            return;
        }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/gui-frame.h.wizard"), mainFrameHContent)) {
            return;
        }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/main-frame.h.wizard"), apphContent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/main-frame.cpp.wizard"), appCppContent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/gui-frame.fbp.wizard"), fbContent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/resources.rc.wizard"), rcContent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/wx_pch.h.wizard"), pchContent)) { return; }

        ExpandVariables(projectContent, info);
        ExpandVariables(mainFrameCppContent, info);
        ExpandVariables(mainFrameHContent, info);
        ExpandVariables(apphContent, info);
        ExpandVariables(appCppContent, info);

        // Write the files content into the project directory
        DirSaver ds;
        wxSetWorkingDirectory(info.GetPath());

        wxString projname = info.GetName();
        projname.MakeLower();

        wxString appfilename = projname + wxT("_app");

        WriteFile(wxT("gui.cpp"), mainFrameCppContent);
        WriteFile(wxT("gui.h"), mainFrameHContent);
        WriteFile(appfilename + wxT(".h"), apphContent);
        WriteFile(appfilename + wxT(".cpp"), appCppContent);
        WriteFile(wxT("gui.fbp"), fbContent);
        if(info.GetFlags() & wxWidgetsWinRes) WriteFile(wxT("resources.rc"), rcContent);
        if(info.GetFlags() & wxWidgetsPCH) WriteFile(wxT("wx_pch.h"), pchContent);
        WriteFile(info.GetName() + wxT(".project"), projectContent);

        // If every this is OK, add the project as well
        m_mgr->AddProject(info.GetName() + wxT(".project"));

    } else if(info.GetType() == wxProjectTypeSimpleMain) {

        // we are creating a project of type console app

        wxString projectConent;
        wxString appCppConent;
        wxString pchContent;
        wxString rcContent;

        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/wxmain.project.wizard"), projectConent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/main.cpp.wizard"), appCppConent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/resources.rc.wizard"), rcContent)) { return; }
        if(!ReadFileWithConversion(basedir + wxT("/templates/gizmos/wx_pch.h.wizard"), pchContent)) { return; }

        ExpandVariables(projectConent, info);
        ExpandVariables(appCppConent, info);

        // Write the files content into the project directory
        DirSaver ds;
        wxSetWorkingDirectory(info.GetPath());

        wxString projname = info.GetName();
        projname.MakeLower();

        wxString appfilename = projname;
        WriteFile(appfilename + wxT(".cpp"), appCppConent);
        if(info.GetFlags() & wxWidgetsWinRes) WriteFile(wxT("resources.rc"), rcContent);
        if(info.GetFlags() & wxWidgetsPCH) WriteFile(wxT("wx_pch.h"), pchContent);
        WriteFile(info.GetName() + wxT(".project"), projectConent);

        // If every this is OK, add the project as well
        m_mgr->AddProject(info.GetName() + wxT(".project"));
    }
}

wxString WizardsPlugin::DoGetVirtualFuncImpl(const NewClassInfo& info)
{
    if(info.implAllVirtual == false && info.implAllPureVirtual == false) return wxEmptyString;

    // get list of all parent virtual functions
    std::vector<TagEntryPtr> tmp_tags;
    std::vector<TagEntryPtr> no_dup_tags;
    std::vector<TagEntryPtr> tags;
    if(!info.parents.name.empty()) {
        const ClassParentInfo& pi = info.parents;
        // Load all prototypes / functions of the parent scope
        m_mgr->GetTagsManager()->TagsByScope(pi.name, wxT("prototype"), tmp_tags, false);
        m_mgr->GetTagsManager()->TagsByScope(pi.name, wxT("function"), tmp_tags, false);
    }

    // and finally sort the results
    std::sort(tmp_tags.begin(), tmp_tags.end(), ascendingSortOp());
    GizmosRemoveDuplicates(tmp_tags, no_dup_tags);

    // filter out all non virtual functions
    for(std::vector<TagEntryPtr>::size_type i = 0; i < no_dup_tags.size(); i++) {
        TagEntryPtr tt = no_dup_tags.at(i);
        bool collect(false);
        if(info.implAllVirtual) {
            collect = m_mgr->GetTagsManager()->IsVirtual(tt);
        } else if(info.implAllPureVirtual) {
            collect = m_mgr->GetTagsManager()->IsPureVirtual(tt);
        }

        if(collect) { tags.push_back(tt); }
    }

    wxString impl;
    for(std::vector<TagEntryPtr>::size_type i = 0; i < tags.size(); i++) {
        TagEntryPtr tt = tags.at(i);
        // we are not interested in Ctor-Dtor
        if(tt->IsConstructor() || tt->IsDestructor()) continue;
        impl << m_mgr->GetTagsManager()->FormatFunction(tt, FunctionFormat_Impl, info.name);
    }
    return impl;
}

wxString WizardsPlugin::DoGetVirtualFuncDecl(const NewClassInfo& info, const wxString& separator)
{
    if(info.implAllVirtual == false && info.implAllPureVirtual == false) return wxEmptyString;

    // get list of all parent virtual functions
    std::vector<TagEntryPtr> tmp_tags;
    std::vector<TagEntryPtr> no_dup_tags;
    std::vector<TagEntryPtr> tags;
    if(!info.parents.name.empty()) {
        const ClassParentInfo& pi = info.parents;

        // Load all prototypes / functions of the parent scope
        m_mgr->GetTagsManager()->TagsByScope(pi.name, wxT("prototype"), tmp_tags, false);
        m_mgr->GetTagsManager()->TagsByScope(pi.name, wxT("function"), tmp_tags, false);
    }

    // and finally sort the results
    std::sort(tmp_tags.begin(), tmp_tags.end(), ascendingSortOp());
    GizmosRemoveDuplicates(tmp_tags, no_dup_tags);

    // filter out all non virtual functions
    for(std::vector<TagEntryPtr>::size_type i = 0; i < no_dup_tags.size(); i++) {
        TagEntryPtr tt = no_dup_tags.at(i);

        // Skip c-tors/d-tors
        if(tt->IsDestructor() || tt->IsConstructor()) continue;

        if(info.implAllVirtual && m_mgr->GetTagsManager()->IsVirtual(tt)) {
            tags.push_back(tt);

        } else if(info.implAllPureVirtual && m_mgr->GetTagsManager()->IsPureVirtual(tt)) {
            tags.push_back(tt);
        }
    }

    wxString decl;
    for(std::vector<TagEntryPtr>::size_type i = 0; i < tags.size(); i++) {
        TagEntryPtr tt = tags.at(i);
        wxString ff = m_mgr->GetTagsManager()->FormatFunction(tt);

        if(info.isInline) {
            wxString braces;
            braces << wxT('\n') << separator << wxT("{\n") << separator << wxT("}");
            ff.Replace(wxT(";"), braces);
        }

        decl << separator << ff;
    }
    return decl;
}

void WizardsPlugin::OnGizmos(wxCommandEvent& e)
{
    // open a popup menu
    wxUnusedVar(e);
    wxPoint pt;
    DoPopupButtonMenu(pt);
}

void WizardsPlugin::OnGizmosUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    e.Enable(m_mgr->IsWorkspaceOpen());
}

void WizardsPlugin::GizmosRemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
    std::map<wxString, TagEntryPtr> uniqueSet;
    for(size_t i = 0; i < src.size(); i++) {

        wxString signature = src.at(i)->GetSignature();
        wxString key = m_mgr->GetTagsManager()->NormalizeFunctionSig(signature, 0);
        int hasDefaultValues = signature.Find(wxT("="));

        key.Prepend(src.at(i)->GetName());
        if(uniqueSet.find(key) != uniqueSet.end()) {
            // we already got an instance of this method,
            // incase we have default values in the this Tag, keep this
            // TagEntryPtr, otherwise keep the previous tag
            if(hasDefaultValues != wxNOT_FOUND) { uniqueSet[key] = src.at(i); }

        } else {
            // First time
            uniqueSet[key] = src.at(i);
        }
    }

    // copy the unique set to the output vector
    std::map<wxString, TagEntryPtr>::iterator iter = uniqueSet.begin();
    for(; iter != uniqueSet.end(); iter++) {
        target.push_back(iter->second);
    }
}

void WizardsPlugin::DoPopupButtonMenu(wxPoint pt)
{
#ifdef __WXMSW__
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
#endif

    wxMenu popupMenu;

    std::map<wxString, int> options;
    options[MI_NEW_CODELITE_PLUGIN] = ID_MI_NEW_CODELITE_PLUGIN;
    options[MI_NEW_NEW_CLASS] = ID_MI_NEW_NEW_CLASS;
    options[MI_NEW_WX_PROJECT] = ID_MI_NEW_WX_PROJECT;

    std::map<wxString, int>::iterator iter = options.begin();
    for(; iter != options.end(); iter++) {
        int id = (*iter).second;
        wxString text = (*iter).first;
        wxMenuItem* item = new wxMenuItem(&popupMenu, id, text, text, wxITEM_NORMAL);
        popupMenu.Append(item);
    }
    m_mgr->GetTheApp()->GetTopWindow()->PopupMenu(&popupMenu, pt);
}

#if USE_AUI_TOOLBAR
void WizardsPlugin::OnGizmosAUI(wxAuiToolBarEvent& e)
{
    if(e.IsDropDownClicked()) {
        wxAuiToolBar* tb = static_cast<wxAuiToolBar*>(e.GetEventObject());
        tb->SetToolSticky(e.GetId(), true);

        // line up our menu with the button
        wxRect rect = tb->GetToolRect(e.GetId());
        wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
        pt = m_mgr->GetTheApp()->GetTopWindow()->ScreenToClient(pt);

        DoPopupButtonMenu(pt);
        tb->SetToolSticky(e.GetId(), false);
    }
}
#endif
void WizardsPlugin::OnFolderContentMenu(clContextMenuEvent& event)
{
    event.Skip();
    if(clFileSystemWorkspace::Get().IsOpen() || clCxxWorkspaceST::Get()->IsOpen()) {
        auto menu = event.GetMenu();
        menu->Append(ID_MI_NEW_NEW_CLASS, _("New C++ Class"));
    }
}
