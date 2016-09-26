#include "EclipseCXXThemeImporter.h"
#include "globals.h"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include "xmlutils.h"
#include "wx/dir.h"

EclipseCXXThemeImporter::EclipseCXXThemeImporter()
{
    SetKeywords0(
        "and and_eq asm auto bitand bitor bool break case catch char class compl const const_cast "
        "continue default delete do double dynamic_cast else enum explicit export extern false final "
        "float for friend goto if inline int long mutable namespace new not not_eq operator or "
        "or_eq override private protected public register reinterpret_cast return short signed sizeof "
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

LexerConf::Ptr_t EclipseCXXThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "c++", 3);
    CHECK_PTR_RET_NULL(lexer);

    // Covnert to codelite's XML properties
    AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer,
                "1",
                "Common C style comment",
                m_multiLineComment.colour,
                m_background.colour,
                m_multiLineComment.isBold,
                m_multiLineComment.isItalic);
    AddProperty(lexer,
                "2",
                "Common C++ style comment",
                m_singleLineComment.colour,
                m_background.colour,
                m_singleLineComment.isBold,
                m_singleLineComment.isItalic);
    AddProperty(lexer,
                "3",
                "Doxygen C style comment",
                m_javadoc.colour,
                m_background.colour,
                m_javadoc.isBold,
                m_javadoc.isItalic);
    AddProperty(lexer, "4", "Number", m_number.colour, m_background.colour, m_number.isBold, m_number.isItalic);
    AddProperty(lexer, "5", "C++ keyword", m_keyword.colour, m_background.colour, m_keyword.isBold, m_keyword.isItalic);
    AddProperty(lexer, "6", "String", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
    AddProperty(lexer, "7", "Character", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
    AddProperty(lexer, "8", "Uuid", m_number.colour, m_background.colour, m_number.isBold, m_number.isItalic);
    AddProperty(lexer, "9", "Preprocessor", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "10", "Operator", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "11", "Identifier", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "12", "Open String", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
    AddProperty(lexer,
                "15",
                "Doxygen C++ style comment",
                m_javadoc.colour,
                m_background.colour,
                m_javadoc.isBold,
                m_javadoc.isItalic);
    AddProperty(lexer,
                "17",
                "Doxygen keyword",
                m_javadocKeyword.colour,
                m_background.colour,
                m_javadocKeyword.isBold,
                m_javadocKeyword.isItalic);
    AddProperty(lexer,
                "18",
                "Doxygen keyword error",
                m_javadocKeyword.colour,
                m_background.colour,
                m_javadocKeyword.isBold,
                m_javadocKeyword.isItalic);
    AddProperty(lexer, "16", "Workspace tags", m_klass.colour, m_background.colour, m_klass.isBold, m_klass.isItalic);
    AddProperty(
        lexer, "19", "Local variables", m_variable.colour, m_background.colour, m_variable.isBold, m_variable.isItalic);

    FinalizeImport(lexer);
    return lexer;
}

/**
 * @brief helper method that ensure that if the colour property does not exist
 * or contains an empty string, it returns teh 'defaultVal'
 */
static wxString GetFgColour(wxXmlNode* prop, const wxString& defaultVal)
{
    wxString colour = prop->GetAttribute("Colour", defaultVal);
    if(colour.IsEmpty()) {
        colour = defaultVal;
    }
    return colour;
}

wxFileName EclipseCXXThemeImporter::ToEclipseXML(const wxFileName& codeliteXml, size_t id)
{
    wxXmlDocument doc(codeliteXml.GetFullPath());
    if(!doc.IsOk()) return wxFileName();

    wxXmlNode* props = XmlUtils::FindFirstByTagName(doc.GetRoot(), "Properties");
    if(!props) return wxFileName();

    wxString eclipseXML;
    wxString themeName = doc.GetRoot()->GetAttribute("Theme");
    eclipseXML << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    eclipseXML << "<colorTheme id=\"2\" name=\"" << themeName << "\">\n";
    wxXmlNode* prop = props->GetChildren();
    wxString defaultFgColour;
    while(prop) {
        if(prop->GetAttribute("Name") == "Default") {
            if(themeName == "Default") {
                eclipseXML << "  <foreground color=\"black\" />\n";
                eclipseXML << "  <background color=\"white\" />\n";
                defaultFgColour = "black";

            } else {
                eclipseXML << "  <foreground color=\"" << prop->GetAttribute("Colour") << "\" />\n";
                eclipseXML << "  <background color=\"" << prop->GetAttribute("BgColour") << "\" />\n";
                defaultFgColour = prop->GetAttribute("Colour");
            }
        } else if(prop->GetAttribute("Name") == "Line Numbers") {
            eclipseXML << "  <lineNumber color=\"" << GetFgColour(prop, defaultFgColour) << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Text Selection") {
            eclipseXML << "  <selectionForeground color=\"" << prop->GetAttribute("Colour") << "\" />\n";
            eclipseXML << "  <selectionBackground color=\"" << prop->GetAttribute("BgColour") << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Common C++ style comment") {
            eclipseXML << "  <singleLineComment color=\"" << GetFgColour(prop, defaultFgColour) << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Common C style comment") {
            eclipseXML << "  <multiLineComment color=\"" << GetFgColour(prop, defaultFgColour) << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Number") {
            eclipseXML << "  <number color=\"" << GetFgColour(prop, defaultFgColour) << "\" />\n";

        } else if(prop->GetAttribute("Name") == "String") {
            eclipseXML << "  <string color=\"" << GetFgColour(prop, defaultFgColour) << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Operator") {
            eclipseXML << "  <operator color=\"" << GetFgColour(prop, defaultFgColour) << "\" />\n";

        } else if(prop->GetAttribute("Name") == "C++ keyword") {
            eclipseXML << "  <keyword color=\"" << GetFgColour(prop, defaultFgColour) << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Workspace tags") {
            eclipseXML << "  <class color=\"" << GetFgColour(prop, defaultFgColour) << "\" />\n";

        } else if(prop->GetAttribute("Name") == "Local variables") {
            eclipseXML << "  <localVariable color=\"" << GetFgColour(prop, defaultFgColour) << "\" />\n";
        }
        prop = prop->GetNext();
    }
    eclipseXML << "</colorTheme>\n";
    wxString xmlFile;
    xmlFile << clStandardPaths::Get().GetTempDir() << "/"
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
        const wxString& curfile = files.Item(i);
        arr.push_back(ToEclipseXML(curfile, i));
    }
    return arr;
}
