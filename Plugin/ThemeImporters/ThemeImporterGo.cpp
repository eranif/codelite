#include "ThemeImporterGo.hpp"

ThemeImporterGo::ThemeImporterGo()
{
    // Primary keywords and identifiers
    SetKeywords0(
        "bool byte break case chan complex64 complex128 const continue defer default "
        "else  for func go goto fallthrough false float32 float64 if import int interface int8 int16 int32 int64 "
        "len map nil package range return select string struct switch true type "
        "uint uintptr uint8 uint16 uint32 uint64 var");

    // Secondary keywords and identifiers
    SetFunctionsWordSetIndex(1);
    SetClassWordSetIndex(3);
    SetOthersWordSetIndex(4);
    SetLocalsWordSetIndex(LexerConf::WS_VARIABLES, true);

    // Special task markers
    // will be styled with SCE_C_TASKMARKER
    SetKeywords5("TODO FIXME BUG ATTN");
    SetLangName("go");
    SetFileExtensions("*.go");
}

ThemeImporterGo::~ThemeImporterGo() {}

LexerConf::Ptr_t ThemeImporterGo::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_CPP);
    CHECK_PTR_RET_NULL(lexer);

    // Covnert to codelite's XML properties
    AddProperty(lexer, wxSTC_C_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_C_COMMENT, "Multi line comment", m_multiLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTLINE, "Single line comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTDOC, "Doc comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_C_WORD, "Keyword", m_keyword);
    AddProperty(lexer, wxSTC_C_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_C_STRINGRAW, "String Raw", m_string);
    AddProperty(lexer, wxSTC_C_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_C_UUID, "Uuid", m_number);
    AddProperty(lexer, wxSTC_C_PREPROCESSOR, "Preprocessor", m_editor);
    AddProperty(lexer, wxSTC_C_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_C_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_C_STRINGEOL, "Open String", m_string);
    AddProperty(lexer, wxSTC_C_COMMENTLINEDOC, "Doc snigle line comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORD, "Doc keyword", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORDERROR, "Doc keyword error", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_WORD2, "Function", m_function);
    AddProperty(lexer, wxSTC_C_GLOBALCLASS, "Class", m_klass);
    AddPropertySubstyle(lexer, LexerConf::WS_VARIABLES, "Variable", m_variable);

    // the base for all our substyles
    lexer->SetSubstyleBase(wxSTC_C_IDENTIFIER);

    FinalizeImport(lexer);
    return lexer;
}
