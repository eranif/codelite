#ifndef JSLOOKUPTABLE_H
#define JSLOOKUPTABLE_H

#include "codelite_exports.h"
#include <map>
#include "smart_ptr.h"
#include <wx/string.h>
#include "JSObject.h"
#include "JSLexerAPI.h"

class WXDLLIMPEXP_CL JSLookUpTable
{
    JSObject::Map_t m_classes;
    JSObject::Ptr_t m_globalScope;
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
    JSObject::Ptr_t FindClass(const wxString& path) const;
    
    /**
     * @brief allocate new JS object
     */
    JSObject::Ptr_t NewObject() const;
    
    /**
     * @brief allocate new JS function
     */
    JSObject::Ptr_t NewFunction() const;
    
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
    
    /**
     * @brief populate the lookup table with the global
     * objects (e.g. document, window, console etc)
     */
    void PopulateWithGlobals();
    
    const JSObject::Map_t& GetClassTable() const { return m_classes; }
    JSObject::Map_t& GetClassTable() { return m_classes; }
    
    /**
     * @brief return the global scope
     * @return 
     */
    JSObject::Ptr_t GetGlobalScope() const { return m_globalScope; }
};

#endif // JSLOOKUPTABLE_H
