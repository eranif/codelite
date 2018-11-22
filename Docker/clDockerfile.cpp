#include "clDockerfile.h"

clDockerfile::clDockerfile(const wxString& path)
    : clDockerBuildableFile(path, eDockerFileType::kDockerfile)
{
}

clDockerfile::clDockerfile()
    : clDockerBuildableFile("", eDockerFileType::kDockerfile)
{
}

clDockerfile::~clDockerfile() {}

void clDockerfile::FromJSON(const JSONElement& json, const wxString& workspaceDir)
{
    clDockerBuildableFile::FromJSON(json, workspaceDir);
}

JSONElement clDockerfile::ToJSON(const wxString& workspaceDir) const
{
    JSONElement json = clDockerBuildableFile::ToJSON(workspaceDir);
    return json;
}
