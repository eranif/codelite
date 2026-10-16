#include "php_configuration_data.h"

#include "cl_config.h"
#include "json_utils.h"

PHPConfigurationData::PHPConfigurationData()
    : clConfigItem("PHPConfigurationData")
    , m_xdebugPort(9003)
    , m_xdebugIdeKey("codeliteide")
    , m_xdebugHost("127.0.0.1")
{
    m_phpOptions.Load();
}

void PHPConfigurationData::FromJSON(const JSONItem& json)
{
    m_xdebugPort = json.namedObject("m_xdebugPort").toInt(m_xdebugPort);
    m_xdebugHost = json.namedObject("m_xdebugHost").toString(m_xdebugHost);
    m_flags = json.namedObject("m_flags").toSize_t(m_flags);
    m_xdebugIdeKey = json.namedObject("m_xdebugIdeKey").toString(m_xdebugIdeKey);
    m_xdebugIdeKey.Trim().Trim(false);

    // xdebug IDE can not be an empty string, or else debugging in command line
    // will not work
    if (m_xdebugIdeKey.IsEmpty()) {
        m_xdebugIdeKey = "codeliteide";
    }
}

JSONItem PHPConfigurationData::ToJSON() const
{
    return nlohmann::json{{"m_xdebugPort", m_xdebugPort},
                          {"m_xdebugHost", m_xdebugHost.ToStdString(wxConvUTF8)},
                          {"m_flags", m_flags},
                          {"m_xdebugIdeKey", m_xdebugIdeKey.ToStdString(wxConvUTF8)}};
}

wxString PHPConfigurationData::GetIncludePathsAsString() const
{
    wxString str;
    for (size_t i = 0; i < GetIncludePaths().GetCount(); i++) {
        str << GetIncludePaths().Item(i) << wxT("\n");
    }
    if (str.IsEmpty() == false) {
        str.RemoveLast();
    }
    return str;
}

PHPConfigurationData& PHPConfigurationData::Load()
{
    clConfig conf("php.conf");
    conf.ReadItem(*this);

    m_phpOptions.Load();
    return *this;
}

void PHPConfigurationData::Save()
{
    clConfig conf("php.conf");
    conf.WriteItem(*this);

    m_phpOptions.Save();
}
