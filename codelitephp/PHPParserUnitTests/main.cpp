#include <stdio.h>
#include "PHPSourceFile.h"
#include <wx/ffile.h>
#include <wx/init.h>
#include "PHPLookupTable.h"
#include "PHPExpression.h"
#include "PHPEntityFunction.h"
#include "tester.h"

#ifdef __WXMSW__
#   define SYMBOLS_DB_PATH "%TEMP%"
#else
#   define SYMBOLS_DB_PATH "/tmp" 
#endif

void PrintMatches(const PHPEntityBase::List_t& matches)
{
#if 0
    PHPEntityBase::List_t::const_iterator iter = matches.begin();
    for(; iter != matches.end(); ++iter) {
        (*iter)->PrintStdout(2);
    }
#endif
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
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "use_real_name");
    
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
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "ClassRetVal1");
    
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
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "AbstractFoo");
    
    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_StartsWith | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 3);
    PrintMatches(matches);
    return true;
}

// test word completion when inside the global namespace
// part_w + CTRL+SPACE
TEST_FUNC(test_word_completion)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_word_completion.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    CHECK_BOOL(resolved->GetShortName().IsEmpty());
    CHECK_WXSTRING(resolved->GetFullName(), "\\");
    
    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_Contains | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 2); // 2 matches
    PrintMatches(matches);
    return true;
}

// test word completion when inside a namespace
// namespace bla;
// part_w + CTRL+SPACE
TEST_FUNC(test_word_completion_inside_ns)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_word_completion_inside_ns.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    CHECK_WXSTRING(resolved->GetShortName(), "ns");
    
    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_Contains | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 1);
    PrintMatches(matches);
    return true;
}

// test completing class members
TEST_FUNC(test_class_members)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_class_members.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "ClassWithMembers");
    
    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_Contains | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 4);
    PrintMatches(matches);
    return true;
}

// test completing class members, but this time the class is defined inside a namespace
TEST_FUNC(test_class_with_members_inside_namespace)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_class_with_members_inside_namespace.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "MyClassWithMembers");
    
    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_Contains | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 4);
    PrintMatches(matches);
    return true;
}

// test instantiating and using variable
// and using multiple times
TEST_FUNC(test_variable_1)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_variable_1.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "test_variable_1_return_value");
    
    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_Contains | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 3);
    PrintMatches(matches);
    return true;
}

// test instantiating and using variable
// and using multiple times
TEST_FUNC(test_variable_2)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_variable_2.php"));
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "test_variable_2");
    
    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_Contains | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 5);
    PrintMatches(matches);
    return true;
}

int main(int argc, char** argv)
{
#if 0
    PHPExpression expr("<?php wx\\msg");
    wxPrintf("%s\n", expr.GetExpressionAsString());
#else
    wxFileName symbolsDBPath(SYMBOLS_DB_PATH, "phpsymbols.db");
    symbolsDBPath.Normalize();
    lookup.Open(symbolsDBPath.GetPath());
    lookup.ClearAll();
    Tester::Instance()->RunTests(); // Run all tests
#endif
    return 0;
}
