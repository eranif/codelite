#include "EclipseSQLThemeImporter.h"
#include <wx/stc/stc.h>

EclipseSQLThemeImporter::EclipseSQLThemeImporter()
{
    SetKeywords0("absolute action add admin after aggregate  alias all allocate alter and any are array as asc  "
                 "assertion at authorization  before begin binary bit blob boolean both breadth by  call cascade "
                 "cascaded case cast catalog char character  check class clob close collate collation column commit  "
                 "completion connect connection constraint constraints  constructor continue corresponding create "
                 "cross cube current  current_date current_path current_role current_time current_timestamp  "
                 "current_user cursor cycle  data date day deallocate dec decimal declare default  deferrable deferred "
                 "delete depth deref desc describe descriptor  destroy destructor deterministic dictionary diagnostics "
                 "disconnect  distinct domain double drop dynamic  each else end end-exec equals escape every except  "
                 "exception exec execute external  false fetch first float for foreign found from free full  function  "
                 "general get global go goto grant group grouping  having host hour  identity if ignore immediate in "
                 "indicator initialize initially  inner inout input insert int integer intersect interval  into is "
                 "isolation iterate  join  key  language large last lateral leading left less level like  limit local "
                 "localtime localtimestamp locator  map match minute modifies modify module month  names national "
                 "natural nchar nclob new next no none  not null numeric  object of off old on only open operation "
                 "option  or order ordinality out outer output  pad parameter parameters partial path postfix "
                 "precision prefix  preorder prepare preserve primary  prior privileges procedure public  read reads "
                 "real recursive ref references referencing relative  restrict result return returns revoke right  "
                 "role rollback rollup routine row rows  savepoint schema scroll scope search second section select  "
                 "sequence session session_user set sets size smallint some| space  specific specifictype sql "
                 "sqlexception sqlstate sqlwarning start  state statement static structure system_user  table "
                 "temporary terminate than then time timestamp  timezone_hour timezone_minute to trailing transaction "
                 "translation  treat trigger true  under union unique unknown  unnest update usage user using  value "
                 "values varchar variable varying view  when whenever where with without work write  year  zone index "
                 "exists replace");
    SetFileExtensions("*.sql;*.sqlite");
    m_langName = "sql";
}

EclipseSQLThemeImporter::~EclipseSQLThemeImporter() {}

LexerConf::Ptr_t EclipseSQLThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_SQL);
    AddProperty(lexer, wxSTC_SQL_DEFAULT, "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_COMMENT, "Comment line", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_COMMENTLINE, "Comment block", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_COMMENTDOC, "Comment doc", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_NUMBER, "Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_WORD, "Word", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_STRING, "String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_CHARACTER, "Character", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_SQLPLUS, "SQL Plus", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_SQLPLUS_PROMPT, "SQL Plus prompt", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_OPERATOR, "Operator", m_oper.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_SQLPLUS_COMMENT, "SQL Plus comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_COMMENTLINEDOC, "Comment line doc", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_WORD2, "Word (Set 2)", m_keyword.colour, m_background.colour);
    AddProperty(
        lexer, wxSTC_SQL_COMMENTDOCKEYWORD, "Comment doc keyword", m_javadocKeyword.colour, m_background.colour);
    AddProperty(lexer,
                wxSTC_SQL_COMMENTDOCKEYWORDERROR,
                "Comment doc keyword error",
                m_javadocKeyword.colour,
                m_background.colour);
    AddProperty(lexer, wxSTC_SQL_USER1, "User 1", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_USER1, "User 2", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_USER1, "User 3", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_USER1, "User 4", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_SQL_QUOTEDIDENTIFIER, "Quoted identifier", m_string.colour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
