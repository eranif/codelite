#include "PHPEntityNamespace.h"

PHPEntityNamespace::PHPEntityNamespace() {}

PHPEntityNamespace::~PHPEntityNamespace() {}

void PHPEntityNamespace::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    wxPrintf("%sNamespace name: %s\n", indentString, GetName());

    PHPEntityBase::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        (*iter)->PrintStdout(indent + 4);
    }
}

void PHPEntityNamespace::Store(wxSQLite3Database& db)
{
    try {
        wxSQLite3Statement statement =
            db.PrepareStatement("REPLACE INTO SCOPE_TABLE (ID, SCOPE_TYPE, SCOPE_ID, NAME, LINE_NUMBER, FILE_NAME) "
                                "VALUES (NULL, 0, -1, :NAME, :LINE_NUMBER, :FILE_NAME)");
        statement.Bind(statement.GetParamIndex(":NAME"), GetName());
        statement.Bind(statement.GetParamIndex(":LINE_NUMBER"), GetLine());
        statement.Bind(statement.GetParamIndex(":FILE_NAME"), GetFilename().GetFullPath());
        statement.ExecuteUpdate();
        SetDbId(db.GetLastRowId());
    } catch(wxSQLite3Exception& exc) {
        wxUnusedVar(exc);
    }
}

void PHPEntityNamespace::FromResultSet(wxSQLite3ResultSet& res)
{
    SetDbId(res.GetInt("ID"));
    SetName(res.GetString("NAME"));
    SetFilename(res.GetString("FILE_NAME"));
    SetLine(res.GetInt("LINE_NUMBER"));
}
wxString PHPEntityNamespace::Type() const { return GetName(); }
bool PHPEntityNamespace::Is(eEntityType type) const { return type == kEntityTypeNamespace; }
wxString PHPEntityNamespace::GetDisplayName() const { return GetName(); }
wxString PHPEntityNamespace::GetNameOnly() const { return GetName().AfterLast('\\'); }
wxString PHPEntityNamespace::FormatPhpDoc() const
{
    wxString doc;
    doc << "/**\n"
        << " * @brief \n";
    doc << " */";
    return doc;
}
