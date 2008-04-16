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

void Project::SetDependencies(wxArrayString &deps)
{
	//remove old node
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Dependencies"));
	if (node) {
		m_doc.GetRoot()->RemoveChild(node);
		delete node;
	}

	node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Dependencies"));
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

void Project::SetModified(bool mod)
{
	m_isModified = mod;
}

bool Project::IsModified()
{
	return m_isModified;
}
