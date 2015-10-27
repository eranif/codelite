#include "EclipseTextThemeImporter.h"
#include "cl_standard_paths.h"
#include "globals.h"

EclipseTextThemeImporter::EclipseTextThemeImporter() { SetFileExtensions("*.txt;*.log"); }

EclipseTextThemeImporter::~EclipseTextThemeImporter() {}

LexerConf::Ptr_t EclipseTextThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "text", 1);
    CHECK_PTR_RET_NULL(lexer);

    AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
    AddCommonProperties(lexer);
    FinalizeImport(lexer);
    return lexer;
}
