#include "phpoptions.h"

#include "cl_standard_paths.h"
#include "fileutils.h"
#include "json_utils.h"

#include <wx/filename.h>

PhpOptions::PhpOptions()
    : clConfigItem("PHPConfigurationData")
    , m_phpExe("")
{
    wxFileName newConfigFile = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
                               wxFileName::GetPathSeparator() + "php-general.conf";
    if (!newConfigFile.FileExists()) {
        wxFileName oldConfigFile = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
                                   wxFileName::GetPathSeparator() + "php.conf";
        // first time, copy the values from the old settings
        JSON root(oldConfigFile);
        JSONItem oldJson = root.toElement().namedObject("PHPConfigurationData");

        m_phpExe = oldJson.namedObject("m_phpExe").toString();
        if (m_phpExe.empty()) {
            if (const auto fnPHP = ::FileUtils::FindExe("php")) {
                m_phpExe = fnPHP->GetFullPath();
            }
        }

        m_includePaths = oldJson.namedObject("m_includePaths").toArrayString();

        // Save it
        wxString buf;
        if (FileUtils::ReadBufferFromFile(newConfigFile, buf, 1) && (buf == "[")) {
            FileUtils::WriteFileContent(newConfigFile, "{}");
        }

        if (!newConfigFile.FileExists()) {
            FileUtils::WriteFileContent(newConfigFile, "{}");
        }

        JSON newRoot(newConfigFile);
        JSONItem e = JSONItem::createObject();
        e.addProperty("m_phpExe", m_phpExe);
        e.addProperty("m_includePaths", m_includePaths);
        newRoot.toElement().addProperty(GetName(), e);
        newRoot.save(newConfigFile);
    }
}

void PhpOptions::FromJSON(const JSONItem& json)
{
    m_phpExe = json.namedObject("m_phpExe").toString(m_phpExe);
    if (m_phpExe.IsEmpty()) {
        m_phpExe = FileUtils::FindExe("php").value_or(wxFileName{}).GetFullPath();
    }

    m_includePaths = json.namedObject("m_includePaths").toArrayString();
}

JSONItem PhpOptions::ToJSON() const
{
    return nlohmann::json{
        {"m_phpExe", m_phpExe.ToStdString(wxConvUTF8)}, {"m_includePaths", JsonUtils::ToJson(m_includePaths)}};
}

PhpOptions& PhpOptions::Load()
{
    clConfig config("php-general.conf");
    config.ReadItem(*this);
    return *this;
}

PhpOptions& PhpOptions::Save()
{
    clConfig config("php-general.conf");
    config.WriteItem(*this);
    return *this;
}
