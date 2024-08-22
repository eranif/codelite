//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : swStringDb.cpp              
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

/////////////////////////////////////////////////////////////////////////////
// Name:		swStringDb.cpp
// Purpose:
// Author:		Frank Lichtner
// Modified by:
// Created:		2008-09-01 19:12:19
// RCS-ID:
// Copyright:	© Frank Lichtner
// License:		GNU General Public Licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/filename.h>
#include <wx/zstream.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "swStringDb.h"

#include <wx/listimpl.cpp>
#include <wx/wfstream.h>

//------------------------------------------------------------
#define FRLSWTMPL_VERSION	1000
#define FRLSWTMPL_HEADER	wxT("SnipWiz string db")
//------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(swBase,wxObject)
IMPLEMENT_DYNAMIC_CLASS(swString,swBase)
IMPLEMENT_DYNAMIC_CLASS(swStringSet,wxObject)
IMPLEMENT_DYNAMIC_CLASS(swStringDb,wxObject)
//------------------------------------------------------------
// Constructor
swStringSet::swStringSet()
{

}

//------------------------------------------------------------
// Destructor
swStringSet::~swStringSet()
{
	DeleteAll();
}

//------------------------------------------------------------
// save list
void swStringSet::Serialize(wxSerialize& ar)
{
	swStringList::iterator it;
	swString* pObj = NULL;
	wxUint32 size;
	wxString key, classname;
	
	if (ar.IsStoring())
	{	// storing
		size = m_list.size();
		ar << size;
		for( it = m_list.begin(); it != m_list.end(); ++it ) 
		{ 
			key = it->first;
			ar << key;
			pObj = wxDynamicCast(it->second, swString);
			classname = pObj->GetClassInfo()->GetClassName();
			ar << classname;
			pObj->Serialize(ar);
		}
	}
	else
	{	// loading
		DeleteAll();
		ar	>> size;
		for(wxUint32 i = 0;i < size;i++)
		{
			ar >> key;
			ar >> classname;
			pObj = wxDynamicCast(::wxCreateDynamicObject(classname), swString);
			if(pObj != NULL)
			{
			 	pObj->Serialize(ar);
				m_list[key]	= pObj;
			}	
		}
	}
}

//------------------------------------------------------------
// deletes all entries from list
void swStringSet::DeleteAll()
{
	wxArrayString keys;
	swStringList::iterator it; 
	for( it = m_list.begin(); it != m_list.end(); ++it ) 
		keys.Add(it->first);
	for(size_t i = 0;i < keys.GetCount();i++)
		DeleteKey(keys[i]);
}

//------------------------------------------------------------
// deletes one key
void swStringSet::DeleteKey(const wxString& key)
{
	
	swString*	pObj = NULL;
	swStringList::iterator it = m_list.find(key);
	if(it == m_list.end())
		return;
	pObj = (swString*)m_list[key];
	delete pObj;
	m_list.erase(key);
}

//------------------------------------------------------------
// return string found for key
wxString swStringSet::GetString(const wxString& key)
{
	swString*	pObj = NULL;

	swStringList::iterator it = m_list.find(key);
	if(it == m_list.end())
	{
		return wxEmptyString;
	}
	else
	{
		pObj = wxDynamicCast(m_list[key],swString);
		if(pObj != NULL)
			return pObj->m_string;
		else
			return wxEmptyString;
	}
}

//------------------------------------------------------------
// writes string with key
bool swStringSet::SetString(const wxString& key, const wxString& value)
{
	swString*	pObj = NULL;

	swStringList::iterator it = m_list.find(key);

	if(it == m_list.end())
		m_list[key] = new swString;
	
	pObj = wxDynamicCast(m_list[key],swString);
	if(!pObj)
		return false;
		
	pObj->m_string = value;
	m_list[key] = pObj;

	return true;
}

//------------------------------------------------------------
// checks if key exists
bool swStringSet::IsKey(const wxString & key)
{
	bool found;
	
	if(m_list.find(key) == m_list.end())
		found = false;
	else
		found = true;
	return found;
}

//------------------------------------------------------------
// returns a list with all keys
void swStringSet::GetAllKeys(wxArrayString &keys)
{
	keys.Clear();
	swStringList::iterator itSet;
	for( itSet = m_list.begin(); itSet != m_list.end(); ++itSet ) 
	{
		wxString key = itSet->first;
		keys.Add(key);
	}
}

//------------------------------------------------------------
// string database
//------------------------------------------------------------
swStringDb::swStringDb()
{
	m_compress = false;
}

//------------------------------------------------------------

swStringDb::~swStringDb()
{
}

//------------------------------------------------------------
void swStringDb::DeleteAll()
{
	swStringSetList::iterator it;
	wxArrayString keys;
	swStringSet* pObj;

	for( it = m_list.begin(); it != m_list.end(); ++it ) 
	{
	   keys.Add(it->first);
	   pObj	= it->second;
	   pObj->DeleteAll();
	   delete pObj;
	}
	for(size_t i = 0;i < keys.GetCount();i++)
		m_list.erase(keys[i]);	
}

//------------------------------------------------------------
void swStringDb::DeleteKey(const wxString& set, const wxString& key)
{
	swStringSet* pSet = NULL;
	
	pSet = m_list[set];
	if(pSet != NULL)
		pSet->DeleteKey(key);
	// if set is empty, remove it
	if(pSet->m_list.size() == 0)
	{
		swStringSetList::iterator it = m_list.find(set);
		delete (swStringSet*) pSet;
		m_list.erase(it);
	}
}

//------------------------------------------------------------
wxString swStringDb::GetString(const wxString& set, const wxString& key)
{
	swStringSet*	pSet = NULL;

	pSet = m_list[set];
	if(pSet != NULL)
		return pSet->GetString(key);
	else
		return wxEmptyString;
}

//------------------------------------------------------------
bool swStringDb::SetString(const wxString& set, const wxString& key, const wxString& value)
{
	swStringSet*	pSet = NULL;

	pSet = m_list[set];
	if(pSet == NULL)
		pSet = new swStringSet;
	
	pSet->SetString(key,value);
	m_list[set] = pSet;
	
	return true;
}

//------------------------------------------------------------
bool swStringDb::Load(const wxString& filename)
{
	wxFileName fullname(filename);
	if(!fullname.FileExists())
		return false;
 	wxFileInputStream infile(filename);
	wxZlibInputStream infile1(infile);
 	if(!infile.Ok())
		return false;
	
	if(m_compress)
	{
		wxSerialize ar(infile1, FRLSWTMPL_VERSION, FRLSWTMPL_HEADER);
		if(ar.IsOk())
			Serialize(ar);
		else 
			return false;
	}
	else
	{
		wxSerialize ar(infile, FRLSWTMPL_VERSION, FRLSWTMPL_HEADER);
		if(ar.IsOk())
			Serialize(ar);
		else 
			return false;
	}
	return true;
}

//------------------------------------------------------------
bool swStringDb::Save(const wxString& filename)
{

	wxFileOutputStream outfile(filename);
	wxZlibOutputStream outfile1(outfile);
	if(!outfile.Ok())
		return false;
	
	if(m_compress)
	{
		wxSerialize ar(outfile1, FRLSWTMPL_VERSION, FRLSWTMPL_HEADER);
		if(ar.IsOk())
			Serialize(ar);
		else
			return false;
	}
	else
	{
		wxSerialize ar(outfile, FRLSWTMPL_VERSION, FRLSWTMPL_HEADER);
		if(ar.IsOk())
			Serialize(ar);
		else
			return false;
	}	

	return true;
}

//------------------------------------------------------------
void swStringDb::Serialize(wxSerialize& ar)
{
	swStringSetList::iterator it;

	swStringSet* pObj = NULL;
	wxUint32 size;
	wxString key, classname, dummy;
	
	if (ar.IsStoring())
	{	// storing
		size = m_list.size();
		ar << size;
		for( it = m_list.begin(); it != m_list.end(); ++it ) 
		{ 
			key = it->first;
			ar << key;
			pObj = it->second;
			classname = pObj->GetClassInfo()->GetClassName();
			ar << classname;
			pObj->Serialize(ar);
		}
		ar << m_keyOrder;
		m_snippetSet.Serialize(ar);
	}
	else
	{	// loading
		DeleteAll();
		ar >> size;
		for(wxUint32 i = 0;i < size;i++)
		{
			ar >> key;
			ar >> classname;
			pObj = wxDynamicCast(::wxCreateDynamicObject(classname), swStringSet);
			if(pObj != NULL)
			{
			 	pObj->Serialize(ar);
				m_list[key]	= pObj;
			}	
		}
		ar >> m_keyOrder;
		m_snippetSet.DeleteAll();
		m_snippetSet.Serialize(ar);
	}
	
}

//------------------------------------------------------------
bool swStringDb::IsKey(const wxString& set, const wxString& key)
{
	swStringSet* pSet = NULL;

	pSet = m_list[set];
	if(pSet != NULL)
		return pSet->IsKey(key);
	else
		return false;
}

//------------------------------------------------------------
void swStringDb::GetAllSets(wxArrayString& sets)
{
	sets.Clear();
	swStringSetList::iterator itSet;
	for( itSet = m_list.begin(); itSet != m_list.end(); ++itSet ) 
	{
		wxString set = itSet->first;
		sets.Add(set);
	}
}

//------------------------------------------------------------
bool swStringDb::IsSet(const wxString& set)
{
	if(m_list.find(set)== m_list.end())
		return false;
	return true;
}

//------------------------------------------------------------
// root
//------------------------------------------------------------
void swStringDb::DeleteSnippetAll()
{
	m_snippetSet.DeleteAll();
}
//------------------------------------------------------------
void swStringDb::DeleteSnippetKey(const wxString& key)
{
	m_snippetSet.DeleteKey(key);	
}
//------------------------------------------------------------
void swStringDb::GetAllSnippetKeys(wxArrayString& keys)
{
	m_snippetSet.GetAllKeys(keys);
}
//------------------------------------------------------------
wxString swStringDb::GetSnippetString(const wxString& key)
{
	return m_snippetSet.GetString(key);
}
//------------------------------------------------------------
bool swStringDb::IsSnippetKey(const wxString& key)
{
	return m_snippetSet.IsKey(key);
}
//------------------------------------------------------------
bool swStringDb::SetSnippetString(const wxString& key, const wxString& value)
{
	return m_snippetSet.SetString(key, value);
}
//------------------------------------------------------------
