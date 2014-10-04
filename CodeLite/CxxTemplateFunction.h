#ifndef CXXTEMPLATEFUNCTION_H
#define CXXTEMPLATEFUNCTION_H

#include "codelite_exports.h"
#include "entry.h"
#include "CxxLexerAPI.h"

class WXDLLIMPEXP_CL CxxTemplateFunction
{
    Scanner_t m_scanner;
    Scanner_t m_sigScanner;
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
    
    /**
     * @brief can the template arguments be deduced from the function 
     * signature?
     */
    bool CanTemplateArgsDeduced();
};

#endif // CXXTEMPLATEFUNCTION_H
