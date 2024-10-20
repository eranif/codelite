//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : swStringDb.h              
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
// Name:		swStringDb.h
// Purpose:
// Author:		Frank Lichtner
// Modified by:
// Created:		2008-09-01 19:12:19
// RCS-ID:
// Copyright:	© Frank Lichtner
// Licence:		GNU General Public Licence
/////////////////////////////////////////////////////////////////////////////

#include "wxSerialize.h"

#ifndef SWTEMPLATEFILE_H
#define SWTEMPLATEFILE_H
//------------------------------------------------------------
// base class for one entry
class swBase : public wxObject
{
	DECLARE_DYNAMIC_CLASS( swBase );

public:
	swBase() { ; }
	virtual ~swBase() { ; }

	virtual void Serialize( wxSerialize& ar ) { wxUnusedVar( ar ); }
};
//------------------------------------------------------------
WX_DECLARE_STRING_HASH_MAP( swBase*, swStringList );
//------------------------------------------------------------
// one single string entry
class swString : public swBase
{
	DECLARE_DYNAMIC_CLASS( swString );
	friend class swStringSet;
public:
	swString() { ; }
	virtual ~swString() { ;	}

	void Serialize( wxSerialize& ar )
	{
		if ( ar.IsStoring() )
			ar	<< m_string;
		else
			ar	>> m_string;
	}

protected:
	wxString	m_string;
};

//------------------------------------------------------------
// one set of swStrings
class swStringSet : public wxObject
{
	friend class swStringDb;
	DECLARE_DYNAMIC_CLASS( swStringSet );

public:
	/// check if it is existing key
	bool IsKey( const wxString & key );
	/// returns all keys
	void GetAllKeys( wxArrayString &keys );
	/// set string value at key
	bool SetString( const wxString& key, const wxString& value );
	/// get string value at key
	wxString GetString( const wxString& key );
	/// delete single key
	void DeleteKey( const wxString& key );
	/// delete all keys
	void DeleteAll();

	swStringSet();
	virtual ~swStringSet();

	void Serialize( wxSerialize& ar );

protected:
	swStringList	m_list;	// string set
};
//------------------------------------------------------------
WX_DECLARE_STRING_HASH_MAP( swStringSet*, swStringSetList );
//------------------------------------------------------------
class swStringDb : public wxObject
{
	DECLARE_DYNAMIC_CLASS( swStringDb );

public:
// template access
	/// if true, the file will be compressed
	void SetCompress( bool compress ) { m_compress = compress; }
	/// save StringDb
	bool Save( const wxString& filename );
	/// load StringDb
	bool Load( const wxString& filename );
	/// check if set exists
	bool IsSet( const wxString & set );
	/// returns all sets
	void GetAllSets( wxArrayString &sets );
	/// sets string at set, key
	bool SetString( const wxString & set, const wxString& key, const wxString& value );
	/// gets string at set, key
	wxString GetString( const wxString & set, const wxString& key );
	/// deletes single key
	void DeleteKey( const wxString & set, const wxString& key );
	/// deletes all
	void DeleteAll();

// snippet access
	/// checks if key exists
	bool IsSnippetKey( const wxString & key );
	/// eturns all keys
	void GetAllSnippetKeys( wxArrayString &keys );
	/// sets string at key
	bool SetSnippetString( const wxString& key, const wxString& value );
	/// gets string at key
	wxString GetSnippetString( const wxString& key );
	/// deletes single key
	void DeleteSnippetKey( const wxString& key );

	swStringDb();
	virtual ~swStringDb();
	void Serialize( wxSerialize& ar );

protected:
	swStringSetList	m_list;			// template list
	swStringSet 	m_snippetSet;	// snippet list
	bool 			m_compress;		// compress flag
	wxArrayString	m_keyOrder;		// array to sort snippets menu by user (future)
};
#endif //SWTEMPLATEFILE_H
