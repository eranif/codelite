#include <stdio.h>
#include "PHPSourceFile.h"
#include <wx/ffile.h>
#include <wx/init.h>
#include "PHPLookupTable.h"
#include "PHPExpression.h"
#include "PHPEntityFunction.h"

int main(int argc, char** argv)
{
    wxInitialize(argc, argv);
#if 1
    wxPrintf("Starting...\n");
    PHPLookupTable lookup;
    lookup.Open(::wxGetCwd());

    PHPSourceFile sourceFile(wxFileName("../Tests/Mage.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    
    // Store the results
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr("<?php \\Mage::ins");
    PHPEntityBase::Ptr_t resolvedType = expr.Resolve(lookup);
    if(resolvedType) {
        // Get list of children from the database
        PHPEntityBase::List_t matches =
            lookup.FindChildren(resolvedType->GetDbId(), PHPLookupTable::kLookupFlags_PartialMatch, expr.GetFilter());
    
        wxPrintf("%s => suggested members are:\n", expr.GetExpressionAsString());
        PHPEntityBase::List_t::iterator iter = matches.begin();
        for(; iter != matches.end(); ++iter) {
            PHPEntityBase::Ptr_t entry = *iter;
            if(entry->Is(kEntityTypeFunction)) {
                wxPrintf("%s%s\n", entry->GetName(), entry->Cast<PHPEntityFunction>()->GetSignature());
            } else {
                wxPrintf("%s\n", entry->GetName());
            }
        }
    }
    wxPrintf("Done!...\n");

#else
    wxPrintf("Starting...\n");
    wxString content;
    phpLexerToken token;
    wxFFile fp("../Tests/Mage.php");
    fp.ReadAll(&content);
    PHPScanner_t scanner = ::phpLexerNew(content);
    while(::phpLexerNext(scanner, token)) {
    }
    ::phpLexerDestroy(&scanner);
    wxPrintf("Done!...\n");
#endif
    return 0;
}
