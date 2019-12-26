#include "clDockerBuildableFile.h"
#include "clDockerComposeFile.h"
#include "clDockerSettings.h"
#include "clDockerfile.h"
#include "globals.h"
#include "imanager.h"
#include <wx/filename.h>

clDockerBuildableFile::clDockerBuildableFile(const wxString& path, eDockerFileType type)
    : m_path(path)
    , m_type(type)
{
}
clDockerBuildableFile::clDockerBuildableFile() {}
clDockerBuildableFile::~clDockerBuildableFile() {}

void clDockerBuildableFile::FromJSON(const JSONItem& json, const wxString& workspaceDir)
{
    m_type = (eDockerFileType)json.namedObject("type").toInt((int)eDockerFileType::kDockerfile);
    m_path = json.namedObject("path").toString();
    // The file path is relative, make it absolute
    wxFileName fn(m_path);
    fn.MakeAbsolute(workspaceDir);
    m_path = fn.GetFullPath();

    m_buildOptions = json.namedObject("buildOptions").toString();
    m_runOptions = json.namedObject("runOptions").toString();
}

JSONItem clDockerBuildableFile::ToJSON(const wxString& workspaceDir) const
{
    JSONItem json = JSONItem::createObject();

    // m_path is absolute, convert to relative before we save it
    wxFileName fn(m_path);
    fn.MakeRelativeTo(workspaceDir); // Use Unix style paths
    wxString path = fn.GetFullPath();
    path.Replace("\\", "/");
    json.addProperty("type", (int)m_type);
    json.addProperty("path", path);
    json.addProperty("buildOptions", GetBuildOptions());
    json.addProperty("runOptions", GetRunOptions());
    return json;
}

clDockerBuildableFile::Ptr_t clDockerBuildableFile::New(eDockerFileType type)
{
    switch(type) {
    case eDockerFileType::kDockerCompose:
        return clDockerBuildableFile::Ptr_t(new clDockerComposeFile());
    case eDockerFileType::kDockerfile:
        return clDockerBuildableFile::Ptr_t(new clDockerfile());
    default:
        return clDockerBuildableFile::Ptr_t();
    }
}

wxString clDockerBuildableFile::GetBuildBaseCommand() const
{
    wxString cmd = GetDockerExe();
    ::WrapWithQuotes(cmd);
    cmd << " build";
    return cmd;
}

void clDockerBuildableFile::GetRunBaseCommand(wxString& docker, wxString& args) const
{
    docker = GetDockerExe();
    if(GetType() == eDockerFileType::kDockerfile) {
        args = "run";
    } else /* if(GetType() == eDockerFileType::kDockerCompose)*/ {
        args = "up";
    }
}

wxString clDockerBuildableFile::GetDockerExe() const
{
    clDockerSettings dockerSettings;
    dockerSettings.Load();

    wxFileName dockerCommand;
    if(GetType() == eDockerFileType::kDockerfile) {
        dockerCommand = dockerSettings.GetDocker();
    } else if(GetType() == eDockerFileType::kDockerCompose) {
        dockerCommand = dockerSettings.GetDockerCompose();
    } else {
        // Unknown file type
        return "";
    }

    if(!dockerCommand.FileExists()) {
        clGetManager()->SetStatusMessage(
            _("Can't find docker executable. Please install docker and let me know where it is"), 3);
        return "";
    }
    wxString exepath = dockerCommand.GetFullPath();
    return exepath;
}
