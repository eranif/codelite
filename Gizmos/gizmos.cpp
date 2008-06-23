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
 #include "gizmos.h"
#include "ctags_manager.h"
#include "newplugindata.h"
#include "entry.h"
#include <wx/xrc/xmlres.h>
#include "pluginwizard.h"
#include "globals.h"
#include "dirsaver.h"
#include "workspace.h"
#include "wx/ffile.h"
#include "newclassdlg.h"
#include "newwxprojectdlg.h"
#include <algorithm>

static GizmosPlugin* theGismos = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (theGismos == 0) {
		theGismos = new GizmosPlugin(manager);
	}
	return theGismos;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("Gizmos"));
	info.SetDescription(wxT("Gizmos Plugin - a collection of useful wizards for C++:\nnew Class Wizard, new wxWidgets Wizard, new Plugin Wizard"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

/// Ascending sorting function
struct ascendingSortOp {
	bool operator()(const TagEntryPtr &rStart, const TagEntryPtr &rEnd) {
		return rEnd->GetName().Cmp(rStart->GetName()) > 0;
	}
};

static void GizmosRemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
	for (size_t i=0; i<src.size(); i++) {
		if (i == 0) {
			target.push_back(src.at(0));
		} else {
			if (src.at(i)->GetName() != target.at(target.size()-1)->GetName()) {
				target.push_back(src.at(i));
			}
		}
	}
}

//-------------------------------------
// helper methods
//-------------------------------------
static void ExpandVariables(wxString &content, const NewWxProjectInfo &info)
{
	content.Replace(wxT("$(ProjectName)"), info.GetName());
	wxString projname = info.GetName();
	projname.MakeLower();

	wxString appfilename = projname + wxT("_app");
	wxString framefilename = projname + wxT("_frame");
	content.Replace(wxT("$(MainFrameFile)"), framefilename);
	content.Replace(wxT("$(AppFile)"), appfilename);
	content.Replace(wxT("$(Unicode)"), info.GetFlags() & wxWidgetsUnicode ? wxT("yes") : wxT("no"));
	content.Replace(wxT("$(MWindowsFlag)"), info.GetFlags() & wxWidgetsSetMWindows ? wxT("-mwindows") : wxEmptyString);
	content.Replace(wxT("$(MainFile)"), projname);

	//create the application class name
	wxString initial = appfilename.Mid(0, 1);
	initial.MakeUpper();
	appfilename.SetChar(0, initial.GetChar(0));

	//create the main frame class name
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

static void WriteFile(const wxString &fileName, const wxString &content)
{
	wxFFile file;
	if (!file.Open(fileName, wxT("w+b"))) {
		return;
	}

	file.Write(content);
	file.Close();
}

GizmosPlugin::GizmosPlugin(IManager *manager)
		: IPlugin(manager)
{
	m_longName = wxT("Gizmos Plugin - a collection of useful utils for C++");
	m_shortName = wxT("Gizmos");
}

GizmosPlugin::~GizmosPlugin()
{
}

wxToolBar *GizmosPlugin::CreateToolBar(wxWindow *parent)
{
	//support both toolbars icon size
	int size = m_mgr->GetToolbarIconSize();

	wxToolBar *tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(size, size));

	if (size == 24) {
		tb->AddTool(XRCID("new_plugin"), wxT("New CodeLite Plugin Project"), wxXmlResource::Get()->LoadBitmap(wxT("plugin24")), wxT("New Plugin Wizard..."));
		tb->AddTool(XRCID("new_class"), wxT("Create New Class"), wxXmlResource::Get()->LoadBitmap(wxT("class24")), wxT("New Class..."));
		tb->AddTool(XRCID("new_wx_project"), wxT("New wxWidget Project"), wxXmlResource::Get()->LoadBitmap(wxT("new_wx_project24")), wxT("New wxWidget Project"));
	} else {
		tb->AddTool(XRCID("new_plugin"), wxT("New CodeLite Plugin Project"), wxXmlResource::Get()->LoadBitmap(wxT("plugin16")), wxT("New Plugin Wizard..."));
		tb->AddTool(XRCID("new_class"), wxT("Create New Class"), wxXmlResource::Get()->LoadBitmap(wxT("class16")), wxT("New Class..."));
		tb->AddTool(XRCID("new_wx_project"), wxT("New wxWidget Project"), wxXmlResource::Get()->LoadBitmap(wxT("new_wx_project16")), wxT("New wxWidget Project"));
	}
	tb->Realize();

	//Connect the events to us
	parent->Connect(XRCID("new_plugin"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GizmosPlugin::OnNewPlugin), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("new_plugin"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GizmosPlugin::OnNewPluginUI), NULL, (wxEvtHandler*)this);


	parent->Connect(XRCID("new_class"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GizmosPlugin::OnNewClass), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("new_class"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GizmosPlugin::OnNewClassUI), NULL, (wxEvtHandler*)this);

	parent->Connect(XRCID("new_wx_project"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GizmosPlugin::OnNewWxProject), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("new_wx_project"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GizmosPlugin::OnNewWxProjectUI), NULL, (wxEvtHandler*)this);
	return tb;
}

void GizmosPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	//TODO:: create the menu for the 'Plugin' menu entry in the
	//menu bar
}

void GizmosPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeFileView_Folder) {
		//Create the popup menu for the virtual folders
		wxMenuItem *item(NULL);

		item = new wxMenuItem(menu, wxID_SEPARATOR);
		menu->Prepend(item);
		m_vdDynItems.push_back(item);

		item = new wxMenuItem(menu, XRCID("new_class"), wxT("&New Class..."), wxEmptyString, wxITEM_NORMAL);
		menu->Prepend(item);
		m_vdDynItems.push_back(item);
	}
}

void GizmosPlugin::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeFileView_Folder) {
		std::vector<wxMenuItem*>::iterator iter = m_vdDynItems.begin();
		for (; iter != m_vdDynItems.end(); iter++) {
			menu->Destroy(*iter);
		}
		m_vdDynItems.clear();
	}
}

void GizmosPlugin::UnPlug()
{
	//TODO:: perform the unplug action of this plugin
}

void GizmosPlugin::OnNewPlugin(wxCommandEvent &e)
{
	wxUnusedVar(e);
	//Load the wizard
	PluginWizard *wiz = new PluginWizard(NULL, wxID_ANY);
	NewPluginData data;
	if (wiz->Run(data)) {
		//load the template file and replace all variables with the
		//actual values provided by user
		wxString filename(m_mgr->GetStartupDirectory() + wxT("/templates/gizmos/liteeditor-plugin.project.wizard"));
		wxString content;
		if (!ReadFileWithConversion(filename, content)) {
			return;
		}
		//convert the paths provided by user to relative paths
		DirSaver ds;
		wxSetWorkingDirectory(data.GetProjectPath());
		wxFileName fn(data.GetCodelitePath());

		if (!fn.MakeRelativeTo(wxGetCwd())) {
			wxLogMessage(wxT("Warning: Failed to convert paths to relative path."));
		}
#ifdef __WXMSW__
		wxString dllExt(wxT("dll"));
#else
		wxString dllExt(wxT("so"));
#endif
		wxString clpath = fn.GetFullPath();

		content.Replace(wxT("$(CodeLitePath)"), clpath);
		content.Replace(wxT("$(DllExt)"), dllExt);
		content.Replace(wxT("$(PluginName)"), data.GetPluginName());
		wxString baseFileName = data.GetPluginName();
		baseFileName.MakeLower();
		content.Replace(wxT("$(BaseFileName)"), baseFileName);
		content.Replace(wxT("$(ProjectName)"), data.GetPluginName());

		//save the file to the disk
		wxString projectFileName;
		projectFileName << data.GetProjectPath() << wxT("/") << data.GetPluginName() << wxT(".project");
		wxFFile file;
		if (!file.Open(projectFileName, wxT("w+b"))) {
			return;
		}

		file.Write(content);
		file.Close();

		//Create the plugin source and header files
		wxString srcFile(baseFileName + wxT(".cpp"));
		wxString headerFile(baseFileName + wxT(".h"));

		//---------------------------------------------------------------
		//write the content of the file based on the file template
		//---------------------------------------------------------------

		//Generate the source files
		filename = m_mgr->GetStartupDirectory() + wxT("/templates/gizmos/plugin.cpp.wizard");
		content.Clear();
		if (!ReadFileWithConversion(filename, content)) {
			wxMessageBox(wxT("Failed to load wizard's file 'plugin.cpp.wizard'"), wxT("CodeLite"), wxICON_WARNING | wxOK);
			return;
		}

		// Expand macros
		content.Replace(wxT("$(PluginName)"), data.GetPluginName());
		content.Replace(wxT("$(BaseFileName)"), baseFileName);
		content.Replace(wxT("$(PluginShortName)"), data.GetPluginName());
		content.Replace(wxT("$(PluginLongName)"), data.GetPluginDescription());

		file.Open(srcFile, wxT("w+b"));
		file.Write(content);
		file.Close();

		//create the header file
		filename = m_mgr->GetStartupDirectory() + wxT("/templates/gizmos/plugin.h.wizard");
		content.Clear();
		if (!ReadFileWithConversion(filename, content)) {
			wxMessageBox(wxT("Failed to load wizard's file 'plugin.h.wizard'"), wxT("CodeLite"), wxICON_WARNING | wxOK);
			return;
		}

		// Expand macros
		content.Replace(wxT("$(PluginName)"), data.GetPluginName());
		content.Replace(wxT("$(BaseFileName)"), baseFileName);
		content.Replace(wxT("$(PluginShortName)"), data.GetPluginName());
		content.Replace(wxT("$(PluginLongName)"), data.GetPluginDescription());

		file.Open(headerFile, wxT("w+b"));
		file.Write(content);
		file.Close();

		//add the new project to the workspace
		wxString errMsg;

		//convert the path to be full path as required by the
		//workspace manager
		m_mgr->AddProject(projectFileName);
	}
	wiz->Destroy();
}

void GizmosPlugin::OnNewPluginUI(wxUpdateUIEvent &e)
{
	//we enable the button only when workspace is opened
	e.Enable(m_mgr->IsWorkspaceOpen());
}

void GizmosPlugin::OnNewClassUI(wxUpdateUIEvent &e)
{
	//we enable the button only when workspace is opened
	e.Enable(m_mgr->IsWorkspaceOpen());
}

void GizmosPlugin::OnNewClass(wxCommandEvent &e)
{
	NewClassDlg *dlg = new NewClassDlg(NULL, m_mgr);
	if (dlg->ShowModal() == wxID_OK) {
		//do something with the information here
		NewClassInfo info;
		dlg->GetNewClassInfo(info);

		CreateClass(info);
	}
	dlg->Destroy();
}

void GizmosPlugin::CreateClass(const NewClassInfo &info)
{
	wxString macro(info.name);
	macro.MakeLower();

	//create cpp + h file
	wxString cpp;
	wxString header;

	//----------------------------------------------------
	// header file
	//----------------------------------------------------
	header << wxT("#ifndef __") << macro << wxT("__\n");
	header << wxT("#define __") << macro << wxT("__\n");
	header << wxT("\n");
	header << wxT("class ") << info.name;

	if (info.parents.empty() == false) {
		header << wxT(" : ");
		for (size_t i=0; i< info.parents.size(); i++) {
			ClassParentInfo pi = info.parents.at(i);
			header << pi.access << wxT(" ") << pi.name << wxT(", ");
		}
		header = header.BeforeLast(wxT(','));
	}
	header << wxT(" {\n\n");

	if (info.isSingleton) {
		header << wxT("\tstatic ") << info.name << wxT("* ms_instance;\n\n");
	}

	if (info.isAssingable == false) {
		//declare copy constructor & assingment operator as private
		header << wxT("private:\n");
		header << wxT("\t") << info.name << wxT("(const ") << info.name << wxT("& rhs);\n");
		header << wxT("\t") << info.name << wxT("& operator=(const ") << info.name << wxT("& rhs);\n");
		header << wxT("\n");
	}

	if (info.isSingleton) {
		header << wxT("public:\n");
		header << wxT("\tstatic ") << info.name << wxT("* Instance();\n");
		header << wxT("\tstatic void Release();\n\n");

		header << wxT("private:\n");
		header << wxT("\t") << info.name << wxT("();\n");

		if (info.isVirtualDtor) {
			header << wxT("\tvirtual ~") << info.name << wxT("();\n\n");
		} else {
			header << wxT("\t~") << info.name << wxT("();\n\n");
		}
	} else {
		header << wxT("public:\n");
		header << wxT("\t") << info.name << wxT("();\n");
		if (info.isVirtualDtor) {
			header << wxT("\tvirtual ~") << info.name << wxT("();\n\n");
		} else {
			header << wxT("\t~") << info.name << wxT("();\n\n");
		}

	}

	//add virtual function declaration
	wxString v_decl = DoGetVirtualFuncDecl(info);
	if (v_decl.IsEmpty() == false) {
		header << wxT("public:\n");
		header << v_decl;
	}

	header << wxT("};\n");
	header << wxT("#endif // __") << macro << wxT("__\n");

	//----------------------------------------------------
	// source file
	//----------------------------------------------------
	cpp << wxT("#include \"") << info.fileName << wxT(".h\"\n");
	if (info.isSingleton) {
		cpp << info.name << wxT("* ") << info.name << wxT("::ms_instance = 0;\n\n");
	}
	//ctor/dtor
	cpp << info.name << wxT("::") << info.name << wxT("()\n");
	cpp << wxT("{\n}\n\n");
	cpp << info.name << wxT("::~") << info.name << wxT("()\n");
	cpp << wxT("{\n}\n\n");
	if (info.isSingleton) {
		cpp << info.name << wxT("* ") << info.name << wxT("::Instance()\n");
		cpp << wxT("{\n");
		cpp << wxT("\tif(ms_instance == 0){\n");
		cpp << wxT("\t\tms_instance = new ") << info.name << wxT("();\n");
		cpp << wxT("\t}\n");
		cpp << wxT("\treturn ms_instance;\n");
		cpp << wxT("}\n\n");

		cpp << wxT("void ") << info.name << wxT("::Release()\n");
		cpp << wxT("{\n");
		cpp << wxT("\tif(ms_instance){\n");
		cpp << wxT("\t\tdelete ms_instance;\n");
		cpp << wxT("\t}\n");
		cpp << wxT("\tms_instance = 0;\n");
		cpp << wxT("}\n\n");
	}

	cpp << DoGetVirtualFuncImpl(info);

	wxFFile file;
	wxString srcFile;
	wxString hdrFile;
	srcFile << info.path << wxFileName::GetPathSeparator() << info.fileName << wxT(".cpp");
	hdrFile << info.path << wxFileName::GetPathSeparator() << info.fileName << wxT(".h");

	file.Open(srcFile, wxT("w+b"));
	file.Write(cpp);
	file.Close();

	file.Open(hdrFile, wxT("w+b"));
	file.Write(header);
	file.Close();

	//if we have a selected virtual folder, add the files to it
	wxArrayString paths;
	paths.Add(srcFile);
	paths.Add(hdrFile);

	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
	if (item.m_item.IsOk()) {
		if (m_mgr->AddFilesToVirtualFodler(item.m_item, paths) == false) {
			//probably not a virtual folder
			wxString msg;
			msg << wxT("CodeLite created the class successfully, but was unable to add the generated files to any virtual folder (since non was selected)\n");
			msg << wxT("You can right click on virtual folder (in the 'Files' tab) and manually add them\n");
			msg << wxT("To avoid this error in the future, select the target virtual folder before creating the class");
			wxMessageBox(msg, wxT("CodeLite"), wxOK|wxICON_INFORMATION);
		}
	}
}

void GizmosPlugin::OnNewWxProject(wxCommandEvent &e)
{
	NewWxProjectDlg *dlg = new NewWxProjectDlg(NULL, m_mgr);
	if (dlg->ShowModal() == wxID_OK) {
		//Create the project
		NewWxProjectInfo info;
		dlg->GetProjectInfo(info);
		CreateWxProject(info);
	}
	dlg->Destroy();
}

void GizmosPlugin::OnNewWxProjectUI(wxUpdateUIEvent &e)
{
	//we enable the button only when workspace is opened
	e.Enable(m_mgr->IsWorkspaceOpen());
}

void GizmosPlugin::CreateWxProject(NewWxProjectInfo &info)
{
	//TODO:: Implement this ...
	//we first create the project files
	if (info.GetType() == wxProjectTypeGUI) {

		//we are creating a project of type GUI
		wxString basedir = m_mgr->GetStartupDirectory();

		wxString projectConent;
		wxString mainFrameCppContent;
		wxString mainFrameHContent;
		wxString appCppConent;
		wxString apphConent;

		if (!ReadFileWithConversion(basedir + wxT("/templates/gizmos/wxproject.project.wizard"), projectConent)) {
			return;
		}
		if (!ReadFileWithConversion(basedir + wxT("/templates/gizmos/mainframe.cpp.wizard"), mainFrameCppContent)) {
			return;
		}
		if (!ReadFileWithConversion(basedir + wxT("/templates/gizmos/mainframe.h.wizard"), mainFrameHContent)) {
			return;
		}
		if (!ReadFileWithConversion(basedir + wxT("/templates/gizmos/app.h.wizard"), apphConent)) {
			return;
		}
		if (!ReadFileWithConversion(basedir + wxT("/templates/gizmos/app.cpp.wizard"), appCppConent)) {
			return;
		}

		ExpandVariables(projectConent, info);
		ExpandVariables(mainFrameCppContent, info);
		ExpandVariables(mainFrameHContent, info);
		ExpandVariables(apphConent, info);
		ExpandVariables(appCppConent, info);

		//Write the files content into the project directory
		DirSaver ds;
		wxSetWorkingDirectory(info.GetPath());

		wxString projname = info.GetName();
		projname.MakeLower();

		wxString appfilename = projname + wxT("_app");
		wxString framefilename = projname + wxT("_frame");

		WriteFile(framefilename + wxT(".cpp"), mainFrameCppContent);
		WriteFile(framefilename + wxT(".h"), mainFrameHContent);
		WriteFile(appfilename + wxT(".h"), apphConent);
		WriteFile(appfilename+ wxT(".cpp"), appCppConent);
		WriteFile(info.GetName() + wxT(".project"), projectConent);

		//If every this is OK, add the project as well
		m_mgr->AddProject(info.GetName() + wxT(".project"));
	} else if (info.GetType() == wxProjectTypeSimpleMain) {

		//we are creating a project of type GUI
		wxString basedir = m_mgr->GetStartupDirectory();

		wxString projectConent;
		wxString mainFrameCppContent;
		wxString mainFrameHContent;
		wxString appCppConent;
		wxString apphConent;

		if (!ReadFileWithConversion(basedir + wxT("/templates/gizmos/wxmain.project.wizard"), projectConent)) {
			return;
		}
		if (!ReadFileWithConversion(basedir + wxT("/templates/gizmos/main.cpp.wizard"), appCppConent)) {
			return;
		}

		ExpandVariables(projectConent, info);
		ExpandVariables(appCppConent, info);

		//Write the files content into the project directory
		DirSaver ds;
		wxSetWorkingDirectory(info.GetPath());

		wxString projname = info.GetName();
		projname.MakeLower();

		wxString appfilename = projname;
		WriteFile(appfilename+ wxT(".cpp"), appCppConent);
		WriteFile(info.GetName() + wxT(".project"), projectConent);

		//If every this is OK, add the project as well
		m_mgr->AddProject(info.GetName() + wxT(".project"));
	}
}

wxString GizmosPlugin::DoGetVirtualFuncImpl(const NewClassInfo &info)
{
	if (info.implAllVirtual == false && info.implAllPureVirtual == false)
		return wxEmptyString;

	//get list of all parent virtual functions
	std::vector< TagEntryPtr > tmp_tags;
	std::vector< TagEntryPtr > no_dup_tags;
	std::vector< TagEntryPtr > tags;
	for (std::vector< TagEntryPtr >::size_type i=0; i< info.parents.size(); i++) {
		ClassParentInfo pi = info.parents.at(i);
		m_mgr->GetTagsManager()->TagsByScope(pi.name, tmp_tags);
	}
	// and finally sort the results
	std::sort(tmp_tags.begin(), tmp_tags.end(), ascendingSortOp());
	GizmosRemoveDuplicates(tmp_tags, no_dup_tags);

	//filter out all non virtual functions
	for (std::vector< TagEntryPtr >::size_type i=0; i< no_dup_tags.size(); i++) {
		TagEntryPtr tt = no_dup_tags.at(i);
		bool collect(false);
		if (info.implAllVirtual) {
			collect = m_mgr->GetTagsManager()->IsVirtual(tt);
		} else if (info.implAllPureVirtual) {
			collect = m_mgr->GetTagsManager()->IsPureVirtual(tt);
		}

		if (collect) {
			tags.push_back(tt);
		}
	}

	wxString impl;
	for (std::vector< TagEntryPtr >::size_type i=0; i< tags.size(); i++) {
		TagEntryPtr tt = tags.at(i);
		impl << m_mgr->GetTagsManager()->FormatFunction(tt, true, info.name);
	}
	return impl;
}

wxString GizmosPlugin::DoGetVirtualFuncDecl(const NewClassInfo &info)
{
	if (info.implAllVirtual == false && info.implAllPureVirtual == false)
		return wxEmptyString;

	//get list of all parent virtual functions
	std::vector< TagEntryPtr > tmp_tags;
	std::vector< TagEntryPtr > no_dup_tags;
	std::vector< TagEntryPtr > tags;
	for (std::vector< TagEntryPtr >::size_type i=0; i< info.parents.size(); i++) {
		ClassParentInfo pi = info.parents.at(i);
		m_mgr->GetTagsManager()->TagsByScope(pi.name, tmp_tags);
	}

	// and finally sort the results
	std::sort(tmp_tags.begin(), tmp_tags.end(), ascendingSortOp());
	GizmosRemoveDuplicates(tmp_tags, no_dup_tags);

	//filter out all non virtual functions
	for (std::vector< TagEntryPtr >::size_type i=0; i< no_dup_tags.size(); i++) {
		TagEntryPtr tt = no_dup_tags.at(i);
		bool collect(false);
		if (info.implAllVirtual) {
			collect = m_mgr->GetTagsManager()->IsVirtual(tt);
		} else if (info.implAllPureVirtual) {
			collect = m_mgr->GetTagsManager()->IsPureVirtual(tt);
		}

		if (collect) {
			tags.push_back(tt);
		}
	}

	wxString decl;
	for (std::vector< TagEntryPtr >::size_type i=0; i< tags.size(); i++) {
		TagEntryPtr tt = tags.at(i);
		decl << wxT("\t") << m_mgr->GetTagsManager()->FormatFunction(tt);
	}
	return decl;
}
