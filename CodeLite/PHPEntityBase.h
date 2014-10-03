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

enum eEntityType {
    kEntityTypeVariable = 0,
    kEntityTypeFunction = 1,
    kEntityTypeClass = 2,
    kEntityTypeNamespace = 3,
};

class WXDLLIMPEXP_CL PHPEntityBase
{
public:
    typedef wxSharedPtr<PHPEntityBase> Ptr_t;
    typedef std::list<PHPEntityBase::Ptr_t> List_t;
    typedef std::map<wxString, PHPEntityBase::Ptr_t> Map_t;

protected:
    PHPEntityBase::Map_t m_childrenMap;
    PHPEntityBase::List_t m_children;
    PHPEntityBase* m_parent;
    wxFileName m_filename;
    int m_line;
    int m_column;
    wxString m_name;
    wxString m_docComment;

    // The database identifier
    wxLongLong m_dbId;

public:
    PHPEntityBase();
    virtual ~PHPEntityBase(){};

    const PHPEntityBase::List_t& GetChildren() const { return m_children; }
    PHPEntityBase* Parent() const { return m_parent; }

    /**
     * @brief search for child in the children list of this entity.
     * Optionally, you can request to search for a child with or without the dollar "$" sign
     */
    PHPEntityBase::Ptr_t FindChild(const wxString& name, bool tryPrependingDollar = false) const;

    /**
     * @brief generate a php doc comment that matches this entry
     */
    virtual wxString FormatPhpDoc() const = 0;
    
    // Setters / Getters
    
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

    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }

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
     * @brief return the name only
     */
    virtual wxString GetNameOnly() const = 0;
    
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
     * @brief convert this base class to its concrete
     * @return
     */
    template <typename T> 
    T* Cast() const { return dynamic_cast<T*>(const_cast<PHPEntityBase*>(this)); }

    /**
     * @brief store this object and all its children into the database
     * @param db
     */
    virtual void Store(wxSQLite3Database& db) = 0;

    /**
     * @brief construct this instance from a sqlite3 result set
     */
    virtual void FromResultSet(wxSQLite3ResultSet& res) = 0;

    /**
     * @brief store this entry and all its children
     */
    void StoreRecursive(wxSQLite3Database& db);
};
#endif // PHPENTITYIMPL_H
