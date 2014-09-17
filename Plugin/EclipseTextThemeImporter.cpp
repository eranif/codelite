#include "EclipseTextThemeImporter.h"
#include "cl_standard_paths.h"
#include "globals.h"

EclipseTextThemeImporter::EclipseTextThemeImporter() { SetFileExtensions(""); }

EclipseTextThemeImporter::~EclipseTextThemeImporter() {}

bool EclipseTextThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    if(!Load(eclipseXmlFile)) return false;

    wxXmlDocument codeliteXML;
    wxXmlNode* lexer = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Lexer");
    codeliteXML.SetRoot(lexer);

    // Add the lexer basic properties (laguage, file extensions, keywords, name)
    AddBaseProperties(lexer, "text", "1");

    wxXmlNode* properties = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Properties");
    codeliteXML.GetRoot()->AddChild(properties);

    Property background;
    Property foreground;
    Property lineNumber;
    Property selectionBackground;
    Property selectionForeground;

    // Read the basic properties
    if(!GetProperty("background", background)) return false;
    if(!GetProperty("foreground", foreground)) return false;
    if(!GetProperty("selectionForeground", selectionForeground)) return false;
    if(!GetProperty("selectionBackground", selectionBackground)) return false;
    if(!GetProperty("lineNumber", lineNumber)) return false;

    // Covnert to codelite's XML properties
    AddProperty(properties, "0", "Default", foreground.colour, background.colour);
    AddCommonProperties(properties);
    wxString codeliteXmlFile =
        wxFileName(clStandardPaths::Get().GetUserLexersDir(), GetOutputFile("text")).GetFullPath();
    return ::SaveXmlToFile(&codeliteXML, codeliteXmlFile);
}
