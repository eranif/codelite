// rust support was added in wx3.1
#include "wx/versioninfo.h"
#if wxCHECK_VERSION(3, 1, 0)
#include "ThemeImporterRust.hpp"

ThemeImporterRust::ThemeImporterRust()
{
    SetKeywords0("as break const continue crate else enum extern false fn for if impl in let loop match mod move mut "
                 "pub ref return self Self static struct super trait true type unsafe use where while async await dyn "
                 "abstract become box do final macro override priv typeof unsized virtual yield try union");

    // built in types (std::collection)
    SetKeywords1("vec String str array Vec VecDeque LinkedList HashMap BTreeMap HashSet BTreeSet BinaryHeap");
    SetFileExtensions("*.rs");
    m_langName = "rust";
    m_classesIndex = 3;
    m_localsIndex = 4;
    m_functionsIndex = 5;
}

ThemeImporterRust::~ThemeImporterRust() {}

LexerConf::Ptr_t ThemeImporterRust::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_RUST);
    AddProperty(lexer, wxSTC_RUST_DEFAULT, "Default", m_editor);

    wxString errorColour;
    if(lexer->IsDark()) {
        errorColour = "PINK";
    } else {
        errorColour = "RED";
    }

    AddProperty(lexer, wxSTC_RUST_LEXERROR, "Lex error", errorColour, m_editor.bg_colour);
    AddProperty(lexer, wxSTC_RUST_COMMENTBLOCK, "Comment block", m_multiLineComment);
    AddProperty(lexer, wxSTC_RUST_COMMENTLINE, "Comment line", m_singleLineComment);
    AddProperty(lexer, wxSTC_RUST_COMMENTBLOCKDOC, "Comment block doc", m_javadoc);
    AddProperty(lexer, wxSTC_RUST_COMMENTLINEDOC, "Comment line doc", m_javadoc);
    AddProperty(lexer, wxSTC_RUST_WORD, "Keywords", m_keyword);
    AddProperty(lexer, wxSTC_RUST_WORD2, "Built in types", m_klass);
    AddProperty(lexer, wxSTC_RUST_WORD3, "Other keywords", m_keyword);
    AddProperty(lexer, wxSTC_RUST_WORD4, "Struct", m_klass);
    AddProperty(lexer, wxSTC_RUST_WORD5, "Variables", m_variable);
    AddProperty(lexer, wxSTC_RUST_WORD6, "Functions", m_function);
    AddProperty(lexer, wxSTC_RUST_WORD7, "Word List 7", m_editor);
    AddProperty(lexer, wxSTC_RUST_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_RUST_STRINGR, "String raw", m_string);
    AddProperty(lexer, wxSTC_RUST_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_RUST_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_RUST_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_RUST_LIFETIME, "Lifetime", m_editor);
    AddProperty(lexer, wxSTC_RUST_MACRO, "Macro", m_klass);
    AddProperty(lexer, wxSTC_RUST_BYTESTRING, "Byte string", m_string);
    AddProperty(lexer, wxSTC_RUST_BYTESTRINGR, "Byte string raw", m_string);
    AddProperty(lexer, wxSTC_RUST_BYTECHARACTER, "Byte character", m_string);
    FinalizeImport(lexer);
    return lexer;
}
#endif
