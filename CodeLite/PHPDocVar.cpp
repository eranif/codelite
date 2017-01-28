#include "PHPDocVar.h"
#include <wx/regex.h>

PHPDocVar::PHPDocVar(PHPSourceFile& sourceFile, const wxString& doc)
    : m_isOk(false)
    , m_dbId(wxNOT_FOUND)
    , m_lineNumber(wxNOT_FOUND)
{
    static wxRegEx reVarType(wxT("@(var|variable)[ \t]+([\\a-zA-Z_]{1}[\\a-zA-Z0-9_]*)"));
    if(reVarType.IsValid() && reVarType.Matches(doc)) {
        m_type = reVarType.GetMatch(doc, 2);
        m_type = sourceFile.MakeIdentifierAbsolute(m_type);
        m_isOk = true;
    }

    // @var Type $Name
    static wxRegEx reVarType1(wxT("@(var|variable)[ \t]+([\\a-zA-Z0-9_]+)[ \t]+([\\$]{1}[\\a-zA-Z0-9_]*)"));
    // @var $Name Type
    static wxRegEx reVarType2(wxT("@(var|variable)[ \t]+([\\$]{1}[\\a-zA-Z0-9_]+)[ \t]+([\\a-zA-Z0-9_]+)"));
    if(reVarType1.IsValid() && reVarType1.Matches(doc)) {
        m_type = reVarType1.GetMatch(doc, 2);
        m_type = sourceFile.MakeIdentifierAbsolute(m_type);
        m_name = reVarType1.GetMatch(doc, 3);
        m_isOk = true;
    } else if(reVarType2.IsValid() && reVarType2.Matches(doc)) {
        m_type = reVarType2.GetMatch(doc, 3);
        m_type = sourceFile.MakeIdentifierAbsolute(m_type);
        m_name = reVarType2.GetMatch(doc, 2);
        m_isOk = true;
    }
    m_filename = sourceFile.GetFilename();
}

PHPDocVar::PHPDocVar()
    : m_isOk(false)
    , m_dbId(wxNOT_FOUND)
    , m_lineNumber(wxNOT_FOUND)
{
}

PHPDocVar::~PHPDocVar() {}

void PHPDocVar::Store(wxSQLite3Database& db, wxLongLong parentDdId)
{
    try {
        wxSQLite3Statement statement = db.PrepareStatement(
            "REPLACE INTO PHPDOC_VAR_TABLE (ID, SCOPE_ID, NAME, TYPE, LINE_NUMBER, FILE_NAME) "
            "VALUES (NULL, :SCOPE_ID, :NAME, :TYPE, :LINE_NUMBER, :FILE_NAME)");

        statement.Bind(statement.GetParamIndex(":SCOPE_ID"), parentDdId);
        statement.Bind(statement.GetParamIndex(":NAME"), GetName());
        statement.Bind(statement.GetParamIndex(":TYPE"), GetType());
        statement.Bind(statement.GetParamIndex(":LINE_NUMBER"), GetLineNumber());
        statement.Bind(statement.GetParamIndex(":FILE_NAME"), GetFilename().GetFullPath());
        statement.ExecuteUpdate();
        SetDbId(db.GetLastRowId());
    } catch(wxSQLite3Exception& exc) {
        wxUnusedVar(exc);
    }
}

void PHPDocVar::FromResultSet(wxSQLite3ResultSet& res)
{
    SetDbId(res.GetInt("ID"));
    SetParentDbId(res.GetInt("SCOPE_ID"));
    SetName(res.GetString("NAME"));
    SetType(res.GetString("TYPE"));
    SetLineNumber(res.GetInt("LINE_NUMBER"));
    SetFilename(res.GetString("FILE_NAME"));
}
