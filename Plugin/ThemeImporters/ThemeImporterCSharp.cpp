#include "ThemeImporterCSharp.hpp"

ThemeImporterCSharp::ThemeImporterCSharp()
{
    // keywords
    SetKeywords0("abstract as base bool break byte case catch "
                 "char checked class const continue decimal default "
                 "delegate do double else enum event explicit "
                 "extern false finally fixed float for foreach "
                 "goto if implicit in int interface internal "
                 "is lock long namespace new null object "
                 "operator out override params private protected "
                 "public readonly ref return sbyte sealed short "
                 "sizeof stackalloc static string struct switch "
                 "this throw true try typeof uint ulong "
                 "unchecked unsafe ushort using virtual void "
                 "volatile while");

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

    // Special task markers
    // will be styled with SCE_C_TASKMARKER
    SetKeywords5("TODO FIXME BUG ATTN");

    // Secondary keywords and identifiers
    SetFunctionsWordSetIndex(1);
    SetClassWordSetIndex(3);
    SetOthersWordSetIndex(4);
    SetLocalsWordSetIndex(LexerConf::WS_VARIABLES, true);
    SetFileExtensions("*.cs");
    SetLangName("csharp");
}

LexerConf::Ptr_t ThemeImporterCSharp::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_CPP);
    CHECK_PTR_RET_NULL(lexer);

    AddProperty(lexer, wxSTC_C_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_C_COMMENT, "Block comment", m_multiLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTLINE, "Line comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTDOC, "Comment doc", m_javadoc);
    AddProperty(lexer, wxSTC_C_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_C_WORD, "CSharp keyword", m_keyword);
    AddProperty(lexer, wxSTC_C_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_C_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_C_UUID, "Uuid", m_number);
    AddProperty(lexer, wxSTC_C_PREPROCESSOR, "Preprocessor", m_editor);
    AddProperty(lexer, wxSTC_C_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_C_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_C_STRINGEOL, "Open String", m_string);
    AddProperty(lexer, wxSTC_C_COMMENTLINEDOC, "Doc line comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORD, "Doc keyword", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORDERROR, "Doc keyword error", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_WORD2, "Methods", m_function);
    AddProperty(lexer, wxSTC_C_GLOBALCLASS, "Classes", m_klass);
    AddProperty(lexer, wxSTC_C_TASKMARKER, "Task Markers", m_task);
    AddPropertySubstyle(lexer, LexerConf::WS_VARIABLES, "Variable", m_variable);

    // the base for all our substyles
    lexer->SetSubstyleBase(wxSTC_C_IDENTIFIER);

    FinalizeImport(lexer);
    return lexer;
}
