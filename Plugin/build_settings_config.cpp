//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : build_settings_config.cpp
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
 #include "build_settings_config.h"
#include "conffilelocator.h"
#include "xmlutils.h"
#include <wx/ffile.h>

BuildSettingsConfig::BuildSettingsConfig()
{
	m_doc = new wxXmlDocument();
}

BuildSettingsConfig::~BuildSettingsConfig()
{
	delete m_doc;
}

bool BuildSettingsConfig::Load(const wxString &version)
{
	wxString initialSettings = ConfFileLocator::Instance()->Locate(wxT("config/build_settings.xml"));
	bool loaded = m_doc->Load(initialSettings);
	wxString xmlVersion = m_doc->GetRoot()->GetPropVal(wxT("Version"), wxEmptyString);
	if ( xmlVersion != version ) {
		loaded = m_doc->Load(ConfFileLocator::Instance()->GetDefaultCopy(wxT("config/build_settings.xml")));
	}
	m_fileName = ConfFileLocator::Instance()->GetLocalCopy(wxT("config/build_settings.xml"));
	return loaded;
}

wxXmlNode* BuildSettingsConfig::GetCompilerNode(const wxString& name) const
{
	wxXmlNode *cmpsNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
	if( cmpsNode ){
		if(name.IsEmpty()){
			//return the first compiler
			return XmlUtils::FindFirstByTagName(cmpsNode, wxT("Compiler"));
		}else{
			return XmlUtils::FindNodeByName(cmpsNode, wxT("Compiler"), name);
		}
	}
	return NULL;
}

void BuildSettingsConfig::SetCompiler(CompilerPtr cmp)
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
	if(node){
		wxXmlNode *oldCmp = NULL;
		wxXmlNode *child = node->GetChildren();
		while(child){
			if(child->GetName() == wxT("Compiler") && XmlUtils::ReadString(child, wxT("Name")) == cmp->GetName()){
				oldCmp = child;
				break;
			}
			child = child->GetNext();
		}
		if(oldCmp){
			node->RemoveChild(oldCmp);
			delete oldCmp;
		}
		node->AddChild(cmp->ToXml());

	} else {
		wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compilers"));
		m_doc->GetRoot()->AddChild(node);
		node->AddChild(cmp->ToXml());
	}

	m_doc->Save(m_fileName.GetFullPath());
}

CompilerPtr BuildSettingsConfig::GetCompiler(const wxString &name) const
{
	return new Compiler(GetCompilerNode(name));
}

CompilerPtr BuildSettingsConfig::GetFirstCompiler(BuildSettingsConfigCookie &cookie)
{
	wxXmlNode *cmps = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
	if( cmps ){
		cookie.parent = cmps;
		cookie.child  = NULL;
		return GetNextCompiler(cookie);
	}
	return NULL;
}

CompilerPtr BuildSettingsConfig::GetNextCompiler(BuildSettingsConfigCookie &cookie)
{
	if( cookie.parent == NULL ){
		return NULL;
	}

	if( cookie.child == NULL ){
		cookie.child = cookie.parent->GetChildren();
	}

	while( cookie.child ){
		if( cookie.child->GetName() == wxT("Compiler") ){
			wxXmlNode *n = cookie.child;
			// advance the child to the next child and bail out
			cookie.child = cookie.child->GetNext();

			// incase we dont have more childs to iterate
			// reset the parent as well so the next call to GetNexeLexer() will fail
			if( cookie.child == NULL ){
				cookie.parent = NULL;
			}
			return new Compiler(n);
		}
		cookie.child = cookie.child->GetNext();
	}
	return NULL;
}

bool BuildSettingsConfig::IsCompilerExist(const wxString &name) const
{
	wxXmlNode *node = GetCompilerNode(name);
	return node != NULL;
}

void BuildSettingsConfig::DeleteCompiler(const wxString &name)
{
	wxXmlNode *node = GetCompilerNode(name);
	if(node){
		node->GetParent()->RemoveChild(node);
		delete node;
		m_doc->Save(m_fileName.GetFullPath());
	}
}

void BuildSettingsConfig::SetBuildSystem(BuilderConfigPtr bs)
{
	//find the old setting
	wxXmlNode *node = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("BuildSystem"), bs->GetName());
	if(node){
		node->GetParent()->RemoveChild(node);
		delete node;
	}
	m_doc->GetRoot()->AddChild(bs->ToXml());
	m_doc->Save(m_fileName.GetFullPath());
}

BuilderConfigPtr BuildSettingsConfig::GetBuilderConfig(const wxString &name)
{
	wxXmlNode *node = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("BuildSystem"), name);
	if(node){
		return new BuilderConfig(node);
	}
	return NULL;
}
