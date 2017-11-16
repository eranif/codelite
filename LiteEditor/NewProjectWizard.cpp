//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : NewProjectWizard.cpp
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

#include "NewProjectWizard.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "cl_config.h"
#include "compiler.h"
#include "debuggermanager.h"
#include "editor_config.h"
#include "globals.h"
#include "pluginmanager.h"
#include "windowattrmanager.h"
#include "workspace.h"
#include <wx/dirdlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

class NewProjectDlgData : public SerializedObject
{
    size_t m_flags;
    int m_category;
    wxString m_lastSelection;

public:
    enum { NpSeparateDirectory = 0x00000001 };

public:
    NewProjectDlgData()
        : m_flags(NpSeparateDirectory)
        , m_category(-1)
    {
    }

    virtual ~NewProjectDlgData() {}

    void DeSerialize(Archive& arch)
    {
        arch.Read(wxT("m_flags"), m_flags);
        arch.Read(wxT("m_category"), m_category);
        arch.Read(wxT("m_lastSelection"), m_lastSelection);
    }

    void Serialize(Archive& arch)
    {
        arch.Write(wxT("m_flags"), m_flags);
        arch.Write(wxT("m_category"), m_category);
        arch.Write(wxT("m_lastSelection"), m_lastSelection);
    }

    void SetFlags(size_t flags) { this->m_flags = flags; }
    size_t GetFlags() const { return m_flags; }
    void SetCategory(int category) { this->m_category = category; }
    int GetCategory() const { return m_category; }
    void SetLastSelection(const wxString& lastSelection) { this->m_lastSelection = lastSelection; }
    const wxString& GetLastSelection() const { return m_lastSelection; }
};

// ------------------------------------------------------------
// ------------------------------------------------------------
class NewProjectClientData : public wxClientData
{
    ProjectPtr m_project;
    wxString m_templateName;
    bool m_isPluginManaged;
    bool m_canUseSeparateFolder;

public:
    NewProjectClientData(ProjectPtr project, const wxString& templateName = wxEmptyString, bool isPluginManaged = false,
                         bool canUseSeparateFolder = true)
        : m_templateName(templateName)
        , m_isPluginManaged(isPluginManaged)
        , m_canUseSeparateFolder(canUseSeparateFolder)
    {
        m_project = project;
        if(m_project && m_templateName.IsEmpty()) {
            // use the project name
            m_templateName = m_project->GetName();
        }
    }

    void SetCanUseSeparateFolder(bool canUseSeparateFolder) { this->m_canUseSeparateFolder = canUseSeparateFolder; }
    void SetIsPluginManaged(bool isPluginManaged) { this->m_isPluginManaged = isPluginManaged; }
    bool IsCanUseSeparateFolder() const { return m_canUseSeparateFolder; }
    bool IsPluginManaged() const { return m_isPluginManaged; }
    virtual ~NewProjectClientData() { m_project = NULL; }
    void setProject(const ProjectPtr& project) { this->m_project = project; }
    const ProjectPtr& getProject() const { return m_project; }
    void SetTemplate(const wxString& templateName) { this->m_templateName = templateName; }
    const wxString& GetTemplate() const { return m_templateName; }
};

// ------------------------------------------------------------
// ------------------------------------------------------------

NewProjectWizard::NewProjectWizard(wxWindow* parent, const clNewProjectEvent::Template::Vec_t& additionalTemplates)
    : NewProjectWizardBase(parent)
    , m_selectionMade(false)
{
    m_additionalTemplates = additionalTemplates;
    NewProjectDlgData info;
    EditorConfigST::Get()->ReadObject(wxT("NewProjectDlgData"), &info);

    NewProjImgList images;

    // Get list of project templates (bot the installed ones + user)
    GetProjectTemplateList(m_list);

    // Populate the dataview model
    m_dataviewTemplatesModel->Clear();
    std::map<wxString, wxDataViewItem> categoryMap;
    std::map<wxString, wxStringSet_t> projectsPerCategory;

    // list of compilers
    wxArrayString compilerChoices;

    // list of debuggers
    wxArrayString debuggerChoices;

    // Place the plugins on top
    for(size_t i = 0; i < m_additionalTemplates.size(); ++i) {

        // Add the category if not exists
        const clNewProjectEvent::Template& newTemplate = m_additionalTemplates.at(i);
        if(!newTemplate.m_toolchain.IsEmpty()) { compilerChoices.Add(newTemplate.m_toolchain); }

        if(!newTemplate.m_debugger.IsEmpty()) { debuggerChoices.Add(newTemplate.m_debugger); }

        wxString category = newTemplate.m_category;
        if(categoryMap.count(category) == 0) {
            wxVector<wxVariant> cols;
            wxBitmap bmp = wxXmlResource::Get()->LoadBitmap(newTemplate.m_categoryPng);
            if(!bmp.IsOk()) { bmp = images.Bitmap("gear16"); }
            cols.push_back(DVTemplatesModel::CreateIconTextVariant(category, bmp));
            categoryMap[category] = m_dataviewTemplatesModel->AppendItem(wxDataViewItem(0), cols);
            projectsPerCategory.insert(std::make_pair(category, wxStringSet_t()));
        }

        {
            wxString name = newTemplate.m_template;
            if(projectsPerCategory[category].count(name)) {
                // already exists
                continue;
            }
            projectsPerCategory[category].insert(name); // add it to the unique list

            wxVector<wxVariant> cols;
            wxBitmap bmp = wxXmlResource::Get()->LoadBitmap(newTemplate.m_templatePng);
            if(!bmp.IsOk()) { bmp = images.Bitmap("gear16"); }
            cols.push_back(DVTemplatesModel::CreateIconTextVariant(newTemplate.m_template, bmp));
            m_dataviewTemplatesModel->AppendItem(
                categoryMap[category], cols,
                new NewProjectClientData(NULL, newTemplate.m_template, true, newTemplate.m_allowSeparateFolder));
        }
    }

    wxDataViewItem selection;
    std::list<ProjectPtr>::iterator iter = m_list.begin();
    for(; iter != m_list.end(); ++iter) {
        wxVector<wxVariant> cols;
        wxString internalType = (*iter)->GetProjectInternalType();
        if(internalType.IsEmpty()) internalType = "Others";

        // Add the category node if needed
        if(categoryMap.count(internalType) == 0) {
            cols.clear();
            wxIcon icn;
            icn.CopyFromBitmap(images.Bitmap("gear16"));
            wxDataViewIconText ict(internalType, icn);
            wxVariant v;
            v << ict;
            cols.push_back(v);
            categoryMap[internalType] = m_dataviewTemplatesModel->AppendItem(wxDataViewItem(0), cols);
            projectsPerCategory.insert(std::make_pair(internalType, wxStringSet_t()));
        }

        wxString imgId = (*iter)->GetProjectIconName();
        wxBitmap bmp = images.Bitmap(imgId);
        // Allow the user to override it

        // Remove the entry
        if(projectsPerCategory[internalType].count((*iter)->GetName())) {
            // already exists
            continue;
        }
        projectsPerCategory[internalType].insert((*iter)->GetName()); // add it to the unique list

        cols.clear();
        wxIcon icn;
        icn.CopyFromBitmap(bmp);
        wxDataViewIconText ict((*iter)->GetName(), icn);
        wxVariant v;
        v << ict;

        cols.push_back(v);
        wxDataViewItem item =
            m_dataviewTemplatesModel->AppendItem(categoryMap[internalType], cols, new NewProjectClientData(*iter));
        if((*iter)->GetName() == info.GetLastSelection()) { selection = item; }
    }

    // Get list of compilers from configuration file
    BuildSettingsConfigCookie cookie;
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    while(cmp) {
        compilerChoices.Add(cmp->GetName());
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }

    m_choiceCompiler->Append(compilerChoices);
    if(compilerChoices.IsEmpty() == false) { m_choiceCompiler->SetSelection(0); }

    m_textCtrlProjectPath->SetValue(clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath());

    // Get list of debuggers
    wxArrayString knownDebuggers = DebuggerMgr::Get().GetAvailableDebuggers();
    debuggerChoices.insert(debuggerChoices.end(), knownDebuggers.begin(), knownDebuggers.end());
    m_choiceDebugger->Append(debuggerChoices);
    if(!m_choiceDebugger->IsEmpty()) { m_choiceDebugger->SetSelection(0); }

    {
        // build system
        std::list<wxString> builders;
        wxArrayString knownBuilders;
        BuildManagerST::Get()->GetBuilders(builders);
        std::for_each(builders.begin(), builders.end(),
                      [&](const wxString& builderName) { knownBuilders.Add(builderName); });
        m_choiceBuildSystem->Append(knownBuilders);
        if(!m_choiceBuildSystem->IsEmpty()) { m_choiceBuildSystem->SetSelection(0); }
    }

    m_cbSeparateDir->SetValue(info.GetFlags() & NewProjectDlgData::NpSeparateDirectory);

    // Make sure the old selection is restored
    if(selection.IsOk()) {
        m_dataviewTemplates->Select(selection);
        m_dataviewTemplates->EnsureVisible(selection);
        NewProjectClientData* cd =
            dynamic_cast<NewProjectClientData*>(m_dataviewTemplatesModel->GetClientObject(selection));
        if(cd) { m_projectData.m_srcProject = cd->getProject(); }
    }

    UpdateProjectPage();
}

NewProjectWizard::~NewProjectWizard()
{
    // Keep the options
    NewProjectDlgData info;
    size_t flags(0);

    clConfig::Get().Write("CxxWizard/Compiler", m_choiceCompiler->GetStringSelection());
    clConfig::Get().Write("CxxWizard/Debugger", m_choiceDebugger->GetStringSelection());
    clConfig::Get().Write("CxxWizard/BuildSystem", m_choiceBuildSystem->GetStringSelection());

    if(m_cbSeparateDir->IsChecked()) flags |= NewProjectDlgData::NpSeparateDirectory;

    info.SetFlags(flags);
    wxDataViewItem sel = m_dataviewTemplates->GetSelection();
    if(sel.IsOk()) {
        NewProjectClientData* cd = dynamic_cast<NewProjectClientData*>(m_dataviewTemplatesModel->GetClientObject(sel));
        if(cd && cd->getProject()) {
            info.SetLastSelection(cd->getProject()->GetName());
        } else if(cd) {
            info.SetLastSelection(cd->GetTemplate());
        }
    }

    EditorConfigST::Get()->WriteObject(wxT("NewProjectDlgData"), &info);
}

void NewProjectWizard::UpdateFullFileName()
{
    wxString projectPath;
    projectPath << m_textCtrlProjectPath->GetValue();

    projectPath = projectPath.Trim().Trim(false);

    wxString tmpSep(wxFileName::GetPathSeparator());
    if(!projectPath.EndsWith(tmpSep) && projectPath.IsEmpty() == false) {
        projectPath << wxFileName::GetPathSeparator();
    }

    if(m_txtProjName->GetValue().Trim().Trim(false).IsEmpty()) {
        m_stxtFullFileName->SetLabel(wxEmptyString);
        return;
    }

    if(m_cbSeparateDir->IsChecked()) {
        // append the workspace name
        projectPath << m_txtProjName->GetValue();
        projectPath << wxFileName::GetPathSeparator();
    }

    projectPath << m_txtProjName->GetValue();
    projectPath << wxT(".project");

    m_stxtFullFileName->SetLabel(projectPath);
}

void NewProjectWizard::UpdateProjectPage()
{
    // update the description
    if(m_projectData.m_srcProject) {
        wxString desc = m_projectData.m_srcProject->GetDescription();
        desc = desc.Trim().Trim(false);
        desc.Replace(wxT("\t"), wxT(" "));

        // select the correct compiler
        ProjectSettingsPtr settings = m_projectData.m_srcProject->GetSettings();
        if(settings) {
            ProjectSettingsCookie ck;
            BuildConfigPtr buildConf = settings->GetFirstBuildConfiguration(ck);
            if(buildConf) {
                m_choiceCompiler->SetStringSelection(buildConf->GetCompilerType());
                m_choiceDebugger->SetStringSelection(buildConf->GetDebuggerType());
            }
        }
    }

    // Restore previous selections
    wxString lastCompiler = clConfig::Get().Read("CxxWizard/Compiler", wxString());
    wxString lastDebugger = clConfig::Get().Read("CxxWizard/Debugger", wxString());
    wxString lastBuilder = clConfig::Get().Read("CxxWizard/BuildSystem", wxString("Default"));
    if(!lastDebugger.IsEmpty()) {
        int where = m_choiceDebugger->FindString(lastDebugger);
        if(where != wxNOT_FOUND) { m_choiceDebugger->SetSelection(where); }
    }

    if(!lastCompiler.IsEmpty()) {
        int where = m_choiceCompiler->FindString(lastCompiler);
        if(where != wxNOT_FOUND) { m_choiceCompiler->SetSelection(where); }
    }

    {
        int where = m_choiceBuildSystem->FindString(lastBuilder);
        if(where != wxNOT_FOUND) { m_choiceBuildSystem->SetSelection(where); }
    }
}

void NewProjectWizard::OnBrowseProjectPath(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString path(m_textCtrlProjectPath->GetValue());
    wxString new_path = ::wxDirSelector(_("Select Project Path:"), path, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if(new_path.IsEmpty() == false) {

        static wxString INVALID_CHARS = ",'";
        if(new_path.find_first_of(INVALID_CHARS) != wxString::npos) {
            int answer = ::wxMessageBox(wxString() << _("The selected project path '") << new_path
                                                   << _("'\ncontains some invalid characters\nContinue anyway?"),
                                        "CodeLite", wxYES_NO | wxCANCEL | wxICON_WARNING, this);
            if(answer != wxYES) { return; }
        }
        m_textCtrlProjectPath->SetValue(new_path);
    }
}

void NewProjectWizard::OnItemSelected(wxDataViewEvent& event)
{
    NewProjectClientData* cd =
        dynamic_cast<NewProjectClientData*>(m_dataviewTemplatesModel->GetClientObject(event.GetItem()));
    if(cd) {
        m_projectData.m_srcProject = cd->getProject();
        m_projectData.m_sourceTemplate = cd->GetTemplate();
        UpdateProjectPage();
    }
}

void NewProjectWizard::OnProjectNameChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    UpdateFullFileName();
}

void NewProjectWizard::OnProjectPathUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    UpdateFullFileName();
}

void NewProjectWizard::OnFinish(wxWizardEvent& event)
{
    wxFileName fn(m_stxtFullFileName->GetLabel());

    // Ensure that the target folder exists
    fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // make sure that there is no conflict in files between the template project and the selected path
    if(m_projectData.m_srcProject) {
        ProjectPtr p = m_projectData.m_srcProject;
        wxString base_dir(fn.GetPath());
        std::vector<wxFileName> files;
        p->GetFilesAsVectorOfFileName(files);

        for(size_t i = 0; i < files.size(); ++i) {
            wxFileName f = files.at(i);
            wxString new_file = base_dir + wxT("/") + f.GetFullName();

            if(wxFileName::FileExists(new_file)) {
                // this file already - notify the user
                wxString msg;
                msg << _("The File '") << f.GetFullName() << _("' already exists at the target directory '") << base_dir
                    << wxT("'\n");
                msg << _("Please select a different project path\n");
                msg << _("The file '") << f.GetFullName() << _("' is part of the template project [") << p->GetName()
                    << wxT("]");
                wxMessageBox(msg, _("CodeLite"), wxOK | wxICON_HAND);
                event.Veto();
                return;
            }
        }
    }

    m_projectData.m_name = m_txtProjName->GetValue();
    m_projectData.m_path = fn.GetPath();
    m_projectData.m_cmpType = m_choiceCompiler->GetStringSelection();
    m_projectData.m_debuggerType = m_choiceDebugger->GetStringSelection();
    m_projectData.m_builderName = m_choiceBuildSystem->GetStringSelection();
    event.Skip();
}

void NewProjectWizard::OnPageChanging(wxWizardEvent& event)
{
    if(event.GetDirection()) {
        wxDataViewItem sel = m_dataviewTemplates->GetSelection();
        NewProjectClientData* cd = dynamic_cast<NewProjectClientData*>(m_dataviewTemplatesModel->GetClientObject(sel));

        if(event.GetPage() == m_wizardPageTemplate) {
            // -------------------------------------------------------
            // Switching from the Templates page
            // -------------------------------------------------------
            if(!CheckProjectTemplate()) {
                event.Veto();
                return;
            }

            // Test to see if the selected project allows enabling the 'Create under separate folder'
            if(cd && !cd->IsCanUseSeparateFolder()) {
                m_cbSeparateDir->SetValue(false);
                m_cbSeparateDir->Enable(false);
            } else {
                m_cbSeparateDir->Enable(true);
            }

            m_txtProjName->SetFocus(); // This should have happened in the base-class ctor, but in practice it doesn't

        } else if(event.GetPage() == m_wizardPageDetails) {
            // -------------------------------------------------------
            // Switching from the Name/Path page
            // -------------------------------------------------------
            if(!CheckProjectName() || !CheckProjectPath()) {
                event.Veto();
                return;
            }
        } else if(event.GetPage() == m_wizardPageToolchain) {
            wxFileName fn(m_stxtFullFileName->GetLabel());

            // make sure that there is no conflict in files between the template project and the selected path
            if(m_projectData.m_srcProject) {
                ProjectPtr p = m_projectData.m_srcProject;
                wxString base_dir(fn.GetPath());
                std::vector<wxFileName> files;
                p->GetFilesAsVectorOfFileName(files);

                for(size_t i = 0; i < files.size(); ++i) {
                    wxFileName f = files.at(i);
                    wxString new_file = base_dir + wxT("/") + f.GetFullName();

                    if(wxFileName::FileExists(new_file)) {
                        // this file already - notify the user
                        wxString msg;
                        msg << _("The File '") << f.GetFullName() << _("' already exists at the target directory '")
                            << base_dir << wxT("'\n");
                        msg << _("Please select a different project path\n");
                        msg << _("The file '") << f.GetFullName() << _("' is part of the template project [")
                            << p->GetName() << wxT("]");
                        wxMessageBox(msg, _("CodeLite"), wxOK | wxICON_HAND);
                        event.Veto();
                        return;
                    }
                }
            }
        }
    }
    event.Skip();
}

bool NewProjectWizard::CheckProjectName()
{
    wxString projectName = m_txtProjName->GetValue();
    projectName.Trim().Trim(false);
    if(!::clIsVaidProjectName(projectName) || projectName.IsEmpty()) {
        wxMessageBox(_("Project names may contain only the following characters [a-z0-9_-]"), "CodeLite",
                     wxOK | wxICON_WARNING | wxCENTER, this);
        return false;
    }
    return true;
}

bool NewProjectWizard::CheckProjectPath()
{
    wxFileName fn(m_textCtrlProjectPath->GetValue());
    if(!fn.Exists()) {
        wxMessageBox(_("Invalid project path selected: ") + fn.GetPath(), "CodeLite", wxOK | wxICON_WARNING | wxCENTER,
                     this);
        return false;
    }
    return true;
}

bool NewProjectWizard::CheckProjectTemplate()
{
    // Make sure we have a selection
    wxDataViewItem sel = m_dataviewTemplates->GetSelection();
    if(!sel.IsOk() || !dynamic_cast<NewProjectClientData*>(m_dataviewTemplatesModel->GetClientObject(sel))) {
        ::wxMessageBox(_("Please select a template from the list"), "CodeLite", wxOK | wxCENTER | wxICON_WARNING, this);
        return false;
    }
    return true;
}

void NewProjectWizard::OnCompilerSelected(wxCommandEvent& event)
{
    event.Skip();
    m_selectionMade = true;
}

void NewProjectWizard::OnDebuggerSelected(wxCommandEvent& event)
{
    event.Skip();
    m_selectionMade = true;
}
