#include "JSLookUpTable.h"

JSLookUpTable::JSLookUpTable()
{
}

JSLookUpTable::~JSLookUpTable()
{
}

void JSLookUpTable::AddObject(JSObject::Ptr_t obj)
{
    std::map<wxString, JSObject::Ptr_t>::iterator iter = m_objects.find(obj->GetPath());
    if(iter != m_objects.end()) m_objects.erase(iter);
    
    m_objects.insert(std::make_pair(obj->GetPath(), obj));
}
