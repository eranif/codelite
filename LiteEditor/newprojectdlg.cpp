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
#include "editor_config.h"
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
    : NewProjectDlgBaseClass( parent )
{
    NewProjectDlgData info;
    EditorConfigST::Get()->ReadObject(wxT("NewProjectDlgData"), &info);

    //get list of project templates
    wxImageList *lstImages (NULL);
    GetProjectTemplateList(PluginManager::Get(), m_list, &m_mapImages, &lstImages);

    // assign image list to the list control which takes ownership of it (it will delete the image list)
    m_listTemplates->AssignImageList(lstImages, wxIMAGE_LIST_SMALL);
    MSWSetNativeTheme(m_listTemplates);

    m_chCategories->Clear();
    std::list<ProjectPtr>::iterator iter = m_list.begin();
    std::set<wxString>              categories;

    // Add the 'All' category
    categories.insert(_("All"));
    for (; iter != m_list.end(); iter++) {
        wxString internalType = (*iter)->GetProjectInternalType();
        if (internalType.IsEmpty()) internalType = _("Others");
        // Use wxGetTranslation() here. Some won't have translations, but it'll catch e.g. "GUI"
        categories.insert( wxGetTranslation(internalType) );
    }

    std::set<wxString>::iterator cIter = categories.begin();
    for (; cIter != categories.end(); cIter++) {
        m_chCategories->Append((*cIter));
    }

    // Select 'GUI' to be the default category
    
    int where = info.GetCategory();
    if (where == wxNOT_FOUND) {
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

    m_textCtrlProjectPath->SetValue( WorkspaceST::Get()->GetWorkspaceFileName().GetPath());
    m_txtProjName->SetFocus();

    m_cbSeparateDir->SetValue( info.GetFlags() & NewProjectDlgData::NpSeparateDirectory );
    Centre();

    UpdateProjectPage();
    m_splitter5->SetSashPosition(info.GetSashPosition());
    WindowAttrManager::Load(this, wxT("NewProjectDialog"), NULL);
}

NewProjectDlg::~NewProjectDlg()
{
    // Keep the options
    NewProjectDlgData info;
    size_t            flags (0);

    if(m_cbSeparateDir->IsChecked())
        flags |= NewProjectDlgData::NpSeparateDirectory;

    info.SetFlags(flags);
    info.SetSashPosition(m_splitter5->GetSashPosition());
    info.SetCategory(m_chCategories->GetSelection());
    EditorConfigST::Get()->WriteObject(wxT("NewProjectDlgData"), &info);

    WindowAttrManager::Save(this, wxT("NewProjectDialog"), NULL);
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

    if (m_cbSeparateDir->IsChecked()) {
        // dont check the return
        Mkdir(fn.GetPath());
    }

    wxString projectName = m_txtProjName->GetValue();
    projectName.Trim().Trim(false);
    if(projectName.find_first_not_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")) != wxString::npos) {
        wxMessageBox(_("Project name can only contain alpha numeric characters and/or the underscore '_'"), _("Error"), wxOK | wxICON_HAND | wxCENTER, this);
        return;
    }

    if(projectName.IsEmpty()) {
        wxMessageBox(_("Please provide a name for the project"), _("Error"), wxOK | wxICON_HAND | wxCENTER, this);
        return;
    }

    if ( !wxDirExists(fn.GetPath()) ) {
        wxMessageBox(_("Invalid path: ") + fn.GetPath(), _("Error"), wxOK | wxICON_HAND);
        return;
    }

    // make sure that there is no conflict in files between the template project and the selected path
    if (m_projectData.m_srcProject) {
        ProjectPtr p = m_projectData.m_srcProject;
        wxString base_dir( fn.GetPath() );
        std::vector<wxFileName> files;
        p->GetFiles(files);

        for (size_t i=0; i<files.size(); i++) {
            wxFileName f = files.at(i);
            wxString new_file = base_dir + wxT("/") + f.GetFullName();

            if ( wxFileName::FileExists(new_file) ) {
                // this file already - notify the user
                wxString msg;
                msg << _("The File '") << f.GetFullName() << _("' already exists at the target directory '") << base_dir << wxT("'\n");
                msg << _("Please select a different project path\n");
                msg << _("The file '") << f.GetFullName() << _("' is part of the template project [") << p->GetName() << wxT("]");
                wxMessageBox(msg, _("CodeLite"), wxOK|wxICON_HAND);
                return;
            }
        }
    }

    m_projectData.m_name    = m_txtProjName->GetValue();
    m_projectData.m_path    = fn.GetPath();
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
    projectPath << m_textCtrlProjectPath->GetValue();

    projectPath = projectPath.Trim().Trim(false);

    wxString tmpSep( wxFileName::GetPathSeparator() );
    if ( !projectPath.EndsWith(tmpSep) && projectPath.IsEmpty() == false ) {
        projectPath << wxFileName::GetPathSeparator();
    }

    if ( m_txtProjName->GetValue().Trim().Trim(false).IsEmpty() ) {
        m_stxtFullFileName->SetLabel(wxEmptyString);
        return;
    }

    if ( m_cbSeparateDir->IsChecked()) {
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
    if ( m_projectData.m_srcProject) {
        wxString desc = m_projectData.m_srcProject->GetDescription();
        desc = desc.Trim().Trim(false);
        desc.Replace(wxT("\t"), wxT(" "));
        m_txtDescription->SetValue( desc );

        // select the correct compiler
        ProjectSettingsPtr settings  = m_projectData.m_srcProject->GetSettings();
        if (settings) {
            ProjectSettingsCookie ck;
            BuildConfigPtr buildConf = settings->GetFirstBuildConfiguration(ck);
            if (buildConf) {
                m_chCompiler->SetStringSelection( buildConf->GetCompilerType() );
            }
        }
    }
}

void NewProjectDlg::FillProjectTemplateListCtrl(const wxString& category)
{
    m_listTemplates->DeleteAllItems();

    std::list<ProjectPtr>::iterator iter = m_list.begin();
    for (; iter != m_list.end(); iter++) {
        wxString intType = wxGetTranslation( (*iter)->GetProjectInternalType() );

        if ( (category == _("All")) ||
             (intType == category) ||
             ( (intType == wxEmptyString) && (category == _("Others")) ) ||
             ( (m_chCategories->FindString(intType) == wxNOT_FOUND) && (category == _("Others")) ) ) {
            long item = AppendListCtrlRow(m_listTemplates);
            std::map<wxString,int>::iterator img_iter = m_mapImages.find((*iter)->GetName());
            int imgid(0);
            if (img_iter != m_mapImages.end()) {
                imgid = img_iter->second;
            }

            SetColumnText(m_listTemplates, item, 0, (*iter)->GetName(), imgid);
        }
    }

    m_listTemplates->SetColumnWidth(0, wxLIST_AUTOSIZE);
    if ( m_listTemplates->GetItemCount() ) {
        m_projectData.m_srcProject = FindProject(m_listTemplates->GetItemText(0));
        m_listTemplates->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        UpdateProjectPage();
    }
}

void NewProjectDlg::OnBrowseProjectPath(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString path(m_textCtrlProjectPath->GetValue());
    wxString new_path = wxDirSelector(_("Select Project Path:"), path, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if (new_path.IsEmpty() == false) {
        m_textCtrlProjectPath->SetValue(new_path);
    }
}

void NewProjectDlg::OnProjectPathUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);

    UpdateFullFileName();
}
//////////////////////////////////////////////////////////////////////////

NewProjectDlgData::NewProjectDlgData()
    : m_flags(NpSeparateDirectory)
    , m_sashPosition(150)
    , m_category(-1)
{
}

NewProjectDlgData::~NewProjectDlgData()
{
}

void NewProjectDlgData::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_flags"), m_flags);
    arch.Read(wxT("m_sashPosition"), m_sashPosition);
    arch.Read(wxT("m_category"), m_category);
}

void NewProjectDlgData::Serialize(Archive& arch)
{
    arch.Write(wxT("m_flags"), m_flags);
    arch.Write(wxT("m_sashPosition"), m_sashPosition);
    arch.Write(wxT("m_category"), m_category);
}

void NewProjectDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlProjectPath->IsEmpty() && !m_txtProjName->IsEmpty());
}
