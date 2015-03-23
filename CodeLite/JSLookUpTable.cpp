#include "JSLookUpTable.h"
#include <algorithm>

JSLookUpTable::JSLookUpTable()
{
    m_scopes = &m_actualScopes;

    JSObject::Ptr_t globalScope(new JSObject());
    m_scopes->push_back(globalScope);
}

JSLookUpTable::~JSLookUpTable() {}

void JSLookUpTable::AddObject(JSObject::Ptr_t obj)
{
    std::map<wxString, JSObject::Ptr_t>::iterator iter = m_objects.find(obj->GetPath());
    if(iter != m_objects.end()) m_objects.erase(iter);

    m_objects.insert(std::make_pair(obj->GetPath(), obj));
}

JSObject::Ptr_t JSLookUpTable::CurrentScope() const { return m_scopes->at(m_scopes->size() - 1); }

const wxString& JSLookUpTable::CurrentPath() const { return CurrentScope()->GetPath(); }

void JSLookUpTable::PopScope()
{
    if(m_scopes->size() > 1) m_scopes->pop_back();
}

void JSLookUpTable::PushScope(JSObject::Ptr_t scope) { m_scopes->push_back(scope); }

wxString JSLookUpTable::MakePath(const wxString& pathLastPart)
{
    wxString curpath = CurrentPath();
    if(curpath.IsEmpty()) {
        return pathLastPart;
    } else {
        curpath << "." << pathLastPart;
    }
    return curpath;
}

JSObject::Ptr_t JSLookUpTable::FindObject(const wxString& path) const
{
    std::map<wxString, JSObject::Ptr_t>::const_iterator iter = m_objects.find(path);
    if(iter != m_objects.end()) return iter->second;
    return NULL;
}

void JSLookUpTable::SetTempScope(JSObject::Ptr_t obj)
{
    if(obj) {
        m_tempScopes.clear();
        m_tempScopes.push_back(obj);
        m_scopes = &m_tempScopes;
    } else {
        SwapScopes();
    }
}

void JSLookUpTable::SwapScopes() { m_scopes = &m_actualScopes; }

void JSLookUpTable::Print()
{
    std::for_each(m_objects.begin(), m_objects.end(), [&](const std::pair<wxString, JSObject::Ptr_t>& p) {
        p.second->Print(0);
    });
}
