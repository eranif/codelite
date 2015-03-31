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
    wxPrintf("%s{[%s] Name: \"%s\", Type: \"%s\"\n", indent, GetTypesAsString(), m_name, GetType());
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

wxString JSObject::GetType() const
{
    if(HasFlag(kJS_TypeSetByDocComment)) return m_type;
    if(IsClass()) return GetName();
    return m_type;
}

void JSObject::AddVariable(JSObject::Ptr_t var)
{
    JSObject::Map_t::iterator iter = m_variables.find(var->GetName());
    if(iter != m_variables.end()) m_variables.erase(iter);

    m_variables.insert(std::make_pair(var->GetName(), var));
}
