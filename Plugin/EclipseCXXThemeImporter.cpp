#include "EclipseCXXThemeImporter.h"
#include "globals.h"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include "xmlutils.h"

EclipseCXXThemeImporter::EclipseCXXThemeImporter()
{
    SetKeywords0("and and_eq asm auto bitand bitor bool break case catch char class compl const const_cast "
                 "continue default delete do double dynamic_cast else enum explicit export extern false "
                 "float for friend goto if inline int long mutable namespace new not not_eq operator or "
                 "or_eq private protected public register reinterpret_cast return short signed sizeof "
                 "static static_cast struct switch template this throw true try typedef typeid typename "
                 "union unsigned using virtual void volatile wchar_t while xor xor_eq alignas alignof "
                 "char16_t char32_t constexpr decltype noexcept nullptr static_assert thread_local");

    SetKeywords1("abstract boolean break byte case catch char class const continue debugger default delete "
                 "do double else enum export extends final finally float for function goto if implements "
                 "import in instanceof int interface long native new package private protected public "
                 "return short static super switch synchronized this throw throws transient try typeof var "
                 "void volatile while with");

    SetKeywords2("a addindex addtogroup anchor arg attention author b brief bug c callgraph callergraph category class "
                 "code "
                 "cond copybrief copydetails copydoc 	date def defgroup deprecated details dir  dontinclude dot "
                 "dotfile e else elseif em endcode endcond enddot endhtmlonly endif endlatexonly endlink endmanonly "
                 "endmsc "
                 "endverbatim 	endxmlonly  enum example exception extends  file fn headerfile hideinitializer "
                 "htmlinclude htmlonly if ifnot image implements include includelineno ingroup internal invariant "
                 "	"
                 "interface  latexonly li line link mainpage manonly memberof msc n name namespace nosubgrouping note "
                 "overload p package page par paragraph param post pre private privatesection property 	protected  "
                 "protectedsection protocol public publicsection ref relates relatesalso remarks return retval sa "
                 "section "
                 "see showinitializer since skip skipline struct 	subpage  subsection subsubsection test throw "
                 "todo "
                 "tparam typedef union until var verbatim verbinclude version warning weakgroup xmlonly xrefitem");
    SetFileExtensions("*.cxx;*.hpp;*.cc;*.h;*.c;*.cpp;*.l;*.y;*.c++;*.hh;*.js;*.javascript;*.ipp;*.hxx;*.h++");
}

EclipseCXXThemeImporter::~EclipseCXXThemeImporter() {}

bool EclipseCXXThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    if(!Load(eclipseXmlFile)) return false;

    wxXmlDocument codeliteXML;
    wxXmlNode* lexer = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Lexer");
    codeliteXML.SetRoot(lexer);

    // Add the lexer basic properties (laguage, file extensions, keywords, name)
    AddBaseProperties(lexer, "c++", "3");

    wxXmlNode* properties = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Properties");
    codeliteXML.GetRoot()->AddChild(properties);

    Property background;
    Property foreground;
    Property lineNumber;
    Property selectionBackground;
    Property selectionForeground;
    Property singleLineComment;
    Property multiLineComment;
    Property javadoc;
    Property javadocKeyword;
    Property klass;
    Property variable;
    Property number;
    Property string;
    Property oper;
    Property keyword;

    // Read the basic properties
    if(!GetProperty("background", background)) return false;
    if(!GetProperty("foreground", foreground)) return false;
    if(!GetProperty("lineNumber", lineNumber)) return false;
    if(!GetProperty("selectionForeground", selectionForeground)) return false;
    if(!GetProperty("selectionBackground", selectionBackground)) return false;
    if(!GetProperty("singleLineComment", singleLineComment)) return false;
    if(!GetProperty("multiLineComment", multiLineComment)) return false;
    if(!GetProperty("number", number)) return false;
    if(!GetProperty("string", string)) return false;
    if(!GetProperty("operator", oper)) return false;
    if(!GetProperty("keyword", keyword)) return false;

    // Optionals
    if(!GetProperty("javadoc", javadoc)) {
        javadoc = multiLineComment;
    }

    if(!GetProperty("javadocKeyword", javadocKeyword)) {
        javadocKeyword = multiLineComment;
    }
    if(!GetProperty("class", klass)) {
        klass = foreground;
    }
    if(!GetProperty("localVariable", variable)) {
        variable = foreground;
    }

    // Covnert to codelite's XML properties
    AddProperty(properties, "0", "Default", foreground.colour, background.colour);
    AddProperty(properties,
                "1",
                "Common C style comment",
                multiLineComment.colour,
                background.colour,
                multiLineComment.isBold,
                multiLineComment.isItalic);
    AddProperty(properties,
                "2",
                "Common C++ style comment",
                singleLineComment.colour,
                background.colour,
                singleLineComment.isBold,
                singleLineComment.isItalic);
    AddProperty(properties,
                "3",
                "Doxygen C style comment",
                javadoc.colour,
                background.colour,
                javadoc.isBold,
                javadoc.isItalic);
    AddProperty(properties, "4", "Number", number.colour, background.colour, number.isBold, number.isItalic);
    AddProperty(properties, "5", "C++ keyword", keyword.colour, background.colour, keyword.isBold, keyword.isItalic);
    AddProperty(properties, "6", "String", string.colour, background.colour, string.isBold, string.isItalic);
    AddProperty(properties, "7", "Character", string.colour, background.colour, string.isBold, string.isItalic);
    AddProperty(properties, "8", "Uuid", number.colour, background.colour, number.isBold, number.isItalic);
    AddProperty(properties, "9", "Preprocessor", foreground.colour, background.colour);
    AddProperty(properties, "10", "Operator", foreground.colour, background.colour);
    AddProperty(properties, "11", "Identifier", foreground.colour, background.colour);
    AddProperty(properties, "12", "Open string", string.colour, background.colour, string.isBold, string.isItalic);
    AddProperty(properties,
                "15",
                "Doxygen C++ style comment",
                javadoc.colour,
                background.colour,
                javadoc.isBold,
                javadoc.isItalic);
    AddProperty(properties,
                "17",
                "Doxygen keyword",
                javadocKeyword.colour,
                background.colour,
                javadocKeyword.isBold,
                javadocKeyword.isItalic);
    AddProperty(properties,
                "18",
                "Doxygen keyword error",
                javadocKeyword.colour,
                background.colour,
                javadocKeyword.isBold,
                javadocKeyword.isItalic);
    AddProperty(properties, "16", "Workspace tags", klass.colour, background.colour, klass.isBold, klass.isItalic);
    AddProperty(
        properties, "19", "Local variables", variable.colour, background.colour, variable.isBold, variable.isItalic);

    AddCommonProperties(properties);
    wxString codeliteXmlFile =
        wxFileName(clStandardPaths::Get().GetUserLexersDir(), GetOutputFile("c++")).GetFullPath();
    return ::SaveXmlToFile(&codeliteXML, codeliteXmlFile);
}

wxFileName EclipseCXXThemeImporter::ToEclipseXML(const wxFileName& codeliteXml, size_t id)
{
    wxXmlDocument doc(codeliteXml.GetFullPath());
    if(!doc.IsOk()) return wxFileName();

    wxXmlNode* props = XmlUtils::FindFirstByTagName(doc.GetRoot(), "Properties");
    if(!props) return wxFileName();

    wxString eclipseXML;
    eclipseXML << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    eclipseXML << "<colorTheme id=\"2\" name=\"" << doc.GetRoot()->GetAttribute("Theme") << "\">\n";
    wxXmlNode* prop = props->GetChildren();
    while(prop) {
        if(prop->GetAttribute("Name") == "Default") {
            eclipseXML << "  <foreground color=\"" << prop->GetAttribute("Colour") << "\" />\n";
            eclipseXML << "  <background color=\"" << prop->GetAttribute("BgColour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Line Numbers") {
            eclipseXML << "  <lineNumber color=\"" << prop->GetAttribute("Colour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Text Selection") {
            eclipseXML << "  <selectionForeground color=\"" << prop->GetAttribute("Colour") << "\" />\n";
            eclipseXML << "  <selectionBackground color=\"" << prop->GetAttribute("BgColour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Common C++ style comment") {
            eclipseXML << "  <singleLineComment color=\"" << prop->GetAttribute("Colour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Common C style comment") {
            eclipseXML << "  <multiLineComment color=\"" << prop->GetAttribute("Colour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Number") {
            eclipseXML << "  <number color=\"" << prop->GetAttribute("Colour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "String") {
            eclipseXML << "  <string color=\"" << prop->GetAttribute("Colour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Operator") {
            eclipseXML << "  <operator color=\"" << prop->GetAttribute("Colour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "C++ keyword") {
            eclipseXML << "  <keyword color=\"" << prop->GetAttribute("Colour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Workspace tags") {
            eclipseXML << "  <class color=\"" << prop->GetAttribute("Colour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Local variables") {
            eclipseXML << "  <localVariable color=\"" << prop->GetAttribute("Colour") << "\" />\n";
        }
        prop = prop->GetNext();
    }
    eclipseXML << "</colorTheme>\n";
    wxString xmlFile;
    xmlFile << wxStandardPaths::Get().GetTempDir() << "/"
            << "eclipse-theme-" << id << ".xml";
    FileUtils::WriteFileContent(xmlFile, eclipseXML);
    return wxFileName(xmlFile);
}

std::vector<wxFileName> EclipseCXXThemeImporter::ToEclipseXMLs()
{
    std::vector<wxFileName> arr;
    wxArrayString files;
    wxDir::GetAllFiles(clStandardPaths::Get().GetLexersDir(), &files, "lexer_c++_*.xml");
    for(size_t i = 0; i < files.GetCount(); ++i) {
        arr.push_back( ToEclipseXML(files.Item(i), i));
    }
    return arr;
}
