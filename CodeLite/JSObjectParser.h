#ifndef JSJSONOBJECTPARSER_H
#define JSJSONOBJECTPARSER_H

#include "codelite_exports.h"
#include "JSLexerAPI.h"
#include "JSObject.h"
#include "JSLookUpTable.h"
#include "JSSourceFile.h"

class WXDLLIMPEXP_CL JSObjectParser
{
    JSSourceFile& m_sourceFile;
    JSLookUpTable::Ptr_t m_lookup;
    JSObject::Ptr_t m_result;
    
private:
    wxString GenerateName();
    bool ReadUntil(int until);
    bool ReadSignature(JSObject::Ptr_t scope);
    
public:
    JSObjectParser(JSSourceFile& sourceFile, JSLookUpTable::Ptr_t lookup);
    virtual ~JSObjectParser();

    bool Parse(JSObject::Ptr_t parent);
    JSObject::Ptr_t GetResult() const { return m_result; }
};

#endif // JSJSONOBJECTPARSER_H
