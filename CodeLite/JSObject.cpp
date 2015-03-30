#include "JSObject.h"
#include <algorithm>
#include "file_logger.h"
#include <wx/crt.h>
#include "JSLookUpTable.h"

JSObject::JSObject()
    : m_line(wxNOT_FOUND)
    , m_parent(NULL)
    , m_flags(0)
{
    Extends("Object");
}

JSObject::~JSObject() {}

void JSObject::Print(int depth)
{
    wxString indent(' ', (size_t)depth);
    CL_DEBUG("%s{[%s] Name: \"%s\", Path: \"%s\"\n", indent, GetTypesAsString(), m_name, m_path);
    if(!m_properties.empty()) {
        std::for_each(m_properties.begin(), m_properties.end(), [&](const std::pair<wxString, JSObject::Ptr_t>& p) {
            (p.second)->Print(depth + 2);
        });
    }
}

JSObject::Ptr_t JSObject::NewInstance(const wxString& name)
{
    JSObject::Ptr_t inst(new JSObject());
    inst->SetName(name);
    inst->SetType(GetType());
    inst->SetPath(GetPath());
    return inst;
}

void JSObject::Extends(const wxString& className)
{
    m_extends.insert(className);
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
