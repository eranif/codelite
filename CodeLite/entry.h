//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : entry.h
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
 #ifndef CODELITE_ENTRY_H
#define CODELITE_ENTRY_H

#include <wx/treectrl.h>
#include "readtags.h"
#include <wx/string.h>
#include <map>
#include "smart_ptr.h"

/**
 * TagEntry is a persistent object which is capable of storing and loading itself from
 * various inputs:
 * - tagEntry (ctags structure)
  *
 * It contains all the knowledge of storing and retrieving itself from the database
 *
 * \ingroup CodeLite
 * \version 1.0
 * first version
 *
 * \date 11-11-2006
 * \author Eran
 */
class TagEntry
{
	wxString                     m_path;		///< Tag full path
	wxString                     m_file;		///< File this tag is found
	int                          m_lineNumber;	///< Line number
	wxString                     m_pattern;		///< A pattern that can be used to locate the tag in the file
	wxString                     m_kind;		///< Member, function, class, typedef etc.
	wxString                     m_parent;		///< Direct parent
	wxTreeItemId                 m_hti;		///< Handle to tree item, not persistent item
	wxString                     m_name;		///< Tag name (short name, excluding any scope names)
	std::map<wxString, wxString> m_extFields; ///< Additional extension fields
	long                         m_id;
	wxString                     m_scope;
	bool                         m_differOnByLineNumber;

public:
	/**
	 * Construct a TagEntry from tagEntry struct
	 * \param entry Tag entry
	 */
	TagEntry(const tagEntry& entry);

	/**
	 * Default constructor.
	 */
	TagEntry();

	void FromLine(const wxString &line);

	/**
	 * Copy constructor.
	 */
	TagEntry(const TagEntry& rhs);

	/**
	 * \param rhs Source to copy from (right hand side)
	 * \return this
	 */
	TagEntry& operator=(const TagEntry& rhs);

	/**
	 * Compare two TagEntry objects.
	 * \param rhs Right hand side
	 * \return true if identical, false otherwise
	 */
	bool operator==(const TagEntry& rhs);

	/**
	 *	Destructor
	 */
	virtual ~TagEntry();

	/**
	 * Construct a TagEntry from tagEntry struct.
	 * \param entry Tag entry
	 */
	void Create(const tagEntry& entry);

	/**
	 * Construct a TagEntry from values.
	 * \param fileName File name
	 * \param name Tag name
	 * \param lineNumber Tag line number
	 * \param pattern Pattern
	 * \param kind Tag kind (class, struct, etc)
	 * \param extFields Map of extenstion fields (key:value)
	 * \param project Project name
	 */
	void Create(const wxString &fileName,
				const wxString &name,
				int lineNumber,
				const wxString &pattern,
				const wxString &kind,
				std::map<wxString, wxString>& extFields);

	/**
	 * Test if this entry has been initialised.
	 * \return true if this tag entry has been initialised
	 */
	const bool IsOk() const { return GetKind() != _T("<unknown>"); }

	/**
	 * Test of this tag is a container (class, union, struct or namespace
	 */
	const bool IsContainer() const;

	/**
	 * @brief return true if this tag represents a constructor
	 */
	bool IsConstructor() const;
	/**
	 * @brief return true if this tag represents a destructor
	 */
	bool IsDestructor() const;
	
	/**
	 * @brief return true of the this tag is a function or prototype
	 */
	bool IsMethod() const;
	
	bool IsFunction() const;
	bool IsPrototype() const;
	bool IsMacro() const;
	bool IsClass() const;
	bool IsStruct() const;
	bool IsScopeGlobal() const;
	
	//------------------------------------------
	// Operations
	//------------------------------------------
	bool GetDifferOnByLineNumber() const {return m_differOnByLineNumber;}

	int GetId() const { return m_id; }
	void SetId(int id) { m_id = id;}

	const wxString& GetName() const { return m_name;}
	void SetName(const wxString& name) { m_name = name; }

	const wxString& GetPath() const { return m_path;}
	void SetPath(const wxString& path) { m_path = path; }

	const wxString& GetFile() const { return m_file;}
	void SetFile(const wxString& file) { m_file = file;}

	int GetLine() const { return m_lineNumber;}
	void SetLine(int line) { m_lineNumber = line; }

	wxString GetPattern() const;
	void SetPattern(const wxString& pattern) { m_pattern = pattern; }

	wxString GetKind() const;
	void SetKind(const wxString& kind) { m_kind = kind; }

	const wxString& GetParent() const { return m_parent; }
	void SetParent(const wxString& parent) { m_parent = parent; }

	wxTreeItemId& GetTreeItemId() { return m_hti; }
	void SetTreeItemId(wxTreeItemId& hti) { m_hti = hti; }

	wxString GetAccess() const { return GetExtField(_T("access"));}
	void SetAccess(const wxString &access){m_extFields[wxT("access")] = access;}

	wxString GetSignature() const { return GetExtField(_T("signature")); }
	void SetSignature(const wxString &sig) { m_extFields[wxT("signature")] = sig; }

	void SetInherits ( const wxString &inherits ) { m_extFields[_T("inherits")] = inherits; }
	void SetTyperef  ( const wxString &typeref  ) { m_extFields[_T("typeref")]   = typeref; }

	wxString GetInherits() const { return GetExtField(_T("inherits")); }
	wxString GetTyperef() const { return GetExtField(_T("typeref")); }
	
	void     SetReturnValue(const wxString &retVal  ) { m_extFields[_T("returns")]   = retVal; }
	wxString GetReturnValue() const;
	
	const wxString &GetScope() const {return m_scope;}
	void SetScope(const wxString &scope){m_scope = scope;}

	/**
	 * \return Scope name of the tag.
	 * If path is empty in db or contains just the project name, it will return the literal <global>.
	 * For project tags, an empty string is returned.
	 */
	wxString GetScopeName() const;

	/**
	 * Generate a Key for this tag based on its attributes
	 * \return tag key
	 */
	wxString Key() const;

	/**
	 * Generate a display name for this tag to be used by the symbol tree
	 * \return tag display name
	 */
	wxString GetDisplayName() const;

	/**
	 * Generate a full display name for this tag that includes:
	 * full scope + name + signature
	 * \return tag full display name
	 */
	wxString GetFullDisplayName() const;

	/**
	 * Return the actual name as described in the 'typeref' field
	 * \return real name or wxEmptyString
	 */
	wxString NameFromTyperef(wxString &templateInitList);

	/**
	 * Return the actual type as described in the 'typeref' field
	 * \return real name or wxEmptyString
	 */
	wxString TypeFromTyperef() const;
	//------------------------------------------
	// Extenstion fields
	//------------------------------------------
	wxString GetExtField(const wxString& extField) const
	{
		std::map<wxString, wxString>::const_iterator iter = m_extFields.find(extField);
		if(iter == m_extFields.end())
			return wxEmptyString;
		return iter->second;
	}

	//------------------------------------------
	// Misc
	//------------------------------------------
	void Print();

private:
	/**
	 * Update the path with full path (e.g. namespace::class)
	 * \param path path to add
	 */
	void UpdatePath        (wxString & path);
	bool TypedefFromPattern(const wxString &tagPattern, const wxString &typedefName, wxString &name, wxString &templateInit);
};

typedef SmartPtr<TagEntry> TagEntryPtr;
#endif // CODELITE_ENTRY_H
