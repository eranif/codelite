#include <wx/ffile.h>
#include <wx/init.h>
#include <stdio.h>
#include "PHPSourceFile.h"
#include "PHPLookupTable.h"
#include "PHPExpression.h"
#include "PHPEntityFunction.h"
#include "tester.h"

#ifdef __WXMSW__
#define SYMBOLS_DB_PATH "%TEMP%"
#else
#define SYMBOLS_DB_PATH "/tmp"
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
    PHPSourceFile sourceFile(wxFileName("../Tests/test_this_operator.php"), &lookup);
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
    PHPSourceFile sourceFile(wxFileName("../Tests/test_class_extends.php"), &lookup);
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
    PHPSourceFile sourceFile(wxFileName("../Tests/test_use_alias_operator.php"), &lookup);
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
    PHPSourceFile sourceFile(wxFileName("../Tests/test_expression_parser_for_partial_word.php"), &lookup);
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
    PHPSourceFile sourceFile(wxFileName("../Tests/test_long_chain.php"), &lookup);
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

TEST_FUNC(test_parsing_abstract_class)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_parsing_abstract_class.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "test_parsing_abstract_class_impl");

    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_StartsWith | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 3);
    PrintMatches(matches);
    return true;
}

TEST_FUNC(test_abstract_class_with_self)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_abstract_class_with_self.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "test_parsing_abstract_class_impl1");

    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_StartsWith | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 4);
    PrintMatches(matches);
    return true;
}

// test word completion when inside the global namespace
// part_w + CTRL+SPACE
TEST_FUNC(test_word_completion)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_word_completion.php"), &lookup);
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

// test word completion of a local variable (or anything) after a
// casting
TEST_FUNC(test_word_completion_after_casting)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_word_completion_after_casting.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());

    CHECK_BOOL(resolved);
    CHECK_BOOL(resolved->GetShortName().IsEmpty());
    CHECK_WXSTRING(resolved->GetFullName(), "\\");
    CHECK_WXSTRING(expr.GetFilter(), "$test_word_complet");
    return true;
}

// test word completion when inside a namespace
// namespace bla;
// part_w + CTRL+SPACE
TEST_FUNC(test_word_completion_inside_ns)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_word_completion_inside_ns.php"), &lookup);
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
    PHPSourceFile sourceFile(wxFileName("../Tests/test_class_members.php"), &lookup);
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

TEST_FUNC(test_class_member_initialized)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_class_member_initialized.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "MemberClass");
    return true;
}

// test completing class members, but this time the class is defined inside a namespace
TEST_FUNC(test_class_with_members_inside_namespace)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_class_with_members_inside_namespace.php"), &lookup);
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
    PHPSourceFile sourceFile(wxFileName("../Tests/test_variable_1.php"), &lookup);
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
    PHPSourceFile sourceFile(wxFileName("../Tests/test_variable_2.php"), &lookup);
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

// test instantiating a variable from a global function
// The variable is used within a class method
TEST_FUNC(test_variable_assigned_from_function)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_variable_assigned_from_function.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "test_variable_assigned_from_function_return_value");

    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_Contains | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 1);
    PrintMatches(matches);
    return true;
}

// test instantiating a variable from a global function
// The variable is used within a global function
TEST_FUNC(test_global_variable_assigned_from_function)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_global_variable_assigned_from_function.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "test_global_variable_assigned_from_function_return_value");

    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_Contains | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 1);
    PrintMatches(matches);
    return true;
}

// test instantiating a variable from a global function
// The variable is used within a global function
TEST_FUNC(test_interface)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_interface.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "test_interface_impl");

    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_Contains | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 2);
    PrintMatches(matches);
    return true;
}

// test usage of the parent keyword
TEST_FUNC(test_parent)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_parent.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_WXSTRING(resolved->GetShortName(), "test_parent_subclass");

    PHPEntityBase::List_t matches = lookup.FindChildren(
        resolved->GetDbId(), PHPLookupTable::kLookupFlags_Contains | expr.GetLookupFlags(), expr.GetFilter());
    CHECK_SIZE(matches.size(), 2);
    PrintMatches(matches);
    return true;
}

// test code completion for local variables
TEST_FUNC(test_locals)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_locals.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 3);
    return true;
}

// test code completion for local variables
TEST_FUNC(test_word_complete_of_aliases)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_word_complete_of_aliases.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 2);
    return true;
}

// test code completion for local variables
TEST_FUNC(test_define)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_define.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 1);
    return true;
}

// test code completion for local variables
TEST_FUNC(test_define_in_namespace)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_define_in_namespace.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 1);
    return true;
}

// test code completion for local variables
TEST_FUNC(test_define_with_namespace)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_define_with_namespace.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 2);
    return true;
}

// test code completion for local variables
TEST_FUNC(test_word_completion_local_variale_1)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_word_completion_local_variale_1.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 1);
    return true;
}

// test code completion for local variables
TEST_FUNC(test_word_completion_local_variable_2)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_word_completion_local_variable_2.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 1);
    return true;
}

TEST_FUNC(test_var_assigned_from_require)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_var_assigned_from_require.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 1);
    return true;
}

TEST_FUNC(test_simple_trait)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_simple_trait.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPEntityBase::Ptr_t tr = lookup.FindClass("\\test_simple_trait");
    CHECK_BOOL(tr);
    return true;
}

TEST_FUNC(test_use_trait)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_use_trait.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 2);
    return true;
}

TEST_FUNC(test_goto_def_with_trait)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_goto_def_with_trait.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 2);
    return true;
}

TEST_FUNC(test_trait_alias)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_trait_alias.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetFullName().c_str(), "\\Aliased_Talker");

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 5);
    return true;
}

TEST_FUNC(test_partial_namespace)
{
    {
        // parse the helper file first
        PHPSourceFile sourceFile(wxFileName("../Tests/test_partial_namespace_helper.php"), &lookup);
        sourceFile.SetParseFunctionBody(false);
        sourceFile.Parse();
        lookup.UpdateSourceFile(sourceFile);
    }

    PHPSourceFile sourceFile(wxFileName("../Tests/test_partial_namespace.php"), &lookup);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 1);
    return true;
}


TEST_FUNC(test_extends_with_namespace)
{
    {
        // parse the helper file first
        PHPSourceFile sourceFile(wxFileName("../Tests/test_partial_namespace_helper.php"), &lookup);
        sourceFile.SetParseFunctionBody(false);
        sourceFile.Parse();
        lookup.UpdateSourceFile(sourceFile);
    }

    PHPSourceFile sourceFile(wxFileName("../Tests/test_extends_with_namespace.php"), &lookup);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 1);
    CHECK_WXSTRING((*matches.begin())->GetShortName(), "foo");
    return true;
}

TEST_FUNC(test_php7_function_return_value)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_php7_function_return_value.php"), &lookup);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetFullName().c_str(), "\\test_php7_function_return_value_class");
    return true;
}

TEST_FUNC(test_php7_function_arg_hinting)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_php7_function_arg_hinting.php"), &lookup);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetFullName().c_str(), "\\test_php7_function_arg_hinting_type2");

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 2);
    return true;
}

TEST_FUNC(test_constants)
{
    // Parse the test file
    PHPSourceFile sourceFile(wxFileName("../Tests/test_constants.php"), &lookup);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    // Use this epxression and check
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetFullName().c_str(), "\\MyMoodyClass");

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 3);
    return true;
}

TEST_FUNC(test_phpdoc_var_in_class)
{
    // Parse the test file
    PHPSourceFile sourceFile(wxFileName("../Tests/test_phpdoc_var_in_class.php"), &lookup);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    // Use this epxression and check
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->Type().c_str(), "\\StructB");

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 2);
    return true;
}

TEST_FUNC(test_phpdoc_property)
{
    // Parse the test file
    PHPSourceFile sourceFile(wxFileName("../Tests/test_phpdoc_property.php"), &lookup);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    // Use this epxression and check
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->Type().c_str(), "\\ClassWithProps");

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 4);
    return true;
}

TEST_FUNC(test_phpdoc_method)
{
    // Parse the test file
    PHPSourceFile sourceFile(wxFileName("../Tests/test_phpdoc_method.php"), &lookup);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    // Use this epxression and check
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->Type().c_str(), "\\ClassWithMethods");

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 4);
    return true;
}

TEST_FUNC(test_function_phpdoc)
{
    // Parse the test file
    PHPSourceFile sourceFile(wxFileName("../Tests/test_function_phpdoc.php"), &lookup);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    // Use this epxression and check
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->Type().ToUTF8(), "\\ArgType");

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);
    
    CHECK_SIZE(matches.size(), 2);
    return true;
}

TEST_FUNC(test_foreach)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_foreach.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPEntityBase::Ptr_t myKey = sourceFile.Namespace()->FindChild("$myKey");
    PHPEntityBase::Ptr_t myValue = sourceFile.Namespace()->FindChild("$myValue");
    CHECK_BOOL(myKey && myKey->GetFullName() == "$myKey");
    CHECK_BOOL(myValue && myValue->GetFullName() == "$myValue");
    return true;
}


TEST_FUNC(test_wrong_goto_interface)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_wrong_goto_interface.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    
    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);
    
    return true;
}


TEST_FUNC(test_function_arg_type_hint_and_php_doc)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_function_arg_type_hint_and_php_doc.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);
    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);
    
    CHECK_STRING(resolved->GetFullName().ToUTF8(), "\\test_function_arg_type_hint_and_php_doc");
    return true;
}

TEST_FUNC(test_func_arg_in_lambda_in_assignment)
{
    PHPSourceFile sourceFile(wxFileName("../Tests/test_func_arg_in_lambda_in_assignment.php"), &lookup);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.Parse();
    lookup.UpdateSourceFile(sourceFile);

    PHPExpression expr(sourceFile.GetText());
    PHPEntityBase::Ptr_t resolved = expr.Resolve(lookup, sourceFile.GetFilename().GetFullPath());
    CHECK_BOOL(resolved);

    PHPEntityBase::List_t matches;
    expr.Suggest(resolved, lookup, matches);

    CHECK_SIZE(matches.size(), 1);
    CHECK_WXSTRING((*matches.begin())->GetFullName(), "$lambdaArg");
    return true;
}


//======================-------------------------------------------------
// Main
//======================-------------------------------------------------

static const wxString PERFORMANCE_CODE =
    "<?php\n"
    "$app = new \\Illuminate\\Foundation\\Application();\n"
    "$app->";

int main(int argc, char** argv)
{
    wxInitialize(argc, argv);
#if 0
    PHPLookupTable table;
    table.Open("/home/eran/laravel-test/");
    for(size_t i=0; i<100; ++i) {
        PHPExpression expr(PERFORMANCE_CODE);
        PHPEntityBase::Ptr_t resolved = expr.Resolve(table, "buildin.php");
        if(resolved) {
            PHPEntityBase::List_t matches;
            expr.Suggest(resolved, table, matches);
        }
    }
#else
    {
        wxFileName symbolsDBPath(SYMBOLS_DB_PATH, "phpsymbols.db");
        symbolsDBPath.Normalize();
        lookup.Open(symbolsDBPath.GetPath());
        lookup.ClearAll();
        Tester::Instance()->RunTests(); // Run all tests
    }
#endif
    wxUninitialize();
    return 0;
}
