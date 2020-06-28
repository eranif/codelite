//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPEntityBase.h
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

#ifndef PHPENTITYIMPL_H
#define PHPENTITYIMPL_H

#include "codelite_exports.h"
#include <wx/sharedptr.h>
#include <list>
#include <map>
#include <wx/string.h>
#include <iostream>
#include <wx/filename.h>
#include "wx/wxsqlite3.h"
#include <wx/wxcrtvararg.h> // Needed for wxPrintf
#include "smart_ptr.h"
#include <set>
#include "commentconfigdata.h"
#include "wxStringHash.h"
#include "JSON.h"

// The entity type
class WXDLLIMPEXP_CL PHPLookupTable;
enum eEntityType {
    kEntityTypeVariable = 0,
    kEntityTypeFunction = 1,
    kEntityTypeClass = 2,
    kEntityTypeNamespace = 3,
    kEntityTypeKeyword = 4,
    kEntityTypeFunctionAlias = 5,
};

// Variable flags
enum {
    kVar_Public = (1 << 1),
    kVar_Private = (1 << 2),
    kVar_Protected = (1 << 3),
    kVar_Member = (1 << 4),
    kVar_Reference = (1 << 5),
    kVar_Const = (1 << 6),
    kVar_FunctionArg = (1 << 7),
    kVar_Static = (1 << 8),
    kVar_Define = (1 << 9),
    kVar_Nullable = (1 << 10),
};

// Function flags
enum {
    kFunc_Public = (1 << 1),
    kFunc_Private = (1 << 2),
    kFunc_Protected = (1 << 3),
    kFunc_Final = (1 << 4),
    kFunc_Static = (1 << 5),
    kFunc_Abstract = (1 << 6),
    kFunc_ReturnReference = (1 << 7),
    kFunc_ReturnNullable = (1 << 8),
};

// Class flags
enum {
    kClass_Interface = (1 << 1),
    kClass_Trait = (1 << 2),
    kClass_Abstract = (1 << 3),
};

class WXDLLIMPEXP_CL PHPEntityBase
{
public:
    typedef SmartPtr<PHPEntityBase> Ptr_t;
    typedef std::vector<PHPEntityBase::Ptr_t> List_t;
    typedef std::unordered_map<wxString, PHPEntityBase::Ptr_t> Map_t;

protected:
    PHPEntityBase::Map_t m_childrenMap;
    PHPEntityBase::List_t m_children;
    PHPEntityBase* m_parent;
    wxFileName m_filename;
    int m_line;
    int m_column;
    wxString m_fullname;
    wxString m_shortName;
    wxString m_docComment;
    size_t m_flags;

    // The database identifier
    wxLongLong m_dbId;
    
protected:
    JSONItem BaseToJSON(const wxString& entityType) const;
    void BaseFromJSON(const JSONItem& json);
    
public:
    PHPEntityBase();
    virtual ~PHPEntityBase(){};

    const PHPEntityBase::List_t& GetChildren() const { return m_children; }
    PHPEntityBase* Parent() const { return m_parent; }
    /**
     * @brief set this entity children
     */
    void SetChildren(const PHPEntityBase::List_t& children);

    /**
     * @brief search for child in the children list of this entity.
     * Optionally, you can request to search for a child with or without the dollar "$" sign
     */
    PHPEntityBase::Ptr_t FindChild(const wxString& name, bool tryPrependingDollar = false) const;

    /**
     * @brief generate a php doc comment that matches this entry
     */
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const = 0;
    
    /**
     * @brief serialization to JSON
     */
    virtual JSONItem ToJSON() const = 0;
    /**
     * @brief serialization from JSON
     */
    virtual void FromJSON(const JSONItem& json) = 0;
    
    // Setters / Getters
    void SetFlag(size_t flag, bool b = true) { b ? this->m_flags |= flag : this->m_flags &= ~flag; }
    bool HasFlag(size_t flag) const { return m_flags & flag; }
    size_t GetFlags() const { return m_flags; }
    void SetFlags(size_t flags) { this->m_flags = flags; }
    void SetDocComment(const wxString& docComment) { this->m_docComment = docComment; }
    const wxString& GetDocComment() const { return m_docComment; }
    void SetDbId(wxLongLong dbId) { this->m_dbId = dbId; }
    wxLongLong GetDbId() const { return m_dbId; }
    void SetColumn(int column) { this->m_column = column; }
    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    void SetLine(int line) { this->m_line = line; }
    int GetColumn() const { return m_column; }
    const wxFileName& GetFilename() const { return m_filename; }
    int GetLine() const { return m_line; }

    // Fullname, including the namespace prefix
    void SetFullName(const wxString& fullname);
    const wxString& GetFullName() const { return m_fullname; }

    // Short name (usually, this is the last part after the namespace separator)
    void SetShortName(const wxString& shortName) { this->m_shortName = shortName; }
    const wxString& GetShortName() const { return m_shortName; }

    /**
     * @brief recursive print to stdout this object and all its children
     * @param parent
     */
    void RecursivePrintStdout(PHPEntityBase::Ptr_t parent, int indent);

    /**
     * @brief print this object to the stdout
     */
    virtual void PrintStdout(int indent) const = 0;

    /**
     * @brief convert this object into a string tooltip
     */
    virtual wxString ToTooltip() const { return wxEmptyString; }

    /**
     * @brief return a nicely formatted string to display for this
     * entity, mainly used for UI purposes
     */
    virtual wxString GetDisplayName() const = 0;

    /**
     * @brief return the actual type that this entity deduced to.
     * + class: the return value is the fullpath of the class
     * + namespace: it will be the fullpath of the namespace
     * + function: return the return value type
     * + variable: the typehint
     * @return
     */
    virtual wxString Type() const = 0;

    /**
     * @brief return the underlying type of this instance
     */
    virtual bool Is(eEntityType type) const = 0;

    /**
     * @brief add a child to this scope
     */
    void AddChild(PHPEntityBase::Ptr_t child);

    /**
     * @brief remove a child
     */
    void RemoveChild(PHPEntityBase::Ptr_t child);

    /**
     * @brief convert this base class to its concrete
     * @return
     */
    template <typename T> T* Cast() const { return dynamic_cast<T*>(const_cast<PHPEntityBase*>(this)); }

    /**
     * @brief store this object and all its children into the database
     * @param db
     */
    virtual void Store(PHPLookupTable* lookup) = 0;

    /**
     * @brief construct this instance from a sqlite3 result set
     */
    virtual void FromResultSet(wxSQLite3ResultSet& res) = 0;

    /**
     * @brief store this entry and all its children
     */
    void StoreRecursive(PHPLookupTable* lookup);
};
#endif // PHPENTITYIMPL_H
