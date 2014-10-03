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
    wxPrintf("%s%s: %s", indentString, HasFlag(kMember) ? "Member" : "Variable", GetName());
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

    PHPEntityBase::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        (*iter)->PrintStdout(indent + 4);
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
    if(!HasFlag(kFunctionArg)) {
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
    if(HasFlag(kFunctionArg) || HasFlag(kMember)) {
        try {
            wxSQLite3Statement statement = db.PrepareStatement("INSERT OR REPLACE INTO VARIABLES_TABLE VALUES (NULL, "
                                                               ":SCOPE_ID, :FUNCTION_ID, :NAME, :SCOPE, :TYPEHINT, "
                                                               ":FLAGS, :DOC_COMMENT, :LINE_NUMBER, :FILE_NAME)");
            statement.Bind(statement.GetParamIndex(":SCOPE_ID"),
                           HasFlag(kMember) ? Parent()->GetDbId() : wxLongLong(-1));
            statement.Bind(statement.GetParamIndex(":FUNCTION_ID"),
                           HasFlag(kFunctionArg) ? Parent()->GetDbId() : wxLongLong(-1));
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

void PHPEntityVariable::FromResultSet(wxSQLite3ResultSet& res)
{
    SetDbId(res.GetInt("ID"));
    SetName(res.GetString("NAME"));
    SetTypeHint(res.GetString("TYPEHINT"));
    SetFlags(res.GetInt("FLAGS"));
    SetDocComment(res.GetString("DOC_COMMENT"));
    SetLine(res.GetInt("LINE_NUMBER"));
    SetFilename(res.GetString("FILE_NAME"));
}

wxString PHPEntityVariable::GetScope() const
{
    PHPEntityBase* parent = Parent();
    if(parent && parent->Is(kEntityTypeFunction) && HasFlag(kFunctionArg)) {
        return parent->Cast<PHPEntityFunction>()->GetScope();

    } else if(parent && parent->Is(kEntityTypeClass) && HasFlag(kMember)) {
        return parent->Cast<PHPEntityClass>()->GetName();

    } else {
        return "";
    }
}

wxString PHPEntityVariable::Type() const { return GetTypeHint(); }
bool PHPEntityVariable::Is(eEntityType type) const { return type == kEntityTypeVariable; }
wxString PHPEntityVariable::GetDisplayName() const { return GetName(); }
wxString PHPEntityVariable::GetNameOnly() const { return GetName(); }

wxString PHPEntityVariable::GetNameNoDollar() const
{
    wxString name = GetName();
    if(name.StartsWith("$")) {
        name.RemoveLast();
    }
    name.Trim().Trim(false);
    return name;
}
wxString PHPEntityVariable::FormatPhpDoc() const 
{
    wxString doc;
    doc << "/**\n"
        << " * @var " << GetTypeHint() << "\n"
        << " */";
    return doc;
}
