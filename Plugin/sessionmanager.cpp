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
}

void SessionEntry::Serialize(Archive &arch)
{
	arch.Write(wxT("m_selectedTab"), m_selectedTab);
	arch.Write(wxT("m_tabs"), m_tabs);
	arch.Write(wxT("m_workspaceName"), m_workspaceName);
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
	
	if(!m_fileName.FileExists()){
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
	if(!m_doc.GetRoot()){
		return false;
	}
	
	wxXmlNode *node =  m_doc.GetRoot()->GetChildren();
	while(node){
		if(node->GetName() == wxT("Session")){
			if(XmlUtils::ReadString(node, wxT("Name")) == name){
				//we found our session
				Archive arch;
				arch.SetXmlNode(node);
				session.DeSerialize(arch);
				return true;
			}
		}
		node = node->GetNext();
	}
	return false;
}

bool SessionManager::Save(const wxString &name, SessionEntry &session)
{
	if(!m_doc.GetRoot()){
		return false;
	}
	
	wxXmlNode *node =  m_doc.GetRoot()->GetChildren();
	while(node){
		if(node->GetName() == wxT("Session")){
			if(XmlUtils::ReadString(node, wxT("Name")) == name){
				//we found our session, remove it
				m_doc.GetRoot()->RemoveChild(node);
				delete node;
				break;
			}
		}
		node = node->GetNext();
	}
	
	//create new node and insert it
	wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Session"));
	m_doc.GetRoot()->AddChild(child);
	child->AddProperty(wxT("Name"), name);

	Archive arch;
	arch.SetXmlNode(child);
	session.Serialize(arch);
	//save the file
	return m_doc.Save(m_fileName.GetFullPath());
}
