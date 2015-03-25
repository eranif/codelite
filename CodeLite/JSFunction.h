#ifndef JSFUNCTION_H
#define JSFUNCTION_H

#include "codelite_exports.h"
#include <wx/string.h>
#include "JSObject.h"
#include "smart_ptr.h"

class JSLookUpTable;
class WXDLLIMPEXP_CL JSFunction : public JSObject
{
protected:
    wxString m_signature;
    JSObject::Map_t m_variables; // include local variables + function arguments

public:
    JSFunction(const JSLookUpTable* lookup);
    virtual ~JSFunction();
    
    virtual JSObject::Ptr_t NewInstance(const wxString& name, const JSLookUpTable* lookup);
    
    /**
     * @brief is this object a function?
     */
    virtual bool IsFunction() const { return true; }

    void SetSignature(const wxString& signature) { this->m_signature = signature; }
    const wxString& GetSignature() const { return m_signature; }

    const JSObject::Map_t& GetVariables() const { return m_variables; }
    JSObject::Map_t& GetVariables() { return m_variables; }
    void AddVariable(JSObject::Ptr_t var);
    virtual void Print(int depth);
};

#endif // JSFUNCTION_H
