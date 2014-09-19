#include "EclipseTextThemeImporter.h"
#include "cl_standard_paths.h"
#include "globals.h"

EclipseTextThemeImporter::EclipseTextThemeImporter() { SetFileExtensions(""); }

EclipseTextThemeImporter::~EclipseTextThemeImporter() {}

bool EclipseTextThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    wxXmlNode* properties = InitializeImport(eclipseXmlFile, "text", 1);
    CHECK_PTR_RET_FALSE(properties);
    
    // Covnert to codelite's XML properties
    AddProperty(properties, "0", "Default", m_foreground.colour, m_background.colour);
    AddCommonProperties(properties);
    return FinalizeImport(properties);
}
