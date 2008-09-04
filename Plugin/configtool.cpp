//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : configtool.cpp              
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
#include "conffilelocator.h"
 #include "configtool.h"
#include "xmlutils.h"
#include "serialized_object.h"
#include "wx/filename.h"
#include "wx/ffile.h"

ConfigTool::ConfigTool()
: m_fileName(wxEmptyString)
{
}

ConfigTool::~ConfigTool()
{
}

bool ConfigTool::Load(const wxString &basename)
{
	wxString initialSettings = ConfFileLocator::Instance()->Locate(basename);
	bool loaded = m_doc.Load(initialSettings);
	
	m_fileName = ConfFileLocator::Instance()->GetLocalCopy(basename);
	return loaded;
}

bool ConfigTool::WriteObject(const wxString &name, SerializedObject *obj)
{
	if(m_doc.IsOk() == false){
		return false;
	}
	Archive arch;
	wxXmlNode *child = XmlUtils::FindNodeByName(m_doc.GetRoot(), wxT("ArchiveObject"), name);
	if (child) {
		wxXmlNode *n = m_doc.GetRoot();
		n->RemoveChild(child);
		delete child;
	}

	//create new xml node for this object
	child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ArchiveObject"));
	m_doc.GetRoot()->AddChild(child);
	child->AddProperty(wxT("Name"), name);

	arch.SetXmlNode(child);
	//serialize the object into the archive
	obj->Serialize(arch);
	//save the archive
	return m_doc.Save(m_fileName);
}


bool ConfigTool::ReadObject(const wxString &name, SerializedObject *obj)
{
	if(m_doc.IsOk() == false){
		return false;
	}

	//find the object node in the xml file
	wxXmlNode *node = XmlUtils::FindNodeByName(m_doc.GetRoot(), wxT("ArchiveObject"), name);
	if (node) {
		Archive arch;
		arch.SetXmlNode(node);
		obj->DeSerialize(arch);
		return true;
	}
	return false;
}
