#include "PHPEntityVariable.h"
#include "PHPScannerTokens.h"
#include "PHPEntityFunction.h"
#include "PHPEntityClass.h"
#include "PHPLookupTable.h"

PHPEntityVariable::PHPEntityVariable() {}

PHPEntityVariable::~PHPEntityVariable() {}

void PHPEntityVariable::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    wxPrintf("%s%s: %s", indentString, IsMember() ? "Member" : "Variable", GetShortName());
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
        m_flags &= ~kVar_Private;
        m_flags &= ~kVar_Protected;
        m_flags |= kVar_Public;
        break;
    case kPHP_T_PROTECTED:
        m_flags &= ~kVar_Private;
        m_flags &= ~kVar_Public;
        m_flags |= kVar_Protected;
        break;
    case kPHP_T_PRIVATE:
        m_flags &= ~kVar_Public;
        m_flags &= ~kVar_Protected;
        m_flags |= kVar_Private;
        break;
    default:
        break;
    }
}

wxString PHPEntityVariable::ToFuncArgString() const
{
    if(!IsFunctionArg()) {
        return "";
    }

    wxString str;
    if(!GetTypeHint().IsEmpty()) {
        str << GetTypeHint() << " ";
    }

    if(IsReference()) {
        str << "&";
    }

    str << GetShortName();
    if(!GetDefaultValue().IsEmpty()) {
        str << " = " << GetDefaultValue();
    }
    return str;
}
void PHPEntityVariable::Store(PHPLookupTable* lookup)
{
    if(IsFunctionArg() || IsMember() || IsDefine()) {
        try {
            wxSQLite3Database& db = lookup->Database();
            wxSQLite3Statement statement = db.PrepareStatement(
                "INSERT OR REPLACE INTO VARIABLES_TABLE VALUES (NULL, "
                ":SCOPE_ID, :FUNCTION_ID, :NAME, :FULLNAME, :SCOPE, :TYPEHINT, :DEFAULT_VALUE, "
                ":FLAGS, :DOC_COMMENT, :LINE_NUMBER, :FILE_NAME)");
            wxLongLong functionId, scopeId;
            if(IsFunctionArg()) {
                functionId = Parent()->GetDbId();
            } else {
                functionId = -1;
            }

            if(IsMember() || IsDefine()) {
                scopeId = Parent()->GetDbId();
            } else {
                scopeId = -1;
            }
            statement.Bind(statement.GetParamIndex(":SCOPE_ID"), scopeId);
            statement.Bind(statement.GetParamIndex(":FUNCTION_ID"), functionId);
            statement.Bind(statement.GetParamIndex(":NAME"), GetShortName());
            statement.Bind(statement.GetParamIndex(":FULLNAME"), GetFullName());
            statement.Bind(statement.GetParamIndex(":SCOPE"), GetScope());
            statement.Bind(statement.GetParamIndex(":TYPEHINT"), GetTypeHint());
            statement.Bind(statement.GetParamIndex(":DEFAULT_VALUE"), GetDefaultValue());
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
    SetFullName(res.GetString("FULLNAME"));
    SetShortName(res.GetString("NAME"));
    SetTypeHint(res.GetString("TYPEHINT"));
    SetFlags(res.GetInt("FLAGS"));
    SetDocComment(res.GetString("DOC_COMMENT"));
    SetLine(res.GetInt("LINE_NUMBER"));
    SetFilename(res.GetString("FILE_NAME"));
    SetDefaultValue(res.GetString("DEFAULT_VALUE"));
}

wxString PHPEntityVariable::GetScope() const
{
    PHPEntityBase* parent = Parent();
    if(parent && parent->Is(kEntityTypeFunction) && IsFunctionArg()) {
        return parent->Cast<PHPEntityFunction>()->GetScope();

    } else if(parent && parent->Is(kEntityTypeClass) && IsMember()) {
        return parent->GetFullName();

    } else if(parent && parent->Is(kEntityTypeNamespace) && IsDefine()) {
        return parent->GetFullName();

    } else {
        return "";
    }
}

wxString PHPEntityVariable::Type() const { return GetTypeHint(); }
bool PHPEntityVariable::Is(eEntityType type) const { return type == kEntityTypeVariable; }
wxString PHPEntityVariable::GetDisplayName() const { return GetFullName(); }

wxString PHPEntityVariable::GetNameNoDollar() const
{
    wxString name = GetShortName();
    if(name.StartsWith("$")) {
        name.Remove(0, 1);
    }
    name.Trim().Trim(false);
    return name;
}
wxString PHPEntityVariable::FormatPhpDoc(const CommentConfigData& data) const
{
    wxString doc;
    doc << data.GetCommentBlockPrefix() << "\n"
        << " * @var " << GetTypeHint() << "\n"
        << " */";
    return doc;
}

wxString PHPEntityVariable::ToTooltip() const
{
    if(IsConst() && !GetDefaultValue().IsEmpty()) {
        return GetDefaultValue();
    } else {
        return wxEmptyString;
    }
}
