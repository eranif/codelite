#include "LSP/json_rpc_params.h"

#include <doctest.h>

TEST_CASE("LSP::TextDocumentPositionParams")
{
    LSP::TextDocumentPositionParams expected;
    LSP::TextDocumentPositionParams actual;
    expected.SetPosition({1, 2});
    expected.SetTextDocument(LSP::TextDocumentIdentifier("/path/filename"));

    REQUIRE(actual != expected);
    REQUIRE(actual.GetPosition() != expected.GetPosition());
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

#if 0 // Unimplemented LSP::RenameParams::FromJSON()
TEST_CASE("LSP::RenameParams")
{
    LSP::RenameParams expected;
    LSP::RenameParams actual;
    expected.SetNewName(wxString::FromUTF8("\u4321"));
    expected.SetPosition({1, 2});
    expected.SetTextDocument(LSP::TextDocumentIdentifier("/path/filename"));

    REQUIRE(actual != expected);
    REQUIRE(actual.GetNewName() != expected.GetNewName());
    REQUIRE(actual.GetPosition() != expected.GetPosition());
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());

    actual.(expected.ToJSON());

    CHECK(actual == expected);
}
#endif

TEST_CASE("LSP::ReferenceParams")
{
    LSP::ReferenceParams expected(false);
    LSP::ReferenceParams actual(true);
    expected.SetIncludeDeclaration(false);
    expected.SetPosition({1, 2});
    expected.SetTextDocument(LSP::TextDocumentIdentifier("/path/filename"));

    REQUIRE(actual != expected);
    REQUIRE(actual.GetPosition() != expected.GetPosition());
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());
    REQUIRE(actual.IsIncludeDeclaration() != expected.IsIncludeDeclaration());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::SemanticTokensParams")
{
    LSP::SemanticTokensParams expected;
    LSP::SemanticTokensParams actual;
    expected.SetTextDocument(LSP::TextDocumentIdentifier("/path/filename"));

    REQUIRE(actual != expected);
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::DocumentSymbolParams")
{
    LSP::DocumentSymbolParams expected;
    LSP::DocumentSymbolParams actual;
    expected.SetTextDocument(LSP::TextDocumentIdentifier("/path/filename"));

    REQUIRE(actual != expected);
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::CompletionParams")
{
    LSP::CompletionParams expected;
    LSP::CompletionParams actual;
    expected.SetPosition({1, 2});
    expected.SetTextDocument(LSP::TextDocumentIdentifier("/path/filename"));

    REQUIRE(actual != expected);
    REQUIRE(actual.GetPosition() != expected.GetPosition());
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

#if 0 // Unimplemented LSP::ExecuteCommandParams::FromJSON()
TEST_CASE("LSP::ExecuteCommandParams")
{
    const LSP::ExecuteCommandParams expected(wxString::FromUTF8("command\u4321"), wxString::FromUTF8("arg\u4321"));
    LSP::ExecuteCommandParams actual("", "");

    REQUIRE(actual != expected);
    // No getters

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}
#endif
#if 0 // Unimplemented LSP::CodeActionParams::FromJSON()
TEST_CASE("LSP::CodeActionParams")
{
    LSP::CodeActionParams expected;
    LSP::CodeActionParams actual;
    expected.SetDiagnostics({LSP::Diagnostic{}
                                 .SetMessage("message")
                                 .SetRange({{1, 2}, {3, 4}})});
    expected.SetRange({{2, 4}, {3, 5}});
    expected.SetTextDocument(LSP::TextDocumentIdentifier{"filename"});
    REQUIRE(actual != expected);
    REQUIRE(actual.GetDiagnostics() != expected.GetDiagnostics());
    REQUIRE(actual.GetRange() != expected.GetRange());
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}
#endif
TEST_CASE("LSP::DidOpenTextDocumentParams")
{
    LSP::DidOpenTextDocumentParams expected;
    LSP::DidOpenTextDocumentParams actual;
    expected.SetTextDocument(LSP::TextDocumentItem{"/uri", "langId", "text", 42});
    REQUIRE(actual != expected);
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::DidCloseTextDocumentParams")
{
    LSP::DidCloseTextDocumentParams expected;
    LSP::DidCloseTextDocumentParams actual;
    expected.SetTextDocument(LSP::TextDocumentIdentifier{"/path/filename"});
    REQUIRE(actual != expected);
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::DidChangeTextDocumentParams")
{
    LSP::DidChangeTextDocumentParams expected;
    LSP::DidChangeTextDocumentParams actual;
    expected.SetContentChanges({LSP::TextDocumentContentChangeEvent{}.SetText("text")});
    expected.SetTextDocument(static_cast<LSP::VersionedTextDocumentIdentifier&>(
        LSP::VersionedTextDocumentIdentifier{42}.SetFilename("/path/filename")));

    REQUIRE(actual != expected);
    REQUIRE(actual.GetContentChanges() != expected.GetContentChanges());
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::DidSaveTextDocumentParams")
{
    LSP::DidSaveTextDocumentParams expected;
    LSP::DidSaveTextDocumentParams actual;
    expected.SetText(wxString::FromUTF8("text\u4321"));
    expected.SetTextDocument(LSP::TextDocumentIdentifier{"/path/filename"});

    REQUIRE(actual != expected);
    REQUIRE(actual.GetText() != expected.GetText());
    REQUIRE(actual.GetTextDocument() != expected.GetTextDocument());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}
