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

class WXDLLIMPEXP_CL PHPEntityBase
{
public:
    typedef wxSharedPtr<PHPEntityBase> Ptr_t;
    typedef std::list<PHPEntityBase::Ptr_t> List_t;
    typedef std::map<wxString, PHPEntityBase::Ptr_t> Map_t;

protected:
    PHPEntityBase::Map_t m_children;
    PHPEntityBase* m_parent;
    wxFileName m_filename;
    int m_line;
    int m_column;
    wxString m_name;
    wxString m_scope;
    wxString m_docComment;

    // The database identifier
    wxLongLong m_dbId;

public:
    PHPEntityBase();
    virtual ~PHPEntityBase(){};

    PHPEntityBase* Parent() const { return m_parent; }

    /**
     * @brief search for child in the children list of this entity.
     * Optionally, you can request to search for a child with or without the dollar "$" sign
     */
    PHPEntityBase::Ptr_t FindChild(const wxString& name, bool tryPrependingDollar = false) const;

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
    void SetScope(const wxString& scope) { this->m_scope = scope; }
    const wxString& GetName() const { return m_name; }
    const wxString& GetScope() const { return m_scope; }
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
     * @brief add a child to this scope
     */
    virtual void AddChild(PHPEntityBase::Ptr_t child);

    /**
     * @brief convert this base class to its concrete
     * @return
     */
    template <typename T> T* Cast() const { return dynamic_cast<T*>(const_cast<PHPEntityBase*>(this)); }

    /**
     * @brief store this object and all its children into the database
     * @param db
     */
    virtual void Store(wxSQLite3Database& db) = 0;
    
    /**
     * @brief store this entry and all its children
     */
    void StoreRecursive(wxSQLite3Database& db);
};
#endif // PHPENTITYIMPL_H
