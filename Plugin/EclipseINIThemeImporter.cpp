#include "EclipseINIThemeImporter.h"

EclipseINIThemeImporter::EclipseINIThemeImporter() 
{
    SetFileExtensions("*.ini;*.properties;*.desktop");
}

EclipseINIThemeImporter::~EclipseINIThemeImporter() {}

LexerConf::Ptr_t EclipseINIThemeImporter::Import(const wxFileName& eclipseXmlFile) 
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "properties", wxSTC_LEX_PROPERTIES);
    CHECK_PTR_RET_NULL(lexer);
    
    AddProperty(lexer, wxSTC_PROPS_DEFAULT, "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_PROPS_COMMENT, "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_PROPS_SECTION, "Section", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_PROPS_ASSIGNMENT, "Assignment", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_PROPS_DEFVAL, "Default Value", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_PROPS_KEY, "Key", m_variable.colour, m_background.colour);
    
    FinalizeImport(lexer);
    return lexer;
}
