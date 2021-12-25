#include "ThemeImporterJava.hpp"

ThemeImporterJava::ThemeImporterJava()
{
    // keywords
    SetKeywords0("abstract assert boolean break byte case catch char class const continue default do double else "
                 "extends final  finally float for future generic goto if implements import inner "
                 "instanceof int interface long native new null outer  package private protected public rest "
                 "return short static super switch synchronized this throw throws transient try  var "
                 "void volatile while");
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
    // Secondary keywords and identifiers
    m_functionsIndex = 1;
    // Global classes and typedefs
    m_classesIndex = 3;
    SetFileExtensions("*.java");
    m_langName = "java";
}

ThemeImporterJava::~ThemeImporterJava() {}
LexerConf::Ptr_t ThemeImporterJava::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_CPP);
    CHECK_PTR_RET_NULL(lexer);

    AddProperty(lexer, wxSTC_C_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_C_COMMENT, "Block comment", m_multiLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTLINE, "Line comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTDOC, "Javadoc block comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_C_WORD, "Java keyword", m_keyword);
    AddProperty(lexer, wxSTC_C_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_C_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_C_UUID, "Uuid", m_number);
    AddProperty(lexer, wxSTC_C_PREPROCESSOR, "Preprocessor", m_editor);
    AddProperty(lexer, wxSTC_C_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_C_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_C_STRINGEOL, "Open String", m_string);
    AddProperty(lexer, wxSTC_C_COMMENTLINEDOC, "Javadoc line comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORD, "Javadoc keyword", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORDERROR, "Javadoc keyword error", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_WORD2, "Methods", m_function);
    AddProperty(lexer, wxSTC_C_GLOBALCLASS, "Classes", m_klass);

    FinalizeImport(lexer);
    return lexer;
}
