//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : configurationtoolbase.cpp              
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
 #include "configurationtoolbase.h"
#include "editor_config.h"
#include "xmlutils.h"
#include "serialized_object.h"
#include "wx/filename.h"
#include "wx/ffile.h"

ConfigurationToolBase::ConfigurationToolBase()
: m_fileName(wxEmptyString)
{ 
}

ConfigurationToolBase::~ConfigurationToolBase()
{
} 

bool ConfigurationToolBase::Load(const wxString &fileName)
{
	wxFileName fn(fileName);
	fn.MakeAbsolute();
	m_fileName = fn.GetFullPath();
	
	if(fn.FileExists() == false){
		//no such file, create an empty file
		wxString content;
		content << wxT("<") << GetRootName() << wxT("/>");
		wxFFile file;
		file.Open(fn.GetFullPath(), wxT("w+b"));
		if(file.IsOpened()){
			file.Write(content);
			file.Close();
		}
	}
	return m_doc.Load(m_fileName);
}

bool ConfigurationToolBase::WriteObject(const wxString &name, SerializedObject *obj)
{
	if(m_doc.IsOk() == false){
		return false;
	}
	
	if(!XmlUtils::StaticWriteObject(m_doc.GetRoot(), name, obj))
		return false;
	return m_doc.Save(m_fileName);
}


bool ConfigurationToolBase::ReadObject(const wxString &name, SerializedObject *obj)
{
	if(m_doc.IsOk() == false){
		return false;
	}
	
	return XmlUtils::StaticReadObject(m_doc.GetRoot(), name, obj);
}
