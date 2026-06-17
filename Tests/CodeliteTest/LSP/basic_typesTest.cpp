#include "LSP/basic_types.h"

#include <doctest.h>

TEST_CASE("LSP::Position")
{
    const LSP::Position expected = {1, 2};
    LSP::Position actual;

    REQUIRE(actual != expected);

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::Range")
{
    const LSP::Range expected = {{1, 2}, {3, 4}};
    LSP::Range actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetStart() != expected.GetStart());
    REQUIRE(actual.GetEnd() != expected.GetEnd());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::TextDocumentContentChangeEvent")
{
    LSP::TextDocumentContentChangeEvent expected;
    expected.SetRange({{1, 2}, {3, 4}});
    expected.SetText(wxString::FromUTF8("\u4321"));
    LSP::TextDocumentContentChangeEvent actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetRange() != expected.GetRange());
    REQUIRE(actual.GetText() != expected.GetText());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::TextDocumentIdentifier")
{
    const LSP::TextDocumentIdentifier expected(wxString::FromUTF8("/absolute/path\u4321"));
    LSP::TextDocumentIdentifier actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetPath() != expected.GetPath());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::VersionedTextDocumentIdentifier")
{
    LSP::VersionedTextDocumentIdentifier expected;
    expected.SetFilename(wxString::FromUTF8("/absolute/path\u4321"));
    expected.SetVersion(42);
    LSP::VersionedTextDocumentIdentifier actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetPath() != expected.GetPath());
    REQUIRE(actual.GetVersion() != expected.GetVersion());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::TextEdit")
{
    LSP::TextEdit expected;
    expected.SetNewText(wxString::FromUTF8("\u4321"));
    expected.SetRange({{1, 2}, {3, 4}});
    LSP::TextEdit actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetNewText() != expected.GetNewText());
    REQUIRE(actual.GetRange() != expected.GetRange());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::Location")
{
    LSP::Location expected;
    expected.SetName(wxString::FromUTF8("\u4321"));
    expected.SetPath(wxString::FromUTF8("/absolute/path\u4321"));
    expected.SetPattern(wxString::FromUTF8("\u1324"));
    expected.SetRange({{1, 2}, {3, 4}});
    LSP::Location actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetName() != expected.GetName());
    REQUIRE(actual.GetPath() != expected.GetPath());
    REQUIRE(actual.GetPattern() != expected.GetPattern());
    REQUIRE(actual.GetRange() != expected.GetRange());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::TextDocumentItem")
{
    LSP::TextDocumentItem expected;
    expected.SetLanguageId(wxString::FromUTF8("\u4321"));
    expected.SetText(wxString::FromUTF8("\u5432"));
    expected.SetUri(wxString::FromUTF8("/absolute/path\u4321"));
    expected.SetVersion(42);
    LSP::TextDocumentItem actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetLanguageId() != expected.GetLanguageId());
    REQUIRE(actual.GetPath() != expected.GetPath());
    REQUIRE(actual.GetText() != expected.GetText());
    REQUIRE(actual.GetVersion() != expected.GetVersion());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::ParameterInformation")
{
    LSP::ParameterInformation expected;
    expected.SetDocumentation(wxString::FromUTF8("\u4321"));
    expected.SetLabel(wxString::FromUTF8("\u5432"));
    LSP::ParameterInformation actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetDocumentation() != expected.GetDocumentation());
    REQUIRE(actual.GetLabel() != expected.GetLabel());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::SignatureInformation")
{
    LSP::SignatureInformation expected;
    expected.SetDocumentation(wxString::FromUTF8("\u4321"));
    expected.SetLabel(wxString::FromUTF8("\u5432"));
    expected.SetParameters({LSP::ParameterInformation{}.SetDocumentation("doc").SetLabel("label")});
    LSP::SignatureInformation actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetDocumentation() != expected.GetDocumentation());
    REQUIRE(actual.GetLabel() != expected.GetLabel());
    REQUIRE(actual.GetParameters() != expected.GetParameters());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::SignatureHelp")
{
    LSP::SignatureHelp expected;
    expected.SetActiveParameter(1);
    expected.SetActiveSignature(2);
    expected.SetSignatures(
        {LSP::SignatureInformation()
             .SetDocumentation("SigDoc")
             .SetLabel("SigLabel")
             .SetParameters({LSP::ParameterInformation{}.SetDocumentation("doc").SetLabel("label")})});
    LSP::SignatureHelp actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetActiveParameter() != expected.GetActiveParameter());
    REQUIRE(actual.GetActiveSignature() != expected.GetActiveSignature());
    REQUIRE(actual.GetSignatures() != expected.GetSignatures());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::MarkupContent")
{
    LSP::MarkupContent expected;
    expected.SetKind(wxString::FromUTF8("\u4321"));
    expected.SetValue(wxString::FromUTF8("\u5432"));
    LSP::MarkupContent actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetKind() != expected.GetKind());
    REQUIRE(actual.GetValue() != expected.GetValue());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::Hover")
{
    LSP::Hover expected;
    expected.SetContents(LSP::MarkupContent{}.SetKind("king").SetValue("value"));
    expected.SetRange({{1, 2}, {3, 4}});
    LSP::Hover actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetContents() != expected.GetContents());
    REQUIRE(actual.GetRange() != expected.GetRange());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::Diagnostic")
{
    LSP::Diagnostic expected;
    expected.SetMessage(wxString::FromUTF8("\u4321"));
    expected.SetRange({{1, 2}, {3, 4}});
    expected.SetSeverity(LSP::DiagnosticSeverity::Hint);
    LSP::Diagnostic actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetMessage() != expected.GetMessage());
    REQUIRE(actual.GetRange() != expected.GetRange());
    REQUIRE(actual.GetSeverity() != expected.GetSeverity());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

#if 0 // Unimplemented Command::ToJSON()
TEST_CASE("LSP::Command")
{
    LSP::Command expected;
    expected.SetArguments(wxString::FromUTF8("\u4321"));
    expected.SetCommand(wxString::FromUTF8("\u5432"));
    expected.SetTitle(wxString::FromUTF8("\u1324"));
    LSP::Command actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetArguments() != expected.GetArguments());
    REQUIRE(actual.GetCommand() != expected.GetCommand());
    REQUIRE(actual.GetTitle() != expected.GetTitle());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}
#endif
#if 0 // Unimplemented LSP::DocumentSymbol::ToJSON()
TEST_CASE("LSP::DocumentSymbol")
{
    LSP::DocumentSymbol child;
    child.SetDetail("Detail");
    child.SetKind(LSP::eSymbolKind::kSK_Field);
    child.SetName("name");
    child.SetRange({{11, 22}, {81, 91}});
    child.SetSelectionRange({{32, 42}, {62, 72}});

    LSP::DocumentSymbol expected;
    expected.SetChildren({child});
    expected.SetDetail(wxString::FromUTF8("\u4321"));
    expected.SetKind(LSP::eSymbolKind::kSK_Constructor);
    expected.SetName(wxString::FromUTF8("\u1324"));
    expected.SetRange({{1, 2}, {8, 9}});
    expected.SetSelectionRange({{3, 4}, {6, 7}});
    LSP::DocumentSymbol actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetChildren() != expected.GetChildren());
    REQUIRE(actual.GetDetail() != expected.GetDetail());
    REQUIRE(actual.GetKind() != expected.GetKind());
    REQUIRE(actual.GetRange() != expected.GetRange());
    REQUIRE(actual.GetSelectionRange() != expected.GetSelectionRange());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}
#endif

TEST_CASE("LSP::SymbolInformation")
{
    LSP::SymbolInformation expected;
    expected.SetContainerName(wxString::FromUTF8("\u4321"));
    expected.SetKind(LSP::eSymbolKind::kSK_Constructor);
    expected.SetLocation(LSP::Location{}.SetPath("/my/path/").SetRange({{1, 2}, {3, 4}}));
    expected.SetName(wxString::FromUTF8("\u1324"));
    LSP::SymbolInformation actual;

    REQUIRE(actual != expected);
    REQUIRE(actual.GetContainerName() != expected.GetContainerName());
    REQUIRE(actual.GetKind() != expected.GetKind());
    REQUIRE(actual.GetLocation() != expected.GetLocation());
    REQUIRE(actual.GetName() != expected.GetName());

    actual.FromJSON(expected.ToJSON());

    CHECK(actual == expected);
}

TEST_CASE("LSP::Progress")
{
    const auto expected_kind = LSP::ProgressKind::report;
    const auto expected_kind_string = "report";
    const auto expected_message = "\u4321";
    const double expected_percentage = 4.2;
    const auto expected_token = "\u5432";
    const nlohmann::json json = {
        {"params",
         {{"token", expected_token},
          {"value",
           {{"kind", expected_kind_string}, {"message", expected_message}, {"percentage", expected_percentage}}}}}};

    const LSP::Progress actual = LSP::Progress::FromJSON(json).value_or(LSP::Progress{});

    CHECK(actual.m_kind == expected_kind);
    CHECK(actual.m_message == wxString::FromUTF8(expected_message));
    CHECK(actual.m_percentage == expected_percentage);
    CHECK(actual.m_token == wxString::FromUTF8(expected_token));
}
