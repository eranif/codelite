#include "LLDBSettings.h"
#include <wx/filename.h>
#include "cl_standard_paths.h"
#include <wx/ffile.h>

static const wxString s_DefaultTypes =
"type summary add wxString --summary-string \"${var.m_impl._M_dataplus._M_p}\"\n"
"type summary add wxPoint --summary-string \"x = ${var.x}, y = ${var.y}\"\n"
"type summary add wxRect --summary-string \"(x = ${var.x}, y = ${var.y}) (width = ${var.width}, height = ${var.height})\"\n";

LLDBSettings::LLDBSettings()
    : m_arrItems(50)
    , m_flags(0)
    , m_stackFrames(100)
{
    m_types = s_DefaultTypes;
}

LLDBSettings::~LLDBSettings()
{
}

wxString LLDBSettings::LoadPerspective()
{
    wxFileName fn( clStandardPaths::Get().GetUserDataDir(), "lldb.perspective");
    fn.AppendDir("config");
    
    wxFFile fp(fn.GetFullPath(), "rb");
    if ( fp.IsOpened() ) {
        wxString content;
        fp.ReadAll( &content, wxConvUTF8 );
        return content;
    }
    return "";
}

void LLDBSettings::SavePerspective(const wxString &perspective)
{
    wxFileName fn( clStandardPaths::Get().GetUserDataDir(), "lldb.perspective");
    fn.AppendDir("config");
    
    wxFFile fp(fn.GetFullPath(), "w+b");
    if ( fp.IsOpened() ) {
        fp.Write( perspective );
        fp.Close();
    }
}

void LLDBSettings::FromJSON(const JSONElement& json)
{
    m_arrItems = json.namedObject("m_maxArrayElements").toSize_t(m_arrItems);
    m_stackFrames = json.namedObject("m_maxCallstackFrames").toSize_t(m_stackFrames);
    m_flags = json.namedObject("m_flags").toSize_t(m_flags);
    m_types = json.namedObject("m_types").toString(s_DefaultTypes);
}

JSONElement LLDBSettings::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_maxArrayElements", m_arrItems);
    json.addProperty("m_maxCallstackFrames", m_stackFrames);
    json.addProperty("m_flags", m_flags);
    json.addProperty("m_types", m_types);
    return json;
}

LLDBSettings& LLDBSettings::Load()
{
    wxFileName fn( clStandardPaths::Get().GetUserDataDir(), "lldb.conf");
    fn.AppendDir("config");
     wxFFile fp(fn.GetFullPath(), "rb");
    if ( fp.IsOpened() ) {
        wxString content;
        fp.ReadAll( &content, wxConvUTF8 );
        
        JSONRoot root(content);
        FromJSON( root.toElement() );
        fp.Close();
    }
    return *this;
}

LLDBSettings& LLDBSettings::Save()
{
    wxFileName fn( clStandardPaths::Get().GetUserDataDir(), "lldb.conf");
    fn.AppendDir("config");
    
    wxFFile fp(fn.GetFullPath(), "w+b");
    if ( fp.IsOpened() ) {
        fp.Write( ToJSON().format() );
        fp.Close();
    }
    return *this;
}
