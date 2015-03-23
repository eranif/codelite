#ifndef JSSOURCEFILE_H
#define JSSOURCEFILE_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/filename.h>
#include <vector>
#include "JSLexerAPI.h"
#include "JSObject.h"
#include "JSLookUpTable.h"

class WXDLLIMPEXP_CL JSSourceFile
{
    JSScanner_t m_scanner;
    wxFileName m_filename;
    JSLookUpTable::Ptr_t m_lookup;
    int m_depth;
    
protected:
    bool ReadSignature(JSObject::Ptr_t scope);
    void ParseFunction(JSObject::Ptr_t scope);
    bool ReadUntil(int until, wxString& content);
    bool ReadUntil2(int until, int until2, wxString& content);
    
    // Actions
    void OnFunction();
    
    /**
     * @brief handle this.<something>... while inside a function
     */
    void OnFunctionThisProperty();
    /**
     * @brief Attempt to parse Class[.prototype].Foo
     */
    void OnPropertyOrFunction();
    
    bool NextToken(JSLexerToken& token);
    
public:
    JSSourceFile(JSLookUpTable::Ptr_t lookup, const wxString& fileContent, const wxFileName& filename);
    JSSourceFile(JSLookUpTable::Ptr_t lookup, const wxFileName& filename);

    virtual ~JSSourceFile();

    void Parse(int exitDepth = -1);
};

#endif // JSSOURCEFILE_H
