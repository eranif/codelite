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
    if(!Load(eclipseXmlFile)) return false;
    
    wxXmlDocument codeliteXML;
    wxXmlNode* lexer = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Lexer");
    codeliteXML.SetRoot(lexer);
    
    // Add the lexer basic properties (laguage, file extensions, keywords, name)
    AddBaseProperties(lexer, "makefile", "11");
    
    wxXmlNode* properties = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Properties");
    codeliteXML.GetRoot()->AddChild(properties);
    
    Property background;
    Property foreground;
    Property lineNumber;
    Property selectionBackground;
    Property selectionForeground;
    Property singleLineComment;
    Property multiLineComment;
    Property number;
    Property string;
    Property oper;
    Property keyword;
    Property klass;
    Property variable;

    // Read the basic properties
    if(!GetProperty("background", background)) return false;
    if(!GetProperty("foreground", foreground)) return false;
    if(!GetProperty("selectionForeground", selectionForeground)) return false;
    if(!GetProperty("selectionBackground", selectionBackground)) return false;
    if(!GetProperty("lineNumber", lineNumber)) return false;
    if(!GetProperty("singleLineComment", singleLineComment)) return false;
    if(!GetProperty("multiLineComment", multiLineComment)) return false;
    if(!GetProperty("number", number)) return false;
    if(!GetProperty("string", string)) return false;
    if(!GetProperty("operator", oper)) return false;
    if(!GetProperty("keyword", keyword)) return false;
    if(!GetProperty("class", klass)) {
        klass = foreground;
    }
    if(!GetProperty("localVariable", variable)) {
        variable = foreground;
    }
    AddProperty(properties, "0", "Default", foreground.colour, background.colour);
    AddProperty(properties, "1", "Comment", singleLineComment.colour, background.colour);
    AddProperty(properties, "2", "Preprocessor", keyword.colour, background.colour);
    AddProperty(properties, "3", "Identifier", variable.colour, background.colour);
    AddProperty(properties, "4", "Operator", oper.colour, background.colour);
    AddProperty(properties, "5", "Target", klass.colour, background.colour);
    AddProperty(properties, "9", "ID End Of Line", foreground.colour, background.colour);
    
    AddCommonProperties(properties);
    wxString codeliteXmlFile =
        wxFileName(clStandardPaths::Get().GetUserLexersDir(), GetOutputFile("makefile")).GetFullPath();
    return ::SaveXmlToFile(&codeliteXML, codeliteXmlFile);
}
