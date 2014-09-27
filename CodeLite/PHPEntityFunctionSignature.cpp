#include "PHPEntityFunctionSignature.h"

PHPEntityFunctionSignature::PHPEntityFunctionSignature() {}

PHPEntityFunctionSignature::~PHPEntityFunctionSignature() {}

wxString PHPEntityFunctionSignature::ID() const { return ""; }
void PHPEntityFunctionSignature::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    wxPrintf("%sSignature:\n", indentString);
    PHPEntityBase::Map_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        iter->second->PrintStdout(indent + 4);
    }
}
