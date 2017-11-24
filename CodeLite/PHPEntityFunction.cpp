#include "PHPEntityFunction.h"
#include "PHPEntityVariable.h"
#include "PHPLookupTable.h"
#include "commentconfigdata.h"
#include "file_logger.h"

PHPEntityFunction::PHPEntityFunction() {}

PHPEntityFunction::~PHPEntityFunction() {}

void PHPEntityFunction::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    // Print the indentation
    wxPrintf("%sFunction: %s%s", indentString, GetShortName(), GetSignature());
    wxPrintf(", (%s:%d)\n", GetFilename().GetFullPath(), GetLine());
    if(!m_children.empty()) {
        wxPrintf("%sLocals:\n", indentString);
        PHPEntityBase::List_t::const_iterator iter = m_children.begin();
        for(; iter != m_children.end(); ++iter) {
            (*iter)->PrintStdout(indent + 4);
        }
    }
}

wxString PHPEntityFunction::GetSignature() const
{
    if(!m_strSignature.IsEmpty()) {
        return m_strSignature;
    } else {

        wxString strSignature = "(";
        PHPEntityBase::List_t::const_iterator iter = m_children.begin();
        for(; iter != m_children.end(); ++iter) {
            PHPEntityVariable* var = (*iter)->Cast<PHPEntityVariable>();
            if(var && var->IsFunctionArg()) {
                strSignature << var->ToFuncArgString() << ", ";
            } else {
                break;
            }
        }
        if(strSignature.EndsWith(", ")) { strSignature.RemoveLast(2); }
        strSignature << ")";
        return strSignature;
    }
}

void PHPEntityFunction::Store(PHPLookupTable* lookup)
{
    wxString fullname;
    fullname << GetScope() << "\\" << GetShortName();
    while(fullname.Replace("\\\\", "\\")) {}

    try {
        wxSQLite3Database& db = lookup->Database();
        wxSQLite3Statement statement = db.PrepareStatement(
            "INSERT OR REPLACE INTO FUNCTION_TABLE VALUES(NULL, :SCOPE_ID, :NAME, :FULLNAME, :SCOPE, :SIGNATURE, "
            ":RETURN_VALUE, :FLAGS, :DOC_COMMENT, :LINE_NUMBER, :FILE_NAME)");
        statement.Bind(statement.GetParamIndex(":SCOPE_ID"), Parent()->GetDbId());
        statement.Bind(statement.GetParamIndex(":NAME"), GetShortName());
        statement.Bind(statement.GetParamIndex(":FULLNAME"), fullname);
        statement.Bind(statement.GetParamIndex(":SCOPE"), GetScope());
        statement.Bind(statement.GetParamIndex(":SIGNATURE"), GetSignature());
        statement.Bind(statement.GetParamIndex(":RETURN_VALUE"), GetReturnValue());
        statement.Bind(statement.GetParamIndex(":FLAGS"), (int)GetFlags());
        statement.Bind(statement.GetParamIndex(":DOC_COMMENT"), GetDocComment());
        statement.Bind(statement.GetParamIndex(":LINE_NUMBER"), GetLine());
        statement.Bind(statement.GetParamIndex(":FILE_NAME"), GetFilename().GetFullPath());
        statement.ExecuteUpdate();
        SetDbId(db.GetLastRowId());

    } catch(wxSQLite3Exception& exc) {
        CL_WARNING("PHPEntityFunction::Store: %s", exc.GetMessage());
    }
}

void PHPEntityFunction::FromResultSet(wxSQLite3ResultSet& res)
{
    SetDbId(res.GetInt("ID"));
    SetFullName(res.GetString("FULLNAME"));
    SetShortName(res.GetString("NAME"));
    m_strSignature = res.GetString("SIGNATURE");
    m_strReturnValue = res.GetString("RETURN_VALUE");
    SetFlags(res.GetInt("FLAGS"));
    SetDocComment(res.GetString("DOC_COMMENT"));
    SetLine(res.GetInt("LINE_NUMBER"));
    SetFilename(res.GetString("FILE_NAME"));
}

wxString PHPEntityFunction::GetScope() const
{
    if(Parent()) { return Parent()->GetFullName(); }
    return "";
}

wxString PHPEntityFunction::Type() const { return GetReturnValue(); }
bool PHPEntityFunction::Is(eEntityType type) const { return type == kEntityTypeFunction; }
wxString PHPEntityFunction::GetDisplayName() const { return wxString() << GetShortName() << GetSignature(); }
wxString PHPEntityFunction::FormatPhpDoc(const CommentConfigData& data) const
{
    wxString doc;
    doc << data.GetCommentBlockPrefix() << "\n"
        << " * @brief \n";
    PHPEntityBase::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        const PHPEntityVariable* var = (*iter)->Cast<PHPEntityVariable>();
        if(var) {
            doc << " * @param " << (var->GetTypeHint().IsEmpty() ? "mixed" : var->GetTypeHint()) << " "
                << var->GetFullName() << " \n";
        }
    }
    doc << " * @return " << GetReturnValue() << " \n";
    doc << " */";
    return doc;
}

wxString PHPEntityFunction::GetFullPath() const
{
    wxString fullpath = GetFullName();
    size_t where = fullpath.rfind(GetShortName());
    if(where != wxString::npos) {
        if(where > 0) {
            fullpath = fullpath.Mid(0, where - 1);
            if(fullpath.IsEmpty()) {
                fullpath << "\\";
            } else {
                fullpath << "::";
            }
        }
        fullpath << GetShortName();
    }
    fullpath << GetSignature();
    return fullpath;
}
