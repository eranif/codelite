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
    if(!Is(kPrivate) && !Is(kProtected)) {
        m_flags |= kPublic;
    }
}

void PHPEntityFunction::PrintStdout(int indent) const {
    wxString indentString(' ', indent);
    // Print the indentation
    wxPrintf("%sFunction: %s%s", indentString, GetName(), FormatSignature());
    wxPrintf(", Ln. %d\n", GetLine());
    wxPrintf("%sLocals:\n", indentString);
    PHPEntityBase::Map_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        iter->second->PrintStdout(indent+4);
    }
}

wxString PHPEntityFunction::FormatSignature() const
{
    wxString signature = "(";
    for(size_t i=0; i<m_childrenVec.size(); ++i) {
        PHPEntityVariable* var = m_childrenVec.at(i)->Cast<PHPEntityVariable>();
        if(var && var->Is(PHPEntityVariable::kFunctionArg) ) {
            signature << var->ToFuncArgString() << ", ";
        } else {
            break;
        }
    }
    if(signature.EndsWith(", ")) {
        signature.RemoveLast(2);
    }
    signature << ")";
    return signature;
}

void PHPEntityFunction::AddChild(PHPEntityBase::Ptr_t child)
{
    // add it to the parent map
    PHPEntityBase::AddChild(child); 
    
    // keep a copy in the vector (which preserves the order of adding them)
    m_childrenVec.push_back(child);
}
