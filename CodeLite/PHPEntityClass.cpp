#include "PHPEntityClass.h"
#include <wx/tokenzr.h>

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
    PHPEntityBase::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        (*iter)->PrintStdout(indent + 4);
    }
}

void PHPEntityClass::Store(wxSQLite3Database& db)
{
    try {
        wxSQLite3Statement statement = db.PrepareStatement(
            "REPLACE INTO SCOPE_TABLE (ID, SCOPE_TYPE, SCOPE_ID, NAME, EXTENDS, IMPLEMENTS, USING_TRAITS, DOC_COMMENT, "
            "LINE_NUMBER, FILE_NAME) VALUES (NULL, 1, :SCOPE_ID, :NAME, :EXTENDS, "
            ":IMPLEMENTS, :USING_TRAITS, :DOC_COMMENT, :LINE_NUMBER, :FILE_NAME)");

        statement.Bind(statement.GetParamIndex(":SCOPE_ID"), Parent()->GetDbId());
        statement.Bind(statement.GetParamIndex(":NAME"), GetName());
        statement.Bind(statement.GetParamIndex(":EXTENDS"), GetExtends());
        statement.Bind(statement.GetParamIndex(":IMPLEMENTS"), GetImplementsAsString());
        statement.Bind(statement.GetParamIndex(":USING_TRAITS"), GetTraitsAsString()); // TODO: implement this
        statement.Bind(statement.GetParamIndex(":DOC_COMMENT"), GetDocComment());
        statement.Bind(statement.GetParamIndex(":LINE_NUMBER"), GetLine());
        statement.Bind(statement.GetParamIndex(":FILE_NAME"), GetFilename().GetFullPath());
        statement.ExecuteUpdate();
        SetDbId(db.GetLastRowId());
    } catch(wxSQLite3Exception& exc) {
        wxUnusedVar(exc);
    }
}
void PHPEntityClass::FromResultSet(wxSQLite3ResultSet& res)
{
    SetDbId(res.GetInt("ID"));
    SetName(res.GetString("NAME"));
    SetExtends(res.GetString("EXTENDS"));
    SetImplements(::wxStringTokenize(res.GetString("IMPLEMENTS"), ";", wxTOKEN_STRTOK));
    SetTraits(::wxStringTokenize(res.GetString("USING_TRAITS"), ";", wxTOKEN_STRTOK));
    SetDocComment(res.GetString("DOC_COMMENT"));
    SetLine(res.GetInt("LINE_NUMBER"));
    SetFilename(res.GetString("FILE_NAME"));
}

wxArrayString PHPEntityClass::GetInheritanceArray() const
{
    wxArrayString arr, uniqueArr;
    arr.Add(GetExtends());
    arr.insert(arr.end(), GetImplements().begin(), GetImplements().end());
    arr.insert(arr.end(), GetTraits().begin(), GetTraits().end());

    for(size_t i = 0; i < arr.GetCount(); ++i) {
        if(uniqueArr.Index(arr.Item(i)) == wxNOT_FOUND) {
            uniqueArr.Add(arr.Item(i));
        }
    }
    arr.swap(uniqueArr);
    return arr;
}
wxString PHPEntityClass::Type() const { return GetName(); }
bool PHPEntityClass::Is(eEntityType type) const { return type == kEntityTypeClass; }
wxString PHPEntityClass::GetDisplayName() const { return GetNameOnly(); }
wxString PHPEntityClass::GetNameOnly() const { return GetName().AfterLast('\\'); }
