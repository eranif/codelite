#include "EclipseMakefileThemeImporter.h"
#include "cl_standard_paths.h"
#include "globals.h"

EclipseMakefileThemeImporter::EclipseMakefileThemeImporter()
{
    SetFileExtensions("*.mk;makefile;Makefile;*.mak;*.make");
}

EclipseMakefileThemeImporter::~EclipseMakefileThemeImporter() {}

bool EclipseMakefileThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    wxXmlNode* properties = InitializeImport(eclipseXmlFile, "makefile", 11);
    CHECK_PTR_RET_FALSE(properties);
    
    AddProperty(properties, "0", "Default", m_foreground.colour, m_background.colour);
    AddProperty(properties, "1", "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(properties, "2", "Preprocessor", m_keyword.colour, m_background.colour);
    AddProperty(properties, "3", "Identifier", m_variable.colour, m_background.colour);
    AddProperty(properties, "4", "Operator", m_oper.colour, m_background.colour);
    AddProperty(properties, "5", "Target", m_klass.colour, m_background.colour);
    AddProperty(properties, "9", "ID End Of Line", m_foreground.colour, m_background.colour);
    
    return FinalizeImport(properties);
}
