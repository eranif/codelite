#include "JSObject.h"
#include <algorithm>
#include "file_logger.h"
#include <wx/crt.h>

JSObject::JSObject()
    : m_line(wxNOT_FOUND)
{
}

JSObject::~JSObject() {}

void JSObject::Print(int depth)
{
    wxString indent(' ', (size_t)depth);
    wxPrintf("%s{[%s] Name: \"%s\", Path: \"%s\"\n", indent, m_type, m_name, m_path);
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

JSObject::Ptr_t JSObject::NewInstance(const wxString& name)
{
    JSObject::Ptr_t inst(new JSObject());
    inst->SetName(name);
    inst->SetType(GetType());
    inst->SetPath(GetPath());
    return inst;
}
