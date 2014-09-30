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
    wxPrintf(", Ln. %d\n", GetLine());
    wxPrintf("%sLocals:\n", indentString);
    PHPEntityBase::Map_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        iter->second->PrintStdout(indent + 4);
    }
}

wxString PHPEntityFunction::GetSignature() const
{
    if(!m_strSignature.IsEmpty()) {
        return m_strSignature;
    } else {

        wxString strSignature = "(";
        for(size_t i = 0; i < m_childrenVec.size(); ++i) {
            PHPEntityVariable* var = m_childrenVec.at(i)->Cast<PHPEntityVariable>();
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

void PHPEntityFunction::AddChild(PHPEntityBase::Ptr_t child)
{
    // add it to the parent map
    PHPEntityBase::AddChild(child);

    // keep a copy in the vector (which preserves the order of adding them)
    m_childrenVec.push_back(child);
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
        statement.Bind(statement.GetParamIndex(":RETURN_VALUE"), m_returnValue ? m_returnValue->GetName() : wxString());
        statement.Bind(statement.GetParamIndex(":FLAGS"), (int)GetFlags()); // TODO: implement this
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

void PHPEntityFunction::StoreRecursive(wxSQLite3Database& db)
{
    Store(db);
    for(size_t i = 0; i < m_childrenVec.size(); ++i) {
        m_childrenVec.at(i)->StoreRecursive(db);
    }
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
