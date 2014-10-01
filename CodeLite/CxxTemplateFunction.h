#ifndef CXXTEMPLATEFUNCTION_H
#define CXXTEMPLATEFUNCTION_H

#include "codelite_exports.h"
#include "entry.h"
#include "CxxLexerAPI.h"

class WXDLLIMPEXP_CL CxxTemplateFunction
{
    Scanner_t m_scanner;
    wxArrayString m_list;

public:
    CxxTemplateFunction(TagEntryPtr tag);
    virtual ~CxxTemplateFunction();

public:
    /**
     * @brief parse the template definition list
     */
    virtual void ParseDefinitionList();
    const wxArrayString& GetList() const { return m_list; }
};

#endif // CXXTEMPLATEFUNCTION_H
