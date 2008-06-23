//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : copyright.cpp              
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
 #include <wx/filefn.h>
#include <wx/progdlg.h>
#include <wx/ffile.h>
#include <wx/tokenzr.h>
#include "workspace.h"
#include "copyrights_proj_sel_dlg.h"
#include <wx/app.h>
#include "copyrights_options_dlg.h"
#include "cppwordscanner.h"
#include "globals.h"
#include <wx/msgdlg.h>
#include "cpptoken.h"
#include "copyright.h"
#include <wx/xrc/xmlres.h>
#include <wx/menu.h>
#include <vector>
#include "project.h"

#include "copyrightsconfigdata.h"
#include "globals.h"

static Copyright* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new Copyright(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxGetUserName());
	info.SetName(wxT("Copyright"));
	info.SetDescription(wxT("Copyright Plugin - a small plugin that allows you to place copyright block on top of your source files"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

Copyright::Copyright(IManager *manager)
		: IPlugin(manager)
		, m_topWin(NULL)
		, m_sepItem(NULL)
		, m_projectSepItem(NULL)
		, m_workspaceSepItem(NULL)
{
	m_longName = wxT("Copyright Plugin - a small plugin that allows you to place copyright block on top of your source files");
	m_shortName = wxT("Copyright");
	m_topWin = wxTheApp;
}

Copyright::~Copyright()
{
	m_topWin->Disconnect(XRCID("copyrights_options"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Copyright::OnOptions), NULL, this);
	m_topWin->Disconnect(XRCID("insert_copyrights"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Copyright::OnInsertCopyrights), NULL, this);
	m_topWin->Disconnect(XRCID("batch_insert_copyrights"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Copyright::OnInsertCopyrights), NULL, this);
	m_topWin->Disconnect(XRCID("insert_prj_copyrights"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Copyright::OnProjectInsertCopyrights), NULL, this);
}

wxToolBar *Copyright::CreateToolBar(wxWindow *parent)
{
	// we dont need a toolbar for this plugin
	return NULL;
}

void Copyright::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("insert_copyrights"), wxT("Insert Copyright Block"), wxT("Insert Copyright Block"), wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("batch_insert_copyrights"), wxT("Batch Insert of Copyright Block"), wxT("Batch Insert of Copyright Block"), wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("copyrights_options"), wxT("Settings..."), wxEmptyString, wxITEM_NORMAL);

	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, wxT("Copyrights"), menu);

	// connect events
	m_topWin->Connect(XRCID("copyrights_options"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Copyright::OnOptions), NULL, this);
	m_topWin->Connect(XRCID("insert_copyrights"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Copyright::OnInsertCopyrights), NULL, this);
	m_topWin->Connect(XRCID("batch_insert_copyrights"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Copyright::OnBatchInsertCopyrights), NULL, this);
	m_topWin->Connect(XRCID("insert_prj_copyrights"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Copyright::OnProjectInsertCopyrights), NULL, this);
}

void Copyright::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {

		if ( !m_sepItem ) {
			m_sepItem = menu->AppendSeparator();
		}

		if (!menu->FindItem(XRCID("insert_copyrights"))) {
			menu->Append(XRCID("insert_copyrights"), wxT("Insert Copyright Block"), wxEmptyString);
		}

	} else if (type == MenuTypeFileExplorer) {

	} else if (type == MenuTypeFileView_Workspace) {
		// Workspace menu
		if ( !m_workspaceSepItem ) {
			m_workspaceSepItem = menu->PrependSeparator();
		}

		if (!menu->FindItem(XRCID("batch_insert_copyrights"))) {
			menu->Prepend(XRCID("batch_insert_copyrights"), wxT("Batch Insert of Copyright Block"), wxEmptyString);
		}
		
	} else if (type == MenuTypeFileView_Project) {
		if ( !m_projectSepItem ) {
			m_projectSepItem = menu->PrependSeparator();
		}

		if (!menu->FindItem(XRCID("insert_prj_copyrights"))) {
			menu->Prepend(XRCID("insert_prj_copyrights"), wxT("Insert Copyright Block"), wxEmptyString);
		}

	} else if (type == MenuTypeFileView_Folder) {
		//TODO::Append items for the file view/Virtual folder context menu
	} else if (type == MenuTypeFileView_File) {
		//TODO::Append items for the file view/file context menu
	}
}

void Copyright::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		wxMenuItem *item = menu->FindItem(XRCID("insert_copyrights"));
		if (item) {
			menu->Destroy( item );
		}

		if (m_sepItem) {
			menu->Destroy( m_sepItem );
			m_sepItem = NULL;
		}

	} else if (type == MenuTypeFileExplorer) {
		//TODO::Unhook  items for the file explorer context menu
	} else if (type == MenuTypeFileView_Workspace) {
		
		wxMenuItem *item = menu->FindItem(XRCID("batch_insert_copyrights"));
		if (item) {
			menu->Destroy( item );
		}

		if (m_workspaceSepItem) {
			menu->Destroy( m_workspaceSepItem );
			m_workspaceSepItem = NULL;
		}
		
	} else if (type == MenuTypeFileView_Project) {
		wxMenuItem *item = menu->FindItem(XRCID("insert_prj_copyrights"));
		if (item) {
			menu->Destroy( item );
		}

		if (m_projectSepItem) {
			menu->Destroy( m_projectSepItem );
			m_projectSepItem = NULL;
		}
	} else if (type == MenuTypeFileView_Folder) {
		//TODO::Unhook  items for the file view/Virtual folder context menu
	} else if (type == MenuTypeFileView_File) {
		//TODO::Unhook  items for the file view/file context menu
	}
}

void Copyright::UnPlug()
{
	//TODO:: perform the unplug action for this plugin
}

void Copyright::OnInsertCopyrights(wxCommandEvent& e)
{
	wxUnusedVar( e );

	// read configuration
	CopyrightsConfigData data;
	m_mgr->GetConfigTool()->ReadObject(wxT("CopyrightsConfig"), &data);

	// make sure that the template file exists
	if ( !wxFileName::FileExists(data.GetTemplateFilename()) ) {
		wxMessageBox(wxString::Format(wxT("Template file name '%s', does not exist!"), data.GetTemplateFilename().GetData()), wxT("CodeLite"), wxICON_WARNING|wxOK);
		return;
	}

	// read the copyrights file
	wxString content;
	if ( !ReadFileWithConversion(data.GetTemplateFilename(), content) ) {
		wxMessageBox(wxString::Format(wxT("Failed to read template file '%s'"), data.GetTemplateFilename().c_str()), wxT("CodeLite"), wxICON_WARNING|wxOK);
		return;
	}

	IEditor *editor = m_mgr->GetActiveEditor();
	if ( !editor ) {
		wxMessageBox(wxString::Format(wxT("There is no active editor\n")), wxT("CodeLite"), wxICON_WARNING|wxOK);
		return;
	}

	// verify that the file consist only with comment code
	CppWordScanner scanner( data.GetTemplateFilename() );

	CppTokensMap l;
	scanner.FindAll( l );

	if ( !l.is_empty() ) {
		if ( wxMessageBox(wxT("Template file contains text which is not comment, continue anyways?"), wxT("CodeLite"), wxICON_QUESTION|wxYES_NO) == wxNO ) {
			return;
		}
	}

	// expand constants
	wxString _content = ExpandAllVariables(content, m_mgr->GetWorkspace(), wxEmptyString, editor->GetFileName().GetFullPath());

	// we are good to go :)
	editor->InsertText(0, _content);
}

void Copyright::OnOptions(wxCommandEvent& e)
{
	// pop up the options dialog
	CopyrightsOptionsDlg *dlg = new CopyrightsOptionsDlg(wxTheApp->GetTopWindow(), m_mgr->GetConfigTool());
	dlg->ShowModal();
	dlg->Destroy();
}

void Copyright::OnBatchInsertCopyrights(wxCommandEvent& e)
{
	// pop up the projects selection dialog
	if (m_mgr->IsWorkspaceOpen() == false) {
		wxMessageBox(wxT("Batch insert requires a workspace to be opened"), wxT("CodeLite"), wxICON_WARNING|wxOK);
		return;
	}

	Notebook *book = m_mgr->GetMainNotebook();
	for (size_t i=0; i<book->GetPageCount(); i++) {
		IEditor *editor = dynamic_cast<IEditor*>(book->GetPage(i));
		if (editor && editor->IsModified()) {
			wxMessageBox(wxT("Please save all un-saved documents before continuing"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
			return;
		}
	}

	// read configuration
	CopyrightsConfigData data;
	m_mgr->GetConfigTool()->ReadObject(wxT("CopyrightsConfig"), &data);

	wxString content;
	if ( !Validate(content) ) {
		return;
	}

	CopyrightsProjectSelDlg *dlg = new CopyrightsProjectSelDlg(NULL, m_mgr->GetWorkspace());
	if (dlg->ShowModal() == wxID_OK) {
		wxArrayString projects;
		dlg->GetProjects( projects );
		dlg->Destroy();

		// expand constants
		wxString err_msg;
		std::vector<wxFileName> files;
		std::vector<wxFileName> filtered_files;
		// loop over the project and collect list of files to work with
		for (size_t i=0; i<projects.size(); i++) {
			ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), err_msg);
			if (p) {
				p->GetFiles(files, true);
			}
		}

		wxString mask( data.GetFileMasking() );
		mask.Replace(wxT("*."), wxEmptyString);
		mask = mask.Trim().Trim(false);

		wxArrayString exts = wxStringTokenize(mask, wxT(";"));

		// filter out non-matching files (according to masking)
		for (size_t i=0; i<files.size(); i++) {
			if ( exts.Index(files.at(i).GetExt(), false) != wxNOT_FOUND ) {
				// valid file
				filtered_files.push_back( files.at(i) );
			}
		}
		
		if(filtered_files.empty() == false) {
			MassUpdate(filtered_files, content);
		}

	}
	dlg->Destroy();
}

void Copyright::OnProjectInsertCopyrights(wxCommandEvent& e)
{
	// pop up the projects selection dialog
	if (m_mgr->IsWorkspaceOpen() == false) {
		wxMessageBox(wxT("Batch insert requires a workspace to be opened"), wxT("CodeLite"), wxICON_WARNING|wxOK);
		return;
	}

	Notebook *book = m_mgr->GetMainNotebook();
	for (size_t i=0; i<book->GetPageCount(); i++) {
		IEditor *editor = dynamic_cast<IEditor*>(book->GetPage(i));
		if (editor && editor->IsModified()) {
			wxMessageBox(wxT("Please save all un-saved documents before continuing"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
			return;
		}
	}

	// read configuration
	CopyrightsConfigData data;
	m_mgr->GetConfigTool()->ReadObject(wxT("CopyrightsConfig"), &data);

	wxString content;
	if ( !Validate(content) ) {
		return;
	}

	// get the project to work on
	TreeItemInfo info = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
	wxString project_name = info.m_text;

	wxString err_msg;
	std::vector<wxFileName> files;
	std::vector<wxFileName> filtered_files;
	// loop over the project and collect list of files to work with
	ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(project_name, err_msg);
	if (!p) {
		return;
	}

	p->GetFiles(files, true);

	// filter non matched files
	wxString mask( data.GetFileMasking() );
	mask.Replace(wxT("*."), wxEmptyString);
	mask = mask.Trim().Trim(false);

	wxArrayString exts = wxStringTokenize(mask, wxT(";"));

	// filter out non-matching files (according to masking)
	for (size_t i=0; i<files.size(); i++) {
		if ( exts.Index(files.at(i).GetExt(), false) != wxNOT_FOUND ) {
			// valid file
			filtered_files.push_back( files.at(i) );
		}
	}

	// update files
	if(filtered_files.empty() == false) {
		MassUpdate(filtered_files, content);
	}
}

void Copyright::MassUpdate(const std::vector<wxFileName> &filtered_files, const wxString &content)
{
	// last confirmation from the user
	if (wxMessageBox(wxString::Format(wxT("You are about to modifiy %d files, continue?"), filtered_files.size()), wxT("CodeLite"), wxYES_NO|wxICON_QUESTION) == wxNO) {
		return;
	}
	
	wxProgressDialog* prgDlg = NULL;
	prgDlg = new wxProgressDialog (wxT("Processing file ..."), wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (int)filtered_files.size(), NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE | wxPD_CAN_ABORT );
	prgDlg->GetSizer()->Fit(prgDlg);
	prgDlg->Layout();
	prgDlg->Centre();

	CopyrightsConfigData data;
	m_mgr->GetConfigTool()->ReadObject(wxT("CopyrightsConfig"), &data);

	// now loop over the files and add copyrights block
	for (size_t i=0 ; i< filtered_files.size(); i++) {
		wxFileName fn = filtered_files.at(i);

		wxString file_content;
		wxString _content = ExpandAllVariables(content, m_mgr->GetWorkspace(), wxEmptyString, fn.GetFullPath());
		if (ReadFileWithConversion(fn.GetFullPath(), file_content)) {
			wxString msg;
			msg << wxT("Inserting comment to file: ") << fn.GetFullName();
			if (!prgDlg->Update(i, msg)) {
				prgDlg->Destroy();
				return;
			}

			file_content.Prepend(_content);
			WriteFileWithBackup(fn.GetFullPath(), file_content, data.GetBackupFiles());
		}
	}
	prgDlg->Destroy();
}

bool Copyright::Validate(wxString& content)
{
	CopyrightsConfigData data;
	m_mgr->GetConfigTool()->ReadObject(wxT("CopyrightsConfig"), &data);

	// make sure that the template file exists
	if ( !wxFileName::FileExists(data.GetTemplateFilename()) ) {
		wxMessageBox(wxString::Format(wxT("Template file name '%s', does not exist!"), data.GetTemplateFilename().GetData()), wxT("CodeLite"), wxICON_WARNING|wxOK);
		return false;
	}

	// read the copyrights file
	if ( !ReadFileWithConversion(data.GetTemplateFilename(), content) ) {
		wxMessageBox(wxString::Format(wxT("Failed to read template file '%s'"), data.GetTemplateFilename().c_str()), wxT("CodeLite"), wxICON_WARNING|wxOK);
		return false;
	}

	// verify that the file consist only with comment code
	CppWordScanner scanner( data.GetTemplateFilename() );

	CppTokensMap l;
	scanner.FindAll( l );

	if ( !l.is_empty() ) {
		if ( wxMessageBox(wxT("Template file contains text which is not comment, continue anyways?"), wxT("CodeLite"), wxICON_QUESTION|wxYES_NO) == wxNO ) {
			return false;
		}
	}
	content.Replace(wxT("`"), wxT("'"));
	return true;
}
