#include "PHPEntityNamespace.h"

PHPEntityNamespace::PHPEntityNamespace() {}

PHPEntityNamespace::~PHPEntityNamespace() {}
void PHPEntityNamespace::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    wxPrintf("%sNamespace name: %s\n", indentString, GetPath());
    
    PHPEntityBase::Map_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        iter->second->PrintStdout(indent + 4);
    }
}
