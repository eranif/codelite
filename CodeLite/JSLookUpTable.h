#ifndef JSLOOKUPTABLE_H
#define JSLOOKUPTABLE_H

#include "codelite_exports.h"
#include <map>
#include "JSObject.h"
#include "smart_ptr.h"
#include <wx/string.h>

class WXDLLIMPEXP_CL JSLookUpTable
{
    JSObject::Map_t m_objects;
    JSObject::Vec_t m_actualScopes;
    JSObject::Vec_t m_tempScopes;
    JSObject::Vec_t* m_scopes;
    int m_objSeed;

public:
    typedef SmartPtr<JSLookUpTable> Ptr_t;

public:
    JSLookUpTable();
    virtual ~JSLookUpTable();

    wxString GenerateNewType();

    void AddObject(JSObject::Ptr_t obj);
    JSObject::Ptr_t CurrentScope() const;
    /**
     * @brief pop the current scope (this function does not remove the global scope)
     */
    void PopScope();
    /**
     * @brief push new scope to the stack
     * @param scope
     */
    void PushScope(JSObject::Ptr_t scope);

    /**
     * @brief return the current path (based on the current scope)
     */
    const wxString& CurrentPath() const;

    /**
     * @brief make path from the current path + input provided
     */
    wxString MakePath(const wxString& pathLastPart);

    /**
     * @brief return an object with given path
     */
    JSObject::Ptr_t FindObject(const wxString& path) const;

    /**
     * @brief set scope to obj
     */
    void SetTempScope(JSObject::Ptr_t obj);

    /**
     * @brief switch back to the normal scopes
     */
    void SwapScopes();

    /**
     * @brief return map of visible variables
     */
    JSObject::Map_t GetVisibleVariables();

    void Print();

    /**
     * @brief clear this lookup content
     */
    void Clear();
    
    /**
     * @brief prepare the lookup table for parsing a source file
     */
    void PrepareLookup();
    
    const JSObject::Map_t& GetObjects() const { return m_objects; }
    JSObject::Map_t& GetObjects() { return m_objects; }
};

#endif // JSLOOKUPTABLE_H
