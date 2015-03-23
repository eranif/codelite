#ifndef JSLOOKUPTABLE_H
#define JSLOOKUPTABLE_H

#include "codelite_exports.h"
#include <map>
#include "JSObject.h"
#include "smart_ptr.h"
#include <wx/string.h>

class WXDLLIMPEXP_CL JSLookUpTable
{
    std::map<wxString, JSObject::Ptr_t> m_objects;
    JSObject::Ptr_t m_globalScope;

public:
    typedef SmartPtr<JSLookUpTable> Ptr_t;

public:
    JSLookUpTable();
    virtual ~JSLookUpTable();

    void AddObject(JSObject::Ptr_t obj);
    JSObject::Ptr_t GetGlobalScope() { return m_globalScope; }
};

#endif // JSLOOKUPTABLE_H
