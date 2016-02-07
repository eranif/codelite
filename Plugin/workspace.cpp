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
#include <wx/thread.h>
#include "codelite_events.h"
#include "localworkspace.h"

clCxxWorkspace::clCxxWorkspace()
    : m_saveOnExit(true)
{
    SetWorkspaceType(_("C++"));
}

clCxxWorkspace::~clCxxWorkspace()
{
    if(m_saveOnExit && m_doc.IsOk()) {
        SaveXmlFile();
    }
}

wxString clCxxWorkspace::GetName() const
{
    if(m_doc.IsOk()) {
        return XmlUtils::ReadString(m_doc.GetRoot(), wxT("Name"));
    }
    return wxEmptyString;
}

void clCxxWorkspace::CloseWorkspace()
{
    m_buildMatrix.Reset(NULL);
    if(m_doc.IsOk()) {
        SaveXmlFile();
        m_doc = wxXmlDocument();
    }

    m_fileName.Clear();
    // reset the internal cache objects
    m_projects.clear();

    TagsManagerST::Get()->CloseDatabase();
}

bool clCxxWorkspace::OpenReadOnly(const wxString& fileName, wxString& errMsg)
{
    m_buildMatrix.Reset(NULL);
    wxFileName workSpaceFile(fileName);
    if(!workSpaceFile.FileExists()) {
        return false;
    }
    m_fileName = workSpaceFile;
    m_doc.Load(m_fileName.GetFullPath());
    if(!m_doc.IsOk()) {
        return false;
    }

    m_saveOnExit = false;

    // Make sure we have the WORKSPACE/.codelite folder exists
    {
        wxLogNull nolog;
        wxMkdir(GetPrivateFolder());
    }

    // Load all projects
    wxXmlNode* child = m_doc.GetRoot()->GetChildren();
    std::vector<wxXmlNode*> removedChildren;
    wxString tmperr;
    while(child) {
        if(child->GetName() == wxT("Project")) {
            wxString projectPath = child->GetPropVal(wxT("Path"), wxEmptyString);
            DoAddProject(projectPath, errMsg);
        }
        child = child->GetNext();
    }
    DoUpdateBuildMatrix();
    return true;
}

bool clCxxWorkspace::OpenWorkspace(const wxString& fileName, wxString& errMsg)
{
    CloseWorkspace();
    m_buildMatrix.Reset(NULL);
    wxFileName workSpaceFile(fileName);
    if(workSpaceFile.FileExists() == false) {
        errMsg = wxString::Format(wxT("Could not open workspace file: '%s'"), fileName.c_str());
        return false;
    }

    m_fileName = workSpaceFile;
    m_doc.Load(m_fileName.GetFullPath());
    if(!m_doc.IsOk()) {
        errMsg = wxT("Corrupted workspace file");
        return false;
    }

    // Make sure we have the WORKSPACE/.codelite folder exists
    {
        wxLogNull nolog;
        wxMkdir(GetPrivateFolder());
    }

    SetWorkspaceLastModifiedTime(GetFileLastModifiedTime());

    // This function sets the working directory to the workspace directory!
    ::wxSetWorkingDirectory(m_fileName.GetPath());

    // Load all projects
    wxXmlNode* child = m_doc.GetRoot()->GetChildren();
    std::vector<wxXmlNode*> removedChildren;
    wxString tmperr;
    while(child) {
        if(child->GetName() == wxT("Project")) {
            wxString projectPath = child->GetPropVal(wxT("Path"), wxEmptyString);

            if(!DoAddProject(projectPath, errMsg)) {
                tmperr << wxString::Format(wxT("Error occurred while loading project: \"%s\"\nCodeLite has removed the "
                                               "faulty project from the workspace\n"),
                                           projectPath.c_str());
                removedChildren.push_back(child);
            }
        } else if((child->GetName() == wxT("WorkspaceParserPaths")) ||
                  (child->GetName() == wxT("WorkspaceParserMacros"))) {
            wxString swtlw = wxEmptyString;
            swtlw = XmlUtils::ReadString(m_doc.GetRoot(), "SWTLW");
            if(swtlw.CmpNoCase("yes") == 0) {
                LocalWorkspaceST::Get()->SetParserFlags(LocalWorkspaceST::Get()->GetParserFlags() |
                                                        LocalWorkspace::EnableSWTLW);
                SyncToLocalWorkspaceSTParserPaths();
                SyncToLocalWorkspaceSTParserMacros();
            }
        }
        child = child->GetNext();
    }

    // Delete the faulty projects
    for(size_t i = 0; i < removedChildren.size(); i++) {
        wxXmlNode* ch = removedChildren.at(i);
        ch->GetParent()->RemoveChild(ch);
        delete ch;
    }

    errMsg.Clear();
    TagsManager* mgr = TagsManagerST::Get();
    mgr->CloseDatabase();
    mgr->OpenDatabase(GetTagsFileName().GetFullPath());

    // Update the build matrix
    DoUpdateBuildMatrix();

    // Notify about active project changed
    ProjectPtr activeProject = GetActiveProject();
    if(activeProject) {
        clProjectSettingsEvent evt(wxEVT_ACTIVE_PROJECT_CHANGED);
        evt.SetProjectName(activeProject->GetName());
        evt.SetFileName(activeProject->GetFileName().GetFullPath());
        EventNotifier::Get()->AddPendingEvent(evt);
    }
    return true;
}

BuildMatrixPtr clCxxWorkspace::GetBuildMatrix() const { return m_buildMatrix; }

wxXmlNode* clCxxWorkspace::GetWorkspaceEditorOptions() const
{
    return XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Options"));
}

void clCxxWorkspace::SetWorkspaceEditorOptions(LocalOptionsConfigPtr opts)
{
    wxXmlNode* parent = m_doc.GetRoot();
    wxXmlNode* oldOptions = XmlUtils::FindFirstByTagName(parent, wxT("Options"));
    if(oldOptions) {
        oldOptions->GetParent()->RemoveChild(oldOptions);
        delete oldOptions;
    }
    parent->AddChild(opts->ToXml());
    SaveXmlFile();
}

void clCxxWorkspace::SetBuildMatrix(BuildMatrixPtr mapping)
{
    wxXmlNode* parent = m_doc.GetRoot();
    wxXmlNode* oldMapping = XmlUtils::FindFirstByTagName(parent, wxT("BuildMatrix"));
    if(oldMapping) {
        parent->RemoveChild(oldMapping);
        wxDELETE(oldMapping);
    }
    parent->AddChild(mapping->ToXml());
    SaveXmlFile();

    // force regeneration of makefiles for all projects
    for(std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin(); iter != m_projects.end(); iter++) {
        iter->second->SetModified(true);
    }

    DoUpdateBuildMatrix();
}

bool clCxxWorkspace::CreateWorkspace(const wxString& name, const wxString& path, wxString& errMsg)
{
    // If we have an open workspace, close it
    if(m_doc.IsOk()) {
        if(!SaveXmlFile()) {
            errMsg = wxT("Failed to save current workspace");
            return false;
        }
    }

    if(name.IsEmpty()) {
        errMsg = wxT("Invalid workspace name");
        return false;
    }

    // Create new
    // Open workspace database
    m_fileName = wxFileName(path, name + wxT(".workspace"));

    // Make sure we have the WORKSPACE/.codelite folder exists
    {
        wxLogNull nolog;
        wxMkdir(GetPrivateFolder());
    }

    // This function sets the working directory to the workspace directory!
    ::wxSetWorkingDirectory(m_fileName.GetPath());
    m_buildMatrix.Reset(NULL);

    wxFileName dbFileName = GetTagsFileName();
    TagsManagerST::Get()->OpenDatabase(dbFileName);

    wxXmlNode* root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CodeLite_Workspace"));
    m_doc.SetRoot(root);
    m_doc.GetRoot()->AddProperty(wxT("Name"), name);
    m_doc.GetRoot()->AddProperty(wxT("Database"), dbFileName.GetFullPath(wxPATH_UNIX));

    m_doc.GetRoot()->DeleteAttribute(wxT("SWTLW"));
    if(LocalWorkspaceST::Get()->GetParserFlags() & LocalWorkspace::EnableSWTLW) {
        m_doc.GetRoot()->AddProperty(wxT("SWTLW"), "Yes");
    }

    SaveXmlFile();
    // create an empty build matrix
    DoUpdateBuildMatrix();
    return true;
}

wxString clCxxWorkspace::GetStringProperty(const wxString& propName, wxString& errMsg)
{
    if(!m_doc.IsOk()) {
        errMsg = wxT("No workspace open");
        return wxEmptyString;
    }

    wxXmlNode* rootNode = m_doc.GetRoot();
    if(!rootNode) {
        errMsg = wxT("Corrupted workspace file");
        return wxEmptyString;
    }

    return rootNode->GetPropVal(propName, wxEmptyString);
}

void clCxxWorkspace::AddProjectToBuildMatrix(ProjectPtr prj)
{
    if(!prj) {
        wxMessageBox(_("AddProjectToBuildMatrix was called with NULL project"), _("CodeLite"), wxICON_WARNING | wxOK);
        return;
    }

    BuildMatrixPtr matrix = GetBuildMatrix();
    wxString selConfName = matrix->GetSelectedConfigurationName();

    std::list<WorkspaceConfigurationPtr> wspList = matrix->GetConfigurations();
    std::list<WorkspaceConfigurationPtr>::iterator iter = wspList.begin();
    for(; iter != wspList.end(); iter++) {
        WorkspaceConfigurationPtr workspaceConfig = (*iter);
        WorkspaceConfiguration::ConfigMappingList prjList = workspaceConfig->GetMapping();
        wxString wspCnfName = workspaceConfig->GetName();

        ProjectSettingsCookie cookie;

        // getSettings is a bit misleading, since it actually create new instance which represents the layout
        // of the XML
        ProjectSettingsPtr settings = prj->GetSettings();
        BuildConfigPtr prjBldConf = settings->GetFirstBuildConfiguration(cookie);
        BuildConfigPtr matchConf;

        if(!prjBldConf) {
            // the project does not have any settings, create new one and add it
            prj->SetSettings(settings);

            settings = prj->GetSettings();
            prjBldConf = settings->GetFirstBuildConfiguration(cookie);
            matchConf = prjBldConf;

        } else {

            matchConf = prjBldConf;

            // try to locate the best match to add to the workspace
            while(prjBldConf) {
                wxString projBldConfName = prjBldConf->GetName();
                if(wspCnfName == projBldConfName) {
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

    // this will also reset the build matrix pointer
    SetBuildMatrix(matrix);
}

void clCxxWorkspace::RemoveProjectFromBuildMatrix(ProjectPtr prj)
{
    BuildMatrixPtr matrix = GetBuildMatrix();
    wxString selConfName = matrix->GetSelectedConfigurationName();

    std::list<WorkspaceConfigurationPtr> wspList = matrix->GetConfigurations();
    std::list<WorkspaceConfigurationPtr>::iterator iter = wspList.begin();
    for(; iter != wspList.end(); iter++) {
        WorkspaceConfiguration::ConfigMappingList prjList = (*iter)->GetMapping();

        WorkspaceConfiguration::ConfigMappingList::iterator it = prjList.begin();
        for(; it != prjList.end(); it++) {
            if((*it).m_project == prj->GetName()) {
                prjList.erase(it);
                break;
            }
        }

        (*iter)->SetConfigMappingList(prjList);
        matrix->SetConfiguration((*iter));
    }

    // and set the configuration name
    matrix->SetSelectedConfigurationName(selConfName);

    // this will also reset the build matrix pointer
    SetBuildMatrix(matrix);
}

bool clCxxWorkspace::CreateProject(
    const wxString& name, const wxString& path, const wxString& type, bool addToBuildMatrix, wxString& errMsg)
{
    if(!m_doc.IsOk()) {
        errMsg = wxT("No workspace open");
        return false;
    }

    ProjectPtr proj(new Project());
    proj->Create(name, wxEmptyString, path, type);
    proj->AssociateToWorkspace(this);
    m_projects[name] = proj;

    // make the project path to be relative to the workspace, if it's sensible to do so
    wxFileName tmp(path + wxFileName::GetPathSeparator() + name + wxT(".project"));
    tmp.MakeRelativeTo(m_fileName.GetPath());

    // Add an entry to the workspace file
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
    node->AddProperty(wxT("Name"), name);
    node->AddProperty(wxT("Path"), tmp.GetFullPath(wxPATH_UNIX));

    m_doc.GetRoot()->AddChild(node);

    if(m_projects.size() == 1) {
        SetActiveProject(name, true);
    }

    SaveXmlFile();
    if(addToBuildMatrix) {
        AddProjectToBuildMatrix(proj);
    }
    return true;
}

ProjectPtr clCxxWorkspace::FindProjectByName(const wxString& projName, wxString& errMsg) const
{
    if(!m_doc.IsOk()) {
        errMsg = wxT("No workspace open");
        return NULL;
    }

    std::map<wxString, ProjectPtr>::const_iterator iter = m_projects.find(projName);
    if(iter == m_projects.end()) {
        errMsg = wxT("Invalid project name '");
        errMsg << projName << wxT("'");
        return NULL;
    }
    return iter->second;
}

void clCxxWorkspace::GetProjectList(wxArrayString& list) const
{
    std::map<wxString, ProjectPtr>::const_iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); iter++) {
        wxString name;
        name = iter->first;
        list.Add(name);
    }
}

bool clCxxWorkspace::AddProject(const wxString& path, wxString& errMsg)
{
    if(!m_doc.IsOk()) {
        errMsg = wxT("No workspace open");
        return false;
    }

    wxFileName fn(path);
    if(!fn.FileExists()) {
        errMsg = wxT("File does not exist");
        return false;
    }

    // Load the project into
    ProjectPtr newProject(new Project());
    if(!newProject->Load(path)) {
        errMsg = wxT("Corrupted project file '");
        errMsg << path << wxT("'");
        return false;
    }

    // Try first to find a project with similar name in the workspace
    ProjectPtr proj = FindProjectByName(newProject->GetName(), errMsg);
    if(!proj) {

        // No project could be find, add it to the workspace
        DoAddProject(newProject);

        // Add an entry to the workspace file
        fn.MakeRelativeTo(m_fileName.GetPath());

        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
        node->AddProperty(wxT("Name"), fn.GetName());
        node->AddProperty(wxT("Path"), fn.GetFullPath(wxPATH_UNIX));
        node->AddProperty(wxT("Active"), m_projects.size() == 1 ? wxT("Yes") : wxT("No"));
        m_doc.GetRoot()->AddChild(node);
        if(!SaveXmlFile()) {
            wxMessageBox(
                _("Failed to save workspace file to disk. Please check that you have permission to write to disk"),
                _("CodeLite"),
                wxICON_ERROR | wxOK);
            return false;
        }

        AddProjectToBuildMatrix(newProject);
        return true;

    } else {
        errMsg = wxString::Format(wxT("A project with a similar name '%s' already exists in the workspace"),
                                  proj->GetName().c_str());
        return false;
    }
}

ProjectPtr clCxxWorkspace::DoAddProject(ProjectPtr proj)
{
    if(!proj) {
        return NULL;
    }

    m_projects.insert(std::make_pair(proj->GetName(), proj));
    proj->AssociateToWorkspace(this);
    return proj;
}

ProjectPtr clCxxWorkspace::DoAddProject(const wxString& path, wxString& errMsg)
{
    // Add the project
    ProjectPtr proj(new Project());

    // Convert the path to absolute path
    wxFileName projectFile(path);
    if(projectFile.IsRelative()) {
        projectFile.MakeAbsolute(m_fileName.GetPath());
    }

    if(!proj->Load(projectFile.GetFullPath())) {
        errMsg = wxT("Corrupted project file '");
        errMsg << projectFile.GetFullPath() << wxT("'");
        return NULL;
    }

    // Add an entry to the projects map
    m_projects.insert(std::make_pair(proj->GetName(), proj));
    proj->AssociateToWorkspace(this);
    return proj;
}

bool clCxxWorkspace::RemoveProject(const wxString& name, wxString& errMsg)
{
    ProjectPtr proj = FindProjectByName(name, errMsg);
    if(!proj) {
        return false;
    }

    // remove the associated build configuration with this
    // project
    RemoveProjectFromBuildMatrix(proj);

    // remove the project from the internal map
    std::map<wxString, ProjectPtr>::iterator iter = m_projects.find(proj->GetName());
    if(iter != m_projects.end()) {
        m_projects.erase(iter);
    }

    // update the xml file
    wxXmlNode* root = m_doc.GetRoot();
    wxXmlNode* child = root->GetChildren();
    while(child) {
        if(child->GetName() == wxT("Project") && child->GetPropVal(wxT("Name"), wxEmptyString) == name) {
            if(child->GetPropVal(wxT("Active"), wxEmptyString).CmpNoCase(wxT("Yes")) == 0) {
                // the removed project was active,
                // select new project to be active
                if(!m_projects.empty()) {
                    std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin();
                    SetActiveProject(iter->first, true);
                }
            }
            root->RemoveChild(child);
            delete child;
            break;
        }
        child = child->GetNext();
    }

    // go over the dependencies list of each project and remove the project
    iter = m_projects.begin();
    for(; iter != m_projects.end(); iter++) {
        ProjectPtr p = iter->second;
        if(p) {
            wxArrayString configs;
            // populate the choice control with the list of available configurations for this project
            ProjectSettingsPtr settings = p->GetSettings();
            if(settings) {
                ProjectSettingsCookie cookie;
                BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
                while(bldConf) {
                    configs.Add(bldConf->GetName());
                    bldConf = settings->GetNextBuildConfiguration(cookie);
                }
            }

            // update each configuration of this project
            for(size_t i = 0; i < configs.GetCount(); i++) {

                wxArrayString deps = p->GetDependencies(configs.Item(i));
                int where = deps.Index(name);
                if(where != wxNOT_FOUND) {
                    deps.RemoveAt((size_t)where);
                }

                // update the configuration
                p->SetDependencies(deps, configs.Item(i));
            }
        }
    }
    return SaveXmlFile();
}

wxString clCxxWorkspace::GetActiveProjectName() const
{
    if(!m_doc.IsOk()) {
        return wxEmptyString;
    }

    wxXmlNode* root = m_doc.GetRoot();
    wxXmlNode* child = root->GetChildren();
    while(child) {
        if(child->GetName() == wxT("Project") &&
           child->GetPropVal(wxT("Active"), wxEmptyString).CmpNoCase(wxT("Yes")) == 0) {
            return child->GetPropVal(wxT("Name"), wxEmptyString);
        }
        child = child->GetNext();
    }
    return wxEmptyString;
}

void clCxxWorkspace::SetActiveProject(const wxString& name, bool active)
{
    if(!m_doc.IsOk()) return;

    // update the xml file
    wxXmlNode* root = m_doc.GetRoot();
    wxXmlNode* child = root->GetChildren();
    while(child) {
        if(child->GetName() == wxT("Project") && child->GetPropVal(wxT("Name"), wxEmptyString) == name) {
            XmlUtils::UpdateProperty(child, wxT("Active"), active ? wxT("Yes") : wxT("No"));
            break;
        }
        child = child->GetNext();
    }
    SaveXmlFile();
}

bool clCxxWorkspace::CreateVirtualDirectory(const wxString& vdFullPath, wxString& errMsg, bool mkPath)
{
    wxStringTokenizer tkz(vdFullPath, wxT(":"));
    wxString projName = tkz.GetNextToken();

    wxString fixedPath;
    // Construct new path excluding the first token
    size_t count = tkz.CountTokens();

    for(size_t i = 0; i < count - 1; i++) {
        fixedPath += tkz.GetNextToken();
        fixedPath += wxT(":");
    }
    fixedPath += tkz.GetNextToken();

    ProjectPtr proj = FindProjectByName(projName, errMsg);
    return proj->CreateVirtualDir(fixedPath, mkPath);
}

bool clCxxWorkspace::RemoveVirtualDirectory(const wxString& vdFullPath, wxString& errMsg)
{
    wxStringTokenizer tkz(vdFullPath, wxT(":"));
    wxString projName = tkz.GetNextToken();

    wxString fixedPath;
    // Construct new path excluding the first token
    size_t count = tkz.CountTokens();

    for(size_t i = 0; i < count - 1; i++) {
        fixedPath += tkz.GetNextToken();
        fixedPath += wxT(":");
    }
    fixedPath += tkz.GetNextToken();

    ProjectPtr proj = FindProjectByName(projName, errMsg);
    return proj->DeleteVirtualDir(fixedPath);
}

bool clCxxWorkspace::SaveXmlFile()
{
    // We first remove the Save Workspace To Local Workspace (SWTLW) attribute
    // and then check the current state in the Code Completion tab. Then
    // we read new path values from the LW and set the appropiate attribute value.
    if(m_doc.GetRoot()->GetAttribute(wxT("SWTLW")) != wxEmptyString) {
        m_doc.GetRoot()->DeleteAttribute(wxT("SWTLW"));
    }
    if(LocalWorkspaceST::Get()->GetParserFlags() & LocalWorkspace::EnableSWTLW) {
        m_doc.GetRoot()->AddProperty(wxT("SWTLW"), "Yes");
        SyncFromLocalWorkspaceSTParserPaths();
        SyncFromLocalWorkspaceSTParserMacros();
    }

    bool ok = m_doc.Save(m_fileName.GetFullPath());
    SetWorkspaceLastModifiedTime(GetFileLastModifiedTime());
    EventNotifier::Get()->PostFileSavedEvent(m_fileName.GetFullPath());

    DoUpdateBuildMatrix();
    return ok;
}

void clCxxWorkspace::SyncToLocalWorkspaceSTParserPaths()
{
    wxArrayString inclduePaths;
    wxArrayString excludePaths;
    wxXmlNode* workspaceInclPaths = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserPaths"));
    if(workspaceInclPaths) {
        wxXmlNode* child = workspaceInclPaths->GetChildren();
        while(child) {
            if(child->GetName() == wxT("Exclude")) {
                wxString path = child->GetPropVal(wxT("Path"), wxT(""));
                path.Trim().Trim(false);
                if(path.IsEmpty() == false) {
                    excludePaths.Add(path);
                }
            }

            else if(child->GetName() == wxT("Include")) {
                wxString path = child->GetPropVal(wxT("Path"), wxT(""));
                path.Trim().Trim(false);
                if(path.IsEmpty() == false) {
                    inclduePaths.Add(path);
                }
            }

            child = child->GetNext();
        }
        LocalWorkspaceST::Get()->SetParserPaths(inclduePaths, excludePaths);
    }
}

void clCxxWorkspace::SyncFromLocalWorkspaceSTParserPaths()
{
    //
    // Here we just get the parser paths from the LocalWorkspaceST and write it into the worspace project file.
    //
    wxXmlNode* workspaceInclPaths = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserPaths"));
    if(workspaceInclPaths) {
        m_doc.GetRoot()->RemoveChild(workspaceInclPaths);
        delete workspaceInclPaths;
    }

    //
    // Get workspace parse paths from local workspace file.
    //
    wxArrayString inclduePaths;
    wxArrayString excludePaths;
    LocalWorkspaceST::Get()->GetParserPaths(inclduePaths, excludePaths);

    workspaceInclPaths = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("WorkspaceParserPaths"));
    for(size_t i = 0; i < inclduePaths.GetCount(); i++) {
        wxXmlNode* child = new wxXmlNode(workspaceInclPaths, wxXML_ELEMENT_NODE, wxT("Include"));
        child->AddProperty(wxT("Path"), inclduePaths.Item(i));
    }

    for(size_t i = 0; i < excludePaths.GetCount(); i++) {
        wxXmlNode* child = new wxXmlNode(workspaceInclPaths, wxXML_ELEMENT_NODE, wxT("Exclude"));
        child->AddProperty(wxT("Path"), excludePaths.Item(i));
    }
}

void clCxxWorkspace::SyncToLocalWorkspaceSTParserMacros()
{
    wxString macros;
    wxXmlNode* workspaceMacros = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserMacros"));
    if(workspaceMacros) {
        macros = workspaceMacros->GetNodeContent();
        macros.Trim().Trim(false);
        LocalWorkspaceST::Get()->SetParserMacros(macros);
    }
}

void clCxxWorkspace::SyncFromLocalWorkspaceSTParserMacros()
{
    //
    // Here we just get the parser macros from the LocalWorkspaceST and write it into the worspace project file.
    //
    wxXmlNode* workspaceMacros = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserMacros"));
    if(workspaceMacros) {
        m_doc.GetRoot()->RemoveChild(workspaceMacros);
        delete workspaceMacros;
    }

    //
    // Get workspace parse macros from local workspace file.
    //
    wxString macros;
    LocalWorkspaceST::Get()->GetParserMacros(macros);
    workspaceMacros = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("WorkspaceParserMacros"));
    if(!macros.IsEmpty()) {
        wxXmlNode* contentNode = new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxEmptyString, macros);
        workspaceMacros->AddChild(contentNode);
    }
}

void clCxxWorkspace::Save()
{
    if(m_doc.IsOk()) {
        std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin();
        for(; iter != m_projects.end(); iter++) {
            iter->second->Save();
        }
        SaveXmlFile();
    }
}

bool clCxxWorkspace::AddNewFile(const wxString& vdFullPath, const wxString& fileName, wxString& errMsg)
{
    wxStringTokenizer tkz(vdFullPath, wxT(":"));

    // We should have at least 2 tokens:
    // project:virtual directory
    if(tkz.CountTokens() < 2) return false;

    wxString projName = tkz.GetNextToken();
    wxString fixedPath;
    // Construct new path excluding the first token
    size_t count = tkz.CountTokens();

    for(size_t i = 0; i < count - 1; i++) {
        fixedPath += tkz.GetNextToken();
        fixedPath += wxT(":");
    }
    fixedPath += tkz.GetNextToken();

    ProjectPtr proj = FindProjectByName(projName, errMsg);
    if(!proj) {
        errMsg = wxT("No such project");
        return false;
    }

    return proj->AddFile(fileName, fixedPath);
}

bool clCxxWorkspace::RemoveFile(const wxString& vdFullPath, const wxString& fileName, wxString& errMsg)
{
    wxStringTokenizer tkz(vdFullPath, wxT(":"));
    wxString projName = tkz.GetNextToken();
    wxString fixedPath;

    // Construct new path excluding the first token
    size_t count = tkz.CountTokens();
    if(!count) {
        errMsg = _("Malformed project name");
        return false;
    }

    for(size_t i = 0; i < count - 1; i++) {
        fixedPath += tkz.GetNextToken();
        fixedPath += wxT(":");
    }
    fixedPath += tkz.GetNextToken();

    ProjectPtr proj = FindProjectByName(projName, errMsg);
    if(!proj) {
        errMsg = _("No such project");
        return false;
    }

    bool result = proj->RemoveFile(fileName, fixedPath);
    if(!result) {
        errMsg = _("File removal failed");
    }
    return result;
}

BuildConfigPtr clCxxWorkspace::GetProjBuildConf(const wxString& projectName, const wxString& confName) const
{
    BuildMatrixPtr matrix = GetBuildMatrix();
    if(!matrix) {
        return NULL;
    }

    wxString projConf(confName);

    if(projConf.IsEmpty()) {
        wxString workspaceConfig = matrix->GetSelectedConfigurationName();
        projConf = matrix->GetProjectSelectedConf(workspaceConfig, projectName);
    }

    // Get the project setting and retrieve the selected configuration
    wxString errMsg;
    ProjectPtr proj = FindProjectByName(projectName, errMsg);
    if(proj) {
        ProjectSettingsPtr settings = proj->GetSettings();
        if(settings) {
            return settings->GetBuildConfiguration(projConf, true);
        }
    }
    return NULL;
}

void clCxxWorkspace::ReloadWorkspace()
{
    m_doc = wxXmlDocument();

    wxLogNull noLog;
    // reset the internal cache objects
    m_projects.clear();

    TagsManager* mgr = TagsManagerST::Get();
    mgr->CloseDatabase();

    wxString err_msg;
    if(!OpenWorkspace(m_fileName.GetFullPath(), err_msg)) {
        wxLogMessage(wxT("Reload workspace: ") + err_msg);
    }
}

time_t clCxxWorkspace::GetFileLastModifiedTime() const { return GetFileModificationTime(GetWorkspaceFileName()); }

// Singelton access
static clCxxWorkspace* gs_Workspace = NULL;
void clCxxWorkspaceST::Free()
{
    if(gs_Workspace) {
        delete gs_Workspace;
    }
    gs_Workspace = NULL;
}

clCxxWorkspace* clCxxWorkspaceST::Get()
{
    if(gs_Workspace == NULL) gs_Workspace = new clCxxWorkspace;
    return gs_Workspace;
}

wxString clCxxWorkspace::GetParserMacros()
{
    if(!m_doc.IsOk()) return wxEmptyString;

    wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserMacros"));
    if(node) {
        wxString nodeContent = node->GetNodeContent();
        nodeContent.Trim().Trim(false);
        return nodeContent;
    }
    return wxEmptyString;
}

wxString clCxxWorkspace::GetEnvironmentVariabels()
{
    if(!m_doc.IsOk()) return wxEmptyString;

    // Use the environment variables set in the build matrix ("workspace configuration")
    wxString env;
    if(GetSelectedConfig()) {
        env = GetSelectedConfig()->GetEnvironmentVariables();
        if(!env.IsEmpty()) {
            return env;
        }
    }

    wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Environment"));
    if(node) {
        wxString nodeContent = node->GetNodeContent();
        nodeContent.Trim().Trim(false);
        return nodeContent;
    }
    return wxEmptyString;
}

void clCxxWorkspace::SetEnvironmentVariabels(const wxString& envvars)
{
    if(!m_doc.IsOk()) return;
    if(GetSelectedConfig()) {
        GetSelectedConfig()->SetEnvironmentVariables(envvars);
        SetBuildMatrix(GetBuildMatrix()); // force an XML update
        SaveXmlFile();
        return;
    }

    wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Environment"));
    if(node) {
        m_doc.GetRoot()->RemoveChild(node);
        delete node;
    }

    node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Environment"));
    m_doc.GetRoot()->AddChild(node);

    wxString nodeContent = envvars;
    nodeContent.Trim().Trim(false);

    wxXmlNode* contentNode = new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxEmptyString, nodeContent);
    node->AddChild(contentNode);
    SaveXmlFile();
}

wxArrayString clCxxWorkspace::GetAllProjectPaths()
{
    wxArrayString projects;
    std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); iter++) {
        projects.Add(iter->second->GetFileName().GetFullPath());
    }
    return projects;
}

bool clCxxWorkspace::IsOpen() const { return m_doc.IsOk(); }

bool clCxxWorkspace::IsVirtualDirectoryExists(const wxString& vdFullPath)
{
    wxStringTokenizer tkz(vdFullPath, wxT(":"));
    wxString projName = tkz.GetNextToken();

    wxString fixedPath;
    // Construct new path excluding the first token
    size_t count = tkz.CountTokens();

    for(size_t i = 0; i < count - 1; i++) {
        fixedPath += tkz.GetNextToken();
        fixedPath += wxT(":");
    }
    fixedPath += tkz.GetNextToken();

    wxString errMsg;
    ProjectPtr proj = FindProjectByName(projName, errMsg);
    if(!proj) {
        return false;
    }

    wxXmlNode* vdNode = proj->GetVirtualDir(fixedPath);
    return vdNode != NULL;
}

wxString clCxxWorkspace::GetPrivateFolder() const
{
    wxFileName workspacePath;
    if(IsOpen()) {
        workspacePath = GetWorkspaceFileName();
    } else {
        // try maybe the workspace is opened by a plugin
        clCommandEvent event(wxEVT_CMD_IS_WORKSPACE_OPEN);
        event.SetAnswer(false);
        EventNotifier::Get()->ProcessEvent(event);
        if(event.IsAnswer()) {
            workspacePath = event.GetFileName();
        }
    }
    if(workspacePath.Exists()) {
        // append the .codelite folder
        workspacePath.AppendDir(".codelite");

        // ensure the path exists
        workspacePath.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        return workspacePath.GetPath();
    }
    return "";
}

wxFileName clCxxWorkspace::GetTagsFileName() const
{
    if(!IsOpen()) {
        return wxFileName();
    }

    wxFileName fn_tags(GetPrivateFolder(), GetWorkspaceFileName().GetFullName());
    fn_tags.SetExt("tags");
    return fn_tags;
}

void clCxxWorkspace::CreateCompileCommandsJSON(JSONElement& compile_commands) const
{
    BuildMatrixPtr matrix = clCxxWorkspaceST::Get()->GetBuildMatrix();
    if(!matrix) return;

    wxString workspaceSelConf = matrix->GetSelectedConfigurationName();
    clCxxWorkspace::ProjectMap_t::const_iterator iter = m_projects.begin();

    for(; iter != m_projects.end(); ++iter) {
        BuildConfigPtr buildConf = iter->second->GetBuildConfiguration();
        if(buildConf && buildConf->IsProjectEnabled() && !buildConf->IsCustomBuild() &&
           buildConf->IsCompilerRequired()) {
            iter->second->CreateCompileCommandsJSON(compile_commands);
        }
    }
}

ProjectPtr clCxxWorkspace::GetActiveProject() const { return GetProject(GetActiveProjectName()); }

ProjectPtr clCxxWorkspace::GetProject(const wxString& name) const
{
    clCxxWorkspace::ProjectMap_t::const_iterator iter = m_projects.find(name);
    if(iter == m_projects.end()) {
        return NULL;
    }
    return iter->second;
}

void clCxxWorkspace::GetCompilers(wxStringSet_t& compilers)
{
    clCxxWorkspace::ProjectMap_t::iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        iter->second->GetCompilers(compilers);
    }
}

void clCxxWorkspace::ReplaceCompilers(wxStringMap_t& compilers)
{
    clCxxWorkspace::ProjectMap_t::iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        iter->second->ReplaceCompilers(compilers);
    }
}

void clCxxWorkspace::DoUpdateBuildMatrix()
{
    m_buildMatrix.Reset(new BuildMatrix(XmlUtils::FindFirstByTagName(m_doc.GetRoot(), "BuildMatrix")));
}

void clCxxWorkspace::RenameProject(const wxString& oldname, const wxString& newname)
{
    // Update the build matrix (we work on the XML directly here)
    wxXmlNode* buildMatrixNode = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), "BuildMatrix");
    if(buildMatrixNode) {
        wxXmlNode* child = buildMatrixNode->GetChildren();
        while(child) {
            if(child->GetName() == "WorkspaceConfiguration") {
                wxXmlNode* projectNode = child->GetChildren();
                while(projectNode) {
                    if(projectNode->GetName() == "Project") {
                        wxString name = projectNode->GetAttribute("Name");
                        if(name == oldname) {
                            XmlUtils::UpdateProperty(projectNode, "Name", newname);
                        }
                    }
                    projectNode = projectNode->GetNext();
                }
            }
            child = child->GetNext();
        }
    }

    // Update the list of projects in the workspace
    wxXmlNode* projectNode = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), "Project");
    while(projectNode) {
        if(projectNode->GetAttribute("Name") == oldname) {
            XmlUtils::UpdateProperty(projectNode, "Name", newname);
        }
        projectNode = projectNode->GetNext();
    }
    // Update dependenices for each project
    clCxxWorkspace::ProjectMap_t::iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        iter->second->ProjectRenamed(oldname, newname);
    }

    clCxxWorkspace::ProjectMap_t tmpProjects;
    iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        tmpProjects.insert(std::make_pair(iter->first, iter->second));
    }
    m_projects.swap(tmpProjects);

    // Save everything
    Save();

    // Notify about this change
    clCommandEvent event(wxEVT_PROJ_RENAMED);
    event.SetOldName(oldname);
    event.SetString(newname);
    EventNotifier::Get()->AddPendingEvent(event);
}

bool clCxxWorkspace::IsBuildSupported() const { return true; }
bool clCxxWorkspace::IsProjectSupported() const { return true; }

wxString clCxxWorkspace::GetFilesMask() const
{
    wxString findInFilesMask = "*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.inc;*.mm;*.m;*.xrc;*.ini;*.xml";
    if(IsOpen()) {
        wxString fifMask;
        LocalWorkspaceST::Get()->GetSearchInFilesMask(fifMask, findInFilesMask);
        if(fifMask.IsEmpty()) {
            fifMask = findInFilesMask;
        }
    }
    return findInFilesMask;
}
wxString clCxxWorkspace::GetProjectFromFile(const wxFileName& filename) const
{
    wxString filenameFP = filename.GetFullPath();
    clCxxWorkspace::ProjectMap_t::const_iterator iter = m_projects.begin();
    for(; iter != m_projects.end(); ++iter) {
        wxStringSet_t allFiles;
        iter->second->GetFiles(allFiles);
        if(allFiles.count(filenameFP)) {
            return iter->first;
        }
    }
    return "";
}
void clCxxWorkspace::GetProjectFiles(const wxString& projectName, wxArrayString& files) const
{
    ProjectPtr p = GetProject(projectName.IsEmpty() ? GetActiveProjectName() : projectName);
    CHECK_PTR_RET(p);

    wxStringSet_t setFiles;
    p->GetFiles(setFiles);

    // Convert the set wxArrayString
    std::for_each(setFiles.begin(), setFiles.end(), [&](const wxString& file) { files.Add(file); });
}

void clCxxWorkspace::GetWorkspaceFiles(wxArrayString& files) const
{
    std::for_each(m_projects.begin(), m_projects.end(), [&](const clCxxWorkspace::ProjectMap_t::value_type& v) {
        wxStringSet_t setFiles;
        v.second->GetFiles(setFiles);

        // Convert the set wxArrayString
        std::for_each(setFiles.begin(), setFiles.end(), [&](const wxString& file) { files.Add(file); });
    });
}

WorkspaceConfigurationPtr clCxxWorkspace::GetSelectedConfig() const
{
    if(!GetBuildMatrix()) return NULL;
    wxString buildConf = GetBuildMatrix()->GetSelectedConfigurationName();
    return GetBuildMatrix()->GetConfigurationByName(buildConf);
}
