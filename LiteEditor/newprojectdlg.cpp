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
#include "pluginmanager.h"
#include "windowattrmanager.h"
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "macros.h"
#include "workspace.h"
#include "build_settings_config.h"
#include "manager.h"
#include "dirtraverser.h"
#include <wx/imaglist.h>
#include <set>

NewProjectDlg::NewProjectDlg( wxWindow* parent )
		:
		NewProjectBaseDlg( parent )
{
	m_listTemplates->InsertColumn( 0, wxT("Type") );
	m_listTemplates->SetColumnWidth( 0, m_listTemplates->GetSize().GetWidth() );

	//get list of project templates
	wxImageList *lstImages (NULL);
	GetProjectTemplateList(PluginManager::Get(), m_list, &m_mapImages, &lstImages);
	
	// assign image list to the list control which takes ownership of it (it will delete the image list)
	m_listTemplates->AssignImageList(lstImages, wxIMAGE_LIST_SMALL);
	
	m_chCategories->Clear();
	std::list<ProjectPtr>::iterator iter = m_list.begin();
	std::set<wxString>              categories;
	
	// Add the 'All' category
	categories.insert(wxT("All"));
	for(; iter != m_list.end(); iter++) {
		wxString internalType = (*iter)->GetProjectInternalType();
		if(internalType.IsEmpty()) internalType = wxT("Others");
		categories.insert( internalType );
	}
	
	std::set<wxString>::iterator cIter = categories.begin();
	for(; cIter != categories.end(); cIter++) {
		m_chCategories->Append((*cIter));
	}
	
	// Select the 'Console' to be the default
	int where = m_chCategories->FindString(wxT("Console"));
	if(where == wxNOT_FOUND) {
		where = 0;
	}
	
	m_chCategories->SetSelection(where);
	FillProjectTemplateListCtrl(m_chCategories->GetStringSelection());

	//append list of compilers
	wxArrayString choices;
	//get list of compilers from configuration file
	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	while (cmp) {
		choices.Add(cmp->GetName());
		cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
	}

	m_chCompiler->Append( choices );
	if (choices.IsEmpty() == false) {
		m_chCompiler->SetSelection(0);
	}

	m_dpProjPath->SetPath( WorkspaceST::Get()->GetWorkspaceFileName().GetPath());
	m_txtProjName->SetFocus();
	Centre();

	UpdateProjectPage();
	WindowAttrManager::Load(this, wxT("NewProjectDialog"), NULL);
}

NewProjectDlg::~NewProjectDlg()
{
	WindowAttrManager::Save(this, wxT("NewProjectDialog"), NULL);
}

void NewProjectDlg::OnProjectPathChanged( wxFileDirPickerEvent& event )
{
	UpdateFullFileName();
}

void NewProjectDlg::OnProjectNameChanged(wxCommandEvent& event)
{
	UpdateFullFileName();
}

void NewProjectDlg::OnCreate(wxCommandEvent &event)
{
	//validate that the path part is valid
	wxString projFullPath = m_stxtFullFileName->GetLabel();
	wxFileName fn(projFullPath);

	if(m_cbSeparateDir->IsChecked()){
		// dont check the return
		Mkdir(fn.GetPath());
	}

	// dont allow whitespace in project name
	if(m_txtProjName->GetValue().Find(wxT(" ")) != wxNOT_FOUND){
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

	m_projectData.m_name = m_txtProjName->GetValue();
	m_projectData.m_path = fn.GetPath();
	m_projectData.m_cmpType = m_chCompiler->GetStringSelection();

	EndModal(wxID_OK);
}

void NewProjectDlg::OnTemplateSelected( wxListEvent& event )
{
	m_projectData.m_srcProject = FindProject( event.GetText() );

	UpdateProjectPage();
}

void NewProjectDlg::OnCategorySelected(wxCommandEvent& event)
{
	FillProjectTemplateListCtrl(event.GetString());

	UpdateProjectPage();
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

void NewProjectDlg::UpdateFullFileName()
{
	wxString projectPath;
	projectPath << m_dpProjPath->GetPath();

	projectPath = projectPath.Trim().Trim(false);

	wxString tmpSep( wxFileName::GetPathSeparator() );
	if ( !projectPath.EndsWith(tmpSep) && projectPath.IsEmpty() == false ) {
		projectPath << wxFileName::GetPathSeparator();
	}

	if( m_txtProjName->GetValue().Trim().Trim(false).IsEmpty() ) {
		m_stxtFullFileName->SetLabel(wxEmptyString);
		return;
	}

	if( m_cbSeparateDir->IsChecked()) {
		//append the workspace name
		projectPath << m_txtProjName->GetValue();
		projectPath << wxFileName::GetPathSeparator();
	}

	projectPath << m_txtProjName->GetValue();
	projectPath << wxT(".project");

	m_stxtFullFileName->SetLabel( projectPath );
}

void NewProjectDlg::UpdateProjectPage()
{
	//update the description
	if( m_projectData.m_srcProject) {
		wxString desc = m_projectData.m_srcProject->GetDescription();
		desc = desc.Trim().Trim(false);
		desc.Replace(wxT("\t"), wxT(" "));
		m_txtDescription->SetValue( desc );

		// select the correct compiler
		ProjectSettingsPtr settings  = m_projectData.m_srcProject->GetSettings();
		if(settings){
			ProjectSettingsCookie ck;
			BuildConfigPtr buildConf = settings->GetFirstBuildConfiguration(ck);
			if(buildConf){
				m_chCompiler->SetStringSelection( buildConf->GetCompilerType() );
			}
		}
	}
}

void NewProjectDlg::FillProjectTemplateListCtrl(const wxString& category)
{
	m_listTemplates->DeleteAllItems();

	std::list<ProjectPtr>::iterator iter = m_list.begin();
	for(; iter != m_list.end(); iter++) {
		wxString intType = (*iter)->GetProjectInternalType();

		if( (category == wxT("All")) ||
			(intType == category) ||
			( (intType == wxEmptyString) && (category == wxT("Others")) ) ||
			( (m_chCategories->FindString(intType) == wxNOT_FOUND) && (category == wxT("Others")) ) )
		{
			long item = AppendListCtrlRow(m_listTemplates);
			std::map<wxString,int>::iterator img_iter = m_mapImages.find((*iter)->GetName());
			int imgid(0);
			if(img_iter != m_mapImages.end()) {
				imgid = img_iter->second;
			}
			
			SetColumnText(m_listTemplates, item, 0, (*iter)->GetName(), imgid);
		}
	}

	if( m_listTemplates->GetItemCount() ) {
		m_projectData.m_srcProject = FindProject(m_listTemplates->GetItemText(0));
		m_listTemplates->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		UpdateProjectPage();
	}
}
