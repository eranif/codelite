#ifndef JSFUNCTION_H
#define JSFUNCTION_H

#include "codelite_exports.h"
#include <wx/string.h>
#include "JSObject.h"
#include "smart_ptr.h"

class WXDLLIMPEXP_CL JSFunction : public JSObject
{
protected:
    wxString m_signature;

public:
    JSFunction();
    virtual ~JSFunction();

    void SetSignature(const wxString& signature) { this->m_signature = signature; }
    const wxString& GetSignature() const { return m_signature; }
    
    virtual void Print(int depth);
};

#endif // JSFUNCTION_H
