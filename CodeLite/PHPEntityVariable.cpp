#include "PHPEntityVariable.h"
#include "PHPScannerTokens.h"
#include "PHPEntityFunction.h"
#include "PHPEntityClass.h"

PHPEntityVariable::PHPEntityVariable()
    : m_flags(0)
{
}

PHPEntityVariable::~PHPEntityVariable() {}
void PHPEntityVariable::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    wxPrintf("%s%s: %s", indentString, Is(kMember) ? "Member" : "Variable", GetName());
    if(!GetTypeHint().IsEmpty()) {
        wxPrintf(", TypeHint: %s", GetTypeHint());
    }
    if(!GetExpressionHint().IsEmpty()) {
        wxPrintf(", ExpressionHint: %s", GetExpressionHint());
    }
    if(IsReference()) {
        wxPrintf(", Reference");
    }
    if(!GetDefaultValue().IsEmpty()) {
        wxPrintf(", Default: %s", GetDefaultValue());
    }

    wxPrintf(", Ln. %d", GetLine());
    wxPrintf("\n");

    PHPEntityBase::Map_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        iter->second->PrintStdout(indent + 4);
    }
}

void PHPEntityVariable::SetVisibility(int visibility)
{
    switch(visibility) {
    case kPHP_T_PUBLIC:
        m_flags &= ~(kPrivate | kProtected);
        m_flags |= kPublic;
        break;
    case kPHP_T_PROTECTED:
        m_flags &= ~(kPrivate | kPublic);
        m_flags |= kProtected;
        break;
    case kPHP_T_PRIVATE:
        m_flags &= ~(kPublic | kProtected);
        m_flags |= kPrivate;
        break;
    default:
        break;
    }
}

wxString PHPEntityVariable::ToFuncArgString() const
{
    if(!Is(kFunctionArg)) {
        return "";
    }

    wxString str;
    if(!GetTypeHint().IsEmpty()) {
        str << GetTypeHint() << " ";
    }

    if(IsReference()) {
        str << "&";
    }

    str << GetName();
    if(!GetDefaultValue().IsEmpty()) {
        str << " = " << GetDefaultValue();
    }
    return str;
}
void PHPEntityVariable::Store(wxSQLite3Database& db)
{
    // we keep only the function arguments in the databse and globals
    if(Is(kFunctionArg) || Is(kMember)) {
        try {
            wxSQLite3Statement statement = db.PrepareStatement("INSERT OR REPLACE INTO VARIABLES_TABLE VALUES (NULL, "
                                                               ":SCOPE_ID, :FUNCTION_ID, :NAME, :SCOPE, :TYPEHINT, "
                                                               ":FLAGS, :DOC_COMMENT, :LINE_NUMBER, :FILE_NAME)");
            statement.Bind(statement.GetParamIndex(":SCOPE_ID"), Is(kMember) ? Parent()->GetDbId() : wxLongLong(-1));
            statement.Bind(statement.GetParamIndex(":FUNCTION_ID"),
                           Is(kFunctionArg) ? Parent()->GetDbId() : wxLongLong(-1));
            statement.Bind(statement.GetParamIndex(":NAME"), GetName());
            statement.Bind(statement.GetParamIndex(":SCOPE"), GetScope());
            statement.Bind(statement.GetParamIndex(":TYPEHINT"), GetTypeHint());
            statement.Bind(statement.GetParamIndex(":FLAGS"), (int)GetFlags());
            statement.Bind(statement.GetParamIndex(":DOC_COMMENT"), GetDocComment());
            statement.Bind(statement.GetParamIndex(":LINE_NUMBER"), GetLine());
            statement.Bind(statement.GetParamIndex(":FILE_NAME"), GetFilename().GetFullPath());
            statement.ExecuteUpdate();
            SetDbId(db.GetLastRowId());

        } catch(wxSQLite3Exception& exc) {
            wxUnusedVar(exc);
        }
    }
}

wxString PHPEntityVariable::GetScope() const
{
    if(Is(kFunctionArg) && Parent()) {
        return Parent()->Cast<PHPEntityFunction>()->GetScope();

    } else if(Is(kMember) && Parent()) {
        return Parent()->Cast<PHPEntityClass>()->GetName();

    } else {
        return "";
    }
}
