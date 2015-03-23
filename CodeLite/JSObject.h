#ifndef JSOBJECT_H
#define JSOBJECT_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/filename.h>
#include "smart_ptr.h"
#include <vector>
#include <map>

class WXDLLIMPEXP_CL JSObject
{
public:
    typedef SmartPtr<JSObject> Ptr_t;
    typedef std::vector<JSObject::Ptr_t> Vec_t;
    typedef std::map<wxString, JSObject::Ptr_t> Map_t;

protected:
    wxString m_name;
    wxString m_path;
    wxString m_comment;
    wxFileName m_file;
    int m_line;
    JSObject::Map_t m_properties;
    wxString m_type;

public:
    JSObject();
    virtual ~JSObject();

    /**
     * @brief print this object to the log file
     */
    virtual void Print(int depth);

    /**
     * @brief is this object a function?
     */
    virtual bool IsFunction() const { return false; }
    /**
     * @brief return this object properties
     */
    const JSObject::Map_t& GetProperties() const { return m_properties; }
    void AddChild(JSObject::Ptr_t child) { m_properties.insert(std::make_pair(child->GetName(), child)); }
    template <typename T> T* As() const { return dynamic_cast<T*>(const_cast<JSObject*>(this)); }

    void SetComment(const wxString& comment) { this->m_comment = comment; }
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetComment() const { return m_comment; }
    const wxString& GetName() const { return m_name; }
    void SetFile(const wxFileName& file) { this->m_file = file; }
    void SetLine(int line) { this->m_line = line; }
    const wxFileName& GetFile() const { return m_file; }
    int GetLine() const { return m_line; }
    void SetPath(const wxString& path) { this->m_path = path; }
    const wxString& GetPath() const { return m_path; }
    void SetType(const wxString& type) { this->m_type = type; }
    const wxString& GetType() const { return m_type; }
};

#endif // JSOBJECT_H
