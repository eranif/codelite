#include <stdio.h>
#include "PHPSourceFile.h"
#include <wx/ffile.h>
#include <wx/init.h>
#include "PHPLookupTable.h"
#include "PHPExpression.h"
#include "PHPEntityFunction.h"
#include "tester.h"

void PrintMatches(const PHPEntityBase::List_t& matches)
{
    PHPEntityBase::List_t::const_iterator iter = matches.begin();
    for(; iter != matches.end(); ++iter) {
        (*iter)->PrintStdout(2);
    }
}

PHPLookupTable lookup;
TEST_FUNC(test_this_operator)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_this_operator.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    if(resolved) {
        PHPEntityBase::List_t matches = lookup.FindChildren(
            resolved->GetDbId(), PHPLookupTable::kLookupFlags_StartsWith | expr.GetLookupFlags(), expr.GetFilter());
        CHECK_SIZE(matches.size(), 3);
        PrintMatches(matches);
    }
    return true;
}

TEST_FUNC(test_class_extends)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_class_extends.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    if(resolved) {
        PHPEntityBase::List_t matches = lookup.FindChildren(
            resolved->GetDbId(), PHPLookupTable::kLookupFlags_StartsWith | expr.GetLookupFlags(), expr.GetFilter());
        CHECK_SIZE(matches.size(), 2);
        PrintMatches(matches);
    }
    return true;
}

// Test a simple case of using the 'use' operator:
// use use_real_name as use_alias;
// class use_real_name {}
// $a = new use_alias();
// $a->
TEST_FUNC(test_use_alias_operator)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_use_alias_operator.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved != NULL);
    CHECK_WXSTRING(resolved->GetName(), "\\use_real_name");
    
    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_StartsWith | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 1);
    PrintMatches(matches);
    return true;
}

// Make sure that the expression can detect a partial expression of a just a word, e.g.:
// json_de
TEST_FUNC(test_expression_parser_for_partial_word)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_expression_parser_for_partial_word.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    CHECK_WXSTRING(expr.GetExpressionAsString(), wxT("json_dec"));
    return true;
}

// test a chained expression:
// $a->foo()->bar()->
TEST_FUNC(test_long_chain)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_long_chain.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved != NULL);
    CHECK_WXSTRING(resolved->GetName(), "\\ClassRetVal1");
    
    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_StartsWith | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 3);
    PrintMatches(matches);
    return true;
}

// test a chained expression:
// $a->foo()->bar()->
TEST_FUNC(test_parsing_abstract_class)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_parsing_abstract_class.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved != NULL);
    CHECK_WXSTRING(resolved->GetName(), "\\AbstractFoo");
    
    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_StartsWith | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 3);
    PrintMatches(matches);
    return true;
}

int main(int argc, char** argv)
{
    lookup.Open(::wxGetCwd());
    Tester::Instance()->RunTests(); // Run all tests
    return 0;
}
