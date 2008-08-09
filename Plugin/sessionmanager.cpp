//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : sessionmanager.cpp
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
#include "sessionmanager.h"
#include "xmlutils.h"
#include "wx/ffile.h"

//Session entry
SessionEntry::SessionEntry()
{
}

SessionEntry::~SessionEntry()
{
}

void SessionEntry::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_selectedTab"), m_selectedTab);
	arch.Read(wxT("m_tabs"), m_tabs);
	arch.Read(wxT("m_workspaceName"), m_workspaceName);
	arch.Read(wxT("TabInfoArray"), m_vTabInfoArr);
	// initialize tab info array from m_tabs if in config file wasn't yet tab info array
	if (m_vTabInfoArr.size() == 0 && m_tabs.GetCount() > 0) {
		for (size_t i=0; i<m_tabs.GetCount(); i++) {
			TabInfo oTabInfo;
			oTabInfo.SetFileName(m_tabs.Item(i));
			oTabInfo.SetFirstVisibleLine(0);
			oTabInfo.SetCurrentLine(0);
			m_vTabInfoArr.push_back(oTabInfo);
		}
	}
}

void SessionEntry::Serialize(Archive &arch)
{
	arch.Write(wxT("m_selectedTab"), m_selectedTab);
	// since tabs are saved in TabInfoArray we don't save tabs as string array,
	// there are only read due to read config saved in older version where wasn't TabInfoArray
	//arch.Write(wxT("m_tabs"), m_tabs);
	arch.Write(wxT("m_workspaceName"), m_workspaceName);
	arch.Write(wxT("TabInfoArray"), m_vTabInfoArr);
}


//---------------------------------------------
SessionManager & SessionManager::Get()
{
	static SessionManager theManager;
	return theManager;
}

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
}

bool SessionManager::Load(const wxString &fileName)
{
	m_fileName = wxFileName(fileName);

	if (!m_fileName.FileExists()) {
		//no such file or directory
		//create an empty one
		wxFFile newFile(fileName, wxT("a+"));
		newFile.Write(wxT("<Sessions/>"));
		newFile.Close();
	}

	m_doc.Load(m_fileName.GetFullPath());
	return true;
}

bool SessionManager::FindSession(const wxString &name, SessionEntry &session)
{
	if (!m_doc.GetRoot()) {
		return false;
	}

	// find last active workspace name if searched is Default
	wxString strSessionName = name;
	//wxMessageBox(wxT("FS: ")+strSessionName);
	if (strSessionName == wxT("Default")) {
		wxXmlNode *node =  m_doc.GetRoot()->GetChildren();
		while (node) {
			if (node->GetName() == wxT("LastActiveWorkspace")) {
				//we found our session, remove it
				wxXmlNode *childNode =  node->GetChildren();
				if (childNode) {
					strSessionName = childNode->GetContent();
				}
				break;
			}
			node = node->GetNext();
		}
	}

	//wxMessageBox(wxT("FS2: ")+strSessionName);
	wxXmlNode *node =  m_doc.GetRoot()->GetChildren();
	while (node) {
		if (node->GetName() == wxT("Session")) {
			if (XmlUtils::ReadString(node, wxT("Name")) == strSessionName) {
				//we found our session
				Archive arch;
				arch.SetXmlNode(node);
				session.DeSerialize(arch);
				return true;
			}
		}
		node = node->GetNext();
	}
	// if not found last active, then try find Default
	//wxMessageBox(wxT("FS3: ")+strSessionName);
	if (strSessionName != wxT("Default")) {
		strSessionName = wxT("Default");
		node =  m_doc.GetRoot()->GetChildren();
		while (node) {
			if (node->GetName() == wxT("Session")) {
				if (XmlUtils::ReadString(node, wxT("Name")) == strSessionName) {
					//we found our session
					Archive arch;
					arch.SetXmlNode(node);
					session.DeSerialize(arch);
					return true;
				}
			}
			node = node->GetNext();
		}
	}
	return false;
}

bool SessionManager::Save(const wxString &name, SessionEntry &session)
{
	if (!m_doc.GetRoot()) {
		return false;
	}

	// if saving Default then change name to workspace anme and path
	wxString strSessionName = name;
	if (strSessionName == wxT("Default"))
		strSessionName = session.GetWorkspaceName();
	// if session entry has no workspace name, then do not save
	if (strSessionName.length() == 0)
		return false;

	bool bSessionRemoved = false;
	bool bLastActWrkspcRemoved = false;
	wxXmlNode *node =  m_doc.GetRoot()->GetChildren();
	while (node) {
		if (node->GetName() == wxT("Session")) {
			if (XmlUtils::ReadString(node, wxT("Name")) == strSessionName) {
				//we found our session, remove it
				m_doc.GetRoot()->RemoveChild(node);
				delete node;
				break;
			}
		}
		node = node->GetNext();
	}
	node =  m_doc.GetRoot()->GetChildren();
	while (node) {
		if (node->GetName() == wxT("LastActiveWorkspace")) {
			//we found our session, remove it
			m_doc.GetRoot()->RemoveChild(node);
			// remove CDATA child node
			wxXmlNode *childNode = NULL;
			wxXmlNode *nextChild = node->GetChildren();
			while (nextChild) {
				childNode = nextChild;
				nextChild = childNode->GetNext();
				node->RemoveChild(childNode);
				delete childNode;
			}
			delete node;
			//bLastActWrkspcRemoved = true;
			break;
		}
//		if(bSessionRemoved && bLastActWrkspcRemoved)
//			break;
		node = node->GetNext();
	}

	//create new node with workspace name (and path) and insert it
	wxXmlNode *lastWorkspaceName = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("LastActiveWorkspace"));
	wxXmlNode *lastWorkspaceNameT = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT("LastActiveWorkspace"), session.GetWorkspaceName());
	lastWorkspaceName->AddChild(lastWorkspaceNameT);
	m_doc.GetRoot()->AddChild(lastWorkspaceName);

	//create new node and insert it
	wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Session"));
	m_doc.GetRoot()->AddChild(child);
	child->AddProperty(wxT("Name"), strSessionName);

//	wxXmlProperty *lastActiveWrkspc = new wxXmlProperty(wxT("LastActiveWrkspc"), session.GetWorkspaceName());
//	if(m_doc.GetRoot()->HasProp(wxT("LastActiveWrkspc")))
//		m_doc.GetRoot()->DeleteProperty(wxT("LastActiveWrkspc"));
//	m_doc.GetRoot()->SetProperties(lastActiveWrkspc);

	Archive arch;
	arch.SetXmlNode(child);
	session.Serialize(arch);
	//save the file
	return m_doc.Save(m_fileName.GetFullPath());
}
