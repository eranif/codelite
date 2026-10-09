#include "php_project_settings_data.h"

#include "clToolBar.h"
#include "globals.h"
#include "json_utils.h"
#include "php_configuration_data.h"
#include "php_utils.h"

#include <map>
#include <set>
#include <wx/tokenzr.h>
#include <wx/uri.h>

PHPProjectSettingsData::PHPProjectSettingsData()
    : m_runAs(0)
    , m_flags(kOpt_PauseWhenExeTermiantes | kOpt_RunCurrentEditor)
{
}

wxArrayString PHPProjectSettingsData::GetIncludePathAsArray() const
{
    PHPProjectSettingsData s = *this;
    s.MergeWithGlobalSettings();
    wxArrayString paths = wxStringTokenize(s.m_includePath, "\r\n", wxTOKEN_STRTOK);
    return paths;
}

void PHPProjectSettingsData::FromJSON(const JSONItem& ele)
{
    m_runAs = ele.namedObject("m_runAs").toInt(0);
    m_phpExe = ele.namedObject("m_phpExe").toString();
    m_indexFile = ele.namedObject("m_indexFile").toString();
    m_args = ele.namedObject("m_args").toString();
    m_workingDirectory = ele.namedObject("m_workingDirectory").toString(::wxGetCwd());
    m_projectURL = ele.namedObject("m_projectURL").toString();
    m_includePath = ele.namedObject("m_includePath").toString();
    m_flags = ele.namedObject("m_flags").toSize_t(m_flags);
    m_phpIniFile = ele.namedObject("m_phpIniFile").toString();
    m_fileMapping = ele.namedObject("m_fileMapping").toStringMap();
}

JSONItem PHPProjectSettingsData::ToJSON() const
{
    return nlohmann::json{{"m_runAs", m_runAs},
                          {"m_phpExe", m_phpExe.ToStdString(wxConvUTF8)},
                          {"m_indexFile", m_indexFile.ToStdString(wxConvUTF8)},
                          {"m_args", m_args.ToStdString(wxConvUTF8)},
                          {"m_workingDirectory", m_workingDirectory.ToStdString(wxConvUTF8)},
                          {"m_projectURL", m_projectURL.ToStdString(wxConvUTF8)},
                          {"m_includePath", m_includePath.ToStdString(wxConvUTF8)},
                          {"m_flags", m_flags},
                          {"m_phpIniFile", m_phpIniFile.ToStdString(wxConvUTF8)},
                          {"m_fileMapping", JsonUtils::ToJson(m_fileMapping)}};
}

void PHPProjectSettingsData::MergeWithGlobalSettings()
{
    PHPConfigurationData globalData;
    globalData.Load();

    if (GetPhpExe().IsEmpty()) {
        SetPhpExe(globalData.GetPhpExe());
    }

    // Append the include paths (keep uniqueness)
    wxArrayString paths = ::wxStringTokenize(m_includePath, "\r\n", wxTOKEN_STRTOK);
    const wxArrayString& globalIncPaths = globalData.GetIncludePaths();
    for (size_t i = 0; i < globalIncPaths.GetCount(); ++i) {
        wxString path = wxFileName(globalIncPaths.Item(i), "").GetPath(wxPATH_GET_VOLUME, wxPATH_UNIX);
        if (paths.Index(path) == wxNOT_FOUND) {
            paths.Add(path);
        }
    }

    m_includePath = ::wxJoin(paths, '\n');
}

wxString PHPProjectSettingsData::GetMappdPath(const wxString& sourcePath,
                                              bool useUrlScheme,
                                              const wxStringMap_t& additionalMapping) const
{
    wxFileName fnSource(sourcePath);
    wxStringMap_t fullMapping;
    fullMapping.insert(m_fileMapping.begin(), m_fileMapping.end());
    fullMapping.insert(additionalMapping.begin(), additionalMapping.end());

    wxString sourceFullPath = fnSource.GetFullPath();
    for (const auto& p : fullMapping) {
        if (sourceFullPath.StartsWith(p.first)) {
            sourceFullPath.Remove(0, p.first.length());
            sourceFullPath.Prepend(p.second + "/");
            sourceFullPath.Replace("\\", "/");
            while (sourceFullPath.Replace("//", "/")) {}

            if (useUrlScheme) {
                sourceFullPath = ::FileNameToURI(sourceFullPath);
            }
            return sourceFullPath;
        }
    }

    if (useUrlScheme) {

        wxString asUrlScheme = sourcePath;
        asUrlScheme.Replace("\\", "/");

        while (asUrlScheme.Replace("//", "/"))
            ;

        asUrlScheme = ::FileNameToURI(asUrlScheme);
        return asUrlScheme;

    } else {
        wxString filePath;
        if (sourcePath.Contains(" ")) {
            filePath = sourcePath;
            filePath.Prepend('"').Append('"');
        }
        // return the path without changing it
        return filePath;
    }
}
