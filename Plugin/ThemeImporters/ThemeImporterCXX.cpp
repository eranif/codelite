#include "ThemeImporterCXX.hpp"

#include "ColoursAndFontsManager.h"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include "globals.h"
#include "xmlutils.h"

#include <wx/dir.h>

ThemeImporterCXX::ThemeImporterCXX()
{
    // Primary keywords and identifiers
    SetKeywords0(
        "and and_eq asm auto bitand bitor bool break case catch char class compl const const_cast "
        "continue default delete do double dynamic_cast else enum explicit export extern false final "
        "float for friend goto if inline int long mutable namespace new not not_eq operator or "
        "or_eq override private protected public register reinterpret_cast return short signed sizeof "
        "static static_cast struct switch template this throw true try typedef typeid typename "
        "union unsigned using virtual void volatile wchar_t while xor xor_eq alignas alignof "
        "char16_t char32_t constexpr decltype noexcept nullptr static_assert thread_local co_return co_await co_yield");

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
    SetFunctionsWordSetIndex(1);
    SetClassWordSetIndex(3);
    SetOthersWordSetIndex(4);
    SetLocalsWordSetIndex(LexerConf::WS_VARIABLES, true);
    SetLangName("c++");
    // Special task markers
    // will be styled with SCE_C_TASKMARKER
    SetKeywords5("TODO FIXME BUG ATTN");

    // Preprocessor definitions (we are going to use it for functions)
    SetFileExtensions("*.cxx;*.hpp;*.cc;*.h;*.c;*.cpp;*.l;*.y;*.c++;*.hh;*.ipp;*.hxx;*.h++");
}

ThemeImporterCXX::~ThemeImporterCXX() {}

LexerConf::Ptr_t ThemeImporterCXX::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_CPP);
    CHECK_PTR_RET_NULL(lexer);

    // Convert to CodeLite's XML properties
    AddProperty(lexer, wxSTC_C_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_C_COMMENT, "Common C style comment", m_multiLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTLINE, "Common C++ style comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_C_COMMENTDOC, "Doxygen C style comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_C_WORD, "C++ keyword", m_keyword);
    AddProperty(lexer, wxSTC_C_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_C_STRINGRAW, "String Raw", m_string);
    AddProperty(lexer, wxSTC_C_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_C_UUID, "Uuid", m_number);
    AddProperty(lexer, wxSTC_C_PREPROCESSOR, "Preprocessor", m_editor);
    AddProperty(lexer, wxSTC_C_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_C_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_C_STRINGEOL, "Open String", m_string);
    AddProperty(lexer, wxSTC_C_COMMENTLINEDOC, "Doxygen C++ style comment", m_javadoc);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORD, "Doxygen keyword", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_COMMENTDOCKEYWORDERROR, "Doxygen keyword error", m_javadocKeyword);
    AddProperty(lexer, wxSTC_C_WORD2, "Functions", m_function);
    AddProperty(lexer, wxSTC_C_GLOBALCLASS, "Classes", m_klass);
    AddProperty(lexer, wxSTC_C_TASKMARKER, "Task Markers", m_task);
    AddPropertySubstyle(lexer, LexerConf::WS_VARIABLES, "Variables", m_variable);

    // the base for all our substyles
    lexer->SetSubstyleBase(wxSTC_C_IDENTIFIER);

    FinalizeImport(lexer);
    return lexer;
}
