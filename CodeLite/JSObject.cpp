#include "JSObject.h"
#include <algorithm>
#include "file_logger.h"
#include <wx/crt.h>
#include "JSLookUpTable.h"

JSObject::JSObject(const JSLookUpTable* lookup)
    : m_line(wxNOT_FOUND)
    , m_parent(NULL)
{
    Extends("Object", lookup);
}

JSObject::~JSObject() {}

void JSObject::Print(int depth)
{
    wxString indent(' ', (size_t)depth);
    CL_DEBUG("%s{[%s] Name: \"%s\", Path: \"%s\"\n", indent, m_type, m_name, m_path);
    if(!m_properties.empty()) {
        std::for_each(m_properties.begin(), m_properties.end(), [&](const std::pair<wxString, JSObject::Ptr_t>& p) {
            (p.second)->Print(depth + 2);
        });
    }
}

const wxString& JSObject::GetType() const
{
    if(m_type.IsEmpty() && IsFunction()) {
        return GetPath();
    } else {
        return m_type;
    }
}

JSObject::Ptr_t JSObject::NewInstance(const wxString& name, const JSLookUpTable* lookup)
{
    JSObject::Ptr_t inst = lookup->NewObject();
    inst->SetName(name);
    inst->SetType(GetType());
    inst->SetPath(GetPath());
    return inst;
}

void JSObject::Extends(const wxString& className, const JSLookUpTable* lookup)
{
    if(!lookup) return;
    JSObject::Ptr_t parent = lookup->FindClass(className);
    if(parent) {
        m_extends.insert(className);
        m_properties.insert(parent->GetProperties().begin(), parent->GetProperties().end());
    }
}

void JSObject::AddProperty(JSObject::Ptr_t child)
{
    std::pair<JSObject::Map_t::iterator, bool> res = m_properties.insert(std::make_pair(child->GetName(), child));
    if(res.second) {
        // success insert
        child->m_parent = this;
    }
}

bool JSObject::IsClass() const
{
    return !m_properties.empty();
}
