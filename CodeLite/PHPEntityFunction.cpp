#include "PHPEntityFunction.h"
#include "PHPEntityVariable.h"

PHPEntityFunction::PHPEntityFunction()
    : m_flags(0)
{
}

PHPEntityFunction::~PHPEntityFunction() {}

void PHPEntityFunction::SetFlags(size_t flags)
{
    this->m_flags = flags;
    if(!HasFlag(kPrivate) && !HasFlag(kProtected)) {
        m_flags |= kPublic;
    }
}

void PHPEntityFunction::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    // Print the indentation
    wxPrintf("%sFunction: %s%s", indentString, GetName(), GetSignature());
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
            if(var && var->HasFlag(PHPEntityVariable::kFunctionArg)) {
                strSignature << var->ToFuncArgString() << ", ";
            } else {
                break;
            }
        }
        if(strSignature.EndsWith(", ")) {
            strSignature.RemoveLast(2);
        }
        strSignature << ")";
        return strSignature;
    }
}

void PHPEntityFunction::Store(wxSQLite3Database& db)
{
    try {
        wxSQLite3Statement statement = db.PrepareStatement(
            "INSERT OR REPLACE INTO FUNCTION_TABLE VALUES(NULL, :SCOPE_ID, :NAME, :SCOPE, :SIGNATURE, "
            ":RETURN_VALUE, :FLAGS, :DOC_COMMENT, :LINE_NUMBER, :FILE_NAME)");
        statement.Bind(statement.GetParamIndex(":SCOPE_ID"), Parent()->GetDbId());
        statement.Bind(statement.GetParamIndex(":NAME"), GetName());
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
        wxPrintf("error: %s\n", exc.GetMessage());
        wxUnusedVar(exc);
    }
}
void PHPEntityFunction::FromResultSet(wxSQLite3ResultSet& res)
{
    SetDbId(res.GetInt("ID"));
    SetName(res.GetString("NAME"));
    m_strSignature = res.GetString("SIGNATURE");
    m_strReturnValue = res.GetString("RETURN_VALUE");
    SetFlags(res.GetInt("FLAGS"));
    SetDocComment(res.GetString("DOC_COMMENT"));
    SetLine(res.GetInt("LINE_NUMBER"));
    SetFilename(res.GetString("FILE_NAME"));
}

wxString PHPEntityFunction::GetScope() const
{
    if(Parent()) {
        return Parent()->GetName();
    }
    return "";
}

wxString PHPEntityFunction::Type() const { return GetReturnValue(); }
bool PHPEntityFunction::Is(eEntityType type) const { return type == kEntityTypeFunction; }
wxString PHPEntityFunction::GetDisplayName() const { return wxString() << GetName() << GetSignature(); }
wxString PHPEntityFunction::GetNameOnly() const { return GetName(); }

wxString PHPEntityFunction::FormatPhpDoc() const
{
    wxString doc;
    doc << "/**\n"
        << " * @brief \n";
    PHPEntityBase::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        const PHPEntityVariable* var = (*iter)->Cast<PHPEntityVariable>();
        doc << " * @param " << (var->GetTypeHint().IsEmpty() ? "<unknown>" : var->GetTypeHint()) << " "
            << var->GetName() << " \n";
    }
    doc << " * @return " << GetReturnValue() << " \n";
    doc << " */";
    return doc;
}
