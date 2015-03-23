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
    JSObject::Vec_t m_objects;
    JSObject::Vec_t m_scopes;
    int m_depth;
    
protected:
    bool ReadSignature(JSObject::Ptr_t scope);
    bool ReadUntil(int until, wxString& content);
    bool ReadUntil2(int until, int until2, wxString& content);
    
    // Actions
    void OnFunction();
    void OnAnonFunction();
    
    /**
     * @brief handle this.<something>... while inside a function
     */
    void OnFunctionThisProperty();
    
    bool NextToken(JSLexerToken& token);
    wxString GetCurrentPath();
    JSObject::Ptr_t CurrentScope();
    
    /**
     * @brief return the full path for object "name"
     */
    wxString BuildPath(const wxString& name);
    
public:
    JSSourceFile(JSLookUpTable::Ptr_t lookup, const wxString& fileContent, const wxFileName& filename);
    JSSourceFile(JSLookUpTable::Ptr_t lookup, const wxFileName& filename);

    virtual ~JSSourceFile();

    void Parse(int exitDepth = -1);
};

#endif // JSSOURCEFILE_H
