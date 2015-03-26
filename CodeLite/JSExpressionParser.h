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
    wxString m_wordCompleteFilter;

    enum eCompleteType {
        kCodeComplete,
        kWordComplete,
        kFunctionTipComplete,
    };
    eCompleteType m_completeType;

private:
    JSLexerToken::Vec_t CreateExpression(const wxString& text);

public:
    JSExpressionParser(const wxString& fulltext);
    virtual ~JSExpressionParser();

    /**
     * @brief resolve an expression into JSObject
     */
    JSObject::Ptr_t Resolve(JSLookUpTable::Ptr_t lookup, const wxString& filename);
    
    /**
     * @brief return the word complete filter
     */
    const wxString& GetWordCompleteFilter() const { return m_wordCompleteFilter; }
    
    /**
     * @brief return true if the expression is a word complete (i.e. it does not end with ".")
     */
    bool IsWordComplete() const { return m_completeType == kWordComplete; }
};

#endif // JSEXPRESSIONPARSER_H
