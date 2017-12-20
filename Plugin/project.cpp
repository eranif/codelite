//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : project.cpp
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
#include "cl_command_event.h"
#include "compiler_command_line_parser.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "fileextmanager.h"
#include "globals.h"
#include "macromanager.h"
#include "macros.h"
#include "plugin.h"
#include "project.h"
#include "workspace.h"
#include "wx/arrstr.h"
#include "wxArrayStringAppender.h"
#include "wx_xml_compatibility.h"
#include "xmlutils.h"
#include <algorithm>
#include <wx/app.h>
#include <wx/ffile.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/sstream.h>
#include <wx/tokenzr.h>

static wxStringMap_t s_backticks;

#define EXCLUDE_FROM_BUILD_FOR_CONFIG "ExcludeProjConfig"

// ============---------------------
// Project class
// ============---------------------

Project::Project()
    : m_tranActive(false)
    , m_isModified(false)
    , m_workspace(NULL)
{
    // initialize it with default settings
    m_settings.Reset(new ProjectSettings(NULL));
}

Project::~Project() { m_settings.Reset(NULL); }

bool Project::Create(const wxString& name, const wxString& description, const wxString& path, const wxString& projType)
{
    m_fileName = wxFileName(path, name);
    m_fileName.SetExt("project");
    m_fileName.MakeAbsolute();

    // Ensure that the target folder exists
    m_fileName.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    m_projectPath = m_fileName.GetPath();

    wxXmlNode* root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CodeLite_Project"));
    m_doc.SetRoot(root);
    m_doc.GetRoot()->AddProperty(wxT("Name"), name);
    m_doc.GetRoot()->AddAttribute("Version", WORKSPACE_XML_VERSION);

    wxXmlNode* descNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Description"));
    XmlUtils::SetNodeContent(descNode, description);
    m_doc.GetRoot()->AddChild(descNode);

    // Create the default virtual directories
    wxXmlNode *srcNode = NULL, *headNode = NULL;

    srcNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
    srcNode->AddProperty(wxT("Name"), wxT("src"));
    m_doc.GetRoot()->AddChild(srcNode);

    headNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
    headNode->AddProperty(wxT("Name"), wxT("include"));
    m_doc.GetRoot()->AddChild(headNode);

    // creae dependencies node
    wxXmlNode* depNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Dependencies"));
    root->AddChild(depNode);

    // this will also create settings
    SaveXmlFile();

    ProjectSettingsPtr settings = GetSettings();
    settings->SetProjectType(projType);

    DoBuildCacheFromXml();
    SetSettings(settings);
    SetModified(true);
    return true;
}

bool Project::Load(const wxString& path)
{
    if(!m_doc.Load(path)) {
        return false;
    }

    // ConvertToUnixFormat(m_doc.GetRoot());

    // Workaround WX bug: load the plugins data (GetAllPluginsData will strip any trailing whitespaces)
    // and then set them back
    std::map<wxString, wxString> pluginsData;
    GetAllPluginsData(pluginsData);
    SetAllPluginsData(pluginsData, false);

    m_fileName = path;
    m_fileName.MakeAbsolute();
    m_projectPath = m_fileName.GetPath();

    DoBuildCacheFromXml();
    SetModified(true);
    SetProjectLastModifiedTime(GetFileLastModifiedTime());

    DoUpdateProjectSettings();
    return true;
}

wxXmlNode* Project::GetVirtualDir(const wxString& vdFullPath)
{
    if(m_virtualFoldersTable.count(vdFullPath) == 0) {
        return nullptr;
    }
    return m_virtualFoldersTable[vdFullPath]->GetXmlNode();
}

wxXmlNode* Project::CreateVD(const wxString& vdFullPath, bool mkpath)
{
    wxXmlNode* oldVd = GetVirtualDir(vdFullPath);
    if(oldVd) {
        // VD already exist
        return oldVd;
    }

    wxStringTokenizer tkz(vdFullPath, wxT(":"));

    size_t count = tkz.CountTokens();
    clProjectFolder::Ptr_t currentFolder = GetRootFolder();
    for(size_t i = 0; i < (count - 1); ++i) {
        wxString token = tkz.GetNextToken();
        if(!currentFolder->IsFolderExists(this, token)) {
            if(mkpath) {
                // Create the child folder
                currentFolder = currentFolder->AddFolder(this, token);
            } else {
                // Don't create it, return nullptr
                return nullptr;
            }
        } else {
            // Move to the next folder
            currentFolder = currentFolder->GetChild(this, token);
        }
    }
    currentFolder = currentFolder->AddFolder(this, tkz.GetNextToken());

    // if not in transaction save the changes
    if(!InTransaction()) {
        SaveXmlFile();
    }
    return currentFolder->GetXmlNode();
}

bool Project::IsFileExist(const wxString& fileName) { return m_filesTable.count(fileName); }

bool Project::AddFile(const wxString& fileName, const wxString& virtualDirPath)
{
    clProjectFolder::Ptr_t folder = GetFolder(virtualDirPath);
    if(!folder) {
        return false;
    }

    clProjectFile::Ptr_t file = folder->AddFile(this, fileName);
    if(!file) {
        return false;
    }

    if(!InTransaction()) {
        SaveXmlFile();
    }
    SetModified(true);
    return true;
}

bool Project::CreateVirtualDir(const wxString& vdFullPath, bool mkpath) { return CreateVD(vdFullPath, mkpath) != NULL; }

bool Project::DeleteVirtualDir(const wxString& vdFullPath)
{
    clProjectFolder::Ptr_t folder = GetFolder(vdFullPath);
    if(!folder) {
        return false;
    }
    folder->DeleteRecursive(this);
    SetModified(true);
    return SaveXmlFile();
}

bool Project::RemoveFile(const wxString& fileName, const wxString& virtualDir)
{
    clProjectFile::Ptr_t file = GetFile(fileName);
    if(!file) {
        return false;
    }
    file->Delete(this, true);

    // Erase it from our parent folder
    clProjectFolder::Ptr_t parentFolder = GetFolder(virtualDir);
    if(parentFolder) {
        parentFolder->GetFiles().erase(fileName);
    }

    SetModified(true);
    if(InTransaction()) {
        return true;
    } else {
        return SaveXmlFile();
    }
}

wxString Project::GetName() const { return m_doc.GetRoot()->GetPropVal(wxT("Name"), wxEmptyString); }

ProjectTreePtr Project::AsTree()
{
    ProjectItem item(GetName(), GetName(), GetFileName().GetFullPath(), ProjectItem::TypeProject);
    ProjectTreePtr ptp(new ProjectTree(item.Key(), item));

    wxXmlNode* child = m_doc.GetRoot()->GetChildren();
    while(child) {
        RecursiveAdd(child, ptp, ptp->GetRoot());
        child = child->GetNext();
    }
    return ptp;
}

void Project::RecursiveAdd(wxXmlNode* xmlNode, ProjectTreePtr& ptp, ProjectTreeNode* nodeParent)
{
    // Build the key for this node
    std::list<wxString> nameList;

    wxXmlNode* parent = xmlNode->GetParent();
    while(parent) {
        nameList.push_front(parent->GetPropVal(wxT("Name"), wxEmptyString));
        parent = parent->GetParent();
    }

    wxString key;
    for(size_t i = 0; i < nameList.size(); i++) {
        key += nameList.front();
        key += wxT(":");
        nameList.pop_front();
    }
    key += xmlNode->GetPropVal(wxT("Name"), wxEmptyString);

    // Create the tree node data
    ProjectItem item;
    if(xmlNode->GetName() == wxT("Project")) {
        item =
            ProjectItem(key, xmlNode->GetPropVal(wxT("Name"), wxEmptyString), wxEmptyString, ProjectItem::TypeProject);

    } else if(xmlNode->GetName() == wxT("VirtualDirectory")) {
        item = ProjectItem(key, xmlNode->GetPropVal(wxT("Name"), wxEmptyString), wxEmptyString,
                           ProjectItem::TypeVirtualDirectory);

    } else if(xmlNode->GetName() == wxT("File")) {
        wxFileName filename(xmlNode->GetPropVal(wxT("Name"), wxEmptyString));
        // Convert this file name to absolute path
        filename.MakeAbsolute(m_fileName.GetPath());
        item = ProjectItem(key, filename.GetFullName(), filename.GetFullPath(), ProjectItem::TypeFile);

    } else {
        // un-recognised or not viewable item in the tree,
        // skip it and its children
        return;
    }

    ProjectTreeNode* newNode = ptp->AddChild(item.Key(), item, nodeParent);
    // This node has children, add them as well
    wxXmlNode* children = xmlNode->GetChildren();

    while(children) {
        RecursiveAdd(children, ptp, newNode);
        children = children->GetNext();
    }
    SetModified(true);
}

bool Project::SaveXmlFile()
{
    wxString projectXml;
    wxStringOutputStream sos(&projectXml);

    // Set the workspace XML version
    wxString version;
    if(!m_doc.GetRoot()->GetAttribute("Version", &version)) {
        m_doc.GetRoot()->AddAttribute("Version", WORKSPACE_XML_VERSION);
    }

    bool ok = m_doc.Save(sos);

    wxFFile file(m_fileName.GetFullPath(), wxT("w+b"));
    if(!file.IsOpened()) {
        ok = false;

    } else {
        file.Write(projectXml);
        file.Close();
    }

    SetProjectLastModifiedTime(GetFileLastModifiedTime());
    EventNotifier::Get()->PostFileSavedEvent(m_fileName.GetFullPath());

    DoUpdateProjectSettings();
    return ok;
}

void Project::Save()
{
    m_tranActive = false;
    if(m_doc.IsOk()) {
        SaveXmlFile();
    }
}

void Project::GetFilesByVirtualDir(const wxString& vdFullPath, wxArrayString& files)
{
    wxXmlNode* vd = GetVirtualDir(vdFullPath);
    if(vd) {
        wxXmlNode* child = vd->GetChildren();
        while(child) {
            if(child->GetName() == wxT("File")) {
                wxFileName fileName(child->GetPropVal(wxT("Name"), wxEmptyString));
                fileName.MakeAbsolute(m_fileName.GetPath());
                files.Add(fileName.GetFullPath());
            }
            child = child->GetNext();
        }
    }
}

wxXmlNode* Project::GetProjectEditorOptions() const
{
    return XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Options"));
}

void Project::SetProjectEditorOptions(LocalOptionsConfigPtr opts)
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

ProjectSettingsPtr Project::GetSettings() const { return m_settings; }

void Project::SetSettings(ProjectSettingsPtr settings)
{
    wxXmlNode* oldSettings = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Settings"));
    if(oldSettings) {
        oldSettings->GetParent()->RemoveChild(oldSettings);
        delete oldSettings;
    }
    m_doc.GetRoot()->AddChild(settings->ToXml());

    // SaveXmlFile will update the internal pointer
    SaveXmlFile();
}

void Project::SetGlobalSettings(BuildConfigCommonPtr globalSettings)
{
    wxXmlNode* settings = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Settings"));
    wxXmlNode* oldSettings = XmlUtils::FindFirstByTagName(settings, wxT("GlobalSettings"));
    if(oldSettings) {
        oldSettings->GetParent()->RemoveChild(oldSettings);
        delete oldSettings;
    }
    settings->AddChild(globalSettings->ToXml());
    SaveXmlFile();
}

wxArrayString Project::GetDependencies() const
{
    wxArrayString result;
    wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Dependencies"));
    if(node) {
        wxXmlNode* child = node->GetChildren();
        while(child) {
            if(child->GetName() == wxT("Project")) {
                result.Add(XmlUtils::ReadString(child, wxT("Name")));
            }
            child = child->GetNext();
        }
    }
    return result;
}

void Project::SetModified(bool mod) { m_isModified = mod; }

bool Project::IsModified() { return m_isModified; }

wxString Project::GetDescription() const
{
    wxXmlNode* root = m_doc.GetRoot();
    if(root) {
        wxXmlNode* node = XmlUtils::FindFirstByTagName(root, wxT("Description"));
        if(node) {
            return node->GetNodeContent();
        }
    }
    return wxEmptyString;
}

void Project::CopyTo(const wxString& new_path, const wxString& new_name, const wxString& description)
{
    // first save the xml document to the destination folder
    wxFileName newFile(new_path, new_name);
    newFile.SetExt("project");
    if(!m_doc.Save(newFile.GetFullPath())) {
        return;
    }

    // load the new xml and modify it
    wxXmlDocument doc;
    if(!doc.Load(newFile.GetFullPath())) {
        return;
    }

    // update the 'Name' property
    XmlUtils::UpdateProperty(doc.GetRoot(), wxT("Name"), new_name);
    XmlUtils::UpdateProperty(doc.GetRoot(), "Version", WORKSPACE_XML_VERSION);

    // set description
    wxXmlNode* descNode(NULL);

    // update the description
    descNode = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("Description"));
    if(!descNode) {
        descNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Description"));
        doc.GetRoot()->AddChild(descNode);
    }
    XmlUtils::SetNodeContent(descNode, description);

    // Remove the 'Dependencies'
    wxXmlNode* deps = doc.GetRoot()->GetChildren();
    while(deps) {
        if(deps->GetName() == wxT("Dependencies")) {
            doc.GetRoot()->RemoveChild(deps);
            delete deps;

            // restart the search from the begining
            deps = doc.GetRoot()->GetChildren();

        } else {
            // try next child
            deps = deps->GetNext();
        }
    }

    // add an empty deps node
    deps = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Dependencies"));
    doc.GetRoot()->AddChild(deps);

    // Remove virtual folders
    wxXmlNode* vd = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("VirtualDirectory"));
    while(vd) {
        doc.GetRoot()->RemoveChild(vd);
        delete vd;
        vd = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("VirtualDirectory"));
    }

    // add all files under this path
    wxXmlNode* srcNode(NULL);
    wxXmlNode* headNode(NULL);
    wxXmlNode* rcNode(NULL);

    // copy the files to their new location
    std::for_each(m_filesTable.begin(), m_filesTable.end(), [&](const FilesMap_t::value_type& vt) {
        wxFileName fn = vt.first;
        wxCopyFile(fn.GetFullPath(), new_path + wxT("/") + fn.GetFullName());

        wxXmlNode* file_node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
        file_node->AddProperty(wxT("Name"), fn.GetFullName());

        switch(FileExtManager::GetType(fn.GetFullName())) {
        case FileExtManager::TypeSourceC:
        case FileExtManager::TypeSourceCpp:

            // source file
            if(!srcNode) {
                srcNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
                srcNode->AddProperty(wxT("Name"), wxT("src"));
                doc.GetRoot()->AddChild(srcNode);
            }
            srcNode->AddChild(file_node);
            break;

        case FileExtManager::TypeHeader:
            // header file
            if(!headNode) {
                headNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
                headNode->AddProperty(wxT("Name"), wxT("include"));
                doc.GetRoot()->AddChild(headNode);
            }
            headNode->AddChild(file_node);
            break;

        default:
            // resource file
            if(!rcNode) {
                rcNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
                rcNode->AddProperty(wxT("Name"), wxT("resources"));
                doc.GetRoot()->AddChild(rcNode);
            }
            rcNode->AddChild(file_node);
            break;
        }
    });

    doc.Save(newFile.GetFullPath());
}

clProjectFile::Ptr_t Project::FileFromXml(wxXmlNode* node, const wxString& vd)
{
    clProjectFile::Ptr_t file(new clProjectFile());

    // files are kept relative to the project file
    // Convert the path to unix format
    wxString fileName;
    wxXmlProperty* props = node->GetProperties();
    while(props) {
        if(props->GetName() == wxT("Name")) {
            wxString val = props->GetValue();
            val.Replace(wxT("\\"), wxT("/"));
            props->SetValue(val);
            fileName = val;
            break;
        }
        props = props->GetNext();
    }

    wxFileName tmp(fileName);
    tmp.MakeAbsolute(GetProjectPath());

    file->SetFilenameRelpath(fileName);
    file->SetFilename(tmp.GetFullPath());
    file->SetFlags(XmlUtils::ReadLong(node, "Flags", 0));
    file->SetXmlNode(node);

    wxString excludeConfigs = XmlUtils::ReadString(node, EXCLUDE_FROM_BUILD_FOR_CONFIG);
    file->SetExcludeConfigs(this, ::wxStringTokenize(excludeConfigs, ";", wxTOKEN_STRTOK));
    file->SetVirtualFolder(vd);
    return file;
}

void Project::DoBuildCacheFromXml()
{
    m_filesTable.clear();
    m_virtualFoldersTable.clear();

    // Update the cache from the XML
    std::queue<std::pair<wxXmlNode*, clProjectFolder::Ptr_t> > Q;
    Q.push({ m_doc.GetRoot(), GetRootFolder() });
    while(!Q.empty()) {
        wxXmlNode* node = Q.front().first;
        clProjectFolder::Ptr_t folder = Q.front().second;
        Q.pop();

        wxXmlNode* child = node->GetChildren();
        while(child) {
            if(child->GetName() == "File" && folder) {
                clProjectFile::Ptr_t file = FileFromXml(child, folder->GetFullpath());
                // Cache the file
                m_filesTable.insert({ file->GetFilename(), file });
                // Add this file to the folder
                folder->GetFiles().insert(file->GetFilename());

            } else if(child->GetName() == "VirtualDirectory") {
                wxString folderName = child->GetPropVal("Name", wxEmptyString);
                clProjectFolder::Ptr_t newFolder(new clProjectFolder(
                    folder->GetFullpath().IsEmpty() ? folderName : folder->GetFullpath() + ":" + folderName, child));
                // Cache this folder
                m_virtualFoldersTable.insert({ newFolder->GetFullpath(), newFolder });
                Q.push({ child, newFolder });
            }
            child = child->GetNext();
        }
    }
}

void Project::SetFiles(ProjectPtr src)
{
    // first remove all the virtual directories from this project
    // Remove virtual folders
    wxXmlNode* vd = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("VirtualDirectory"));
    while(vd) {
        m_doc.GetRoot()->RemoveChild(vd);
        delete vd;
        vd = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("VirtualDirectory"));
    }
    m_filesTable.clear();
    m_virtualFoldersTable.clear();

    // sanity
    if(!src || !src->m_doc.GetRoot()) return;

    // copy the virtual directories from the src project
    wxXmlNode* child = src->m_doc.GetRoot()->GetChildren();
    while(child) {
        if(child->GetName() == wxT("VirtualDirectory")) {
            // create a new VirtualDirectory like this one
            wxXmlNode* newNode = new wxXmlNode(*child);
            m_doc.GetRoot()->AddChild(newNode);
        }
        child = child->GetNext();
    }
    SaveXmlFile();
    DoBuildCacheFromXml();
}

bool Project::RenameFile(const wxString& oldName, const wxString& virtualDir, const wxString& newName)
{
    if(m_virtualFoldersTable.count(virtualDir) == 0) {
        return false;
    }
    clProjectFolder::Ptr_t folder = m_virtualFoldersTable[virtualDir];
    folder->RenameFile(this, oldName, newName);

    if(InTransaction()) {
        return true;
    } else {
        return SaveXmlFile();
    }
}

wxString Project::GetVDByFileName(const wxString& file)
{
    if(m_filesTable.count(file) == 0) {
        return "";
    }
    return m_filesTable[file]->GetVirtualFolder();
}

bool Project::RenameVirtualDirectory(const wxString& oldVdPath, const wxString& newName)
{
    if(m_virtualFoldersTable.count(oldVdPath) == 0) {
        return false;
    }
    clProjectFolder::Ptr_t folder = m_virtualFoldersTable[oldVdPath];
    if(folder->Rename(this, newName)) {
        return SaveXmlFile();
    }
    return false;
}

wxArrayString Project::GetDependencies(const wxString& configuration) const
{
    wxArrayString result;

    // dependencies are located directly under the root level
    wxXmlNode* node = m_doc.GetRoot()->GetChildren();
    while(node) {
        if(node->GetName() == wxT("Dependencies") && node->GetPropVal(wxT("Name"), wxEmptyString) == configuration) {
            // we have our match
            wxXmlNode* child = node->GetChildren();
            while(child) {
                if(child->GetName() == wxT("Project")) {
                    result.Add(XmlUtils::ReadString(child, wxT("Name")));
                }
                child = child->GetNext();
            }
            return result;
        }
        node = node->GetNext();
    }

    // if we are here, it means no match for the given configuration
    // return the default dependencies
    return GetDependencies();
}

void Project::SetDependencies(wxArrayString& deps, const wxString& configuration)
{
    // first try to locate the old node
    wxXmlNode* node = m_doc.GetRoot()->GetChildren();
    while(node) {
        if(node->GetName() == wxT("Dependencies") && node->GetPropVal(wxT("Name"), wxEmptyString) == configuration) {
            // we have our match
            node->GetParent()->RemoveChild(node);
            delete node;
            break;
        }
        node = node->GetNext();
    }

    // create new dependencies node
    node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Dependencies"));
    node->AddProperty(wxT("Name"), configuration);
    m_doc.GetRoot()->AddChild(node);

    // create a node for each dependency in the array
    for(size_t i = 0; i < deps.GetCount(); i++) {
        wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
        child->AddProperty(wxT("Name"), deps.Item(i));
        node->AddChild(child);
    }

    // save changes
    SaveXmlFile();
    SetModified(true);
}

bool Project::FastAddFile(const wxString& fileName, const wxString& virtualDir)
{
    return AddFile(fileName, virtualDir);
}

void Project::DoGetVirtualDirectories(wxXmlNode* parent, TreeNode<wxString, VisualWorkspaceNode>* tree)
{
    wxXmlNode* child = parent->GetChildren();
    while(child) {
        if(child->GetName() == wxT("VirtualDirectory")) {

            VisualWorkspaceNode data;
            data.name = XmlUtils::ReadString(child, wxT("Name"));
            data.type = ProjectItem::TypeVirtualDirectory;

            TreeNode<wxString, VisualWorkspaceNode>* node =
                new TreeNode<wxString, VisualWorkspaceNode>(data.name, data, tree);
            tree->AddChild(node);

            // test to see if it has children
            if(child->GetChildren()) {
                DoGetVirtualDirectories(child, node);
            }
        }
        child = child->GetNext();
    }
}

TreeNode<wxString, VisualWorkspaceNode>*
Project::GetVirtualDirectories(TreeNode<wxString, VisualWorkspaceNode>* workspace)
{
    VisualWorkspaceNode data;
    data.name = GetName();
    data.type = ProjectItem::TypeProject;

    TreeNode<wxString, VisualWorkspaceNode>* parent =
        new TreeNode<wxString, VisualWorkspaceNode>(GetName(), data, workspace);
    DoGetVirtualDirectories(m_doc.GetRoot(), parent);
    workspace->AddChild(parent);
    return parent;
}

bool Project::GetUserData(const wxString& name, SerializedObject* obj)
{
    if(!m_doc.IsOk()) {
        return false;
    }

    Archive arch;
    wxXmlNode* userData = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("UserData"));
    if(userData) {
        wxXmlNode* dataNode = XmlUtils::FindNodeByName(userData, wxT("Data"), name);
        if(dataNode) {
            arch.SetXmlNode(dataNode);
            obj->DeSerialize(arch);
            return true;
        }
    }
    return false;
}

bool Project::SetUserData(const wxString& name, SerializedObject* obj)
{
    if(!m_doc.IsOk()) {
        return false;
    }

    Archive arch;

    // locate the 'UserData' node
    wxXmlNode* userData = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("UserData"));
    if(!userData) {
        userData = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("UserData"));
    }

    // try to find a previous data stored under the same name, if we succeed - remove it
    wxXmlNode* dataNode = XmlUtils::FindNodeByName(userData, wxT("Data"), name);
    if(dataNode) {
        // remove old node
        userData->RemoveChild(dataNode);
        delete dataNode;
    }

    // create a new node and set the userData node as the parent
    dataNode = new wxXmlNode(userData, wxXML_ELEMENT_NODE, wxT("Data"));
    dataNode->AddProperty(wxT("Name"), name);

    // serialize the data
    arch.SetXmlNode(dataNode);
    obj->Serialize(arch);
    return SaveXmlFile();
}

void Project::SetProjectInternalType(const wxString& internalType)
{
    if(!m_doc.GetRoot()) return;
    XmlUtils::UpdateProperty(m_doc.GetRoot(), wxT("InternalType"), internalType);
}

wxString Project::GetProjectInternalType() const
{
    if(!m_doc.GetRoot()) return "";
    return m_doc.GetRoot()->GetPropVal(wxT("InternalType"), wxEmptyString);
}

void Project::GetAllPluginsData(std::map<wxString, wxString>& pluginsDataMap)
{
    if(!m_doc.IsOk()) {
        return;
    }

    // locate the 'Plugins' node
    wxXmlNode* plugins = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Plugins"));
    if(!plugins) {
        return;
    }

    wxXmlNode* child = plugins->GetChildren();
    while(child) {
        if(child->GetName() == wxT("Plugin")) {
            // get the content
            wxString content = child->GetNodeContent();
            // overcome bug in WX where CDATA content comes out with extra \n and 4xspaces
            content.Trim().Trim(false);
            pluginsDataMap[child->GetPropVal(wxT("Name"), wxEmptyString)] = content;
        }
        child = child->GetNext();
    }
}

wxString Project::GetPluginData(const wxString& pluginName)
{
    if(!m_doc.IsOk()) {
        return wxEmptyString;
    }

    // locate the 'Plugins' node
    wxXmlNode* plugins = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Plugins"));
    if(!plugins) {
        return wxEmptyString;
    }

    // find the node and return its content
    wxXmlNode* dataNode = XmlUtils::FindNodeByName(plugins, wxT("Plugin"), pluginName);
    if(dataNode) {
        return dataNode->GetNodeContent().Trim().Trim(false);
    }
    return wxEmptyString;
}

void Project::SetPluginData(const wxString& pluginName, const wxString& data, bool saveToXml)
{
    if(!m_doc.IsOk()) {
        return;
    }

    // locate the 'Plugins' node
    wxXmlNode* plugins = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Plugins"));
    if(!plugins) {
        plugins = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("Plugins"));
    }

    wxXmlNode* plugin = XmlUtils::FindNodeByName(plugins, wxT("Plugin"), pluginName);
    if(!plugin) {
        plugin = new wxXmlNode(plugins, wxXML_ELEMENT_NODE, wxT("Plugin"));
        plugin->AddProperty(wxT("Name"), pluginName);
    }

    wxString tmpData(data);
    tmpData.Trim().Trim(false);
    XmlUtils::SetCDATANodeContent(plugin, tmpData);

    if(saveToXml) {
        SaveXmlFile();
    }
}

void Project::SetAllPluginsData(const std::map<wxString, wxString>& pluginsDataMap, bool saveToFile /* true */)
{
    if(!m_doc.IsOk()) {
        return;
    }

    // locate the 'Plugins' node
    wxXmlNode* plugins = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Plugins"));
    if(plugins) {
        m_doc.GetRoot()->RemoveChild(plugins);
        delete plugins;
    }

    std::map<wxString, wxString>::const_iterator iter = pluginsDataMap.begin();
    for(; iter != pluginsDataMap.end(); iter++) {
        SetPluginData(iter->first, iter->second, saveToFile);
    }

    if(saveToFile) {
        SaveXmlFile();
    }
}

time_t Project::GetFileLastModifiedTime() const { return GetFileModificationTime(GetFileName()); }

void Project::ConvertToUnixFormat(wxXmlNode* parent)
{
    if(!parent) return;

    wxXmlNode* child = parent->GetChildren();
    while(child) {

        if(child->GetName() == wxT("VirtualDirectory")) {

            ConvertToUnixFormat(child);

        } else if(child->GetName() == wxT("File")) {

            wxXmlProperty* props = child->GetProperties();
            // Convert the path to unix format
            while(props) {
                if(props->GetName() == wxT("Name")) {
                    wxString val = props->GetValue();
                    val.Replace(wxT("\\"), wxT("/"));
                    props->SetValue(val);
                    break;
                }
                props = props->GetNext();
            }
        }
        child = child->GetNext();
    }
}

wxString Project::GetBestPathForVD(const wxString& vdPath)
{
    // Project name
    const wxString basePath = GetFileName().GetPath();
    wxString bestPath;

    // try to open the dir dialog as close as we can to the virtual folder ones
    int matches(0);
    wxArrayString subDirs = wxStringTokenize(vdPath, wxT(":"), wxTOKEN_STRTOK);
    bestPath = basePath;
    for(size_t i = 0; i < subDirs.GetCount(); i++) {
        wxFileName fn(bestPath + wxFileName::GetPathSeparator() + subDirs.Item(i), wxEmptyString);
        if(fn.DirExists()) {
            bestPath << wxFileName::GetPathSeparator() << subDirs.Item(i);
            matches++;
        } else {
            break;
        }
    }

    if(matches) {
        return bestPath;
    }

    // Could not find any match for the virtual directory when tested
    // directly under the project path. Try it again using a path from
    // the first file that we could find under the virtual directory
    wxArrayString files;
    GetFilesByVirtualDir(vdPath, files);
    if(files.IsEmpty() == false) {
        wxFileName f(files.Item(0));
        if(f.MakeAbsolute(GetFileName().GetPath())) {
            bestPath = f.GetPath();
            return bestPath;
        }
    }

    // all failed, return the project path as our default path
    return basePath;
}

wxArrayString Project::GetIncludePaths()
{
    if(!m_cachedIncludePaths.IsEmpty()) {
        return m_cachedIncludePaths;
    }

    BuildMatrixPtr matrix = GetWorkspace()->GetBuildMatrix();
    if(!matrix) {
        return m_cachedIncludePaths;
    }

    wxStringSet_t paths;
    wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

    wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, GetName());
    BuildConfigPtr buildConf = GetWorkspace()->GetProjBuildConf(this->GetName(), projectSelConf);

    // for non custom projects, take the settings from the build configuration
    if(buildConf) {
        // Apply the environment
        EnvSetter es(this);

        // Get the include paths and add them
        wxString projectIncludePaths = buildConf->GetIncludePath();
        wxArrayString projectIncludePathsArr = ::wxStringTokenize(projectIncludePaths, wxT(";"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < projectIncludePathsArr.GetCount(); i++) {
            wxFileName fn;
            if(projectIncludePathsArr.Item(i) == wxT("..")) {
                fn = wxFileName(GetFileName().GetPath(), wxT(""));
                fn.RemoveLastDir();

            } else if(projectIncludePathsArr.Item(i) == wxT(".")) {
                fn = wxFileName(GetFileName().GetPath(), wxT(""));

            } else {
                wxString includePath = projectIncludePathsArr.Item(i);
                includePath = MacroManager::Instance()->Expand(includePath, NULL, GetName(), buildConf->GetName());
                fn = includePath;
                if(fn.IsRelative()) {
                    fn.MakeAbsolute(GetFileName().GetPath());
                }
            }
            paths.insert(fn.GetFullPath());
        }

        // get the compiler options and add them
        const wxString& projectCompileOptions = buildConf->GetCompileOptions();
        const wxString& projectCCompileOptions = buildConf->GetCCompileOptions();

        // C++ options
        wxArrayString projectCompileOptionsArr = wxStringTokenize(projectCompileOptions, wxT(";"), wxTOKEN_STRTOK);

        // C options
        wxArrayString projectCCompileOptionsArr = wxStringTokenize(projectCCompileOptions, wxT(";"), wxTOKEN_STRTOK);

        // Append the C options to the C++ options
        projectCompileOptionsArr.insert(projectCompileOptionsArr.end(), projectCCompileOptionsArr.begin(),
                                        projectCCompileOptionsArr.end());
        for(size_t i = 0; i < projectCompileOptionsArr.GetCount(); i++) {

            wxString cmpOption(projectCompileOptionsArr.Item(i));
            cmpOption.Trim().Trim(false);

            // expand backticks, if the option is not a backtick the value remains
            // unchanged
            wxArrayString includePaths = DoBacktickToIncludePath(cmpOption);
            if(!includePaths.IsEmpty()) {
                std::for_each(includePaths.begin(), includePaths.end(), [&](const wxString& path) {
                    wxFileName fn(path, "");
                    paths.insert(fn.GetPath());
                });
            }
        }
    }

    std::for_each(paths.begin(), paths.end(), [&](const wxString& path) { m_cachedIncludePaths.Add(path); });
    return m_cachedIncludePaths;
}

wxArrayString Project::DoBacktickToPreProcessors(const wxString& backtick)
{
    wxArrayString paths;
    wxString cmpOption = DoExpandBacktick(backtick);
    CompilerCommandLineParser cclp(cmpOption);
    return cclp.GetMacros();
}

wxArrayString Project::DoBacktickToIncludePath(const wxString& backtick)
{
    wxArrayString paths;
    wxString cmpOption = DoExpandBacktick(backtick);
    CompilerCommandLineParser cclp(cmpOption, GetFileName().GetPath());
    return cclp.GetIncludes();
}

void Project::DoDeleteVDFromCache(const wxString& vd)
{
    clProjectFolder::Ptr_t folder = GetFolder(vd);
    if(folder) {
        folder->DeleteRecursive(this);
        SaveXmlFile();
    }
}

void Project::ClearAllVirtDirs()
{
    // remove all the virtual directories from this project
    clProjectFolder::Ptr_t rootFolder = GetRootFolder();
    rootFolder->DeleteRecursive(this);
    m_virtualFoldersTable.clear();
    m_filesTable.clear();
    SetModified(true);
    SaveXmlFile();
}

wxString Project::GetProjectIconName() const { return m_doc.GetRoot()->GetPropVal(wxT("IconIndex"), "gear16"); }

void Project::GetReconciliationData(wxString& toplevelDir, wxString& extensions, wxArrayString& ignoreFiles,
                                    wxArrayString& excludePaths, wxArrayString& regexes)
{
    if(!m_doc.IsOk()) {
        return;
    }

    // locate the 'Reconciliation' node
    wxXmlNode* reconciliation = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Reconciliation"));
    if(!reconciliation) {
        return;
    }

    wxXmlNode* dirnode = XmlUtils::FindFirstByTagName(reconciliation, wxT("Topleveldir"));
    if(dirnode) {
        toplevelDir = dirnode->GetNodeContent().Trim().Trim(false);
    }

    wxXmlNode* extnode = XmlUtils::FindFirstByTagName(reconciliation, wxT("Extensions"));
    if(extnode) {
        extensions = extnode->GetNodeContent().Trim().Trim(false);
    }

    wxXmlNode* ignorefilesnode = XmlUtils::FindFirstByTagName(reconciliation, wxT("Ignorefiles"));
    if(ignorefilesnode) {
        ignoreFiles = XmlUtils::ChildNodesContentToArray(ignorefilesnode, "Ignore");
    }

    wxXmlNode* excludesnode = XmlUtils::FindFirstByTagName(reconciliation, wxT("Excludepaths"));
    if(excludesnode) {
        excludePaths = XmlUtils::ChildNodesContentToArray(excludesnode, "Path");
    }

    wxXmlNode* regexnode = XmlUtils::FindFirstByTagName(reconciliation, wxT("Regexes"));
    if(regexnode) {
        regexes = XmlUtils::ChildNodesContentToArray(regexnode, "Regex");
    }
}

void Project::SetReconciliationData(const wxString& toplevelDir, const wxString& extensions,
                                    const wxArrayString& ignoreFiles, const wxArrayString& excludePaths,
                                    wxArrayString& regexes)
{
    if(!m_doc.IsOk()) {
        return;
    }

    wxXmlNode* reconciliation = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Reconciliation"));
    if(!reconciliation) {
        reconciliation = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("Reconciliation"));
    }

    wxXmlNode* dirnode = XmlUtils::FindFirstByTagName(reconciliation, wxT("Topleveldir"));
    if(!dirnode) {
        dirnode = new wxXmlNode(reconciliation, wxXML_ELEMENT_NODE, wxT("Topleveldir"));
    }
    XmlUtils::SetNodeContent(dirnode, toplevelDir);

    wxXmlNode* extsnode = XmlUtils::FindFirstByTagName(reconciliation, wxT("Extensions"));
    if(!extsnode) {
        extsnode = new wxXmlNode(reconciliation, wxXML_ELEMENT_NODE, wxT("Extensions"));
    }
    wxString tmpData(extensions);
    tmpData.Trim().Trim(false);
    XmlUtils::SetCDATANodeContent(extsnode, tmpData);

    wxXmlNode* ignorefilesnode = XmlUtils::FindFirstByTagName(reconciliation, wxT("Ignorefiles"));
    if(!ignorefilesnode) {
        ignorefilesnode = new wxXmlNode(reconciliation, wxXML_ELEMENT_NODE, wxT("Ignorefiles"));
    } else {
        XmlUtils::RemoveChildren(ignorefilesnode);
    }

    for(size_t n = 0; n < ignoreFiles.GetCount(); ++n) {
        wxXmlNode* pathnode = new wxXmlNode(ignorefilesnode, wxXML_ELEMENT_NODE, "Ignore");
        XmlUtils::SetNodeContent(pathnode, ignoreFiles.Item(n));
    }

    wxXmlNode* excludesnode = XmlUtils::FindFirstByTagName(reconciliation, wxT("Excludepaths"));
    if(!excludesnode) {
        excludesnode = new wxXmlNode(reconciliation, wxXML_ELEMENT_NODE, wxT("Excludepaths"));
    } else {
        XmlUtils::RemoveChildren(excludesnode);
    }

    for(size_t n = 0; n < excludePaths.GetCount(); ++n) {
        wxXmlNode* pathnode = new wxXmlNode(excludesnode, wxXML_ELEMENT_NODE, "Path");
        XmlUtils::SetNodeContent(pathnode, excludePaths.Item(n));
    }

    wxXmlNode* regexnode = XmlUtils::FindFirstByTagName(reconciliation, wxT("Regexes"));
    if(!regexnode) {
        regexnode = new wxXmlNode(reconciliation, wxXML_ELEMENT_NODE, wxT("Regexes"));
    } else {
        XmlUtils::RemoveChildren(regexnode);
    }

    for(size_t n = 0; n < regexes.GetCount(); ++n) {
        wxXmlNode* itemnode = new wxXmlNode(regexnode, wxXML_ELEMENT_NODE, "Regex");
        XmlUtils::SetNodeContent(itemnode, regexes.Item(n));
    }
    SaveXmlFile();
}

wxString Project::DoFormatVirtualFolderName(const wxXmlNode* node) const
{
    // we assume that 'node' is a 'File' element
    wxString name;
    wxXmlNode* p = node->GetParent();
    std::list<wxString> q;
    while(p) {
        if(p->GetName() == "VirtualDirectory")
            q.push_front(p->GetPropVal("Name", ""));
        else
            break;
        p = p->GetParent();
    }

    while(!q.empty()) {
        name << q.front() << ":";
        q.pop_front();
    }

    if(name.IsEmpty() == false) name.RemoveLast();
    return name;
}

void Project::SetFileFlags(const wxString& fileName, const wxString& virtualDirPath, size_t flags)
{
    wxXmlNode* vdNode = GetVirtualDir(virtualDirPath);
    if(!vdNode) {
        return;
    }

    // locate our file
    wxFileName tmp(fileName);
    tmp.MakeRelativeTo(m_fileName.GetPath());
    wxString filepath = tmp.GetFullPath(wxPATH_UNIX);
    wxXmlNode* fileNode = XmlUtils::FindNodeByName(vdNode, "File", filepath);
    if(!fileNode) {
        return;
    }

    // we have located the file node
    // updat the flags
    XmlUtils::UpdateProperty(fileNode, "Flags", wxString() << flags);
    SaveXmlFile();
}

size_t Project::GetFileFlags(const wxString& fileName, const wxString& virtualDirPath)
{
    wxXmlNode* vdNode = GetVirtualDir(virtualDirPath);
    if(!vdNode) {
        return 0;
    }

    // locate our file
    wxFileName tmp(fileName);
    tmp.MakeRelativeTo(m_fileName.GetPath());
    wxString filepath = tmp.GetFullPath(wxPATH_UNIX);
    wxXmlNode* fileNode = XmlUtils::FindNodeByName(vdNode, "File", filepath);
    if(!fileNode) {
        return 0;
    }

    return XmlUtils::ReadLong(fileNode, "Flags", 0);
}

const wxStringSet_t& Project::GetExcludeConfigForFile(const wxString& filename) const
{
    clProjectFile::Ptr_t pfile = GetFile(filename);
    if(!pfile) {
        static wxStringSet_t emptySet;
        return emptySet;
    }
    return pfile->GetExcludeConfigs();
}

void Project::SetExcludeConfigsForFile(const wxString& filename, const wxStringSet_t& configs)
{
    clProjectFile::Ptr_t pfile = GetFile(filename);
    if(!pfile) {
        return;
    }

    pfile->SetExcludeConfigs(this, configs);
    wxXmlNode* fileNode = pfile->GetXmlNode();

    // Convert to string and update the XML
    wxString excludeConfigs;
    std::for_each(configs.begin(), configs.end(), [&](const wxString& config) { excludeConfigs << config << ";"; });
    XmlUtils::UpdateProperty(fileNode, EXCLUDE_FROM_BUILD_FOR_CONFIG, excludeConfigs);
    SaveXmlFile();
}

wxString Project::GetCompileLineForCXXFile(const wxString& filenamePlaceholder, bool cxxFile) const
{
    // Return a compilation line for a CXX file
    BuildMatrixPtr matrix = GetWorkspace()->GetBuildMatrix();
    if(!matrix) {
        return "";
    }
    wxString workspaceSelConf = matrix->GetSelectedConfigurationName();
    wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, GetName());
    BuildConfigPtr buildConf = GetWorkspace()->GetProjBuildConf(this->GetName(), projectSelConf);
    if(!buildConf || buildConf->IsCustomBuild() || !buildConf->IsCompilerRequired()) {
        return "";
    }

    CompilerPtr compiler = buildConf->GetCompiler();
    if(!compiler) {
        return "";
    }
    // Build the command line
    wxString commandLine;

    wxString compilerExe = compiler->GetTool(cxxFile ? "CXX" : "CC");
    commandLine << compilerExe << " -c " << filenamePlaceholder << " -o " << filenamePlaceholder << ".o ";

    // Apply the environment
    EnvSetter es(NULL, NULL, GetName(), buildConf->GetName());

    // Clear the backticks cache
    //    s_backticks.clear();

    // Get the compile options
    wxString projectCompileOptions = cxxFile ? buildConf->GetCompileOptions() : buildConf->GetCCompileOptions();
    wxArrayString projectCompileOptionsArr = ::wxStringTokenize(projectCompileOptions, ";", wxTOKEN_STRTOK);
    for(size_t i = 0; i < projectCompileOptionsArr.GetCount(); ++i) {
        wxString cmpOption(projectCompileOptionsArr.Item(i));
        cmpOption.Trim().Trim(false);

        // expand backticks, if the option is not a backtick the value remains
        // unchanged
        commandLine << " " << DoExpandBacktick(cmpOption) << " ";
    }

    // Add the macros
    wxArrayString prepArr;
    buildConf->GetPreprocessor(prepArr);
    for(size_t i = 0; i < prepArr.GetCount(); ++i) {
        commandLine << "-D" << prepArr.Item(i) << " ";
    }

    // Add the include paths
    wxString inclPathAsString = buildConf->GetIncludePath();
    wxArrayString inclPathArr = ::wxStringTokenize(inclPathAsString, ";", wxTOKEN_STRTOK);
    for(size_t i = 0; i < inclPathArr.GetCount(); ++i) {
        wxString incl_path = inclPathArr.Item(i);
        incl_path.Trim().Trim(false);
        if(incl_path.IsEmpty()) continue;

        if(incl_path.Contains(" ")) {
            incl_path.Prepend("\"").Append("\"");
        }

        commandLine << "-I" << incl_path << " ";
    }

    commandLine.Trim().Trim(false);
    return commandLine;
}

wxString Project::DoExpandBacktick(const wxString& backtick) const
{
    wxString tmp;
    wxString cmpOption = backtick;
    cmpOption.Trim().Trim(false);

    // Expand backticks / $(shell ...) syntax supported by codelite
    if(cmpOption.StartsWith(wxT("$(shell "), &tmp) || cmpOption.StartsWith(wxT("`"), &tmp)) {
        cmpOption = tmp;
        tmp.Clear();
        if(cmpOption.EndsWith(wxT(")"), &tmp) || cmpOption.EndsWith(wxT("`"), &tmp)) {
            cmpOption = tmp;
        }

        if(s_backticks.find(cmpOption) == s_backticks.end()) {

            // Expand the backticks into their value
            wxString expandedValue = ::wxShellExec(cmpOption, GetName());
            s_backticks[cmpOption] = expandedValue;
            cmpOption = expandedValue;

        } else {
            cmpOption = s_backticks.find(cmpOption)->second;
        }
    }

    return cmpOption;
}

void Project::CreateCompileCommandsJSON(JSONElement& compile_commands)
{
    wxString cFilePattern = GetCompileLineForCXXFile("$FileName", false);
    wxString cxxFilePattern = GetCompileLineForCXXFile("$FileName", true);
    wxString workingDirectory = m_fileName.GetPath();
    std::for_each(m_filesTable.begin(), m_filesTable.end(), [&](const FilesMap_t::value_type& vt) {
        const wxString& fullpath = vt.second->GetFilename();
        wxString compilePattern;
        FileExtManager::FileType fileType = FileExtManager::GetType(fullpath);
        if(fileType == FileExtManager::TypeSourceC) {
            compilePattern = cFilePattern;
        } else if(fileType == FileExtManager::TypeSourceCpp) {
            compilePattern = cxxFilePattern;
        }

        if(!compilePattern.IsEmpty()) {
            wxString file_name = fullpath;
            if(file_name.Contains(" ")) {
                file_name.Prepend("\"").Append("\"");
            }
            compilePattern.Replace("$FileName", file_name);

            JSONElement json = JSONElement::createObject();
            json.addProperty("file", fullpath);
            json.addProperty("directory", workingDirectory);
            json.addProperty("command", compilePattern);
            compile_commands.append(json);
        }
    });
}

BuildConfigPtr Project::GetBuildConfiguration(const wxString& configName) const
{
    BuildMatrixPtr matrix = GetWorkspace()->GetBuildMatrix();
    if(!matrix) {
        return NULL;
    }

    wxString workspaceSelConf = matrix->GetSelectedConfigurationName();
    wxString projectSelConf =
        configName.IsEmpty() ? matrix->GetProjectSelectedConf(workspaceSelConf, GetName()) : configName;
    BuildConfigPtr buildConf = GetWorkspace()->GetProjBuildConf(this->GetName(), projectSelConf);
    return buildConf;
}

void Project::AssociateToWorkspace(clCxxWorkspace* workspace) { m_workspace = workspace; }

clCxxWorkspace* Project::GetWorkspace()
{
    if(!m_workspace) {
        return clCxxWorkspaceST::Get();

    } else {
        return m_workspace;
    }
}

const clCxxWorkspace* Project::GetWorkspace() const
{
    if(!m_workspace) {
        return clCxxWorkspaceST::Get();

    } else {
        return m_workspace;
    }
}

void Project::GetCompilers(wxStringSet_t& compilers)
{
    ProjectSettingsPtr pSettings = GetSettings();
    CHECK_PTR_RET(pSettings);

    BuildConfigPtr buildConf = GetBuildConfiguration();
    if(buildConf) {
        compilers.insert(buildConf->GetCompilerType());
    }
}

void Project::ReplaceCompilers(wxStringMap_t& compilers)
{
    ProjectSettingsPtr pSettings = GetSettings();
    CHECK_PTR_RET(pSettings);

    ProjectSettingsCookie cookie;
    BuildConfigPtr buildConf = pSettings->GetFirstBuildConfiguration(cookie);
    while(buildConf) {
        if(compilers.count(buildConf->GetCompilerType())) {
            buildConf->SetCompilerType(compilers.find(buildConf->GetCompilerType())->second);
        }
        buildConf = pSettings->GetNextBuildConfiguration(cookie);
    }

    // and update the settings (+ save the XML file)
    SetSettings(pSettings);
}

void Project::DoUpdateProjectSettings()
{
    m_settings.Reset(new ProjectSettings(XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Settings"))));
}

wxArrayString Project::GetPreProcessors(bool clearCache)
{
    wxArrayString pps;
    BuildConfigPtr buildConf = GetBuildConfiguration();
    // for non custom projects, take the settings from the build configuration
    if(buildConf) {

        // Apply the environment
        EnvSetter es(NULL, NULL, GetName(), buildConf->GetName());

        if(clearCache) {
            s_backticks.clear();
        }

        // Get the pre-processors and add them to the array
        wxString projectPPS = buildConf->GetPreprocessor();
        wxArrayString projectPPSArr = ::wxStringTokenize(projectPPS, wxT(";"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < projectPPSArr.GetCount(); i++) {
            projectPPSArr.Item(i).Trim();
            if(pps.Index(projectPPSArr.Item(i)) != wxNOT_FOUND) continue;
            pps.Add(projectPPSArr.Item(i));
        }

        // get the compiler options and add them
        wxString projectCompileOptions = buildConf->GetCompileOptions();
        wxArrayString projectCompileOptionsArr = wxStringTokenize(projectCompileOptions, wxT(";"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < projectCompileOptionsArr.GetCount(); i++) {

            wxString cmpOption(projectCompileOptionsArr.Item(i));
            cmpOption.Trim().Trim(false);

            // expand backticks, if the option is not a backtick the value remains
            // unchanged
            wxArrayString pparr = DoBacktickToPreProcessors(cmpOption);
            if(!pparr.IsEmpty()) {
                pps.insert(pps.end(), pparr.begin(), pparr.end());
            }
        }
    }
    return pps;
}

wxArrayString Project::GetCXXCompilerOptions(bool clearCache, bool noDefines, bool noIncludePaths)
{
    return DoGetCompilerOptions(true, clearCache, noDefines, noIncludePaths);
}

wxArrayString Project::GetCCompilerOptions(bool clearCache, bool noDefines, bool noIncludePaths)
{
    return DoGetCompilerOptions(false, clearCache, noDefines, noIncludePaths);
}

wxArrayString Project::DoGetCompilerOptions(bool cxxOptions, bool clearCache, bool noDefines, bool noIncludePaths)
{
    wxArrayString options;
    BuildConfigPtr buildConf = GetBuildConfiguration();
    // for non custom projects, take the settings from the build configuration
    if(buildConf && !buildConf->IsCustomBuild()) {

        // Apply the environment
        EnvSetter es(NULL, NULL, GetName(), buildConf->GetName());

        if(clearCache) {
            s_backticks.clear();
        }

        // Get the switches from
        wxString optionsStr = cxxOptions ? buildConf->GetCompileOptions() : buildConf->GetCCompileOptions();

        wxArrayString optionsArr = wxStringTokenize(optionsStr, wxT(";"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < optionsArr.GetCount(); ++i) {
            wxString cmpOption(optionsArr.Item(i));
            cmpOption.Trim().Trim(false);
            if(cmpOption.IsEmpty()) continue;

            wxString expandedCmpOption = DoExpandBacktick(cmpOption);
            if(expandedCmpOption != cmpOption) {
                // this was indeed a backtick
                CompilerCommandLineParser cclp(expandedCmpOption, GetFileName().GetPath());
                const wxArrayString& opts = cclp.GetOtherOptions();
                options.insert(options.end(), opts.begin(), opts.end());

            } else {
                options.Add(cmpOption);
            }
        }

        if(!noDefines) {
            wxArrayString macros = GetPreProcessors();
            std::for_each(macros.begin(), macros.end(), wxArrayStringAppender(macros, "-D", true));
            options.insert(options.end(), macros.begin(), macros.end());
        }

        if(!noIncludePaths) {
            wxArrayString includes = GetIncludePaths();
            std::for_each(includes.begin(), includes.end(), wxArrayStringAppender(includes, "-I", true));
            options.insert(options.end(), includes.begin(), includes.end());
        }
    }
    return options;
}

void Project::ProjectRenamed(const wxString& oldname, const wxString& newname)
{
    // dependencies are located directly under the root level
    wxXmlNode* node = m_doc.GetRoot()->GetChildren();
    while(node) {
        if(node->GetName() == wxT("Dependencies")) {
            wxXmlNode* child = node->GetChildren();
            while(child) {
                if(child->GetName() == wxT("Project")) {
                    wxString projectName = XmlUtils::ReadString(child, "Name");
                    if(projectName == oldname) {
                        // update the project name
                        XmlUtils::UpdateProperty(child, "Name", newname);
                    }
                }
                child = child->GetNext();
            }
        }
        node = node->GetNext();
    }

    if(GetName() == oldname) {
        // Update the name
        XmlUtils::UpdateProperty(m_doc.GetRoot(), "Name", newname);
    }
}

void Project::ClearBacktickCache() { s_backticks.clear(); }

void Project::GetUnresolvedMacros(const wxString& configName, wxArrayString& vars) const
{
    vars.clear();
    BuildConfigPtr buildConfig = GetBuildConfiguration(configName);
    if(buildConfig) {
        // Check for environment variables
        // Environment variable has the format of $(VAR_NAME)
        static wxRegEx reEnvironmentVar("\\$\\(([a-z0-9_]+)\\)", wxRE_ICASE | wxRE_ADVANCED);

        wxString includePaths = buildConfig->GetIncludePath();
        wxString libPaths = buildConfig->GetLibPath();
        if(reEnvironmentVar.IsValid()) {
            // Check the include paths
            includePaths = MacroManager::Instance()->Expand(includePaths, clGetManager(), GetName(), configName);
            while(true) {
                if(reEnvironmentVar.Matches(includePaths)) {
                    size_t start, len;
                    if(reEnvironmentVar.GetMatch(&start, &len, 1)) {
                        wxString envVarName = includePaths.Mid(start, len);
                        includePaths = includePaths.Mid(start + len);
                        vars.Add(envVarName);
                        continue;
                    }
                }
                break;
            }

            // Check the include lib paths
            libPaths = MacroManager::Instance()->Expand(libPaths, clGetManager(), GetName(), configName);
            while(true) {
                if(reEnvironmentVar.Matches(libPaths)) {
                    size_t start, len;
                    if(reEnvironmentVar.GetMatch(&start, &len, 1)) {
                        wxString envVarName = libPaths.Mid(start, len);
                        libPaths = includePaths.Mid(start + len);
                        vars.Add(envVarName);
                        continue;
                    }
                }
                break;
            }
        }

        // remove duplicate entries
        wxArrayString uniqueVars;
        for(size_t i = 0; i < vars.size(); ++i) {
            if(uniqueVars.Index(vars.Item(i)) == wxNOT_FOUND) {
                uniqueVars.Add(vars.Item(i));
            }
        }
        vars.swap(uniqueVars);
    }
}

void Project::ClearIncludePathCache() { m_cachedIncludePaths.clear(); }

wxString Project::GetVersion() const
{
    if(!m_doc.IsOk() || m_doc.GetRoot()) return wxEmptyString;
    return m_doc.GetRoot()->GetAttribute("Version");
}

wxArrayString Project::GetCxxUnPreProcessors(bool clearCache)
{
    BuildConfigPtr buildConf = GetBuildConfiguration();
    // for non custom projects, take the settings from the build configuration
    if(!buildConf) return wxArrayString();
    return DoGetUnPreProcessors(clearCache, buildConf->GetCompileOptions());
}

wxArrayString Project::GetCUnPreProcessors(bool clearCache)
{
    BuildConfigPtr buildConf = GetBuildConfiguration();
    // for non custom projects, take the settings from the build configuration
    if(!buildConf) return wxArrayString();
    return DoGetUnPreProcessors(clearCache, buildConf->GetCCompileOptions());
}

wxArrayString Project::DoGetUnPreProcessors(bool clearCache, const wxString& cmpOptions)
{
    wxArrayString pps;
    BuildConfigPtr buildConf = GetBuildConfiguration();
    // for non custom projects, take the settings from the build configuration
    if(!buildConf) return pps;

    // Apply the environment
    EnvSetter es(NULL, NULL, GetName(), buildConf->GetName());

    if(clearCache) {
        s_backticks.clear();
    }

    // Atm, we can only "set" undefined in the compiler options
    wxArrayString projectCompileOptionsArr = ::wxStringTokenize(cmpOptions, ";", wxTOKEN_STRTOK);
    for(size_t i = 0; i < projectCompileOptionsArr.GetCount(); i++) {

        wxString cmpOption(projectCompileOptionsArr.Item(i));
        cmpOption.Trim().Trim(false);

        wxString rest;
        if(cmpOption.StartsWith("-U", &rest)) {
            pps.Add(rest);
        }
    }
    return pps;
}

wxString Project::GetFilesAsString(bool absPath) const
{
    wxString str;
    std::for_each(m_filesTable.begin(), m_filesTable.end(), [&](const FilesMap_t::value_type& vt) {
        if(absPath) {
            str << vt.first << " ";
        } else {
            str << vt.second->GetFilenameRelpath() << " ";
        }
    });

    if(!str.IsEmpty()) {
        str.RemoveLast();
    }
    return str;
}

void Project::GetFilesAsVector(clProjectFile::Vec_t& files) const
{
    if(m_filesTable.empty()) {
        return;
    }
    files.reserve(m_filesTable.size());
    std::for_each(m_filesTable.begin(), m_filesTable.end(),
                  [&](const FilesMap_t::value_type& vt) { files.push_back(vt.second); });
}

void Project::GetFilesAsStringArray(wxArrayString& files, bool absPath) const
{
    if(m_filesTable.empty()) {
        return;
    }
    files.reserve(m_filesTable.size());
    std::for_each(m_filesTable.begin(), m_filesTable.end(), [&](const FilesMap_t::value_type& vt) {
        files.Add(absPath ? vt.second->GetFilename() : vt.second->GetFilenameRelpath());
    });
}

clProjectFolder::Ptr_t Project::GetRootFolder()
{
    if(m_virtualFoldersTable.count("") == 0) {
        m_virtualFoldersTable[""] = clProjectFolder::Ptr_t(new clProjectFolder("", m_doc.GetRoot()));
    }
    return m_virtualFoldersTable[""];
}

clProjectFolder::Ptr_t Project::GetFolder(const wxString& vdFullPath) const
{
    if(m_virtualFoldersTable.count(vdFullPath) == 0) {
        return clProjectFolder::Ptr_t(nullptr);
    }
    return m_virtualFoldersTable.find(vdFullPath)->second;
}

clProjectFile::Ptr_t Project::GetFile(const wxString& fullpath) const
{
    if(m_filesTable.count(fullpath) == 0) {
        return clProjectFile::Ptr_t(nullptr);
    }
    return m_filesTable.find(fullpath)->second;
}

void Project::GetFilesAsVectorOfFileName(std::vector<wxFileName>& files, bool absPath) const
{
    if(m_filesTable.empty()) {
        return;
    }
    files.reserve(m_filesTable.size());
    std::for_each(m_filesTable.begin(), m_filesTable.end(), [&](const FilesMap_t::value_type& vt) {
        files.push_back(absPath ? vt.second->GetFilename() : vt.second->GetFilenameRelpath());
    });
}

bool Project::IsEmpty() const { return m_virtualFoldersTable.empty() && m_filesTable.empty(); }

void Project::GetFolders(const wxString& vdFullPath, wxArrayString& folders)
{
    folders.Clear();
    clProjectFolder::Ptr_t parentFolder = vdFullPath.IsEmpty() ? GetRootFolder() : GetFolder(vdFullPath);
    if(!parentFolder) return;

    clProjectFolder::Vect_t res;
    parentFolder->GetSubfolders(folders, false);
}

void Project::GetFiles(const wxString& vdFullPath, wxArrayString& files)
{
    files.Clear();
    clProjectFolder::Ptr_t parentFolder = vdFullPath.IsEmpty() ? GetRootFolder() : GetFolder(vdFullPath);
    if(!parentFolder) return;
    const wxStringSet_t& filesSet = parentFolder->GetFiles();
    files.Alloc(filesSet.size());
    std::for_each(filesSet.begin(), filesSet.end(), [&](const wxString& s) { files.Add(s); });
}

bool Project::IsVirtualDirectoryEmpty(const wxString& vdFullPath) const
{
    clProjectFolder::Ptr_t folder = GetFolder(vdFullPath);
    if(!folder) {
        return true;
    }

    // Its faster to check first the files list
    if(!folder->GetFiles().empty()) {
        return false;
    }

    wxArrayString folders;
    folder->GetSubfolders(folders, false);
    return folders.IsEmpty();
}

bool Project::IsFileExcludedFromConfig(const wxString& filename, const wxString& configName) const
{
    clProjectFile::Ptr_t pfile = GetFile(filename);
    BuildConfigPtr buildConf = GetBuildConfiguration(configName);
    if(!pfile || !buildConf) {
        return false;
    }
    return (pfile->GetExcludeConfigs().count(buildConf->GetName()) > 0);
}

void Project::RemoveExcludeConfigForFile(const wxString& filename, const wxString& configName)
{
    clProjectFile::Ptr_t pfile = GetFile(filename);
    BuildConfigPtr buildConf = GetBuildConfiguration(configName);
    if(!pfile || !buildConf) {
        return;
    }
    wxStringSet_t& excludeConfigs = pfile->GetExcludeConfigs();
    if(excludeConfigs.count(buildConf->GetName())) {
        excludeConfigs.erase(buildConf->GetName());
        // Update the xml
        SetExcludeConfigsForFile(filename, excludeConfigs);
    }
}

void Project::AddExcludeConfigForFile(const wxString& filename, const wxString& configName)
{
    clProjectFile::Ptr_t pfile = GetFile(filename);
    BuildConfigPtr buildConf = GetBuildConfiguration(configName);
    if(!pfile || !buildConf) {
        return;
    }
    wxStringSet_t& excludeConfigs = pfile->GetExcludeConfigs();
    if(excludeConfigs.count(buildConf->GetName()) == 0) {
        excludeConfigs.insert(buildConf->GetName());
        // Update the xml
        SetExcludeConfigsForFile(filename, excludeConfigs);
    }
}

bool clProjectFolder::RenameFile(Project* project, const wxString& fullpath, const wxString& newName)
{
    if(!project) {
        return false;
    }
    if(GetXmlNode()) {
        return false;
    }

    // Locate the file in this virtual folder
    if(m_files.count(fullpath) == 0) {
        return false;
    }

    // Check that this file exists in the project
    if(project->m_filesTable.count(fullpath) == 0) {
        return false;
    }

    clProjectFile::Ptr_t file = project->m_filesTable[fullpath];
    file->Rename(project, newName);

    // Remove the old file from the folder and insert the new one
    m_files.erase(fullpath);
    m_files.insert(file->GetFilename());

    // Update the project files table
    project->m_filesTable.erase(fullpath);
    project->m_filesTable.insert({ file->GetFilename(), file });
    return true;
}

bool clProjectFolder::Rename(Project* project, const wxString& newName)
{
    if(m_xmlNode) {
        wxString oldnameFullpath = m_fullpath;
        XmlUtils::UpdateProperty(m_xmlNode, "Name", newName);
        // Update the new name
        m_name = newName;
        size_t where = m_fullpath.rfind(':');
        if(where != wxString::npos) {
            // Remove the last part
            m_fullpath = m_fullpath.Mid(0, where);
            m_fullpath << ":" << newName;
        } else {
            // Not found, replace it entirely
            m_fullpath = newName;
        }

        // Update the cache

        // Update all the files that are related to this folder
        std::for_each(m_files.begin(), m_files.end(), [&](const wxString& filename) {
            if(project->m_filesTable.count(filename)) {
                clProjectFile::Ptr_t file = project->m_filesTable[filename];
                file->SetVirtualFolder(GetFullpath());
            }
        });

        // Next, the folder path in the cache
        clProjectFolder::Ptr_t p = project->m_virtualFoldersTable[oldnameFullpath];
        project->m_virtualFoldersTable.erase(oldnameFullpath);
        project->m_virtualFoldersTable[m_fullpath] = p;
        return true;
    }
    return false;
}

clProjectFolder::Ptr_t clProjectFolder::AddFolder(Project* project, const wxString& name)
{
    wxString fullpath = GetFullpath().IsEmpty() ? (name) : (GetFullpath() + ":" + name);
    if(project->m_virtualFoldersTable.count(fullpath)) {
        return project->m_virtualFoldersTable[fullpath];
    }

    wxXmlNode* node = new wxXmlNode(m_xmlNode, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
    node->AddProperty("Name", name);

    clProjectFolder::Ptr_t childFolder(new clProjectFolder(fullpath, node));
    project->m_virtualFoldersTable[fullpath] = childFolder;
    return childFolder;
}

bool clProjectFolder::IsFolderExists(Project* project, const wxString& name) const
{
    wxString fullpath = GetFullpath().IsEmpty() ? (name) : (GetFullpath() + ":" + name);
    return (project->m_virtualFoldersTable.count(fullpath) > 0);
}

clProjectFolder::Ptr_t clProjectFolder::GetChild(Project* project, const wxString& name) const
{
    wxString fullpath = GetFullpath().IsEmpty() ? (name) : (GetFullpath() + ":" + name);
    if(project->m_virtualFoldersTable.count(fullpath)) {
        return project->m_virtualFoldersTable[fullpath];
    }
    return clProjectFolder::Ptr_t(nullptr);
}

void clProjectFolder::GetSubfolders(wxArrayString& folders, bool recursive) const
{
    folders.Clear();
    if(!m_xmlNode) {
        return;
    }

    std::vector<wxString> foldersV;
    std::queue<std::pair<wxXmlNode*, wxString> > q;
    q.push({ m_xmlNode, GetFullpath() });

    while(!q.empty()) {

        const std::pair<wxXmlNode*, wxString>& p = q.front();
        wxXmlNode* node = p.first;
        wxString prefix = p.second;
        q.pop();

        wxXmlNode* child = node->GetChildren();
        while(child) {
            if(child->GetName() == "VirtualDirectory") {
                wxString name = child->GetPropVal("Name", "");
                wxString childpath = prefix.IsEmpty() ? (name) : (prefix + ":" + name);
                foldersV.push_back(childpath);
                if(recursive) {
                    q.push({ child, childpath });
                }
            }
            child = child->GetNext();
        }
    }

    folders.Alloc(foldersV.size());
    std::for_each(foldersV.begin(), foldersV.end(), [&](const wxString& s) { folders.Add(s); });
}

void clProjectFolder::DeleteRecursive(Project* project)
{
    wxArrayString folders;
    GetSubfolders(folders);
    for(size_t i = 0; i < folders.size(); ++i) {
        clProjectFolder::Ptr_t folder = project->GetFolder(folders.Item(i));
        if(folder) {
            folder->DeleteAllFiles(project);
            project->m_virtualFoldersTable.erase(folder->GetFullpath());
        }
    }

    // Now delete this folder
    // Delete all files belonged to this folder
    DeleteAllFiles(project);
    // Remove this folder from the cache
    project->m_virtualFoldersTable.erase(GetFullpath());

    // Update the XML
    if(m_xmlNode) {
        wxXmlNode* parent = m_xmlNode->GetParent();
        // Can fail only if we are the top most wxXmlNode (wxXmlDocument::GetRoot())
        if(parent) {
            parent->RemoveChild(m_xmlNode);
            wxDELETE(m_xmlNode);
        }
    }
}

void clProjectFolder::DeleteAllFiles(Project* project)
{
    // Remove all children files
    std::for_each(m_files.begin(), m_files.end(), [&](const wxString& filename) {
        clProjectFile::Ptr_t file = project->GetFile(filename);
        if(file) {
            file->Delete(project, true);
        }
    });
    m_files.clear();
}

clProjectFile::Ptr_t clProjectFolder::AddFile(Project* project, const wxString& fullpath)
{
    if(project->m_filesTable.count(fullpath)) {
        // Already exists
        return clProjectFile::Ptr_t(nullptr);
    }

    wxFileName tmp(fullpath);
    tmp.MakeRelativeTo(project->m_fileName.GetPath());

    // Create the XML ndoe
    wxXmlNode* node = new wxXmlNode(m_xmlNode, wxXML_ELEMENT_NODE, wxT("File"));
    node->AddProperty(wxT("Name"), tmp.GetFullPath(wxPATH_UNIX));

    clProjectFile::Ptr_t file(new clProjectFile());
    file->SetFilename(fullpath);
    file->SetFilenameRelpath(tmp.GetFullPath(wxPATH_UNIX));
    file->SetXmlNode(node);
    file->SetVirtualFolder(GetFullpath());

    // Add thie file to the cache
    project->m_filesTable.insert({ fullpath, file });
    m_files.insert(fullpath);
    return file;
}

void clProjectFile::Delete(Project* project, bool deleteXml)
{
    // Remove this file from the files-cache
    project->m_filesTable.erase(GetFilename());

    if(deleteXml && m_xmlNode) {
        wxXmlNode* parent = m_xmlNode->GetParent();
        if(parent) {
            parent->RemoveChild(m_xmlNode);
            wxDELETE(m_xmlNode);
        }
    }

    // Update the exclude-files table
    bool is_excluded = (project->m_excludeFiles.count(GetFilename()) > 0);
    if(is_excluded) {
        project->m_excludeFiles.erase(GetFilename());
    }
}

void clProjectFile::Rename(Project* project, const wxString& newName)
{
    wxFileName oldName = m_filename;
    {
        wxFileName fn(m_filename);
        fn.SetFullName(newName);
        m_filename = fn.GetFullPath();
    }
    {
        wxFileName fn(m_filenameRelpath);
        fn.SetFullName(newName);
        m_filenameRelpath = fn.GetFullPath(wxPATH_UNIX);
    }               // Update the XML
    if(m_xmlNode) { // update the new name
        XmlUtils::UpdateProperty(m_xmlNode, wxT("Name"), m_filenameRelpath);
    }

    // Update the exclude-files table
    bool is_excluded = (project->m_excludeFiles.count(oldName.GetFullPath()) > 0);
    if(is_excluded) {
        project->m_excludeFiles.erase(oldName.GetFullPath());
        project->m_excludeFiles.insert(m_filename);
    }
}

void clProjectFile::SetExcludeConfigs(Project* project, const wxStringSet_t& excludeConfigs)
{
    this->m_excludeConfigs = excludeConfigs;
    project->m_excludeFiles.insert(GetFilename());
    if(m_excludeConfigs.empty()) {
        project->m_excludeFiles.erase(GetFilename());
    } else {
        project->m_excludeFiles.insert(GetFilename());
    }
}

void clProjectFile::SetExcludeConfigs(Project* project, const wxArrayString& excludeConfigs)
{
    this->m_excludeConfigs.clear();
    this->m_excludeConfigs.insert(excludeConfigs.begin(), excludeConfigs.end());
    if(m_excludeConfigs.empty()) {
        project->m_excludeFiles.erase(GetFilename());
    } else {
        project->m_excludeFiles.insert(GetFilename());
    }
}
