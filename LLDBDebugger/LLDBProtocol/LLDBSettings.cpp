#include "LLDBSettings.h"
#include <wx/filename.h>
#include "cl_standard_paths.h"
#include <wx/ffile.h>

#ifdef __WXMAC__
    static const wxString s_DefaultTypes =
    "type summary add wxString --summary-string \"${var.m_impl}\"\n"
    "type summary add wxPoint --summary-string \"x = ${var.x}, y = ${var.y}\"\n"
    "type summary add wxRect --summary-string \"(x = ${var.x}, y = ${var.y}) (width = ${var.width}, height = ${var.height})\"\n";
#else
    static const wxString s_DefaultTypes =
    "type summary add wxString --summary-string \"${var.m_impl._M_dataplus._M_p}\"\n"
    "type summary add wxPoint --summary-string \"x = ${var.x}, y = ${var.y}\"\n"
    "type summary add wxRect --summary-string \"(x = ${var.x}, y = ${var.y}) (width = ${var.width}, height = ${var.height})\"\n";
#endif

LLDBSettings::LLDBSettings()
    : m_arrItems(50)
    , m_flags(kLLDBOptionRaiseCodeLite)
    , m_stackFrames(100)
    , m_proxyIp("127.0.0.1")
    , m_proxyPort(13610)
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
    m_proxyPort = json.namedObject("m_proxyPort").toInt(13610);
    m_proxyIp = json.namedObject("m_proxyIp").toString("127.0.0.1");
    m_lastLocalFolder = json.namedObject("m_lastLocalFolder").toString();
    m_lastRemoteFolder = json.namedObject("m_lastRemoteFolder").toString();
}

JSONElement LLDBSettings::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_maxArrayElements", m_arrItems);
    json.addProperty("m_maxCallstackFrames", m_stackFrames);
    json.addProperty("m_flags", m_flags);
    json.addProperty("m_types", m_types);
    json.addProperty("m_proxyPort", m_proxyPort);
    json.addProperty("m_proxyIp", m_proxyIp);
    json.addProperty("m_lastLocalFolder", m_lastLocalFolder);
    json.addProperty("m_lastRemoteFolder", m_lastRemoteFolder);
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
