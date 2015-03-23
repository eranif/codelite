#include "JSObject.h"
#include <algorithm>
#include "file_logger.h"

JSObject::JSObject()
    : m_line(wxNOT_FOUND)
{
}

JSObject::~JSObject() {}

void JSObject::Print(int depth)
{
    wxString indent(' ', (size_t)depth);
    CL_DEBUG("%s{[Object] Name: \"%s\", Path: \"%s\". Doc: \"%s\"", indent, m_name, m_path, m_comment);
    if(!m_properties.empty()) {
        std::for_each(m_properties.begin(), m_properties.end(), [&](const std::pair<wxString, JSObject::Ptr_t>& p) {
            (p.second)->Print(depth + 2);
        });
    }
}
