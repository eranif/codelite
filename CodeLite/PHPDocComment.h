#ifndef PHPDOCCOMMENT_H
#define PHPDOCCOMMENT_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/arrstr.h>
#include <map>
#include "PHPSourceFile.h"

class WXDLLIMPEXP_CL PHPDocComment
{
    wxString m_comment;
    std::map<wxString, wxString> m_params;
    wxArrayString m_paramsArr;
    wxString m_returnValue;
    wxString m_varType;
    wxString m_varName;
    
public:
    PHPDocComment(PHPSourceFile& sourceFile, const wxString& comment);
    virtual ~PHPDocComment();

    void SetComment(const wxString& comment) { this->m_comment = comment; }

    const wxString& GetVar() const;
    const wxString& GetReturn() const;
    const wxString& GetParam(const wxString &name) const;
    const wxString& GetParam(size_t index) const;
};

#endif // PHPDOCCOMMENT_H
