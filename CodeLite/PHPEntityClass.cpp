#include "PHPEntityClass.h"

PHPEntityClass::PHPEntityClass() {}

PHPEntityClass::~PHPEntityClass() {}
wxString PHPEntityClass::ID() const { return GetName(); }

void PHPEntityClass::PrintStdout(int indent) const 
{
    wxString indentString(' ', indent);
    wxPrintf("%sClass name: %s", indentString, GetName());
    if(!GetExtends().IsEmpty()) {
        wxPrintf(", extends %s", GetExtends());
    }
    if(!GetImplements().IsEmpty()) {
        wxPrintf(", implements: ");
        for(size_t i=0; i<GetImplements().GetCount(); ++i) {
            wxPrintf("%s ", GetImplements().Item(i));
        }
    }
    
    wxPrintf("\n");
    PHPEntityBase::Map_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        iter->second->PrintStdout(indent+4);
    }
}
