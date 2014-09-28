#include "PHPEntityNamespace.h"

PHPEntityNamespace::PHPEntityNamespace() {}

PHPEntityNamespace::~PHPEntityNamespace() {}

void PHPEntityNamespace::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    wxPrintf("%sNamespace name: %s\n", indentString, GetName());

    PHPEntityBase::Map_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        iter->second->PrintStdout(indent + 4);
    }
}
void PHPEntityNamespace::Store(wxSQLite3Database& db)
{
    try {
        wxSQLite3Statement statement =
            db.PrepareStatement("INSERT OR REPLACE INTO NAMESPACE_TABLE VALUES(NULL, :NAME)");
        statement.Bind(statement.GetParamIndex(":NAME"), GetName());
        statement.ExecuteUpdate();
        SetDbId(db.GetLastRowId());
    } catch(wxSQLite3Exception& exc) {
        wxUnusedVar(exc);
    }
}
