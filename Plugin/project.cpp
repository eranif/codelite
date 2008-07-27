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
#include "xmlutils.h"
#include <wx/tokenzr.h>
#include "wx/arrstr.h"
#include "dirsaver.h"

const wxString Project::STATIC_LIBRARY = wxT("Static Library");
const wxString Project::DYNAMIC_LIBRARY = wxT("Dynamic Library");
const wxString Project::EXECUTABLE = wxT("Executable");

Project::Project()
		: m_tranActive(false)
		, m_isModified(false)
{
}

Project::~Project()
{
}

bool Project::Create(const wxString &name, const wxString &description, const wxString &path, const wxString &projType)
{
	m_fileName = path + wxFileName::GetPathSeparator() + name + wxT(".project");
	m_fileName.MakeAbsolute();

	wxXmlNode *root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CodeLite_Project"));
	m_doc.SetRoot(root);
	m_doc.GetRoot()->AddProperty(wxT("Name"), name);

	wxXmlNode *descNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Description"));
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

	//creae dependencies node
	wxXmlNode *depNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Dependencies"));
	root->AddChild(depNode);

	m_doc.Save(m_fileName.GetFullPath());
	//create build settings
	SetSettings(new ProjectSettings(NULL));
	ProjectSettingsPtr settings = GetSettings();
	settings->SetProjectType(projType);
	SetSettings(settings);
	SetModified(true);
	return true;
}

bool Project::Load(const wxString &path)
{
	if ( !m_doc.Load(path) ) {
		return false;
	}

	m_fileName = path;
	m_fileName.MakeAbsolute();
	SetModified(true);
	return true;
}

wxXmlNode *Project::GetVirtualDir(const wxString &vdFullPath)
{
	wxStringTokenizer tkz(vdFullPath, wxT(":"));

	wxXmlNode *parent = m_doc.GetRoot();
	while ( tkz.HasMoreTokens() ) {
		parent = XmlUtils::FindNodeByName(parent, wxT("VirtualDirectory"), tkz.GetNextToken());
		if ( !parent ) {
			return NULL;
		}
	}
	return parent;
}

wxXmlNode *Project::CreateVD(const wxString &vdFullPath, bool mkpath)
{
	wxXmlNode *oldVd = GetVirtualDir(vdFullPath);
	if ( oldVd ) {
		// VD already exist
		return oldVd;
	}

	wxStringTokenizer tkz(vdFullPath, wxT(":"));

	wxXmlNode *parent = m_doc.GetRoot();
	size_t count = tkz.CountTokens();
	for (size_t i=0; i<count-1; i++) {
		wxString token = tkz.NextToken();
		wxXmlNode *p = XmlUtils::FindNodeByName(parent, wxT("VirtualDirectory"), token);
		if ( !p ) {
			if ( mkpath ) {

				//add the node
				p = new wxXmlNode(parent, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
				p->AddProperty(wxT("Name"), token);

			} else {
				return NULL;
			}
		}
		parent = p;
	}

	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
	node->AddProperty(wxT("Name"), tkz.GetNextToken());

	parent->AddChild(node);

	//if not in transaction save the changes
	if (!InTransaction()) {
		m_doc.Save(m_fileName.GetFullPath());
	}
	return node;
}

bool Project::IsFileExist(const wxString &fileName)
{
	//find the file under this node
	// Convert the file path to be relative to
	// the project path
	DirSaver ds;

	::wxSetWorkingDirectory(m_fileName.GetPath());
	wxFileName tmp(fileName);
	tmp.MakeRelativeTo(m_fileName.GetPath());

	std::vector<wxFileName> files;
	GetFiles(files);

	for (size_t i=0; i<files.size(); i++) {
		if (files.at(i).GetFullPath() == tmp.GetFullPath()) {
			return true;
		}
	}
	return false;
}

bool Project::AddFile(const wxString &fileName, const wxString &virtualDirPath)
{
	wxXmlNode *vd = GetVirtualDir(virtualDirPath);
	if ( !vd ) {
		return false;
	}

	// Convert the file path to be relative to
	// the project path
	DirSaver ds;

	::wxSetWorkingDirectory(m_fileName.GetPath());
	wxFileName tmp(fileName);
	tmp.MakeRelativeTo(m_fileName.GetPath());

	// if we already have a file with the same name, return false
	if (this->IsFileExist(fileName)) {
		return false;
	}

	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
	node->AddProperty(wxT("Name"), tmp.GetFullPath());
	vd->AddChild(node);
	if (!InTransaction()) {
		m_doc.Save(m_fileName.GetFullPath());
	}
	SetModified(true);
	return true;
}

bool Project::CreateVirtualDir(const wxString &vdFullPath, bool mkpath)
{
	return CreateVD(vdFullPath, mkpath) != NULL;
}

bool Project::DeleteVirtualDir(const wxString &vdFullPath)
{
	wxXmlNode *vd = GetVirtualDir(vdFullPath);
	if ( vd ) {
		wxXmlNode *parent = vd->GetParent();
		if ( parent ) {
			parent->RemoveChild( vd );
		}

		delete vd;
		SetModified(true);
		return m_doc.Save(m_fileName.GetFullPath());
	}
	return false;
}

bool Project::RemoveFile(const wxString &fileName, const wxString &virtualDir)
{
	wxXmlNode *vd = GetVirtualDir(virtualDir);
	if ( !vd ) {
		return false;
	}

	// Convert the file path to be relative to
	// the project path
	DirSaver ds;

	::wxSetWorkingDirectory(m_fileName.GetPath());
	wxFileName tmp(fileName);
	tmp.MakeRelativeTo(m_fileName.GetPath());

	wxXmlNode *node = XmlUtils::FindNodeByName(vd, wxT("File"), tmp.GetFullPath());
	if ( node ) {
		node->GetParent()->RemoveChild( node );
		delete node;
	}
	SetModified(true);
	return m_doc.Save(m_fileName.GetFullPath());;
}

wxString Project::GetName() const
{
	return m_doc.GetRoot()->GetPropVal(wxT("Name"), wxEmptyString);
}

ProjectTreePtr Project::AsTree()
{
	ProjectItem item(GetName(), GetName(), GetFileName().GetFullPath(), ProjectItem::TypeProject);
	ProjectTreePtr ptp(new ProjectTree(item.Key(), item));

	wxXmlNode *child = m_doc.GetRoot()->GetChildren();
	while ( child ) {
		RecursiveAdd(child, ptp, ptp->GetRoot());
		child = child->GetNext();
	}
	return ptp;
}

void Project::RecursiveAdd(wxXmlNode *xmlNode, ProjectTreePtr &ptp, ProjectTreeNode *nodeParent)
{
	// Build the key for this node
	std::list<wxString> nameList;

	wxXmlNode *parent = xmlNode->GetParent();
	while ( parent ) {
		nameList.push_front(parent->GetPropVal(wxT("Name"), wxEmptyString));
		parent = parent->GetParent();
	}

	wxString key;
	for (size_t i=0; i<nameList.size(); i++) {
		key += nameList.front();
		key += wxT(":");
		nameList.pop_front();
	}
	key += xmlNode->GetPropVal(wxT("Name"), wxEmptyString);

	// Create the tree node data
	ProjectItem item;
	if ( xmlNode->GetName() == wxT("Project") ) {
		item = ProjectItem(key, xmlNode->GetPropVal(wxT("Name"), wxEmptyString), wxEmptyString, ProjectItem::TypeProject);
	} else if ( xmlNode->GetName() == wxT("VirtualDirectory") ) {
		item = ProjectItem(key, xmlNode->GetPropVal(wxT("Name"), wxEmptyString), wxEmptyString, ProjectItem::TypeVirtualDirectory);
	} else if ( xmlNode->GetName() == wxT("File") ) {
		wxFileName filename(xmlNode->GetPropVal(wxT("Name"), wxEmptyString));
		//convert this file name to absolute path
		DirSaver ds;
		::wxSetWorkingDirectory(m_fileName.GetPath());
		filename.MakeAbsolute();
		item = ProjectItem(key, filename.GetFullName(), filename.GetFullPath(), ProjectItem::TypeFile);
	} else {
		// un-recognised or not viewable item in the tree,
		// skip it and its children
		return;
	}

	ProjectTreeNode *newNode = ptp->AddChild(item.Key(), item, nodeParent);
	// This node has children, add them as well
	wxXmlNode *children = xmlNode->GetChildren();

	while ( children ) {
		RecursiveAdd(children, ptp, newNode);
		children = children->GetNext();
	}
	SetModified(true);
}

void Project::Save()
{
	m_tranActive = false;
	if ( m_doc.IsOk() )
		m_doc.Save(m_fileName.GetFullPath());
}

void Project::GetFilesByVirtualDir(const wxString &vdFullPath, wxArrayString &files)
{
	wxXmlNode *vd = GetVirtualDir(vdFullPath);
	if ( vd ) {
		wxXmlNode *child = vd->GetChildren();
		while ( child ) {
			if ( child->GetName() == wxT("File")) {
				wxFileName fileName(
				    child->GetPropVal(wxT("Name"), wxEmptyString)
				);
				fileName.MakeAbsolute(m_fileName.GetPath());
				files.Add(fileName.GetFullPath());
			}
			child = child->GetNext();
		}
	}
}

void Project::GetFiles(std::vector<wxFileName> &files, bool absPath)
{
	if (absPath) {
		DirSaver ds;
		::wxSetWorkingDirectory(m_fileName.GetPath());

		GetFiles(m_doc.GetRoot(), files, true);
	} else {
		GetFiles(m_doc.GetRoot(), files, false);
	}
}

void Project::GetFiles(wxXmlNode *parent, std::vector<wxFileName> &files, bool absPath)
{
	if ( !parent ) {
		return;
	}

	wxXmlNode *child = parent->GetChildren();
	while (child) {
		if (child->GetName() == wxT("File")) {
			wxString fileName = child->GetPropVal(wxT("Name"), wxEmptyString);
			wxFileName tmp(fileName);
			if (absPath) {
				tmp.MakeAbsolute();
			}
			files.push_back(tmp);
		} else if (child->GetChildren()) {// we could also add a check for VirtualDirectory only
			GetFiles(child, files, absPath);
		}
		child = child->GetNext();
	}
}

ProjectSettingsPtr Project::GetSettings() const
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Settings"));
	return new ProjectSettings(node);
}

void Project::SetSettings(ProjectSettingsPtr settings)
{
	wxXmlNode *oldSettings = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Settings"));
	if (oldSettings) {
		oldSettings->GetParent()->RemoveChild(oldSettings);
		delete oldSettings;
	}
	m_doc.GetRoot()->AddChild(settings->ToXml());
	m_doc.Save(m_fileName.GetFullPath());
}

wxArrayString Project::GetDependencies() const
{
	wxArrayString result;
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Dependencies"));
	if (node) {
		wxXmlNode *child = node->GetChildren();
		while (child) {
			if (child->GetName() == wxT("Project")) {
				result.Add(XmlUtils::ReadString(child, wxT("Name")));
			}
			child = child->GetNext();
		}
	}
	return result;
}

void Project::SetModified(bool mod)
{
	m_isModified = mod;
}

bool Project::IsModified()
{
	return m_isModified;
}

wxString Project::GetDescription() const
{
	wxXmlNode *root = m_doc.GetRoot();
	if (root) {
		wxXmlNode *node = XmlUtils::FindFirstByTagName(root, wxT("Description"));
		if (node) {
			return node->GetNodeContent();
		}
	}
	return wxEmptyString;
}

void Project::CopyTo(const wxString& new_path, const wxString& new_name, const wxString& description)
{
	// first save the xml document to the destination folder

	wxString newFile = new_path + new_name + wxT(".project");
	if ( !m_doc.Save( newFile ) ) {
		return;
	}

	// load the new xml and modify it
	wxXmlDocument doc;
	if ( !doc.Load( newFile ) ) {
		return;
	}

	// update the 'Name' property
	XmlUtils::UpdateProperty(doc.GetRoot(), wxT("Name"), new_name);

	// set description
	wxXmlNode *descNode(NULL);

	// update the description
	descNode = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("Description"));
	if (!descNode) {
		descNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Description"));
		doc.GetRoot()->AddChild(descNode);
	}
	XmlUtils::SetNodeContent(descNode, description);

	// Remove the 'Dependencies'
	wxXmlNode *deps = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("Dependencies"));
	if (deps) {
		doc.GetRoot()->RemoveChild(deps);
		delete deps;
	}

	// add an empty deps node
	deps = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Dependencies"));
	doc.GetRoot()->AddChild(deps);

	// Remove virtual folders
	wxXmlNode *vd = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("VirtualDirectory"));
	while (vd) {
		doc.GetRoot()->RemoveChild( vd );
		delete vd;
		vd = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("VirtualDirectory"));
	}

	// add all files under this path
	std::vector<wxFileName> files;
	GetFiles(files, true);

	wxXmlNode *srcNode(NULL);
	wxXmlNode *headNode(NULL);
	wxXmlNode *rcNode(NULL);

	// copy the files to their new location
	for (size_t i=0; i<files.size(); i++) {
		wxFileName fn = files.at(i);
		wxCopyFile(fn.GetFullPath(), new_path + wxT("/") + fn.GetFullName());

		// add source file under the 'src' while headers are added under 'include'
		wxString e(fn.GetExt());
		e = e.MakeLower();

		wxXmlNode *file_node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
		file_node->AddProperty(wxT("Name"), fn.GetFullName());

		if ( e == wxT("cpp") || e == wxT("cxx") || e == wxT("c") || e == wxT("c++") || e == wxT("cc") ) {
			// source file
			if ( !srcNode ) {
				srcNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
				srcNode->AddProperty(wxT("Name"), wxT("src"));
				doc.GetRoot()->AddChild(srcNode);
			}
			srcNode->AddChild(file_node);

		} else if ( e == wxT("h") || e == wxT("hpp") || e == wxT("h++") || e == wxT("hxx")|| e == wxT("hh")  || e == wxT("inc") || e == wxT("inl") ) {
			// header file
			if (!headNode) {
				headNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
				headNode->AddProperty(wxT("Name"), wxT("include"));
				doc.GetRoot()->AddChild(headNode);
			}
			headNode->AddChild(file_node);

		} else {
			// resource file
			if ( !rcNode ) {
				rcNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
				rcNode->AddProperty(wxT("Name"), wxT("resources"));
				doc.GetRoot()->AddChild(rcNode);
			}
			rcNode->AddChild(file_node);
		}
	}
	doc.Save(newFile);
}

void Project::SetFiles(ProjectPtr src)
{
	// first remove all the virtual directories from this project
	// Remove virtual folders
	wxXmlNode *vd = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("VirtualDirectory"));
	while (vd) {
		m_doc.GetRoot()->RemoveChild( vd );
		delete vd;
		vd = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("VirtualDirectory"));
	}

	// copy the virtual directories from the src project
	wxXmlNode *child = src->m_doc.GetRoot()->GetChildren();
	while ( child ) {
		if ( child->GetName() == wxT("VirtualDirectory") ) {
			// create a new VirtualDirectory like this one
			wxXmlNode *newNode = new wxXmlNode(*child);
			m_doc.GetRoot()->AddChild(newNode);
		}
		child = child->GetNext();
	}
	m_doc.Save(m_fileName.GetFullPath());
}

bool Project::RenameFile(const wxString& oldName, const wxString& virtualDir, const wxString& newName)
{
	wxXmlNode *vd = GetVirtualDir(virtualDir);
	if ( !vd ) {
		return false;
	}

	// Convert the file path to be relative to
	// the project path
	DirSaver ds;

	::wxSetWorkingDirectory(m_fileName.GetPath());
	wxFileName tmp(oldName);
	tmp.MakeRelativeTo(m_fileName.GetPath());

	wxXmlNode *node = XmlUtils::FindNodeByName(vd, wxT("File"), tmp.GetFullPath());
	if ( node ) {
		// update the new name
		tmp.SetFullName(newName);
		XmlUtils::UpdateProperty(node, wxT("Name"), tmp.GetFullPath());
	}

	SetModified(true);
	return m_doc.Save(m_fileName.GetFullPath());;
}

wxString Project::GetVDByFileName(const wxString& file)
{
	//find the file under this node
	// Convert the file path to be relative to
	// the project path
	DirSaver ds;

	::wxSetWorkingDirectory(m_fileName.GetPath());
	wxFileName tmp(file);
	tmp.MakeRelativeTo(m_fileName.GetPath());

	wxString path(wxEmptyString);
	wxXmlNode *fileNode = FindFile(m_doc.GetRoot(), tmp.GetFullPath());

	if (fileNode) {
		wxXmlNode *parent = fileNode->GetParent();
		while ( parent ) {
			if (parent->GetName() == wxT("VirtualDirectory")) {
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
	wxXmlNode *child = parent->GetChildren();
	while ( child ) {
		wxString name = child->GetName();
		if (name == wxT("File") && child->GetPropVal(wxT("Name"), wxEmptyString) == file) {
			return child;
		}

		if (child->GetName() == wxT("VirtualDirectory")) {
			wxXmlNode *n = FindFile(child, file);
			if (n) {
				return n;
			}
		}
		child = child->GetNext();
	}
	return NULL;
}

bool Project::RenameVirtualDirectory(const wxString& oldVdPath, const wxString& newName)
{
	wxXmlNode *vdNode = GetVirtualDir(oldVdPath);
	if (vdNode) {
		XmlUtils::UpdateProperty(vdNode, wxT("Name"), newName);
		return m_doc.Save(m_fileName.GetFullPath());
	}
	return false;
}

wxArrayString Project::GetDependencies(const wxString& configuration) const
{
	wxArrayString result;

	// dependencies are located directly under the root level
	wxXmlNode *node = m_doc.GetRoot()->GetChildren();
	while (node) {
		if ( node->GetName() == wxT("Dependencies") && node->GetPropVal(wxT("Name"), wxEmptyString) == configuration) {
			// we have our match
			wxXmlNode *child = node->GetChildren();
			while (child) {
				if (child->GetName() == wxT("Project")) {
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
	wxXmlNode *node = m_doc.GetRoot()->GetChildren();
	while (node) {
		if ( node->GetName() == wxT("Dependencies") && node->GetPropVal(wxT("Name"), wxEmptyString) == configuration) {
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
	
	//create a node for each dependency in the array
	for (size_t i=0; i<deps.GetCount(); i++) {
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
		child->AddProperty(wxT("Name"), deps.Item(i));
		node->AddChild(child);
	}

	//save changes
	m_doc.Save(m_fileName.GetFullPath());
	SetModified(true);	
}

void Project::GetFiles(std::vector<wxFileName>& files, std::vector<wxFileName>& absFiles)
{
	DirSaver ds;
	::wxSetWorkingDirectory(m_fileName.GetPath());
	GetFiles(m_doc.GetRoot(), files, absFiles);
}

void Project::GetFiles(wxXmlNode *parent, std::vector<wxFileName>& files, std::vector<wxFileName>& absFiles)
{
	if ( !parent ) {
		return;
	}

	wxXmlNode *child = parent->GetChildren();
	while (child) {
		if (child->GetName() == wxT("File")) {
			wxString fileName = child->GetPropVal(wxT("Name"), wxEmptyString);
			wxFileName tmp(fileName);
			
			// append the file as it appears
			files.push_back(tmp);
			
			// convert to absolute path
			tmp.MakeAbsolute();
			absFiles.push_back(tmp);
			
		} else if (child->GetChildren()) {// we could also add a check for VirtualDirectory only
			GetFiles(child, files, absFiles);
		}
		child = child->GetNext();
	}
}
