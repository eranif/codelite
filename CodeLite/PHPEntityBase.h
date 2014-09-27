#ifndef PHPENTITYIMPL_H
#define PHPENTITYIMPL_H

#include "codelite_exports.h"
#include <wx/sharedptr.h>
#include <list>
#include <map>
#include <wx/string.h>
#include <iostream>
#include <wx/filename.h>

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

public:
    PHPEntityBase();
    virtual ~PHPEntityBase(){};
    
    PHPEntityBase* Parent() const {
        return m_parent;
    }
    
    /**
     * @brief search for child in the children list of this entity.
     * Optionally, you can request to search for a child with or without the dollar "$" sign
     */
    PHPEntityBase::Ptr_t FindChild(const wxString &name, bool tryPrependingDollar = false) const;
    
    // Setters / Getters
    void SetColumn(int column) { this->m_column = column; }
    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    void SetLine(int line) { this->m_line = line; }
    int GetColumn() const { return m_column; }
    const wxFileName& GetFilename() const { return m_filename; }
    int GetLine() const { return m_line; }
    
    /**
     * @brief recursive print to stdout this object and all its children
     * @param parent
     */
    void RecursivePrintStdout(PHPEntityBase::Ptr_t parent, int indent);

    /**
     * @brief return a ID string that identifies this entity
     */
    virtual wxString ID() const = 0;

    /**
     * @brief print this object to the stdout
     */
    virtual void PrintStdout(int indent) const = 0;

    /**
     * @brief add a child to this scope
     */
    void AddChild(PHPEntityBase::Ptr_t child);

    /**
     * @brief convert this base class to its concrete
     * @return
     */
    template <typename T> T* Cast() const { return dynamic_cast<T*>(const_cast<PHPEntityBase*>(this)); }
};
#endif // PHPENTITYIMPL_H
