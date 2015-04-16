#include "EclipseMakefileThemeImporter.h"
#include "cl_standard_paths.h"
#include "globals.h"

EclipseMakefileThemeImporter::EclipseMakefileThemeImporter()
{
    SetFileExtensions("*.mk;makefile;Makefile;*.mak;*.make");
}

EclipseMakefileThemeImporter::~EclipseMakefileThemeImporter() {}

LexerConf::Ptr_t EclipseMakefileThemeImporter::Import(const wxFileName& eclipseXmlFile)
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
