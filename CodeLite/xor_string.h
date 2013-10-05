#ifndef XORSTRING_H
#define XORSTRING_H

#include <wx/string.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL XORString
{
    wxString m_value;
    
protected:
    wxString toHexString(const wxString &value) const;
    wxString fromHexString(const wxString& hexString) const;
    wxString XOR(const wxString& str, const wxChar KEY) const;
    
public:
    XORString(const wxString &value);
    XORString();
    virtual ~XORString();
    
    wxString Decrypt(const wxChar byte = 's') const;
    wxString Encrypt(const wxChar byte = 's') const;
};

#endif // XORSTRING_H
