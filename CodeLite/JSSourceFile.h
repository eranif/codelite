#ifndef JSSOURCEFILE_H
#define JSSOURCEFILE_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/filename.h>
#include <vector>
#include "JSLexerAPI.h"
#include "JSObject.h"

class WXDLLIMPEXP_CL JSSourceFile
{
    JSScanner_t m_scanner;
    wxFileName m_filename;
    JSObject::Vec_t m_objects;
    JSObject::Vec_t m_scopes;
    int m_depth;
    
protected:
    bool ProcessStatement();
    void ReadSignature(JSObject::Ptr_t scope);
    
    
    // Actions
    void OnGlobalFunction();
    
    void printStatement();
    void skipToNextStatement();
    
    bool NextToken(JSLexerToken& token);
    
public:
    JSSourceFile(const wxString& fileContent, const wxFileName& filename);
    JSSourceFile(const wxFileName& filename);

    virtual ~JSSourceFile();
    
    void Parse(int exitDepth = -1);
};

#endif // JSSOURCEFILE_H
