//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editor_config.cpp
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
#include "dirsaver.h"
#include "precompiled_header.h"
#include "editor_config.h"
#include <wx/xml/xml.h>
#include "xmlutils.h"
#include "dirtraverser.h"
#include <wx/ffile.h>

//-------------------------------------------------------------------------------------------
SimpleLongValue::SimpleLongValue()
{
}

SimpleLongValue::~SimpleLongValue()
{
}

void SimpleLongValue::Serialize(Archive &arch)
{
	arch.Write(wxT("m_value"), m_value);
}

void SimpleLongValue::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_value"), m_value);
}

//-------------------------------------------------------------------------------------------
SimpleStringValue::SimpleStringValue()
{
}

SimpleStringValue::~SimpleStringValue()
{
}

void SimpleStringValue::Serialize(Archive &arch)
{
	arch.Write(wxT("m_value"), m_value);
}

void SimpleStringValue::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_value"), m_value);
}

//-------------------------------------------------------------------------------------------

wxString EditorConfig::m_svnRevision;

EditorConfig::EditorConfig()
{
	m_doc = new wxXmlDocument();
}

EditorConfig::~EditorConfig()
{
	delete m_doc;
}

bool EditorConfig::Load()
{
	//first try to load the user's settings
	m_fileName = wxFileName(wxT("config/codelite.xml"));
	m_fileName.MakeAbsolute();

	if (!m_fileName.FileExists()) {
		//try to load the default settings
		m_fileName = wxFileName(wxT("config/codelite.xml.default"));
		m_fileName.MakeAbsolute();

		if ( !m_fileName.FileExists() ) {
			//create a new empty file with this name so the load function will not
			//fail
			wxFFile file(m_fileName.GetFullPath(), wxT("a"));
			wxString content;
			content << wxT("<LiteEditor Revision=\"")
			<< m_svnRevision
			<< wxT("\">")
			<< wxT("</LiteEditor>");

			if (file.IsOpened()) {
				file.Write(content);
				file.Close();
			}
		}
	}

	// load the main configuration file
	if (!m_doc->Load(m_fileName.GetFullPath())) {
		return false;
	}

	// load CodeLite lexers
	LoadLexers();

	// make sure that the file name is set to .xml and not .default
	m_fileName.SetFullName(wxT("codelite.xml"));
	return true;
}

void EditorConfig::SaveLexers()
{
	std::map<wxString, LexerConfPtr>::iterator iter = m_lexers.begin();
	for (; iter != m_lexers.end(); iter++) {
		iter->second->Save();
	}
}

wxXmlNode* EditorConfig::GetLexerNode(const wxString& lexerName)
{
	wxXmlNode *lexersNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Lexers"));
	if ( lexersNode ) {
		return XmlUtils::FindNodeByName(lexersNode, wxT("Lexer"), lexerName);
	}
	return NULL;
}

LexerConfPtr EditorConfig::GetLexer(const wxString &lexerName)
{
	if (m_lexers.find(lexerName) == m_lexers.end()) {
		return NULL;
	}

	return m_lexers.find(lexerName)->second;
}

wxString EditorConfig::LoadPerspective(const wxString &Name) const
{
	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));
	if ( !layoutNode ) {
		//add an Layout node
		wxXmlNode *newChild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Layout"));
		m_doc->GetRoot()->AddChild(newChild);
		m_doc->Save(m_fileName.GetFullPath());
		layoutNode = newChild;
	}

	wxXmlNode *child = layoutNode->GetChildren();
	while ( child ) {
		if ( child->GetName() == wxT("Perspective") ) {
			if (child->GetPropVal(wxT("Name"), wxEmptyString) == Name) {
				return child->GetPropVal(wxT("Value"), wxEmptyString);
			}
		}
		child = child->GetNext();
	}

	return wxEmptyString;
}

//long EditorConfig::LoadNotebookStyle(const wxString &nbName)
//{
//	long style = wxNOT_FOUND;
//	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));
//	if (!layoutNode) {
//		return style;
//	}
//
//	wxXmlNode *child = layoutNode->GetChildren();
//	while ( child ) {
//		if ( child->GetName() == wxT("Notebook") ) {
//			if (child->GetPropVal(wxT("Name"), wxEmptyString) == nbName) {
//				wxString strStyle = child->GetPropVal(wxT("Style"), wxEmptyString);
//				strStyle.ToLong(&style);
//				break;
//			}
//		}
//		child = child->GetNext();
//	}
//	return style;
//}
//
//void EditorConfig::SaveNotebookStyle(const wxString &nbName, long style)
//{
//	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));
//	if ( !layoutNode ) {
//		return;
//	}
//
//	wxXmlNode *child = layoutNode->GetChildren();
//	while ( child ) {
//		if ( child->GetName() == wxT("Notebook") ) {
//			if (child->GetPropVal(wxT("Name"), wxEmptyString) == nbName) {
//				wxString strStyle;
//				strStyle << style;
//				XmlUtils::UpdateProperty(child, wxT("Style"), strStyle);
//				m_doc->Save(m_fileName.GetFullPath());
//				return;
//			}
//		}
//		child = child->GetNext();
//	}
//
//	wxXmlNode *newChild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Notebook"));
//	newChild->AddProperty(wxT("Name"), nbName);
//
//	wxString strStyle;
//	strStyle << style;
//	newChild->AddProperty(wxT("Style"), strStyle);
//	layoutNode->AddChild(newChild);
//	m_doc->Save(m_fileName.GetFullPath());
//}
//

EditorConfig::ConstIterator EditorConfig::LexerEnd()
{
	return m_lexers.end();
}

EditorConfig::ConstIterator EditorConfig::LexerBegin()
{
	return m_lexers.begin();
}

OptionsConfigPtr EditorConfig::GetOptions() const
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Options"));
	// node can be null ...
	return new OptionsConfig(node);
}

void EditorConfig::SetOptions(OptionsConfigPtr opts)
{
	// locate the current node
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Options"));
	if ( node ) {
		m_doc->GetRoot()->RemoveChild(node);
		delete node;
	}

	m_doc->GetRoot()->AddChild(opts->ToXml());
	m_doc->Save(m_fileName.GetFullPath());
}

void EditorConfig::SetTagsDatabase(const wxString &dbName)
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("TagsDatabase"));
	if ( node ) {
		XmlUtils::UpdateProperty(node, wxT("Path"), dbName);
	} else {
		//create new node
		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("TagsDatabase"));
		node->AddProperty(wxT("Path"), dbName);
		m_doc->GetRoot()->AddChild(node);
	}
	m_doc->Save(m_fileName.GetFullPath());
}

wxString EditorConfig::GetTagsDatabase() const
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("TagsDatabase"));
	if (node) {
		return XmlUtils::ReadString(node, wxT("Path"));
	} else {
		return wxEmptyString;
	}
}

void EditorConfig::GetRecentlyOpenedFies(wxArrayString &files)
{
	//find the root node of the recent files
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("RecentFiles"));
	if (node) {
		wxXmlNode *child = node->GetChildren();
		while (child) {
			if (child->GetName() == wxT("File")) {
				wxString fileName = XmlUtils::ReadString(child, wxT("Name"));
				// wxXmlDocument Saves/Loads items in reverse order, so prepend, not add.
				files.Insert(fileName, 0);
			}
			child = child->GetNext();
		}
	}
}

void EditorConfig::SetRecentlyOpenedFies(const wxArrayString &files)
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("RecentFiles"));
	if (node) {
		wxXmlNode *root = m_doc->GetRoot();
		root->RemoveChild(node);
		delete node;
	}

	//create new entry in the configuration file
	node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("RecentFiles"));
	m_doc->GetRoot()->AddChild(node);

	for (size_t i=0; i<files.GetCount(); i++) {
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
		child->AddProperty(wxT("Name"), files.Item(i));
		node->AddChild(child);
	}

	//save the data to disk
	m_doc->Save(m_fileName.GetFullPath());
}


void EditorConfig::GetRecentlyOpenedWorkspaces(wxArrayString &files)
{
	//find the root node of the recent files
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("RecentWorkspaces"));
	if (node) {
		wxXmlNode *child = node->GetChildren();
		while (child) {
			if (child->GetName() == wxT("File")) {
				wxString fileName = XmlUtils::ReadString(child, wxT("Name"));
				// wxXmlDocument Saves/Loads items in reverse order, so prepend, not add.
				files.Insert(fileName, 0);
			}
			child = child->GetNext();
		}
	}
}

void EditorConfig::SetRecentlyOpenedWorkspaces(const wxArrayString &files)
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("RecentWorkspaces"));
	if (node) {
		wxXmlNode *root = m_doc->GetRoot();
		root->RemoveChild(node);
		delete node;
	}

	node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("RecentWorkspaces"));
	m_doc->GetRoot()->AddChild(node);

	for (size_t i=0; i<files.GetCount(); i++) {
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
		child->AddProperty(wxT("Name"), files.Item(i));
		node->AddChild(child);
	}

	//save the data to disk
	m_doc->Save(m_fileName.GetFullPath());
}

bool EditorConfig::WriteObject(const wxString &name, SerializedObject *obj)
{
	Archive arch;

	wxXmlNode *child = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("ArchiveObject"), name);
	if (child) {
		wxXmlNode *n = m_doc->GetRoot();
		n->RemoveChild(child);
		delete child;
	}

	//create new xml node for this object
	child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ArchiveObject"));
	m_doc->GetRoot()->AddChild(child);
	child->AddProperty(wxT("Name"), name);

	arch.SetXmlNode(child);
	//serialize the object into the archive
	obj->Serialize(arch);
	//save the archive
	return m_doc->Save(m_fileName.GetFullPath());
}

bool EditorConfig::ReadObject(const wxString &name, SerializedObject *obj)
{
	//find the object node in the xml file
	wxXmlNode *node = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("ArchiveObject"), name);
	if (node) {
		Archive arch;
		arch.SetXmlNode(node);
		obj->DeSerialize(arch);
		return true;
	}
	return false;
}

wxString EditorConfig::GetRevision() const
{
	return XmlUtils::ReadString(m_doc->GetRoot(), wxT("Revision"), wxEmptyString);
}

void EditorConfig::SetRevision(const wxString &rev)
{
	wxXmlNode *root = m_doc->GetRoot();
	if (!root) {
		return;
	}

	XmlUtils::UpdateProperty(root, wxT("Revision"), rev);
	m_doc->Save(m_fileName.GetFullPath());
}


void EditorConfig::SaveLongValue(const wxString &name, long value)
{
	SimpleLongValue data;
	data.SetValue(value);
	WriteObject(name, &data);
}


bool EditorConfig::GetLongValue(const wxString &name, long &value)
{
	SimpleLongValue data;
	if (ReadObject(name, &data)) {
		value = data.GetValue();
		return true;
	}
	return false;
}

wxString EditorConfig::GetStringValue(const wxString &key)
{
	SimpleStringValue data;
	ReadObject(key, &data);
	return data.GetValue();
}

void EditorConfig::SaveStringValue(const wxString &key, const wxString &value)
{
	SimpleStringValue data;
	data.SetValue(value);
	WriteObject(key, &data);
}

void EditorConfig::LoadLexers()
{
	wxString theme = GetStringValue(wxT("LexerTheme"));
	if (theme.IsEmpty()) {
		theme = wxT("Default");
		SaveStringValue(wxT("LexerTheme"), wxT("Default"));
	}

	//when this function is called, the working directory is located at the
	//startup directory
	DirSaver ds;
	wxSetWorkingDirectory(m_fileName.GetPath());

	wxString cwd = wxGetCwd();

	//load all lexer configuration files
	DirTraverser traverser(wxT("*.xml"));
	wxString path_( cwd + wxT("/../lexers/") + theme + wxT("/") );
	if (wxDir::Exists(path_) == false) {
		//the directory does not exist
		//fallback to 'Default'
		theme = wxT("Default");
		SaveStringValue(wxT("LexerTheme"), wxT("Default"));
		path_ = cwd + wxT("/../lexers/") + theme + wxT("/");
	}

	wxDir dir(path_);
	dir.Traverse(traverser);

	wxArrayString files = traverser.GetFiles();
	m_lexers.clear();
	for (size_t i=0; i<files.GetCount(); i++) {

		wxString fileToLoad( files.Item(i) );

		//try to locate a file with the same name but with the user extension
		wxFileName fn(files.Item(i));
		wxString userLexer( fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR) + fn.GetName() +  wxT(".") + wxGetUserName() + wxT("_xml"));
		if ( wxFileName::FileExists( userLexer ) ) {
			fileToLoad = userLexer;
		}

		LexerConfPtr lexer(new LexerConf( fileToLoad ));
		m_lexers[lexer->GetName()] = lexer;
	}
}

//--------------------------------------------------
// Simple rectangle class wrapper
//--------------------------------------------------
SimpleRectValue::SimpleRectValue()
{
}

SimpleRectValue::~SimpleRectValue()
{
}

void SimpleRectValue::DeSerialize(Archive& arch)
{
	wxPoint pos;
	wxSize size;
	
	arch.Read(wxT("TopLeft"), pos);
	arch.Read(wxT("Size"), size);
	
	m_rect = wxRect(pos, size);
}

void SimpleRectValue::Serialize(Archive& arch)
{
	arch.Write(wxT("TopLeft"), m_rect.GetTopLeft());
	arch.Write(wxT("Size"), m_rect.GetSize());
}
