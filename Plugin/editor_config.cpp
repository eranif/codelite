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
#include "globals.h"

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
wxString EditorConfig::m_version;

EditorConfig::EditorConfig()
		: m_transcation(false)
{
	m_doc = new wxXmlDocument();
}

EditorConfig::~EditorConfig()
{
	delete m_doc;
}

bool EditorConfig::DoLoadDefaultSettings()
{
	//try to load the default settings
	m_fileName = wxFileName(wxT("config/codelite.xml.default"));
	m_fileName.MakeAbsolute();

	if ( !m_fileName.FileExists() ) {
		//create a new empty file with this name so the load function will not
		//fail
		wxFFile file(m_fileName.GetFullPath(), wxT("a"));
		wxString content;
		content << wxT("<CodeLite Revision=\"")
		<< m_svnRevision
		<< wxT("\"")
		<< wxT(" Version=\"")
		<< m_version
		<< wxT("\">")
		<< wxT("</CodeLite>");

		if (file.IsOpened()) {
			file.Write(content);
			file.Close();
		}
	}
	return m_doc->Load(m_fileName.GetFullPath());
}

bool EditorConfig::Load()
{
	//first try to load the user's settings
	m_fileName = wxFileName(wxT("config/codelite.xml"));
	m_fileName.MakeAbsolute();
	bool userSettingsLoaded(false);
	bool loadSuccess       (false);

	if (!m_fileName.FileExists()) {
		loadSuccess = DoLoadDefaultSettings();

	} else {
		userSettingsLoaded = true;
		loadSuccess = m_doc->Load(m_fileName.GetFullPath());
	}

	if ( !loadSuccess ) {
		return false;
	}

	// Check the codelite-version for this file
	wxString version;
	bool found = m_doc->GetRoot()->GetPropVal(wxT("Version"), &version);
	if ( userSettingsLoaded ) {
		if(!found || (found && version != this->m_version)) {
			if(DoLoadDefaultSettings() == false) {
				return false;
			}
		}
	}

	// load CodeLite lexers
	LoadLexers(false);

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
	wxString nodeName = wxT("Lexers");
	SendCmdEvent(wxEVT_EDITOR_CONFIG_CHANGED, (void*) &nodeName);
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
//				DoSave();
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
//	DoSave();
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
	OptionsConfigPtr opts = new OptionsConfig(node);

	// import legacy tab-width setting into opts
	long tabWidth(opts->GetTabWidth());
	if (const_cast<EditorConfig*>(this)->GetLongValue(wxT("EditorTabWidth"), tabWidth)) {
		opts->SetTabWidth(tabWidth);
	}

	return opts;
}

void EditorConfig::SetOptions(OptionsConfigPtr opts)
{
	// remove legacy tab-width setting
	wxXmlNode *child = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("ArchiveObject"), wxT("EditorTabWidth"));
	if (child) {
		m_doc->GetRoot()->RemoveChild(child);
		delete child;
	}

	// locate the current node
	wxString nodeName = wxT("Options");
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
	if ( node ) {
		m_doc->GetRoot()->RemoveChild(node);
		delete node;
	}
	m_doc->GetRoot()->AddChild(opts->ToXml());

	DoSave();
	SendCmdEvent(wxEVT_EDITOR_CONFIG_CHANGED, (void*) &nodeName);
}

void EditorConfig::SetTagsDatabase(const wxString &dbName)
{
	wxString nodeName = wxT("TagsDatabase");
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
	if ( node ) {
		XmlUtils::UpdateProperty(node, wxT("Path"), dbName);
	} else {
		//create new node
		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, nodeName);
		node->AddProperty(wxT("Path"), dbName);
		m_doc->GetRoot()->AddChild(node);
	}

	DoSave();
	SendCmdEvent(wxEVT_EDITOR_CONFIG_CHANGED, (void*) &nodeName);
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

void EditorConfig::GetRecentItems(wxArrayString &files, const wxString nodeName)
{
	if (nodeName.IsEmpty()) {
		return;
}
	//find the root node
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
	if (node) {
		wxXmlNode *child = node->GetChildren();
		while (child) {
			if (child->GetName() == wxT("File")) {
				wxString fileName = XmlUtils::ReadString(child, wxT("Name"));
				// wxXmlDocument Saves/Loads items in reverse order, so prepend, not add.
				if( wxFileExists( fileName ) ) files.Insert(fileName, 0);
			}
			child = child->GetNext();
		}
	}
}

void EditorConfig::SetRecentItems(const wxArrayString &files, const wxString nodeName)
{
	if (nodeName.IsEmpty()) {
		return;
	}
	//find the root node
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
	if (node) {
		wxXmlNode *root = m_doc->GetRoot();
		root->RemoveChild(node);
		delete node;
	}
	//create new entry in the configuration file
	node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, nodeName);
	m_doc->GetRoot()->AddChild(node);
	for (size_t i=0; i<files.GetCount(); i++) {
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
		child->AddProperty(wxT("Name"), files.Item(i));
		node->AddChild(child);
	}

	//save the data to disk
	DoSave();
	SendCmdEvent(wxEVT_EDITOR_CONFIG_CHANGED, (void*) &nodeName);
}

bool EditorConfig::WriteObject(const wxString &name, SerializedObject *obj)
{
	if(!XmlUtils::StaticWriteObject(m_doc->GetRoot(), name, obj))
		return false;

	//save the archive
	bool res = DoSave();
	SendCmdEvent(wxEVT_EDITOR_CONFIG_CHANGED, (void*) &name);
	return res;
}

bool EditorConfig::ReadObject(const wxString &name, SerializedObject *obj)
{
	//find the object node in the xml file
	return XmlUtils::StaticReadObject(m_doc->GetRoot(), name, obj);
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
	DoSave();
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

void EditorConfig::LoadLexers(bool loadDefault)
{
	m_lexers.clear();

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
		wxString userLexer( fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR) + fn.GetName() +  wxT(".") + clGetUserName() + wxT("_xml"));
		if ( wxFileName::FileExists( userLexer ) ) {
			if (!loadDefault) {
				fileToLoad = userLexer;
			} else {
				// backup the old file
				wxRenameFile(userLexer, userLexer + wxT(".orig"));
				wxRemoveFile(userLexer);
			}
		}

		LexerConfPtr lexer(new LexerConf( fileToLoad ));
		m_lexers[lexer->GetName()] = lexer;
	}
}

void EditorConfig::Begin()
{
	m_transcation = true;
}

void EditorConfig::Save()
{
	m_transcation = false;
	DoSave();
}

bool EditorConfig::DoSave() const
{
	if (m_transcation) {
		return true;
	}
	return m_doc->Save(m_fileName.GetFullPath());
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

