#include "LLDBSettings.h"
#include <wx/filename.h>
#include "cl_standard_paths.h"

LLDBSettings::LLDBSettings()
    : clConfigItem("lldb")
    , m_flags(kBreakOnFirstEntry)
{
}

LLDBSettings::~LLDBSettings()
{
}

void LLDBSettings::FromJSON(const JSONElement& json)
{
    m_flags = json.namedObject("m_flags").toSize_t();
}

JSONElement LLDBSettings::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_flags", m_flags);
    return element;
}

LLDBSettings& LLDBSettings::Load()
{
    clConfig conf("lldb.conf");
    conf.ReadItem( this );
    return *this;
}

LLDBSettings& LLDBSettings::Save()
{
    clConfig conf("lldb.conf");
    conf.WriteItem( this );
    return *this;
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
