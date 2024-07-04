#include "PHPEntityClass.h"
#include <wx/tokenzr.h>
#include <algorithm>
#include "PHPLookupTable.h"

PHPEntityClass::PHPEntityClass() {}

PHPEntityClass::~PHPEntityClass() {}

void PHPEntityClass::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    wxPrintf("%sClass name: %s", indentString, GetFullName());
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

void PHPEntityClass::Store(PHPLookupTable* lookup)
{
    try {
        wxSQLite3Database& db = lookup->Database();
        wxSQLite3Statement statement = db.PrepareStatement(
            "REPLACE INTO SCOPE_TABLE (ID, SCOPE_TYPE, SCOPE_ID, NAME, FULLNAME, EXTENDS, "
            "IMPLEMENTS, USING_TRAITS, FLAGS, DOC_COMMENT, "
            "LINE_NUMBER, FILE_NAME) VALUES (NULL, 1, :SCOPE_ID, :NAME, :FULLNAME, :EXTENDS, "
            ":IMPLEMENTS, :USING_TRAITS, :FLAGS, :DOC_COMMENT, :LINE_NUMBER, :FILE_NAME)");

        statement.Bind(statement.GetParamIndex(":SCOPE_ID"), Parent()->GetDbId());
        statement.Bind(statement.GetParamIndex(":NAME"), GetShortName());
        statement.Bind(statement.GetParamIndex(":FULLNAME"), GetFullName());
        statement.Bind(statement.GetParamIndex(":EXTENDS"), GetExtends());
        statement.Bind(statement.GetParamIndex(":IMPLEMENTS"), GetImplementsAsString());
        statement.Bind(statement.GetParamIndex(":USING_TRAITS"), GetTraitsAsString());
        statement.Bind(statement.GetParamIndex(":FLAGS"), (int)GetFlags());
        statement.Bind(statement.GetParamIndex(":DOC_COMMENT"), GetDocComment());
        statement.Bind(statement.GetParamIndex(":LINE_NUMBER"), GetLine());
        statement.Bind(statement.GetParamIndex(":FILE_NAME"), GetFilename().GetFullPath());
        statement.ExecuteUpdate();
        SetDbId(db.GetLastRowId());

        // Now that we got the class saved, store any PHPDocVar
        std::for_each(
            m_varPhpDocs.begin(), m_varPhpDocs.end(), [&](PHPDocVar::Ptr_t doc) { doc->Store(db, GetDbId()); });
        lookup->UpdateClassCache(GetFullName());
    } catch(wxSQLite3Exception& exc) {
        wxUnusedVar(exc);
    }
}

void PHPEntityClass::FromResultSet(wxSQLite3ResultSet& res)
{
    SetDbId(res.GetInt("ID"));
    SetFullName(res.GetString("FULLNAME"));
    SetShortName(res.GetString("NAME"));
    SetExtends(res.GetString("EXTENDS"));
    SetImplements(::wxStringTokenize(res.GetString("IMPLEMENTS"), ";", wxTOKEN_STRTOK));
    SetTraits(::wxStringTokenize(res.GetString("USING_TRAITS"), ";", wxTOKEN_STRTOK));
    SetDocComment(res.GetString("DOC_COMMENT"));
    SetLine(res.GetInt("LINE_NUMBER"));
    SetFilename(res.GetString("FILE_NAME"));
    SetFlags(res.GetInt("FLAGS"));
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

wxString PHPEntityClass::Type() const { return GetFullName(); }
bool PHPEntityClass::Is(eEntityType type) const { return type == kEntityTypeClass; }
wxString PHPEntityClass::GetDisplayName() const { return GetShortName(); }
wxString PHPEntityClass::FormatPhpDoc(const CommentConfigData& data) const
{
    wxString doc;
    doc << data.GetCommentBlockPrefix() << "\n"
        << " * @class " << GetFullName() << "\n"
        << " * @brief \n"
        << " */";
    return doc;
}

void PHPEntityClass::FromJSON(const JSONItem& json)
{
    BaseFromJSON(json);
    m_extends = json.namedObject("extends").toString();
    m_implements = json.namedObject("implements").toArrayString();
    m_traits = json.namedObject("traits").toArrayString();
}

JSONItem PHPEntityClass::ToJSON() const
{
    JSONItem json = BaseToJSON("c");
    json.addProperty("extends", m_extends);
    json.addProperty("implements", m_implements);
    json.addProperty("traits", m_traits);
    return json;
}
