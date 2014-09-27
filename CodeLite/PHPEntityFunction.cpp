#include "PHPEntityFunction.h"

PHPEntityFunction::PHPEntityFunction()
    : m_flags(0)
{
}

PHPEntityFunction::~PHPEntityFunction() {}

wxString PHPEntityFunction::ID() const
{
    wxString ID;
    if(m_parent) {
        ID << m_parent->ID() << "\\";
    }
    ID << GetName();
    return ID;
}

void PHPEntityFunction::SetFlags(size_t flags)
{
    this->m_flags = flags;
    if(!Is(kPrivate) && !Is(kProtected)) {
        m_flags |= kPublic;
    }
}

void PHPEntityFunction::PrintStdout(int indent) const {
    wxString indentString(' ', indent);
    wxPrintf("%sFunction name: %s", indentString, GetName());
    wxPrintf(", Ln. %d\n", GetLine());
    m_signature->PrintStdout(indent+4);
    
    PHPEntityBase::Map_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        iter->second->PrintStdout(indent+4);
    }
}
