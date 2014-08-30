#include "php_configuration_data.h"
#include "php_utils.h"
#include <wx/ffile.h>
#include <wx/stdpaths.h>
#include "php_utils.h"
#include <cl_config.h>

PHPConfigurationData::PHPConfigurationData()
    : clConfigItem("PHPConfigurationData")
    , m_errorReporting(wxT("E_ALL & ~E_NOTICE"))
    , m_xdebugPort(9000)
    , m_flags(0x0)
{
    if ( m_ccIncludePath.IsEmpty() ) {
        m_ccIncludePath.Add( ::GetCCResourceDirectory() );
    }
}

PHPConfigurationData::~PHPConfigurationData()
{
}

void PHPConfigurationData::FromJSON(const JSONElement &json)
{
    m_includePaths = json.namedObject("m_includePaths").toArrayString();
    m_phpExe = json.namedObject("m_phpExe").toString("php");
    m_errorReporting = json.namedObject("m_errorReporting").toString("E_ALL & ~E_NOTICE");
    m_xdebugPort = json.namedObject("m_xdebugPort").toInt(9000);
    m_flags = json.namedObject("m_flags").toSize_t();
    m_xdebugIdeKey = json.namedObject("m_xdebugIdeKey").toString("phplite");
    bool nodeExists = json.hasNamedObject("m_ccIncludePath");
    m_ccIncludePath = json.namedObject("m_ccIncludePath").toArrayString();
    
    if ( !nodeExists && m_ccIncludePath.IsEmpty() ) {
        m_ccIncludePath.Add( ::GetCCResourceDirectory() );
    }
}

JSONElement PHPConfigurationData::ToJSON() const
{
    JSONElement e = JSONElement::createObject(GetName());
    e.addProperty("m_includePaths", m_includePaths);
    e.addProperty("m_phpExe", m_phpExe);
    e.addProperty("m_errorReporting", m_errorReporting);
    e.addProperty("m_xdebugPort", m_xdebugPort);
    e.addProperty("m_ccIncludePath", m_ccIncludePath);
    e.addProperty("m_flags", m_flags);
    e.addProperty("m_xdebugIdeKey", m_xdebugIdeKey);
    return e;
}

wxString PHPConfigurationData::GetIncludePathsAsString() const
{
    wxString str;
    for(size_t i=0; i<m_includePaths.GetCount(); i++) {
        str << m_includePaths.Item(i) << wxT("\n");
    }
    if(str.IsEmpty() == false) {
        str.RemoveLast();
    }
    return str;
}

wxString PHPConfigurationData::GetCCIncludePathsAsString() const
{
    wxString str;
    for(size_t i=0; i<m_ccIncludePath.GetCount(); i++) {
        str << m_ccIncludePath.Item(i) << wxT("\n");
    }
    if(str.IsEmpty() == false) {
        str.RemoveLast();
    }
    return str;
}

PHPConfigurationData& PHPConfigurationData::Load()
{
    clConfig conf("php.conf");
    conf.ReadItem( this );
    return *this;
}

void PHPConfigurationData::Save()
{
    clConfig conf("php.conf");
    conf.WriteItem( this );
}
