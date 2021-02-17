// rust support was added in wx3.1
#include "wx/versioninfo.h"
#if wxCHECK_VERSION(3, 1, 0)
#include "EclipseRustThemeImporter.h"

EclipseRustThemeImporter::EclipseRustThemeImporter()
{
    SetKeywords0("as break const continue crate else enum extern false fn for if impl in let loop match mod move mut "
                 "pub ref return self Self static struct super trait true type unsafe use where while async await dyn "
                 "abstract become box do final macro override priv typeof unsized virtual yield try union");
    SetFileExtensions("*.rs");
    m_langName = "rust";
}

EclipseRustThemeImporter::~EclipseRustThemeImporter() {}

LexerConf::Ptr_t EclipseRustThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_RUST);
    AddProperty(lexer, wxSTC_RUST_DEFAULT, "Default", m_foreground.colour, m_background.colour);

    wxString errorColour;
    if(lexer->IsDark()) {
        errorColour = "PINK";
    } else {
        errorColour = "RED";
    }

    AddProperty(lexer, wxSTC_RUST_LEXERROR, "Lex error", errorColour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_COMMENTBLOCK, "Comment block", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_COMMENTLINE, "Comment line", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_COMMENTBLOCKDOC, "Comment block doc", m_javadoc.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_COMMENTLINEDOC, "Comment line doc", m_javadoc.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_WORD, "Word", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_WORD2, "Word2", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_WORD3, "Word3", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_WORD4, "Word4", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_WORD5, "Word5", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_WORD6, "Word6", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_WORD7, "Word7", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_STRING, "String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_STRINGR, "String raw", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_CHARACTER, "Character", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_OPERATOR, "Operator", m_oper.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_IDENTIFIER, "Identifier", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_LIFETIME, "Lifetime", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_MACRO, "Macro", wxColour(m_klass.colour).GetAsString(wxC2S_HTML_SYNTAX),
                m_background.colour);
    AddProperty(lexer, wxSTC_RUST_BYTESTRING, "Byte string", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_BYTESTRINGR, "Byte string raw", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_RUST_BYTECHARACTER, "Byte character", m_string.colour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
#endif
