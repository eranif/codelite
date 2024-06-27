#include "PHPDocVar.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>

PHPDocVar::PHPDocVar(PHPSourceFile& sourceFile, const wxString& doc)
    : m_isOk(false)
    , m_dbId(wxNOT_FOUND)
    , m_lineNumber(wxNOT_FOUND)
{
    Parse(sourceFile, doc);
}

PHPDocVar::PHPDocVar()
    : m_isOk(false)
    , m_dbId(wxNOT_FOUND)
    , m_lineNumber(wxNOT_FOUND)
{
}

PHPDocVar::~PHPDocVar() {}

void PHPDocVar::Parse(PHPSourceFile& sourceFile, const wxString& doc)
{
    wxString sname;
    wxString stype;
    m_isOk = false;
    wxStringTokenizer tokenizer(doc, " \n\r", wxTOKEN_STRTOK);

    // @var Type $name
    // @var Type
    // @var $Name Type
    if(!tokenizer.HasMoreTokens() || tokenizer.GetNextToken() != "@var") {
        return;
    }

    // Next word should be the type
    if(!tokenizer.HasMoreTokens()) {
        return;
    }
    stype = tokenizer.GetNextToken();

    // Next comes the name
    if(tokenizer.HasMoreTokens()) {
        sname = tokenizer.GetNextToken();
    }

    // Handle common developer mistake
    if (stype.StartsWith("$")) {
        sname.swap(stype);
    }

    // TODO Support nullable parameters
    if (stype.StartsWith("?")) {
        stype.Remove(0, 1);
    }

    // Got the type
    m_type = sourceFile.MakeIdentifierAbsolute(stype);
    m_isOk = true;

    // Found the name
    m_name = sname;
}

void PHPDocVar::Store(wxSQLite3Database& db, wxLongLong parentDdId)
{
    try {
        wxSQLite3Statement statement =
            db.PrepareStatement("REPLACE INTO PHPDOC_VAR_TABLE (ID, SCOPE_ID, NAME, TYPE, LINE_NUMBER, FILE_NAME) "
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
