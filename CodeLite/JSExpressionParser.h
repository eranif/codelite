#ifndef JSEXPRESSIONPARSER_H
#define JSEXPRESSIONPARSER_H

#include "codelite_exports.h"
#include <wx/string.h>
#include "JSLexerAPI.h"
#include "JSLookUpTable.h"

class WXDLLIMPEXP_CL JSExpressionParser
{
    wxString m_text;
    JSLexerToken::Vec_t m_expression;
    
private:
    JSLexerToken::Vec_t CreateExpression(const wxString& text);
    
public:
    JSExpressionParser(const wxString& fulltext);
    virtual ~JSExpressionParser();
    
    /**
     * @brief resolve an expression into JSObject
     */
    JSObject::Ptr_t Resolve(JSLookUpTable::Ptr_t lookup, const wxString &filename);
};

#endif // JSEXPRESSIONPARSER_H
