#include "JSLookUpTable.h"
#include <algorithm>
#include "JSFunction.h"

JSLookUpTable::JSLookUpTable()
    : m_objSeed(0)
{
    m_scopes = &m_actualScopes;

    JSObject::Ptr_t globalScope(new JSFunction());
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

void JSLookUpTable::PushScope(JSObject::Ptr_t scope)
{
    if(scope && scope->IsFunction()) {
        m_scopes->push_back(scope);
    }
}

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
    // only if the new scope is a function we use it
    if(obj && obj->IsFunction()) {
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
    std::for_each(
        m_objects.begin(), m_objects.end(), [&](const std::pair<wxString, JSObject::Ptr_t>& p) { p.second->Print(0); });
}

JSObject::Map_t JSLookUpTable::GetVisibleVariables()
{
    // we start from the inner scope so incase we have a collision
    // the inner scope variable "wins"
    JSObject::Map_t variables;
    for(int i = (int)m_scopes->size() - 1; i >= 0; --i) {
        const JSObject::Map_t& scopeVariables = m_scopes->at(i)->As<JSFunction>()->GetVariables();
        variables.insert(scopeVariables.begin(), scopeVariables.end());
    }
    return variables;
}

wxString JSLookUpTable::GenerateNewType()
{
    wxString type;
    type << "__object" << ++m_objSeed;
    return type;
}
