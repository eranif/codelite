#include "PHPEntityVariable.h"
#include "PHPScannerTokens.h"

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
