#include "ColoursAndFontsManager.h"
#include "EclipseCXXThemeImporter.h"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include "globals.h"
#include "wx/dir.h"
#include "xmlutils.h"

EclipseCXXThemeImporter::EclipseCXXThemeImporter()
{
    SetKeywords0("and and_eq asm auto bitand bitor bool break case catch char class compl const const_cast "
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
    AddProperty(lexer, "1", "Common C style comment", m_multiLineComment.colour, m_background.colour,
                m_multiLineComment.isBold, m_multiLineComment.isItalic);
    AddProperty(lexer, "2", "Common C++ style comment", m_singleLineComment.colour, m_background.colour,
                m_singleLineComment.isBold, m_singleLineComment.isItalic);
    AddProperty(lexer, "3", "Doxygen C style comment", m_javadoc.colour, m_background.colour, m_javadoc.isBold,
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
    AddProperty(lexer, "15", "Doxygen C++ style comment", m_javadoc.colour, m_background.colour, m_javadoc.isBold,
                m_javadoc.isItalic);
    AddProperty(lexer, "17", "Doxygen keyword", m_javadocKeyword.colour, m_background.colour, m_javadocKeyword.isBold,
                m_javadocKeyword.isItalic);
    AddProperty(lexer, "18", "Doxygen keyword error", m_javadocKeyword.colour, m_background.colour,
                m_javadocKeyword.isBold, m_javadocKeyword.isItalic);
    AddProperty(lexer, "16", "Workspace tags", m_klass.colour, m_background.colour, m_klass.isBold, m_klass.isItalic);
    AddProperty(lexer, "19", "Local variables", m_variable.colour, m_background.colour, m_variable.isBold,
                m_variable.isItalic);

    FinalizeImport(lexer);
    return lexer;
}

#define GET_LEXER_STYLE_FG_COLOUR(style_number) cxxLexer->GetProperty(style_number).GetFgColour()
#define GET_LEXER_STYLE_BG_COLOUR(style_number) cxxLexer->GetProperty(style_number).GetBgColour()

wxFileName EclipseCXXThemeImporter::ToEclipseXML(LexerConf::Ptr_t cxxLexer, size_t id)
{
    wxString eclipseXML;
    wxString themeName = cxxLexer->GetThemeName();
    eclipseXML << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    eclipseXML << "<colorTheme id=\"2\" name=\"" << themeName << "\">\n";
    eclipseXML << "  <foreground color=\"" << GET_LEXER_STYLE_FG_COLOUR(11) << "\" />\n";
    eclipseXML << "  <background color=\"" << GET_LEXER_STYLE_BG_COLOUR(0) << "\" />\n";
    eclipseXML << "  <lineNumber color=\"" << GET_LEXER_STYLE_FG_COLOUR(33) << "\" />\n";
    eclipseXML << "  <selectionForeground color=\"" << GET_LEXER_STYLE_FG_COLOUR(SEL_TEXT_ATTR_ID) << "\" />\n";
    eclipseXML << "  <selectionBackground color=\"" << GET_LEXER_STYLE_BG_COLOUR(SEL_TEXT_ATTR_ID) << "\" />\n";
    eclipseXML << "  <singleLineComment color=\"" << GET_LEXER_STYLE_FG_COLOUR(2) << "\" />\n";
    eclipseXML << "  <multiLineComment color=\"" << GET_LEXER_STYLE_FG_COLOUR(1) << "\" />\n";
    eclipseXML << "  <number color=\"" << GET_LEXER_STYLE_FG_COLOUR(4) << "\" />\n";
    eclipseXML << "  <string color=\"" << GET_LEXER_STYLE_FG_COLOUR(6) << "\" />\n";
    eclipseXML << "  <operator color=\"" << GET_LEXER_STYLE_FG_COLOUR(10) << "\" />\n";
    eclipseXML << "  <keyword color=\"" << GET_LEXER_STYLE_FG_COLOUR(5) << "\" />\n";
    eclipseXML << "  <class color=\"" << GET_LEXER_STYLE_FG_COLOUR(16) << "\" />\n";
    eclipseXML << "  <localVariable color=\"" << GET_LEXER_STYLE_FG_COLOUR(19) << "\" />\n";
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
    wxArrayString themes = ColoursAndFontsManager::Get().GetAllThemes();
    for(size_t i = 0; i < themes.size(); ++i) {
        LexerConf::Ptr_t cxxLexer = ColoursAndFontsManager::Get().GetLexer("c++", themes.Item(i));
        if(!cxxLexer) continue;
        arr.push_back(ToEclipseXML(cxxLexer, i));
    }
    return arr;
}
