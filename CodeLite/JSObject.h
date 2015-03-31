#ifndef JSOBJECT_H
#define JSOBJECT_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/filename.h>
#include "smart_ptr.h"
#include <vector>
#include <map>
#include <set>

class WXDLLIMPEXP_CL JSObject
{
public:
    typedef SmartPtr<JSObject> Ptr_t;
    typedef std::vector<JSObject::Ptr_t> Vec_t;
    typedef std::map<wxString, JSObject::Ptr_t> Map_t;

protected:
    wxString m_name;
    wxString m_comment;
    wxFileName m_file;
    int m_line;
    JSObject::Map_t m_properties;
    wxString m_type;
    std::set<wxString> m_extends;
    JSObject* m_parent;
    size_t m_flags;

    // include local variables + function arguments
    JSObject::Map_t m_variables;

    // Function signature
    wxString m_signature;

    enum eJSObjectFlags {
        kJS_Undefined = (1 << 0),
        kJS_GlobalScope = (1 << 1),
        kJS_Class = (1 << 2),
        kJS_Function = (1 << 3),
        kJS_TypeSetByDocComment = (1 << 4),
    };

protected:
    inline bool HasFlag(JSObject::eJSObjectFlags flag) const { return (m_flags & flag); }
    inline void EnableFlag(JSObject::eJSObjectFlags flag, bool b = true)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

protected:
    const wxString& GetTypesAsString() const { return m_type; }

public:
    JSObject();
    virtual ~JSObject();

    inline bool IsUndefined() const { return HasFlag(kJS_Undefined); }
    inline bool IsClass() const { return HasFlag(kJS_Class); }
    inline bool IsGlobalScope() const { return HasFlag(kJS_Undefined); }
    inline void SetUndefined() { EnableFlag(kJS_Undefined); }
    inline void SetGlobalScope() { EnableFlag(kJS_GlobalScope); }
    inline void SetClass()
    {
        EnableFlag(kJS_Class);
        EnableFlag(kJS_Function, false);
    }
    inline void SetFunction()
    {
        EnableFlag(kJS_Function);
        EnableFlag(kJS_Class, false);
    }

    inline void SetTypeByDocComment() { EnableFlag(kJS_TypeSetByDocComment); }

    /**
     * @brief add a variable to this object
     */
    void AddVariable(JSObject::Ptr_t var);

    /**
     * @brief return list of variables available for this object
     */
    const JSObject::Map_t& GetVariables() const { return m_variables; }
    /**
     * @brief return list of variables available for this object (non-const version)
     */
    JSObject::Map_t& GetVariables() { return m_variables; }

    /**
     * @brief return the "extends" list of this object
     */
    const std::set<wxString>& GetExtends() const { return m_extends; }

    /**
     * @brief make this object extending 'className'
     * @param className
     */
    void Extends(const wxString& className);

    /**
     * @brief allocate a new instance of this object
     */
    virtual JSObject::Ptr_t NewInstance(const wxString& name);

    /**
     * @brief print this object to the log file
     */
    virtual void Print(int depth);

    /**
     * @brief is this object a function?
     */
    virtual bool IsFunction() const { HasFlag(kJS_Function); }

    /**
     * @brief return this object properties
     */
    const JSObject::Map_t& GetProperties() const { return m_properties; }
    JSObject::Map_t& GetProperties() { return m_properties; }
    /**
     * @brief add a property to the object
     */
    void AddProperty(JSObject::Ptr_t property);
    template <typename T> T* As() const { return dynamic_cast<T*>(const_cast<JSObject*>(this)); }

    void SetComment(const wxString& comment) { this->m_comment = comment; }
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetComment() const { return m_comment; }
    const wxString& GetName() const { return m_name; }
    void SetFile(const wxFileName& file) { this->m_file = file; }
    void SetLine(int line) { this->m_line = line; }
    const wxFileName& GetFile() const { return m_file; }
    int GetLine() const { return m_line; }
    void SetSignature(const wxString& signature) { this->m_signature = signature; }
    const wxString& GetSignature() const { return m_signature; }
    /**
     * @brief add possible type to this object.
     * @param type Class name
     * @param clear if true (the default), clear any previous types
     */
    void AddType(const wxString& type, bool clear = true)
    {
        if(clear) {
            m_type.Clear();
        }

        if(!m_type.IsEmpty()) {
            m_type << "|";
        }
        m_type << type;
    }

    void SetType(const wxString& type) { AddType(type, true); }
    wxString GetType() const;
};

#endif // JSOBJECT_H
