//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : newprojectdlg.cpp              
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
 #include "newprojectdlg.h"
#include "globals.h"
#include "macros.h"
#include "workspace.h"
#include "build_settings_config.h"
#include "manager.h"
#include "dirtraverser.h"

NewProjectDlg::NewProjectDlg( wxWindow* parent )
		:
		NewProjectBaseDlg( parent )
{
	//get list of project templates
	GetProjectTemplateList(m_list);
	std::list<ProjectPtr>::iterator iter = m_list.begin();
	for (; iter != m_list.end(); iter++) {
		wxString n = (*iter)->GetName();
		m_projTypes->Append((*iter)->GetName());
	}

	iter = m_list.begin();
	if ( iter != m_list.end() ) {
		m_projTypes->SetStringSelection((*iter)->GetName());
		m_projectData.m_srcProject = (*iter);
		
		wxString desc = m_projectData.m_srcProject->GetDescription();
		desc = desc.Trim().Trim(false);
		desc.Replace(wxT("\t"), wxT(" "));
		m_textCtrlDescription->SetValue(desc);
	}

	//append list of compilers
	wxArrayString choices;
	//get list of compilers from configuration file
	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	while (cmp) {
		choices.Add(cmp->GetName());
		cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
	}

	m_choiceCompilerType->Append( choices );
	if (choices.IsEmpty() == false) {
		m_choiceCompilerType->SetSelection(0);
	}
	
	m_textCtrlProjectPath->SetValue( WorkspaceST::Get()->GetWorkspaceFileName().GetPath());
	m_textCtrlProjName->SetFocus();
	Centre();
}

void NewProjectDlg::GetProjectTemplateList ( std::list<ProjectPtr> &list )
{
	wxString tmplateDir = ManagerST::Get()->GetStarupDirectory() + PATH_SEP + wxT ( "templates/projects" );

	//read all files under this directory
	DirTraverser dt ( wxT ( "*.project" ) );

	wxDir dir ( tmplateDir );
	dir.Traverse ( dt );

	wxArrayString &files = dt.GetFiles();

	if ( files.GetCount() > 0 ) {
		for ( size_t i=0; i<files.GetCount(); i++ ) {
			ProjectPtr proj ( new Project() );
			if ( !proj->Load ( files.Item ( i ) ) ) {
				//corrupted xml file?
				wxLogMessage ( wxT ( "Failed to load template project: " ) + files.Item ( i ) + wxT ( " (corrupted XML?)" ) );
				continue;
			}
			list.push_back ( proj );
		}
	} else {
		//if we ended up here, it means the installation got screwed up since
		//there should be at least 8 project templates !
		//create 3 default empty projects
		ProjectPtr exeProj ( new Project() );
		ProjectPtr libProj ( new Project() );
		ProjectPtr dllProj ( new Project() );
		libProj->Create ( wxT ( "Static Library" ), wxEmptyString, tmplateDir, Project::STATIC_LIBRARY );
		dllProj->Create ( wxT ( "Dynamic Library" ), wxEmptyString, tmplateDir, Project::DYNAMIC_LIBRARY );
		exeProj->Create ( wxT ( "Executable" ), wxEmptyString, tmplateDir, Project::EXECUTABLE );
		list.push_back ( libProj );
		list.push_back ( dllProj );
		list.push_back ( exeProj );
	}
}

void NewProjectDlg::OnProjectPathUpdated( wxCommandEvent& event )
{
	wxString projectPath;
	projectPath << m_textCtrlProjectPath->GetValue();

	projectPath = projectPath.Trim().Trim(false);

	wxString tmpSep( wxFileName::GetPathSeparator() );
	if ( !projectPath.EndsWith(tmpSep) && projectPath.IsEmpty() == false ) {
		projectPath << wxFileName::GetPathSeparator();
	}
	
	if( m_textCtrlProjName->GetValue().Trim().Trim(false).IsEmpty() ) {
		m_staticTextProjectFileFullPath->SetLabel(wxEmptyString);
		return;
	}
	
	if( m_checkBoxCreateSeparateDir->IsChecked()) {
		//append the workspace name 
		projectPath << m_textCtrlProjName->GetValue();
		projectPath << wxFileName::GetPathSeparator();
	}
	
	projectPath << m_textCtrlProjName->GetValue();
	projectPath << wxT(".project");

	m_staticTextProjectFileFullPath->SetLabel( projectPath );
}

void NewProjectDlg::OnProjectPathPicker( wxCommandEvent& event )
{
	const wxString& dir = wxDirSelector(wxT("Choose a folder:"), WorkspaceST::Get()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
	if ( !dir.empty() ) {
		m_textCtrlProjectPath->SetValue( dir );
	}
}

void NewProjectDlg::OnButtonCreate(wxCommandEvent &e)
{
	//validate that the path part is valid
	wxString projFullPath = m_staticTextProjectFileFullPath->GetLabel();
	wxFileName fn(projFullPath);
	
	if(m_checkBoxCreateSeparateDir->IsChecked()){
		// dont check the return
		Mkdir(fn.GetPath());
	}
	
	// dont allow whitespace in project name
	if(m_textCtrlProjName->GetValue().Find(wxT(" ")) != wxNOT_FOUND){
		wxMessageBox(_("Whitespace is not allowed in project name"), wxT("Error"), wxOK | wxICON_HAND | wxCENTER, this);
		return;
	}
	
	if ( !wxDirExists(fn.GetPath()) ) {
		wxMessageBox(_("Invalid path: ") + fn.GetPath(), wxT("Error"), wxOK | wxICON_HAND);
		return;
	}
	
	// make sure that there is no conflict in files between the template project and the selected path
	if(m_projectData.m_srcProject) {
		ProjectPtr p = m_projectData.m_srcProject;
		wxString base_dir( fn.GetPath() );
		std::vector<wxFileName> files;
		p->GetFiles(files);
		
		for(size_t i=0; i<files.size(); i++){
			wxFileName f = files.at(i);
			wxString new_file = base_dir + wxT("/") + f.GetFullName();
			
			if( wxFileName::FileExists(new_file) ) {
				// this file already - notify the user
				wxString msg;
				msg << wxT("The File '") << f.GetFullName() << wxT("' already exists at the target directory '") << base_dir << wxT("'\n");
				msg << wxT("Please select a different project path\n");
				msg << wxT("The file '") << f.GetFullName() << wxT("' is part of the template project [") << p->GetName() << wxT("]");
				wxMessageBox(msg, wxT("CodeLite"), wxOK|wxICON_HAND);
				return;
			}
		}
	}
	
	m_projectData.m_name = m_textCtrlProjName->GetValue();
	m_projectData.m_path = fn.GetPath();
	m_projectData.m_cmpType = m_choiceCompilerType->GetStringSelection();

	EndModal(wxID_OK);
}

void NewProjectDlg::OnItemSelected( wxCommandEvent& event )
{
	m_projectData.m_srcProject = FindProject( event.GetString() );
	
	//update the description
	if( m_projectData.m_srcProject) {
		wxString desc = m_projectData.m_srcProject->GetDescription();
		desc = desc.Trim().Trim(false);
		desc.Replace(wxT("\t"), wxT(" "));
		m_textCtrlDescription->SetValue( desc ); 
	}
}

ProjectPtr NewProjectDlg::FindProject(const wxString &name)
{
	std::list<ProjectPtr>::iterator iter = m_list.begin();
	for (; iter != m_list.end(); iter++) {
		if ((*iter)->GetName() == name) {
			return (*iter);
		}
	}
	return NULL;
}
