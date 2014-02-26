//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : workspace.cpp
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
#include "workspace.h"
#include <wx/log.h>
#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#include "environmentconfig.h"
#include "evnvarlist.h"
#include "ctags_manager.h"
#include "project.h"
#include "xmlutils.h"
#include <wx/tokenzr.h>
#include "macros.h"
#include "wx/regex.h"
#include "globals.h"
#include "macros.h"
#include "wx_xml_compatibility.h"
#include "plugin.h"
#include "event_notifier.h"
#include "cl_command_event.h"

Workspace::Workspace()
{
}

Workspace::~Workspace()
{
    if ( m_doc.IsOk() ) {
        SaveXmlFile();
    }
}

wxString Workspace::GetName() const
{
    if (m_doc.IsOk()) {
        return XmlUtils::ReadString(m_doc.GetRoot(), wxT("Name"));
    }
    return wxEmptyString;
}

void Workspace::CloseWorkspace()
{
    if (m_doc.IsOk()) {
        SaveXmlFile();
        m_doc = wxXmlDocument();
    }

    m_fileName.Clear();
    // reset the internal cache objects
    m_projects.clear();

    TagsManagerST::Get()->CloseDatabase();
}


bool Workspace::OpenWorkspace(const wxString &fileName, wxString &errMsg)
{
    CloseWorkspace();
    wxFileName workSpaceFile(fileName);
    if (workSpaceFile.FileExists() == false) {
        errMsg = wxString::Format(wxT("Could not open workspace file: '%s'"), fileName.c_str());
        return false;
    }
    
    m_fileName = workSpaceFile;
    m_doc.Load(m_fileName.GetFullPath());
    if ( !m_doc.IsOk() ) {
        errMsg = wxT("Corrupted workspace file");
        return false;
    }
    
    // Make sure we have the WORKSPACE/.codelite folder exists
    {
        wxLogNull nolog;
        wxMkdir( GetPrivateFolder() );
    }
    
    SetWorkspaceLastModifiedTime(GetFileLastModifiedTime());

    // This function sets the working directory to the workspace directory!
    ::wxSetWorkingDirectory(m_fileName.GetPath());

    // Load all projects
    wxXmlNode *child = m_doc.GetRoot()->GetChildren();
    std::vector<wxXmlNode*> removedChildren;
    wxString tmperr;
    while (child) {
        if (child->GetName() == wxT("Project")) {
            wxString projectPath = child->GetPropVal(wxT("Path"), wxEmptyString);

            if ( !DoAddProject(projectPath, errMsg) ) {
                tmperr << wxString::Format(wxT("Error occured while loading project: \"%s\"\nCodeLite has removed the faulty project from the workspace\n"), projectPath.c_str());
                removedChildren.push_back(child);
            }
        }
        child = child->GetNext();
    }

    // Delete the faulty projects
    for(size_t i=0; i<removedChildren.size(); i++) {
        wxXmlNode *ch = removedChildren.at(i);
        ch->GetParent()->RemoveChild(ch);
        delete ch;
    }

    errMsg.Clear();
    TagsManager *mgr = TagsManagerST::Get();
    mgr->CloseDatabase();
    mgr->OpenDatabase( GetTagsFileName().GetFullPath() );
    return true;
}

BuildMatrixPtr Workspace::GetBuildMatrix() const
{
    return new BuildMatrix( XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("BuildMatrix")) );
}

wxXmlNode* Workspace::GetWorkspaceEditorOptions() const
{
    return XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Options"));
}

void Workspace::SetWorkspaceEditorOptions(LocalOptionsConfigPtr opts)
{
    wxXmlNode *parent = m_doc.GetRoot();
    wxXmlNode *oldOptions = XmlUtils::FindFirstByTagName(parent, wxT("Options"));
    if (oldOptions) {
        oldOptions->GetParent()->RemoveChild(oldOptions);
        delete oldOptions;
    }
    parent->AddChild(opts->ToXml());
    SaveXmlFile();
}

void Workspace::SetBuildMatrix(BuildMatrixPtr mapping)
{
    wxXmlNode *parent = m_doc.GetRoot();
    wxXmlNode *oldMapping = XmlUtils::FindFirstByTagName(parent, wxT("BuildMatrix"));
    if (oldMapping) {
        parent->RemoveChild(oldMapping);
        delete oldMapping;
    }
    parent->AddChild(mapping->ToXml());
    SaveXmlFile();

    // force regeneration of makefiles for all projects
    for (std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin(); iter != m_projects.end(); iter++) {
        iter->second->SetModified(true);
    }
}

bool Workspace::CreateWorkspace(const wxString &name, const wxString &path, wxString &errMsg)
{
    // If we have an open workspace, close it
    if ( m_doc.IsOk() ) {
        if ( !SaveXmlFile() ) {
            errMsg = wxT("Failed to save current workspace");
            return false;
        }
    }

    if ( name.IsEmpty() ) {
        errMsg = wxT("Invalid workspace name");
        return false;
    }

    // Create new
    // Open workspace database
    m_fileName = wxFileName(path, name + wxT(".workspace"));
    
    // Make sure we have the WORKSPACE/.codelite folder exists
    {
        wxLogNull nolog;
        wxMkdir( GetPrivateFolder() );
    }
    
    // This function sets the working directory to the workspace directory!
    ::wxSetWorkingDirectory(m_fileName.GetPath());

    wxFileName dbFileName = GetTagsFileName();
    TagsManagerST::Get()->OpenDatabase(dbFileName);

    wxXmlNode *root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CodeLite_Workspace"));
    m_doc.SetRoot(root);
    m_doc.GetRoot()->AddProperty(wxT("Name"), name);
    m_doc.GetRoot()->AddProperty(wxT("Database"), dbFileName.GetFullPath(wxPATH_UNIX));

    SaveXmlFile();
    //create an empty build matrix
    SetBuildMatrix(new BuildMatrix(NULL));
    return true;
}

wxString Workspace::GetStringProperty(const wxString &propName, wxString &errMsg)
{
    if ( !m_doc.IsOk() ) {
        errMsg = wxT("No workspace open");
        return wxEmptyString;
    }

    wxXmlNode *rootNode = m_doc.GetRoot();
    if ( !rootNode ) {
        errMsg = wxT("Corrupted workspace file");
        return wxEmptyString;
    }

    return rootNode->GetPropVal(propName, wxEmptyString);
}

void Workspace::AddProjectToBuildMatrix(ProjectPtr prj)
{
    if (!prj) {
        wxMessageBox(_("AddProjectToBuildMatrix was called with NULL project"), _("CodeLite"), wxICON_WARNING|wxOK);
        return;
    }

    BuildMatrixPtr matrix = GetBuildMatrix();
    wxString selConfName = matrix->GetSelectedConfigurationName();

    std::list<WorkspaceConfigurationPtr> wspList = matrix->GetConfigurations();
    std::list<WorkspaceConfigurationPtr>::iterator iter = wspList.begin();
    for (; iter !=  wspList.end(); iter++) {
        WorkspaceConfigurationPtr workspaceConfig = (*iter);
        WorkspaceConfiguration::ConfigMappingList prjList = workspaceConfig->GetMapping();
        wxString wspCnfName = workspaceConfig->GetName();

        ProjectSettingsCookie cookie;

        // getSettings is a bit misleading, since it actually create new instance which represents the layout
        // of the XML
        ProjectSettingsPtr settings = prj->GetSettings();
        BuildConfigPtr prjBldConf = settings->GetFirstBuildConfiguration(cookie);
        BuildConfigPtr matchConf;

        if ( !prjBldConf ) {
            // the project does not have any settings, create new one and add it
            prj->SetSettings(settings);

            settings = prj->GetSettings();
            prjBldConf = settings->GetFirstBuildConfiguration(cookie);
            matchConf = prjBldConf;

        } else {

            matchConf = prjBldConf;

            // try to locate the best match to add to the workspace
            while ( prjBldConf ) {
                wxString projBldConfName = prjBldConf->GetName();
                if (wspCnfName == projBldConfName) {
                    // we found a suitable match use it instead of the default one
                    matchConf = prjBldConf;
                    break;
                }
                prjBldConf = settings->GetNextBuildConfiguration(cookie);
            }
        }

        ConfigMappingEntry entry(prj->GetName(), matchConf->GetName());
        prjList.push_back(entry);
        (*iter)->SetConfigMappingList(prjList);
        matrix->SetConfiguration((*iter));
    }

    // and set the configuration name
    matrix->SetSelectedConfigurationName(selConfName);

    SetBuildMatrix(matrix);
}

void Workspace::RemoveProjectFromBuildMatrix(ProjectPtr prj)
{
    BuildMatrixPtr matrix = GetBuildMatrix();
    wxString selConfName = matrix->GetSelectedConfigurationName();

    std::list<WorkspaceConfigurationPtr> wspList = matrix->GetConfigurations();
    std::list<WorkspaceConfigurationPtr>::iterator iter = wspList.begin();
    for (; iter !=  wspList.end(); iter++) {
        WorkspaceConfiguration::ConfigMappingList prjList = (*iter)->GetMapping();

        WorkspaceConfiguration::ConfigMappingList::iterator it = prjList.begin();
        for (; it != prjList.end(); it++) {
            if ((*it).m_project == prj->GetName()) {
                prjList.erase(it);
                break;
            }
        }

        (*iter)->SetConfigMappingList(prjList);
        matrix->SetConfiguration((*iter));
    }

    // and set the configuration name
    matrix->SetSelectedConfigurationName(selConfName);
    SetBuildMatrix(matrix);
}

bool Workspace::CreateProject(const wxString &name, const wxString &path, const wxString &type, bool addToBuildMatrix, wxString &errMsg)
{
    if ( !m_doc.IsOk() ) {
        errMsg = wxT("No workspace open");
        return false;
    }

    ProjectPtr proj(new Project());
    proj->Create(name, wxEmptyString, path, type);
    m_projects[name] = proj;

    // make the project path to be relative to the workspace, if it's sensible to do so
    wxFileName tmp(path + wxFileName::GetPathSeparator() + name + wxT(".project"));
    tmp.MakeRelativeTo(m_fileName.GetPath());

    // Add an entry to the workspace file
    wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
    node->AddProperty(wxT("Name"), name);
    node->AddProperty(wxT("Path"), tmp.GetFullPath(wxPATH_UNIX));

    m_doc.GetRoot()->AddChild(node);

    if ( m_projects.size() == 1 ) {
        SetActiveProject(name, true);
    }

    SaveXmlFile();
    if (addToBuildMatrix) {
        AddProjectToBuildMatrix(proj);
    }
    return true;
}

ProjectPtr Workspace::FindProjectByName(const wxString &projName, wxString &errMsg) const
{
    if ( !m_doc.IsOk() ) {
        errMsg = wxT("No workspace open");
        return NULL;
    }

    std::map<wxString, ProjectPtr>::const_iterator iter = m_projects.find(projName);
    if ( iter == m_projects.end() ) {
        errMsg = wxT("Invalid project name '");
        errMsg << projName << wxT("'");
        return NULL;
    }
    return iter->second;
}

void Workspace::GetProjectList(wxArrayString &list)
{
    std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin();
    for (; iter != m_projects.end(); iter++) {
        wxString name;
        name = iter->first;
        list.Add(name);
    }
}

bool Workspace::AddProject(const wxString & path, wxString &errMsg)
{
    if ( !m_doc.IsOk() ) {
        errMsg = wxT("No workspace open");
        return false;
    }

    wxFileName fn(path);
    if ( !fn.FileExists() ) {
        errMsg = wxT("File does not exist");
        return false;
    }

    // Load the project into
    ProjectPtr newProject(new Project());
    if ( !newProject->Load(path) ) {
        errMsg = wxT("Corrupted project file '");
        errMsg << path << wxT("'");
        return false;
    }

    // Try first to find a project with similar name in the workspace
    ProjectPtr proj = FindProjectByName(newProject->GetName(), errMsg);
    if ( !proj ) {

        // No project could be find, add it to the workspace
        DoAddProject(newProject);

        // Add an entry to the workspace file
        fn.MakeRelativeTo(m_fileName.GetPath());

        wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
        node->AddProperty(wxT("Name"), fn.GetName());
        node->AddProperty(wxT("Path"), fn.GetFullPath(wxPATH_UNIX));
        node->AddProperty(wxT("Active"), m_projects.size() == 1 ? wxT("Yes") : wxT("No"));
        m_doc.GetRoot()->AddChild(node);
        if (!SaveXmlFile()) {
            wxMessageBox(_("Failed to save workspace file to disk. Please check that you have permission to write to disk"),
                         _("CodeLite"), wxICON_ERROR | wxOK);
            return false;
        }

        AddProjectToBuildMatrix( newProject );
        return true;

    } else {
        errMsg = wxString::Format(wxT("A project with a similar name '%s' already exists in the workspace"), proj->GetName().c_str());
        return false;
    }
}


ProjectPtr Workspace::DoAddProject(ProjectPtr proj)
{
    if(!proj)
        return NULL;

    m_projects[proj->GetName()] = proj;
    return proj;
}

ProjectPtr Workspace::DoAddProject(const wxString &path, wxString &errMsg)
{
    // Add the project
    ProjectPtr proj(new Project());
    if ( !proj->Load(path) ) {
        errMsg = wxT("Corrupted project file '");
        errMsg << path << wxT("'");
        return NULL;
    }
    // Add an entry to the projects map
    m_projects[proj->GetName()] = proj;
    return proj;
}

bool Workspace::RemoveProject(const wxString &name, wxString &errMsg)
{
    ProjectPtr proj = FindProjectByName(name, errMsg);
    if ( !proj ) {
        return false;
    }

    //remove the associated build configuration with this
    //project
    RemoveProjectFromBuildMatrix(proj);

    // remove the project from the internal map
    std::map<wxString, ProjectPtr>::iterator iter = m_projects.find(proj->GetName());
    if ( iter != m_projects.end() ) {
        m_projects.erase(iter);
    }

    // update the xml file
    wxXmlNode *root = m_doc.GetRoot();
    wxXmlNode *child = root->GetChildren();
    while ( child ) {
        if ( child->GetName() == wxT("Project") && child->GetPropVal(wxT("Name"), wxEmptyString) == name ) {
            if ( child->GetPropVal(wxT("Active"), wxEmptyString).CmpNoCase(wxT("Yes")) == 0) {
                // the removed project was active,
                // select new project to be active
                if ( !m_projects.empty() ) {
                    std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin();
                    SetActiveProject(iter->first, true);
                }
            }
            root->RemoveChild( child );
            delete child;
            break;
        }
        child = child->GetNext();
    }

    //go over the dependencies list of each project and remove the project
    iter = m_projects.begin();
    for (; iter != m_projects.end(); iter++) {
        ProjectPtr p = iter->second;
        if (p) {
            wxArrayString configs;
            // populate the choice control with the list of available configurations for this project
            ProjectSettingsPtr settings = p->GetSettings();
            if ( settings ) {
                ProjectSettingsCookie cookie;
                BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
                while (bldConf) {
                    configs.Add(bldConf->GetName());
                    bldConf = settings->GetNextBuildConfiguration(cookie);
                }
            }

            // update each configuration of this project
            for (size_t i=0; i<configs.GetCount(); i++) {

                wxArrayString deps = p->GetDependencies(configs.Item(i));
                int where = deps.Index(name);
                if (where != wxNOT_FOUND) {
                    deps.RemoveAt((size_t)where);
                }

                // update the configuration
                p->SetDependencies(deps, configs.Item(i));
            }
        }
    }
    return SaveXmlFile();
}

wxString Workspace::GetActiveProjectName()
{
    if ( !m_doc.IsOk() ) {
        return wxEmptyString;
    }

    wxXmlNode *root = m_doc.GetRoot();
    wxXmlNode *child = root->GetChildren();
    while ( child ) {
        if ( child->GetName() == wxT("Project") && child->GetPropVal(wxT("Active"), wxEmptyString).CmpNoCase(wxT("Yes")) == 0) {
            return child->GetPropVal(wxT("Name"), wxEmptyString);
        }
        child = child->GetNext();
    }
    return wxEmptyString;
}

void Workspace::SetActiveProject(const wxString &name, bool active)
{
    if ( !m_doc.IsOk() )
        return;

    // update the xml file
    wxXmlNode *root = m_doc.GetRoot();
    wxXmlNode *child = root->GetChildren();
    while ( child ) {
        if ( child->GetName() == wxT("Project") && child->GetPropVal(wxT("Name"), wxEmptyString) == name ) {
            XmlUtils::UpdateProperty(child, wxT("Active"), active ? wxT("Yes") : wxT("No"));
            break;
        }
        child = child->GetNext();
    }
    SaveXmlFile();
}

bool Workspace::CreateVirtualDirectory(const wxString &vdFullPath, wxString &errMsg, bool mkPath)
{
    wxStringTokenizer tkz(vdFullPath, wxT(":"));
    wxString projName = tkz.GetNextToken();

    wxString fixedPath;
    // Construct new path excluding the first token
    size_t count = tkz.CountTokens();

    for (size_t i=0; i<count-1; i++) {
        fixedPath += tkz.GetNextToken();
        fixedPath += wxT(":");
    }
    fixedPath += tkz.GetNextToken();

    ProjectPtr proj = FindProjectByName(projName, errMsg);
    return proj->CreateVirtualDir(fixedPath, mkPath);
}

bool Workspace::RemoveVirtualDirectory(const wxString &vdFullPath, wxString &errMsg)
{
    wxStringTokenizer tkz(vdFullPath, wxT(":"));
    wxString projName = tkz.GetNextToken();

    wxString fixedPath;
    // Construct new path excluding the first token
    size_t count = tkz.CountTokens();

    for (size_t i=0; i<count-1; i++) {
        fixedPath += tkz.GetNextToken();
        fixedPath += wxT(":");
    }
    fixedPath += tkz.GetNextToken();

    ProjectPtr proj = FindProjectByName(projName, errMsg);
    return proj->DeleteVirtualDir(fixedPath);
}

bool Workspace::SaveXmlFile()
{
    bool ok = m_doc.Save(m_fileName.GetFullPath());
    SetWorkspaceLastModifiedTime(GetFileLastModifiedTime());
    EventNotifier::Get()->PostFileSavedEvent( m_fileName.GetFullPath() );
    return ok;
}

void Workspace::Save()
{
    if ( m_doc.IsOk() ) {
        std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin();
        for (; iter != m_projects.end(); iter ++) {
            iter->second->Save();
        }
        SaveXmlFile();
    }
}

bool Workspace::AddNewFile(const wxString &vdFullPath, const wxString &fileName, wxString &errMsg)
{
    wxStringTokenizer tkz(vdFullPath, wxT(":"));

    // We should have at least 2 tokens:
    // project:virtual directory
    if(tkz.CountTokens() < 2)
        return false;

    wxString projName = tkz.GetNextToken();
    wxString fixedPath;
    // Construct new path excluding the first token
    size_t count = tkz.CountTokens();

    for (size_t i=0; i<count-1; i++) {
        fixedPath += tkz.GetNextToken();
        fixedPath += wxT(":");
    }
    fixedPath += tkz.GetNextToken();

    ProjectPtr proj = FindProjectByName(projName, errMsg);
    if ( !proj ) {
        errMsg = wxT("No such project");
        return false;
    }

    return proj->AddFile(fileName, fixedPath);
}

bool Workspace::RemoveFile(const wxString &vdFullPath, const wxString &fileName, wxString &errMsg)
{
    wxStringTokenizer tkz(vdFullPath, wxT(":"));
    wxString projName = tkz.GetNextToken();
    wxString fixedPath;

    // Construct new path excluding the first token
    size_t count = tkz.CountTokens();
    if (!count) {
        errMsg = _("Malformed project name");
        return false;
    }

    for (size_t i=0; i<count-1; i++) {
        fixedPath += tkz.GetNextToken();
        fixedPath += wxT(":");
    }
    fixedPath += tkz.GetNextToken();

    ProjectPtr proj = FindProjectByName(projName, errMsg);
    if ( !proj ) {
        errMsg = _("No such project");
        return false;
    }

    bool result = proj->RemoveFile(fileName, fixedPath);
    if ( !result ) {
        errMsg = _("File removal failed");
    }
    return result;
}

BuildConfigPtr Workspace::GetProjBuildConf(const wxString &projectName, const wxString &confName) const
{
    BuildMatrixPtr matrix = GetBuildMatrix();
    wxString projConf(confName);

    if (projConf.IsEmpty()) {
        wxString workspaceConfig = matrix->GetSelectedConfigurationName();
        projConf = matrix->GetProjectSelectedConf(workspaceConfig, projectName);
    }

    //Get the project setting and retrieve the selected configuration
    wxString errMsg;
    ProjectPtr proj = FindProjectByName(projectName, errMsg);
    if ( proj ) {
        ProjectSettingsPtr settings = proj->GetSettings();
        if (settings) {
            return settings->GetBuildConfiguration(projConf, true);
        }
    }
    return NULL;
}

void Workspace::ReloadWorkspace()
{
    m_doc = wxXmlDocument();

    wxLogNull noLog;
    // reset the internal cache objects
    m_projects.clear();

    TagsManager *mgr = TagsManagerST::Get();
    mgr->CloseDatabase();

    wxString err_msg;
    if (!OpenWorkspace(m_fileName.GetFullPath(), err_msg)) {
        wxLogMessage(wxT("Reload workspace: ")+ err_msg);
    }
}

time_t Workspace::GetFileLastModifiedTime() const
{
    return GetFileModificationTime(GetWorkspaceFileName());
}

// Singelton access
static Workspace* gs_Workspace = NULL;
void WorkspaceST::Free()
{
    if(gs_Workspace) {
        delete gs_Workspace;
    }
    gs_Workspace = NULL;
}

Workspace* WorkspaceST::Get()
{
    if(gs_Workspace == NULL)
        gs_Workspace = new Workspace;
    return gs_Workspace;
}

wxString Workspace::GetEnvironmentVariabels()
{
    if(!m_doc.IsOk())
        return wxEmptyString;

    wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Environment"));
    if(node) {
        wxString nodeContent = node->GetNodeContent();
        nodeContent.Trim().Trim(false);
        return nodeContent;
    }
    return wxEmptyString;
}

void Workspace::SetEnvironmentVariabels(const wxString& envvars)
{
    if(!m_doc.IsOk())
        return;

    wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Environment"));
    if(node) {
        m_doc.GetRoot()->RemoveChild(node);
        delete node;
    }

    node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Environment"));
    m_doc.GetRoot()->AddChild(node);

    wxString nodeContent = envvars;
    nodeContent.Trim().Trim(false);

    wxXmlNode *contentNode = new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxEmptyString, nodeContent);
    node->AddChild( contentNode );
    SaveXmlFile();
}

wxArrayString Workspace::GetAllProjectPaths()
{
    wxArrayString projects;
    std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); iter++) {
        projects.Add(iter->second->GetFileName().GetFullPath());
    }
    return projects;
}

bool Workspace::IsOpen() const
{
    return m_doc.IsOk();
}


bool Workspace::IsVirtualDirectoryExists(const wxString& vdFullPath)
{
    wxStringTokenizer tkz(vdFullPath, wxT(":"));
    wxString projName = tkz.GetNextToken();

    wxString fixedPath;
    // Construct new path excluding the first token
    size_t count = tkz.CountTokens();

    for (size_t i=0; i<count-1; i++) {
        fixedPath += tkz.GetNextToken();
        fixedPath += wxT(":");
    }
    fixedPath += tkz.GetNextToken();

    wxString errMsg;
    ProjectPtr proj = FindProjectByName(projName, errMsg);
    if ( !proj ) {
        return false;
    }

    wxXmlNode *vdNode = proj->GetVirtualDir(fixedPath);
    return vdNode != NULL;
}

wxString Workspace::GetPrivateFolder() const
{
    return (wxString() << GetWorkspaceFileName().GetPath() << wxFILE_SEP_PATH << ".codelite");
}

wxFileName Workspace::GetTagsFileName() const
{
    if ( !IsOpen() ) {
        return wxFileName();
    }
    
    wxFileName fn_tags(GetPrivateFolder(), GetWorkspaceFileName().GetFullName());
    fn_tags.SetExt("tags");
    return fn_tags;
}
