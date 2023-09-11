#include "ThemeImporterMakefile.hpp"

#include "cl_standard_paths.h"
#include "globals.h"

ThemeImporterMakefile::ThemeImporterMakefile() { SetFileExtensions("*.mk;makefile;Makefile;*.mak;*.make"); }

ThemeImporterMakefile::~ThemeImporterMakefile() {}

LexerConf::Ptr_t ThemeImporterMakefile::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "makefile", wxSTC_LEX_MAKEFILE);
    CHECK_PTR_RET_NULL(lexer);

    AddProperty(lexer, wxSTC_MAKE_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_MAKE_COMMENT, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_MAKE_PREPROCESSOR, "Preprocessor", m_keyword);
    AddProperty(lexer, wxSTC_MAKE_IDENTIFIER, "Identifier", m_variable);
    AddProperty(lexer, wxSTC_MAKE_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_MAKE_TARGET, "Target", m_klass);
    AddProperty(lexer, wxSTC_MAKE_IDEOL, "ID End Of Line", m_editor);

    FinalizeImport(lexer);
    return lexer;
}
