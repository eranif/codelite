//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : entry.cpp              
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
 #include "precompiled_header.h"
#include "tags_database.h"

#include "entry.h"
#include "tokenizer.h"
#include "language.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

TagEntry::TagEntry(const tagEntry& entry)
{
	Create(entry);
}

TagEntry::TagEntry()
: m_path(wxEmptyString)
, m_file(wxEmptyString)
, m_lineNumber(-1)
, m_pattern(wxEmptyString)
, m_kind(wxT("<unknown>"))
, m_parent(wxEmptyString)
, m_name(wxEmptyString)
, m_id(wxNOT_FOUND)
, m_scope(wxEmptyString)
, m_differOnByLineNumber(false)
{
}

TagEntry::~TagEntry()
{
}

TagEntry::TagEntry(const TagEntry& rhs)
{
	*this = rhs;
}

TagEntry& TagEntry::operator=(const TagEntry& rhs)
{
	m_id = rhs.m_id;
	m_file = rhs.m_file.c_str();
	m_kind = rhs.m_kind.c_str();
	m_parent = rhs.m_parent.c_str();
	m_pattern = rhs.m_pattern.c_str();
	m_lineNumber = rhs.m_lineNumber;
	m_name = rhs.m_name.c_str();
	m_path = rhs.m_path.c_str();
	m_hti = rhs.m_hti;
	m_scope = rhs.m_scope.c_str();
	m_differOnByLineNumber = rhs.m_differOnByLineNumber;
	
	// loop over the map and copy item by item
	// we use the c_str() method to force our own copy of the string and to avoid 
	// ref counting which may cause crash when sharing wxString among threads
	m_extFields.clear();
	std::map<wxString, wxString>::const_iterator iter = rhs.m_extFields.begin();
	for( ; iter != rhs.m_extFields.end(); iter ++ ){
		m_extFields[iter->first.c_str()] = iter->second.c_str();
	}
	return *this;
}

bool TagEntry::operator ==(const TagEntry& rhs)
{
	//Note: tree item id is not used in this function!
	bool res = 	
		m_scope == rhs.m_scope &&
		m_file == rhs.m_file &&
		m_kind == rhs.m_kind &&
		m_parent == rhs.m_parent &&
		m_pattern == rhs.m_pattern &&
		m_name == rhs.m_name &&
		m_path == rhs.m_path &&
		m_lineNumber == rhs.m_lineNumber &&
		GetInherits() == rhs.GetInherits() &&
		GetAccess() == rhs.GetAccess() &&
		GetSignature() == rhs.GetSignature() &&
		GetTyperef() == rhs.GetTyperef();
	
	bool res2 = m_scope == rhs.m_scope &&
		m_file == rhs.m_file &&
		m_kind == rhs.m_kind &&
		m_parent == rhs.m_parent &&
		m_pattern == rhs.m_pattern &&
		m_name == rhs.m_name &&
		m_path == rhs.m_path &&
		GetInherits() == rhs.GetInherits() &&
		GetAccess() == rhs.GetAccess() &&
		GetSignature() == rhs.GetSignature() &&
		GetTyperef() == rhs.GetTyperef();
		
	if(res2 && !res) {
		// the entries are differs only in the line numbers
		m_differOnByLineNumber = true;
	}
	return res;
}

void TagEntry::Create(const wxString &fileName, 
					  const wxString &name, 
					  int lineNumber, 
					  const wxString &pattern, 
					  const wxString &kind, 
					  std::map<wxString, wxString>& extFields)
{
	SetName( name );
	SetLine( lineNumber );
	SetKind( kind.IsEmpty() ? wxT("<unknown>") : kind );
	SetPattern( pattern );
	SetFile( fileName );
	SetId(-1);
	m_extFields = extFields;
	wxString path;

	// Check if we can get full name (including path)
	path = GetExtField(wxT("class"));
	if(!path.IsEmpty()){
		UpdatePath( path ) ;
	}else{
		path = GetExtField(wxT("struct"));
		if(!path.IsEmpty()){
			UpdatePath( path ) ;
		}else{
			path = GetExtField(wxT("namespace"));
			if(!path.IsEmpty()){
				UpdatePath( path ) ;
			}else{
				path = GetExtField(wxT("interface"));
				if(!path.IsEmpty()){
					UpdatePath( path ) ;
				}else{
					path = GetExtField(wxT("enum"));
					if(!path.IsEmpty()){
						UpdatePath( path ) ;
					}else{
						path = GetExtField(wxT("union"));
						wxString tmpname = path.AfterLast(wxT(':'));
						if(!path.IsEmpty()){
							if(!tmpname.StartsWith(wxT("__anon"))) {
								UpdatePath( path ) ;
							} else {
								// anonymouse union, remove the anonymous part from its name
								path = path.BeforeLast(wxT(':'));
								path = path.BeforeLast(wxT(':'));
								UpdatePath( path ) ;
							}
						}
					}
				}
			}
		}
	}

	if(!path.IsEmpty()){
		SetScope(path);
	}else{
		SetScope(wxT("<global>"));
	}

	// If there is no path, path is set to name
	if( GetPath().IsEmpty() )
		SetPath( GetName() );

	// Get the parent name
	StringTokenizer tok(GetPath(), wxT("::"));
	wxString parent;

	(tok.Count() < 2) ? parent = wxT("<global>") : parent = tok[tok.Count()-2];
	SetParent(parent);
}

void TagEntry::Create(const tagEntry& entry)
{
	// Get other information from the string data and store it into map
	for (int i = 0;  i < entry.fields.count;  ++i)
	{
		wxString key = _U(entry.fields.list[i].key);
		wxString value = _U(entry.fields.list[i].value);
		m_extFields[key] = value;
	}
	Create(	_U(entry.file), 
		_U(entry.name), 
		entry.address.lineNumber, 
		_U(entry.address.pattern), 
		_U(entry.kind), 
		m_extFields);
}

void TagEntry::Print()
{
	std::cout << "======================================" << std::endl;
	std::cout << "Name:\t\t" << GetName() << std::endl;
	std::cout << "File:\t\t" << GetFile() << std::endl;
	std::cout << "Line:\t\t" << GetLine() << std::endl;
	std::cout << "Pattern\t\t" << GetPattern() << std::endl;
	std::cout << "Kind:\t\t" << GetKind() << std::endl;
	std::cout << "Parent:\t\t" << GetParent() << std::endl;

	std::cout << " ---- Ext fields: ---- " << std::endl;
	std::map<wxString, wxString>::const_iterator iter = m_extFields.begin();
	for(; iter != m_extFields.end(); iter++)
		std::cout << iter->first << ":\t\t" << iter->second << std::endl;
	std::cout << "======================================" << std::endl;
}

wxString TagEntry::Key() const 
{
	wxString key;
	if(GetKind() == wxT("prototype") || GetKind() == wxT("macro")) {
		key << GetKind() << wxT(": ");
	}
	
	key << GetPath() << GetSignature();
	return key;
}

wxString TagEntry::GetDisplayName() const
{
	wxString name;
	name << GetName() << GetSignature();
	if(GetKind() == wxT("prototype"))
	{
		name << wxT(": [prototype]");
	}
	return name;
}

wxString TagEntry::GetFullDisplayName() const
{
	wxString name;

	if( GetParent() == wxT("<global>") ){
		name << GetDisplayName(); 
	} else {
		name << GetParent() << wxT("::") << GetName() << GetSignature();
	}

	return name;
}

TagEntry::TagEntry(wxSQLite3ResultSet& rs)
{
	m_extFields.clear();
	m_id = rs.GetInt(0);
	m_name = rs.GetString(1);
	m_file = rs.GetString(2);
	m_lineNumber = rs.GetInt(3);
	m_kind = rs.GetString(4);
	m_extFields[wxT("access")] = rs.GetString(5);
	m_extFields[wxT("signature")] = rs.GetString(6);
	m_pattern = rs.GetString(7);
	m_parent  = rs.GetString(8);
	m_extFields[wxT("inherits")] = rs.GetString(9);
	m_path = rs.GetString(10);
	m_extFields[wxT("typeref")] = rs.GetString(11);
	m_scope = rs.GetString(12);
}

//----------------------------------------------------------------------------
// Database operations
//----------------------------------------------------------------------------

int TagEntry::Store(wxSQLite3Statement& insertPerepareStmnt, TagsDatabase *db)
{
	// If this node is a dummy, (IsOk() == false) we dont insert it to database
	if( !IsOk() )
		return TagOk;

	try
	{
		// see TagsDatabase::GetInsertOneStatement() for the order of binding
		insertPerepareStmnt.Bind(1, GetName());
		insertPerepareStmnt.Bind(2, GetFile());
		insertPerepareStmnt.Bind(3, GetLine());
		insertPerepareStmnt.Bind(4, GetKind());
		insertPerepareStmnt.Bind(5, GetAccess());
		insertPerepareStmnt.Bind(6, GetSignature());
		insertPerepareStmnt.Bind(7, GetPattern());
		insertPerepareStmnt.Bind(8, GetParent());
		insertPerepareStmnt.Bind(9, GetInherits());
		insertPerepareStmnt.Bind(10, GetPath());
		insertPerepareStmnt.Bind(11, GetTyperef());
		insertPerepareStmnt.Bind(12, GetScope());
		insertPerepareStmnt.ExecuteUpdate();
		insertPerepareStmnt.Reset();
		
		// update the ID
//		SetId( db->LastRowId() );

	}
	catch(wxSQLite3Exception& exc)
	{
		if(exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;
		return TagError;
	}
	return TagOk;
}

int TagEntry::Update(wxSQLite3Statement& updatePerepareStmnt)
{
	// If this node is a dummy, (IsOk() == false) we dont update it to database
	if( !IsOk() )
		return TagOk;

	try
	{
		// see TagsDatabase::GetUpdateOneStatement() function for the order of binding
		updatePerepareStmnt.Bind(1, GetName());
		updatePerepareStmnt.Bind(2, GetFile());
		updatePerepareStmnt.Bind(3, GetLine());
		updatePerepareStmnt.Bind(4, GetAccess());
		updatePerepareStmnt.Bind(5, GetPattern());
		updatePerepareStmnt.Bind(6, GetParent());
		updatePerepareStmnt.Bind(7, GetInherits());
		updatePerepareStmnt.Bind(8, GetTyperef());
		updatePerepareStmnt.Bind(9, GetScope());
		updatePerepareStmnt.Bind(10, GetKind());
		updatePerepareStmnt.Bind(11, GetSignature());
		updatePerepareStmnt.Bind(12, GetPath());
		updatePerepareStmnt.ExecuteUpdate();
		updatePerepareStmnt.Reset();
	}
	catch(wxSQLite3Exception& WXUNUSED(exc))
	{
		return TagError;
	}
	return TagOk;
}


int TagEntry::Delete(wxSQLite3Statement &deletePreparedStmnt)
{
	// Delete this record from database.
	// Delete is done using the index
	try
	{
		deletePreparedStmnt.Bind(1, GetKind());			// Kind
		deletePreparedStmnt.Bind(2, GetSignature());	// Signature
		deletePreparedStmnt.Bind(3, GetPath());			// Path
		deletePreparedStmnt.ExecuteUpdate();
		deletePreparedStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		wxUnusedVar(exc);
		return TagError;
	}
	return TagOk;
}

wxString TagEntry::GetScopeName() const
{
	return GetScope();
}

wxString TagEntry::GetKind() const {
	wxString kind(m_kind);
	kind.Trim();
	return kind;
}


const bool TagEntry::IsContainer() const
{
	return	GetKind() == wxT("class")  || 
		GetKind() == wxT("struct") || 
		GetKind() == wxT("union")  || 
		GetKind() == wxT("namespace") ||
		GetKind() == wxT("project");
}

void TagEntry::UpdatePath(wxString & path)
{
	if(!path.IsEmpty())
	{
		wxString name(path);
		name += wxT("::");
		name += GetName();
		SetPath(name);
	}
}

wxString TagEntry::TypeFromTyperef() const
{
	wxString typeref = GetTyperef();
	if( typeref.IsEmpty() == false )
	{
		wxString name = typeref.BeforeFirst(wxT(':'));
		return name;		
	}
	return wxEmptyString;
}


wxString TagEntry::NameFromTyperef(wxString &templateInitList)
{
	wxString typeref = GetTyperef();
	if( typeref.IsEmpty() == false )
	{
		wxString name = typeref.AfterFirst(wxT(':'));
		return name;		
	}

	// incase our entry is a typedef, and it is not marked as typeref, 
	// try to get the real name from the pattern
	if( GetKind() == wxT("typedef"))
	{
		wxString name;
		if(TypedefFromPattern(GetPattern(), GetName(),name, templateInitList))
			return name;
	}
	return wxEmptyString;
}

bool TagEntry::TypedefFromPattern(const wxString &tagPattern, const wxString &typedefName, wxString &name, wxString &templateInit)
{
	CppScanner sc;
	//remove the pattern prefix & suffix
	wxString pattern(tagPattern);
	pattern.StartsWith(wxT("/^"), &pattern);
	sc.SetText( _C(pattern) );

	int type(0);
	int depth(0);
	bool found(false);
	wxString token;
	while((type = sc.yylex()) != 0)
	{
		if(type == 0)
			return false;

		if(!found){
			if(type != TYPEDEF){
				continue;
			}else{
				found = true;
				continue;
			}
		}

		token = _U(sc.YYText());
		if(token == typedefName){
			//we found the end token, break
			break;
		}

		switch(type)
		{
		case CLCL:
			if(depth == 0){
				name << token;
			}else{
				templateInit << token;
			}
			break;
			
		case IDENTIFIER:
			if(depth == 0){
				name << token;
			}else{
				templateInit << token;
			}
			break;
			
		case wxT('<'):
			depth++;
			if(depth > 0){ templateInit << token; }
			break;
			
		case wxT('>'):
			if(depth > 0){ templateInit << token; }
			depth--;
			break;
			
		case wxT('{'):
		case wxT('('):
		case wxT('['):
			if(depth > 0){ templateInit << token; }
			depth++;
			break;
		
		case wxT('}'):
		case wxT(')'):
		case wxT(']'):
			if(depth > 0){ templateInit << token; }
			depth--;
			break;
			
		default:
			if(depth > 0){ templateInit << token; }
			break;
		}
	}
	return true;
}

wxString TagEntry::GetDeleteOneStatement()
{
	return wxT("DELETE FROM TAGS WHERE Kind=? AND Signature=? AND Path=?");
}

wxString TagEntry::GetUpdateOneStatement()
{
	return wxT("UPDATE TAGS SET Name=?, File=?, Line=?, Access=?, Pattern=?, Parent=?, Inherits=?, Typeref=?, Scope=? WHERE Kind=? AND Signature=? AND Path=?");
}

wxString TagEntry::GetInsertOneStatement()
{
	return wxT("INSERT INTO TAGS VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");	
}

wxString TagEntry::GetPattern()
{
	//since ctags's pattern is regex, forward slashes are escaped. ('/' becomes '\/')
	m_pattern.Replace(wxT("\\\\"), wxT("\\"));
	m_pattern.Replace(wxT("\\/"), wxT("/"));
	return m_pattern;
}
