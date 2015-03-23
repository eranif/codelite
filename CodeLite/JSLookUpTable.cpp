#include "JSLookUpTable.h"

JSLookUpTable::JSLookUpTable()
    : m_globalScope(new JSObject())
{
    m_globalScope->SetName("__GLOBAL__");
    m_globalScope->SetPath("__GLOBAL__");
}

JSLookUpTable::~JSLookUpTable() {}

void JSLookUpTable::AddObject(JSObject::Ptr_t obj)
{
    std::map<wxString, JSObject::Ptr_t>::iterator iter = m_objects.find(obj->GetPath());
    if(iter != m_objects.end()) m_objects.erase(iter);

    m_objects.insert(std::make_pair(obj->GetPath(), obj));
}
