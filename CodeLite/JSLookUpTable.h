#ifndef JSLOOKUPTABLE_H
#define JSLOOKUPTABLE_H

#include "codelite_exports.h"
#include <map>
#include "smart_ptr.h"
#include <wx/string.h>
#include "JSObject.h"

class WXDLLIMPEXP_CL JSLookUpTable
{
    JSObject::Map_t m_classes;
    JSObject::Map_t m_tmpClasses;
    JSObject::Ptr_t m_globalScope;
    JSObject::Vec_t m_actualScopes;
    JSObject::Vec_t m_tempScopes;
    JSObject::Vec_t* m_scopes;
    int m_objSeed;

public:
    typedef SmartPtr<JSLookUpTable> Ptr_t;

private:
    void InitializeGlobalScope();
    JSObject::Ptr_t DoFindSingleType(const wxString& type) const;
    
public:
    JSLookUpTable();
    virtual ~JSLookUpTable();

    wxString GenerateNewType();
    
    /**
     * @brief return object properties (parent's properties included)
     */
    JSObject::Map_t GetObjectProperties(JSObject::Ptr_t o) const;
    
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
     * @brief the temp class table is maintained per file
     * and it contains annonymous classes (usually instantiated via JSONLiteral)
     */
    void ClearTempClassTable();
    
    /**
     * @brief create new temporary object
     */
    JSObject::Ptr_t NewTempObject();
    
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

    /**
     * @brief initialize our class table from 'other'
     * @param other the source lookup table to copy from
     * @param move if set to true, the class table is moved. i.e. 
     * after the copy it will be cleared from the 'other' lookup
     */
    void CopyClassTable(JSLookUpTable::Ptr_t other, bool move = true);
    
    /**
     * @brief return the global scope
     * @return
     */
    JSObject::Ptr_t GetGlobalScope() const { return m_globalScope; }
};

#endif // JSLOOKUPTABLE_H
