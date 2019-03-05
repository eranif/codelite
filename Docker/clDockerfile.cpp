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

void clDockerfile::FromJSON(const JSONItem& json, const wxString& workspaceDir)
{
    clDockerBuildableFile::FromJSON(json, workspaceDir);
}

JSONItem clDockerfile::ToJSON(const wxString& workspaceDir) const
{
    JSONItem json = clDockerBuildableFile::ToJSON(workspaceDir);
    return json;
}
