#include <stdio.h>
#include "PHPSourceFile.h"
#include <wx/ffile.h>
#include <wx/init.h>
#include "PHPLookupTable.h"
#include "PHPExpression.h"

int main(int argc, char **argv)
{
    wxInitialize(argc, argv); 
#if 1
    wxPrintf("Starting...\n");
    PHPLookupTable lookup;
    lookup.Open(::wxGetCwd());
    
    PHPSourceFile sourceFile(wxFileName("../Tests/test_chain.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    sourceFile.PrintStdout();
    
    // Store the results
    lookup.UpdateSourceFile( sourceFile );
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolvedType = expr.Resolve(lookup);
    if(resolvedType) {
        wxPrintf("%s => suggest members of: %s\n", expr.GetExpressionAsString(), resolvedType->Type());
    }
    wxPrintf("Done!...\n");
    
#else
    wxPrintf("Starting...\n");
    wxString content;
    phpLexerToken token;
    wxFFile fp("../Tests/Mage.php");
    fp.ReadAll(&content);
    PHPScanner_t scanner = ::phpLexerNew(content);
    while(::phpLexerNext(scanner, token)) {}
    ::phpLexerDestroy(&scanner);
    wxPrintf("Done!...\n");
#endif
    return 0;
}
