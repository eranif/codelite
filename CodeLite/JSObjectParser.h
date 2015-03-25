#ifndef JSJSONOBJECTPARSER_H
#define JSJSONOBJECTPARSER_H

#include "codelite_exports.h"
#include "JSLexerAPI.h"
#include "JSObject.h"
#include "JSLookUpTable.h"

class WXDLLIMPEXP_CL JSObjectParser
{
    JSLookUpTable::Ptr_t m_lookup;
    JSScanner_t m_scanner;
    JSObject::Ptr_t m_result;

private:
    wxString GenerateName();
    bool ReadUntil(int until);

public:
    JSObjectParser(const wxString& text, JSLookUpTable::Ptr_t lookup);
    virtual ~JSObjectParser();

    bool Parse(JSObject::Ptr_t parent);
    JSObject::Ptr_t GetResult() const { return m_result; }
};

#endif // JSJSONOBJECTPARSER_H
