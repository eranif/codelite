#include "ThemeImporterMakefile.hpp"

#include "cl_standard_paths.h"
#include "globals.h"

ThemeImporterMakefile::ThemeImporterMakefile() { SetFileExtensions("*.mk;makefile;Makefile;*.mak;*.make"); }

ThemeImporterMakefile::~ThemeImporterMakefile() {}

LexerConf::Ptr_t ThemeImporterMakefile::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "makefile", 11);
    CHECK_PTR_RET_NULL(lexer);

    AddProperty(lexer, "0", "Default", m_editor);
    AddProperty(lexer, "1", "Comment", m_singleLineComment);
    AddProperty(lexer, "2", "Preprocessor", m_keyword);
    AddProperty(lexer, "3", "Identifier", m_variable);
    AddProperty(lexer, "4", "Operator", m_oper);
    AddProperty(lexer, "5", "Target", m_klass);
    AddProperty(lexer, "9", "ID End Of Line", m_editor);

    FinalizeImport(lexer);
    return lexer;
}
