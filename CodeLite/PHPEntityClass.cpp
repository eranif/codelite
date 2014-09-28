#include "PHPEntityClass.h"

PHPEntityClass::PHPEntityClass() {}

PHPEntityClass::~PHPEntityClass() {}
wxString PHPEntityClass::ID() const { return GetName(); }

void PHPEntityClass::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    wxPrintf("%sClass name: %s", indentString, GetName());
    if(!GetExtends().IsEmpty()) {
        wxPrintf(", extends %s", GetExtends());
    }
    if(!GetImplements().IsEmpty()) {
        wxPrintf(", implements: ");
        for(size_t i = 0; i < GetImplements().GetCount(); ++i) {
            wxPrintf("%s ", GetImplements().Item(i));
        }
    }

    wxPrintf("\n");
    PHPEntityBase::Map_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        iter->second->PrintStdout(indent + 4);
    }
}

void PHPEntityClass::Store(wxSQLite3Database& db)
{
    try {
        wxSQLite3Statement statement =
            db.PrepareStatement("INSERT OR REPLACE INTO CLASS_TABLE VALUES(NULL, :PARENT_ID, :NAME, :SCOPE, :EXTENDS, "
                                ":IMPLEMENTS, :USING_TRAITS, :DOC_COMMENT, :LINE_NUMBER, :FILE_NAME)");
        statement.Bind(statement.GetParamIndex(":PARENT_ID"), Parent()->GetDbId());
        statement.Bind(statement.GetParamIndex(":NAME"), GetName());
        statement.Bind(statement.GetParamIndex(":SCOPE"), GetScope());
        statement.Bind(statement.GetParamIndex(":EXTENDS"), GetExtends());
        statement.Bind(statement.GetParamIndex(":IMPLEMENTS"), GetImplementsAsString());
        statement.Bind(statement.GetParamIndex(":USING_TRAITS"), wxString()); // TODO: implement this
        statement.Bind(statement.GetParamIndex(":DOC_COMMENT"), GetDocComment());
        statement.Bind(statement.GetParamIndex(":LINE_NUMBER"), GetLine());
        statement.Bind(statement.GetParamIndex(":FILE_NAME"), GetFilename().GetFullPath());
        statement.ExecuteUpdate();
        SetDbId(db.GetLastRowId());
    } catch(wxSQLite3Exception& exc) {
        wxUnusedVar(exc);
    }
}
