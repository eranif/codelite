#include "ThemeImporterJavaScript.hpp"

#include "cl_standard_paths.h"
#include "fileutils.h"
#include "globals.h"
#include "wx/dir.h"
#include "xmlutils.h"

#include <wx/stc/stc.h>

ThemeImporterJavaScript::ThemeImporterJavaScript()
{
    // Primary keywords and identifiers
    SetKeywords0("abstract arguments boolean break byte "
                 "case catch char class const "
                 "continue debugger default delete do "
                 "double else enum eval export "
                 "extends false final finally float "
                 "for function goto if implements "
                 "import in instanceof int interface "
                 "let long native new null "
                 "package private protected public return "
                 "short static super switch synchronized "
                 "this throw throws transient true "
                 "try typeof var void volatile "
                 "while with yield prototype undefined StringtoString NaN ");

    // Secondary keywords and identifiers
    SetKeywords1(
        "activeElement addEventListener adoptNode anchors applets "
        "baseURI body close cookie createAttribute createComment createDocumentFragment createElement createTextNode "
        "doctype documentElement documentMode documentURI domain "
        "domConfig embeds forms getElementById getElementsByClassName getElementsByName getElementsByTagName hasFocus "
        "head images implementation importNode inputEncoding lastModified links normalize normalizeDocument open "
        "querySelector"
        "querySelectorAll readyState referrer removeEventListener renameNode scripts strictErrorChecking title URL "
        "write writeln NaN name NumberObject valueOf");

    // Documentation comment keywords
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

    // Used for wxSTC_C_GLOBALCLASS ("classes")
    SetKeywords3("Math Array Date document window NumberObject URL");

    // used for functions
    m_functionsIndex = 1;

    // Global classes and typedefs, it already contains values, so be careful here
    // so when used, append the values
    m_classesIndex = 3;
    SetFileExtensions("*.js;*.javascript;*.qml;*.json;*.ts");
}

ThemeImporterJavaScript::~ThemeImporterJavaScript() {}

LexerConf::Ptr_t ThemeImporterJavaScript::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "javascript", wxSTC_LEX_CPP);
    CHECK_PTR_RET_NULL(lexer);

    // Covnert to codelite's XML properties
    AddProperty(lexer, wxSTC_C_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_C_COMMENT, "Block comment", m_multiLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTLINE, "Single line comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTDOC, "Javadoc block comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_C_WORD, "JavaScript keyword", m_keyword);
    AddProperty(lexer, wxSTC_C_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_C_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_C_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_C_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_C_STRINGEOL, "Open String", m_string);
    AddProperty(lexer, wxSTC_C_COMMENTLINEDOC, "Javadoc single line comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_WORD2, "JavaScript functions", m_function);
    AddProperty(lexer, wxSTC_C_GLOBALCLASS, "JavaScript global classes", m_klass);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORD, "Javadoc keyword", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORDERROR, "Javadoc keyword error", m_javadocKeyword);
    FinalizeImport(lexer);
    return lexer;
}
