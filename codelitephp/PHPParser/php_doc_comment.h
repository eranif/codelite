#ifndef PHPDOCCOMMENT_H
#define PHPDOCCOMMENT_H

#include <wx/string.h>
#include <wx/arrstr.h>

class PHPDocComment
{
    wxString      m_comment;
    wxArrayString m_params;
    wxString      m_returnValue;
    wxString      m_varType;

public:
    PHPDocComment(const wxString &comment);
    virtual ~PHPDocComment();

    void setComment(const wxString& comment) {
        this->m_comment = comment;
    }

    const wxString &getVar() const;
    const wxString &getReturn() const;
    const wxString &getParam(size_t n) const;
};

#endif // PHPDOCCOMMENT_H
