#include "ThemeImporterMakefile.hpp"

#include "cl_standard_paths.h"
#include "globals.h"

ThemeImporterMakefile::ThemeImporterMakefile()
{
    SetFileExtensions("*.mk;makefile;Makefile;*.mak;*.make");
}

ThemeImporterMakefile::~ThemeImporterMakefile() {}

LexerConf::Ptr_t ThemeImporterMakefile::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "makefile", 11);
    CHECK_PTR_RET_NULL(lexer);

    AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "1", "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, "2", "Preprocessor", m_keyword.colour, m_background.colour);
    AddProperty(lexer, "3", "Identifier", m_variable.colour, m_background.colour);
    AddProperty(lexer, "4", "Operator", m_oper.colour, m_background.colour);
    AddProperty(lexer, "5", "Target", m_klass.colour, m_background.colour);
    AddProperty(lexer, "9", "ID End Of Line", m_foreground.colour, m_background.colour);

    FinalizeImport(lexer);
    return lexer;
}
