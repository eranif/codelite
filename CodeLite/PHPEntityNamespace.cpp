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
        wxSQLite3Statement statement = db.PrepareStatement("REPLACE INTO SCOPE_TABLE (ID, SCOPE_TYPE, SCOPE_ID, NAME) "
                                                           "VALUES (NULL, 0, -1, :NAME)");
        statement.Bind(statement.GetParamIndex(":NAME"), GetName());
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
}
wxString PHPEntityNamespace::Type() const { return GetName(); }
bool PHPEntityNamespace::Is(eEntityType type) const { return type == kEntityTypeNamespace; }
wxString PHPEntityNamespace::GetDisplayName() const { return GetName(); }
wxString PHPEntityNamespace::GetNameOnly() const { return GetName().AfterLast('\\'); }
