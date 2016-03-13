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
#include "project.h"
#include "workspace.h"
#include <wx/app.h>
#include <wx/regex.h>
#include "globals.h"
#include <wx/log.h>
#include "fileextmanager.h"
#include "xmlutils.h"
#include <wx/tokenzr.h>
#include "wx/arrstr.h"
#include "dirsaver.h"
#include "globals.h"
#include "macros.h"
#include "wx_xml_compatibility.h"
#include "plugin.h"
#include "event_notifier.h"
#include <wx/sstream.h>
#include <wx/ffile.h>
#include "cl_command_event.h"
#include "environmentconfig.h"
#include "compiler_command_line_parser.h"
#include <algorithm>
#include "wxArrayStringAppender.h"
#include <wx/regex.h>
#include "macromanager.h"

const wxString Project::STATIC_LIBRARY = wxT("Static Library");
const wxString Project::DYNAMIC_LIBRARY = wxT("Dynamic Library");
const wxString Project::EXECUTABLE = wxT("Executable");

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

Project::~Project()
{
    m_vdCache.clear();
    m_settings.Reset(NULL);
}

bool Project::Create(const wxString& name, const wxString& description, const wxString& path, const wxString& projType)
{
    m_vdCache.clear();

    m_fileName = wxFileName(path, name);
    m_fileName.SetExt("project");
    m_fileName.MakeAbsolute();

    // Ensure that the target folder exists
    m_fileName.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    m_projectPath = m_fileName.GetPath();

    wxXmlNode* root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CodeLite_Project"));
    m_doc.SetRoot(root);
    m_doc.GetRoot()->AddProperty(wxT("Name"), name);

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
    SetSettings(settings);
    SetModified(true);
    return true;
}

bool Project::Load(const wxString& path)
{
    if(!m_doc.Load(path)) {
        return false;
    }

    ConvertToUnixFormat(m_doc.GetRoot());

    // Workaround WX bug: load the plugins data (GetAllPluginsData will strip any trailing whitespaces)
    // and then set them back
    std::map<wxString, wxString> pluginsData;
    GetAllPluginsData(pluginsData);
    SetAllPluginsData(pluginsData, false);

    m_vdCache.clear();

    m_fileName = path;
    m_fileName.MakeAbsolute();
    m_projectPath = m_fileName.GetPath();

    SetModified(true);
    SetProjectLastModifiedTime(GetFileLastModifiedTime());

    DoUpdateProjectSettings();

    return true;
}

wxXmlNode* Project::GetVirtualDir(const wxString& vdFullPath)
{
    wxArrayString paths = wxStringTokenize(vdFullPath, ":", wxTOKEN_STRTOK);

    // test the cache
    std::map<wxString, wxXmlNode*>::iterator iter = m_vdCache.find(vdFullPath);
    if(iter != m_vdCache.end()) {
        return iter->second;
    }

    wxString filename = m_fileName.GetFullPath();

    wxXmlNode* parent = m_doc.GetRoot();
    for(size_t i = 0; i < paths.GetCount(); ++i) {
        wxString curpath = paths.Item(i);
        parent = XmlUtils::FindNodeByName(parent, wxT("VirtualDirectory"), curpath);
        if(!parent) {
            m_vdCache[vdFullPath] = NULL;
            return NULL;
        }
    }

    // cache the result
    m_vdCache[vdFullPath] = parent;
    return parent;
}

wxXmlNode* Project::CreateVD(const wxString& vdFullPath, bool mkpath)
{
    wxXmlNode* oldVd = GetVirtualDir(vdFullPath);
    if(oldVd) {
        // VD already exist
        return oldVd;
    }

    wxStringTokenizer tkz(vdFullPath, wxT(":"));

    wxXmlNode* parent = m_doc.GetRoot();
    size_t count = tkz.CountTokens();
    for(size_t i = 0; i < count - 1; i++) {
        wxString token = tkz.NextToken();
        wxXmlNode* p = XmlUtils::FindNodeByName(parent, wxT("VirtualDirectory"), token);
        if(!p) {
            if(mkpath) {

                // add the node
                p = new wxXmlNode(parent, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
                p->AddProperty(wxT("Name"), token);

            } else {
                return NULL;
            }
        }
        parent = p;
    }

    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
    node->AddProperty(wxT("Name"), tkz.GetNextToken());

    parent->AddChild(node);

    // if not in transaction save the changes
    if(!InTransaction()) {
        SaveXmlFile();
    }

    // cache the result
    m_vdCache[vdFullPath] = node;

    return node;
}

bool Project::IsFileExist(const wxString& fileName)
{
    // find the file under this node
    // Convert the file path to be relative to
    // the project path
    DirSaver ds;

    ::wxSetWorkingDirectory(m_fileName.GetPath());
    wxFileName tmp(fileName);
    tmp.MakeRelativeTo(m_fileName.GetPath());

    std::vector<wxFileName> files;
    GetFiles(files);

    for(size_t i = 0; i < files.size(); i++) {
        if(files.at(i).GetFullPath().Cmp(tmp.GetFullPath(wxPATH_UNIX)) == 0) {
            return true;
        }
    }
    return false;
}

bool Project::AddFile(const wxString& fileName, const wxString& virtualDirPath)
{
    wxXmlNode* vd = GetVirtualDir(virtualDirPath);
    if(!vd) {
        return false;
    }

    // Convert the file path to be relative to
    // the project path
    DirSaver ds;

    ::wxSetWorkingDirectory(m_fileName.GetPath());
    wxFileName tmp(fileName);
    tmp.MakeRelativeTo(m_fileName.GetPath());

    // if we already have a file with the same name, return false
    if(this->IsFileExist(fileName)) {
        return false;
    }

    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
    node->AddProperty(wxT("Name"), tmp.GetFullPath(wxPATH_UNIX));
    vd->AddChild(node);
    if(!InTransaction()) {
        SaveXmlFile();
    }
    SetModified(true);
    return true;
}

bool Project::CreateVirtualDir(const wxString& vdFullPath, bool mkpath) { return CreateVD(vdFullPath, mkpath) != NULL; }

bool Project::DeleteVirtualDir(const wxString& vdFullPath)
{
    wxXmlNode* vd = GetVirtualDir(vdFullPath);
    if(vd) {
        wxXmlNode* parent = vd->GetParent();
        if(parent) {
            parent->RemoveChild(vd);
        }

        // remove the entry from the cache
        DoDeleteVDFromCache(vdFullPath);

        delete vd;
        SetModified(true);
        return SaveXmlFile();
    }
    return false;
}

bool Project::RemoveFile(const wxString& fileName, const wxString& virtualDir)
{
    wxXmlNode* vd = GetVirtualDir(virtualDir);
    if(!vd) {
        return false;
    }

    wxFileName tmp(fileName);
    tmp.MakeRelativeTo(m_fileName.GetPath());

    wxXmlNode* node = XmlUtils::FindNodeByName(vd, wxT("File"), tmp.GetFullPath(wxPATH_UNIX));
    if(node) {
        node->GetParent()->RemoveChild(node);
        delete node;

    } else {
        wxLogMessage(wxT("Failed to remove file %s from project"), tmp.GetFullPath(wxPATH_UNIX).c_str());
    }
    SetModified(true);

    if(InTransaction())
        return true;
    else
        return SaveXmlFile();
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
        item = ProjectItem(
            key, xmlNode->GetPropVal(wxT("Name"), wxEmptyString), wxEmptyString, ProjectItem::TypeVirtualDirectory);

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

wxString Project::GetFiles(bool absPath)
{
    std::vector<wxFileName> files;
    GetFiles(files, absPath);

    wxString temp;
    for(size_t i = 0; i < files.size(); i++) temp << wxT("\"") << files.at(i).GetFullPath() << wxT("\" ");

    if(temp.IsEmpty() == false) temp.RemoveLast();

    return temp;
}

void Project::GetFiles(wxStringSet_t& files)
{
    DirSaver ds;
    FileNameVector_t v;
    ::wxSetWorkingDirectory(m_fileName.GetPath());
    GetFiles(m_doc.GetRoot(), v, true);
    for(size_t i = 0; i < v.size(); i++) {
        files.insert(v.at(i).GetFullPath());
    }
}

void Project::GetFiles(std::vector<wxFileName>& files, bool absPath)
{
    if(absPath) {
        GetFiles(m_doc.GetRoot(), files, true);

    } else {
        GetFiles(m_doc.GetRoot(), files, false);
    }
}

void Project::GetFiles(wxXmlNode* parent, std::vector<wxFileName>& files, bool absPath)
{
    if(!parent) {
        return;
    }

    wxXmlNode* child = parent->GetChildren();
    while(child) {
        if(child->GetName() == wxT("File")) {
            wxString fileName = child->GetPropVal(wxT("Name"), wxEmptyString);
            wxFileName tmp(fileName);
            if(absPath) {
                tmp.MakeAbsolute(GetProjectPath());
            }

            files.push_back(tmp);
        } else if(child->GetChildren()) { // we could also add a check for VirtualDirectory only
            GetFiles(child, files, absPath);
        }
        child = child->GetNext();
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
    std::vector<wxFileName> files;
    GetFiles(files, true);

    wxXmlNode* srcNode(NULL);
    wxXmlNode* headNode(NULL);
    wxXmlNode* rcNode(NULL);

    // copy the files to their new location
    for(size_t i = 0; i < files.size(); i++) {
        wxFileName fn = files.at(i);
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
    }

    doc.Save(newFile.GetFullPath());
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
}

bool Project::RenameFile(const wxString& oldName, const wxString& virtualDir, const wxString& newName)
{
    wxXmlNode* vd = GetVirtualDir(virtualDir);
    if(!vd) {
        return false;
    }

    // Convert the file path to be relative to
    // the project path
    DirSaver ds;

    ::wxSetWorkingDirectory(m_fileName.GetPath());
    wxFileName tmp(oldName);
    tmp.MakeRelativeTo(m_fileName.GetPath());

    wxXmlNode* node = XmlUtils::FindNodeByName(vd, wxT("File"), tmp.GetFullPath(wxPATH_UNIX));
    if(node) {
        // update the new name
        tmp.SetFullName(newName);
        XmlUtils::UpdateProperty(node, wxT("Name"), tmp.GetFullPath(wxPATH_UNIX));
    }

    SetModified(true);

    if(InTransaction())
        return true;
    else
        return SaveXmlFile();
}

wxString Project::GetVDByFileName(const wxString& file)
{
    // find the file under this node
    // Convert the file path to be relative to
    // the project path
    DirSaver ds;

    ::wxSetWorkingDirectory(m_fileName.GetPath());
    wxFileName tmp(file);
    tmp.MakeRelativeTo(m_fileName.GetPath());

    wxString path(wxEmptyString);
    wxXmlNode* fileNode = FindFile(m_doc.GetRoot(), tmp.GetFullPath(wxPATH_UNIX));

    if(fileNode) {
        wxXmlNode* parent = fileNode->GetParent();
        while(parent) {
            if(parent->GetName() == wxT("VirtualDirectory")) {
                path.Prepend(parent->GetPropVal(wxT("Name"), wxEmptyString));
                path.Prepend(wxT(":"));
            } else {
                break;
            }
            parent = parent->GetParent();
        }
    }
    wxString trunc_path(path);
    path.StartsWith(wxT(":"), &trunc_path);
    return trunc_path;
}

wxXmlNode* Project::FindFile(wxXmlNode* parent, const wxString& file)
{
    wxXmlNode* child = parent->GetChildren();
    while(child) {
        wxString name = child->GetName();
        if(name == wxT("File") && child->GetPropVal(wxT("Name"), wxEmptyString) == file) {
            return child;
        }

        if(child->GetName() == wxT("VirtualDirectory")) {
            wxXmlNode* n = FindFile(child, file);
            if(n) {
                return n;
            }
        }
        child = child->GetNext();
    }
    return NULL;
}

bool Project::RenameVirtualDirectory(const wxString& oldVdPath, const wxString& newName)
{
    wxXmlNode* vdNode = GetVirtualDir(oldVdPath);
    if(vdNode) {
        XmlUtils::UpdateProperty(vdNode, wxT("Name"), newName);
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

void Project::GetFiles(std::vector<wxFileName>& files, std::vector<wxFileName>& absFiles)
{
    DirSaver ds;
    ::wxSetWorkingDirectory(m_fileName.GetPath());
    GetFiles(m_doc.GetRoot(), files, absFiles);
}

void Project::GetFiles(wxXmlNode* parent, std::vector<wxFileName>& files, std::vector<wxFileName>& absFiles)
{
    if(!parent) {
        return;
    }

    wxXmlNode* child = parent->GetChildren();
    while(child) {
        if(child->GetName() == wxT("File")) {
            wxString fileName = child->GetPropVal(wxT("Name"), wxEmptyString);
            wxFileName tmp(fileName);

            // append the file as it appears
            files.push_back(tmp);

            // convert to absolute path
            tmp.MakeAbsolute();
            absFiles.push_back(tmp);

        } else if(child->GetChildren()) { // we could also add a check for VirtualDirectory only
            GetFiles(child, files, absFiles);
        }
        child = child->GetNext();
    }
}

bool Project::FastAddFile(const wxString& fileName, const wxString& virtualDir)
{
    wxXmlNode* vd = GetVirtualDir(virtualDir);
    if(!vd) {
        return false;
    }

    // Convert the file path to be relative to
    // the project path
    DirSaver ds;

    ::wxSetWorkingDirectory(m_fileName.GetPath());
    wxFileName tmp(fileName);
    tmp.MakeRelativeTo(m_fileName.GetPath());

    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
    node->AddProperty(wxT("Name"), tmp.GetFullPath(wxPATH_UNIX));
    vd->AddChild(node);
    if(!InTransaction()) {
        SaveXmlFile();
    }
    SetModified(true);
    return true;
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

TreeNode<wxString, VisualWorkspaceNode>* Project::GetVirtualDirectories(
    TreeNode<wxString, VisualWorkspaceNode>* workspace)
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

wxArrayString Project::GetIncludePaths(bool clearCache)
{
    wxArrayString paths;

    BuildMatrixPtr matrix = GetWorkspace()->GetBuildMatrix();
    if(!matrix) {
        return paths;
    }

    wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

    wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, GetName());
    BuildConfigPtr buildConf = GetWorkspace()->GetProjBuildConf(this->GetName(), projectSelConf);

    // for non custom projects, take the settings from the build configuration
    if(buildConf) {
        // Apply the environment
        EnvSetter es(this);
        
        if(clearCache) {
            s_backticks.clear();
        }

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
            paths.Add(fn.GetFullPath());
        }

        // get the compiler options and add them
        const wxString& projectCompileOptions = buildConf->GetCompileOptions();
        const wxString& projectCCompileOptions = buildConf->GetCCompileOptions();

        // C++ options
        wxArrayString projectCompileOptionsArr = wxStringTokenize(projectCompileOptions, wxT(";"), wxTOKEN_STRTOK);

        // C options
        wxArrayString projectCCompileOptionsArr = wxStringTokenize(projectCCompileOptions, wxT(";"), wxTOKEN_STRTOK);

        // Append the C options to the C++ options
        projectCompileOptionsArr.insert(
            projectCompileOptionsArr.end(), projectCCompileOptionsArr.begin(), projectCCompileOptionsArr.end());
        for(size_t i = 0; i < projectCompileOptionsArr.GetCount(); i++) {

            wxString cmpOption(projectCompileOptionsArr.Item(i));
            cmpOption.Trim().Trim(false);

            // expand backticks, if the option is not a backtick the value remains
            // unchanged
            wxArrayString includePaths = DoBacktickToIncludePath(cmpOption);
            if(!includePaths.IsEmpty()) {
                paths.insert(paths.end(), includePaths.begin(), includePaths.end());
            }
        }
    }
    return paths;
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
    NodeMap_t::iterator iter = m_vdCache.lower_bound(vd);
    if(iter == m_vdCache.end()) return;

    if(iter->first.StartsWith(vd) == false) return;

    NodeMap_t::iterator first = iter;
    ++iter;

    // Loop and search for the first iterator that does not start with our prefix
    for(; iter != m_vdCache.end(); ++iter) {
        if(iter->first.StartsWith(vd) == false) break;
    }
    m_vdCache.erase(first, iter);
}

void Project::ClearAllVirtDirs()
{
    // remove all the virtual directories from this project
    wxXmlNode* vd = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("VirtualDirectory"));
    while(vd) {
        m_doc.GetRoot()->RemoveChild(vd);
        delete vd;
        vd = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("VirtualDirectory"));
    }
    m_vdCache.clear();
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
    const wxArrayString& ignoreFiles, const wxArrayString& excludePaths, wxArrayString& regexes)
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

void Project::GetFilesMetadata(Project::FileInfoVector_t& files) const
{
    std::queue<wxXmlNode*> elements;
    if(!m_doc.IsOk() || !m_doc.GetRoot()) return;

    elements.push(m_doc.GetRoot());
    files.reserve(1000); // make room for at least for 1000 files (should be enough for most projects)

    while(!elements.empty()) {
        wxXmlNode* element = elements.front();
        elements.pop();

        while(element) {
            if(element->GetName() == wxT("File")) {

                // files are kept relative to the project file
                wxString fileName = element->GetAttribute(wxT("Name"), wxEmptyString);
                wxFileName tmp(fileName);
                tmp.MakeAbsolute(GetProjectPath());
                FileInfo fi;
                fi.SetFilenameRelpath(fileName);
                fi.SetFilename(tmp.GetFullPath());
                fi.SetFlags(XmlUtils::ReadLong(element, "Flags", 0));

                wxString excludeConfigs = XmlUtils::ReadString(element, EXCLUDE_FROM_BUILD_FOR_CONFIG);
                fi.SetExcludeConfigs(::wxStringTokenize(excludeConfigs, ";", wxTOKEN_STRTOK));
                fi.SetVirtualFolder(DoFormatVirtualFolderName(element));
                files.push_back(fi);

            } else if(element->GetChildren()) {
                elements.push(element->GetChildren());
            }
            element = element->GetNext();
        }
    }

    // releaes any un-needed memory
    Project::FileInfoVector_t(files).swap(files);
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

wxArrayString Project::GetExcludeConfigForFile(const wxString& filename, const wxString& virtualDirPath)
{
    wxArrayString configs;
    wxXmlNode* vdNode = GetVirtualDir(virtualDirPath);
    if(!vdNode) {
        return configs;
    }

    // locate our file
    wxFileName tmp(filename);
    tmp.MakeRelativeTo(m_fileName.GetPath());
    wxString filepath = tmp.GetFullPath(wxPATH_UNIX);
    wxXmlNode* fileNode = XmlUtils::FindNodeByName(vdNode, "File", filepath);
    if(!fileNode) {
        return configs;
    }

    wxString excludeConfigs = XmlUtils::ReadString(fileNode, EXCLUDE_FROM_BUILD_FOR_CONFIG);
    configs = ::wxStringTokenize(excludeConfigs, ";", wxTOKEN_STRTOK);
    return configs;
}

void Project::SetExcludeConfigForFile(
    const wxString& filename, const wxString& virtualDirPath, const wxArrayString& configs)
{
    wxXmlNode* vdNode = GetVirtualDir(virtualDirPath);
    if(!vdNode) {
        return;
    }

    // locate our file
    wxFileName tmp(filename);
    tmp.MakeRelativeTo(m_fileName.GetPath());
    wxString filepath = tmp.GetFullPath(wxPATH_UNIX);
    wxXmlNode* fileNode = XmlUtils::FindNodeByName(vdNode, "File", filepath);
    if(!fileNode) {
        return;
    }

    // Make sure the list is unique
    wxStringSet_t unique_set;
    unique_set.insert(configs.begin(), configs.end());
    wxArrayString uniqueArr;
    wxStringSet_t::iterator iter = unique_set.begin();
    for(; iter != unique_set.end(); ++iter) {
        uniqueArr.Add(*iter);
    }

    wxString excludeConfigs = ::wxJoin(uniqueArr, ';');
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
    s_backticks.clear();

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
    FileNameVector_t files;
    GetFiles(files, true);

    wxString cFilePattern = GetCompileLineForCXXFile("$FileName", false);
    wxString cxxFilePattern = GetCompileLineForCXXFile("$FileName", true);
    wxString workingDirectory = m_fileName.GetPath();

    for(size_t i = 0; i < files.size(); ++i) {
        const wxFileName& fn = files.at(i);
        const wxString fullpath = fn.GetFullPath();

        wxString pattern;
        FileExtManager::FileType fileType = FileExtManager::GetType(fullpath);
        if(fileType == FileExtManager::TypeSourceC) {
            pattern = cFilePattern;

        } else if(fileType == FileExtManager::TypeSourceCpp) {
            pattern = cxxFilePattern;

        } else {
            continue;
        }

        wxString file_name = fullpath;
        if(file_name.Contains(" ")) {
            file_name.Prepend("\"").Append("\"");
        }
        pattern.Replace("$FileName", file_name);

        JSONElement json = JSONElement::createObject();
        json.addProperty("file", fullpath);
        json.addProperty("directory", workingDirectory);
        json.addProperty("command", pattern);
        compile_commands.append(json);
    }
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
    std::map<wxString, wxArrayString> configs;
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
    }
}
