#include "clDockerComposeFile.h"

clDockerComposeFile::clDockerComposeFile(const wxString& path)
    : clDockerBuildableFile(path, eDockerFileType::kDockerCompose)
{
}

clDockerComposeFile::clDockerComposeFile()
    : clDockerBuildableFile("", eDockerFileType::kDockerCompose)
{
}

clDockerComposeFile::~clDockerComposeFile() {}

void clDockerComposeFile::FromJSON(const JSONElement& json, const wxString& workspaceDir)
{
    clDockerBuildableFile::FromJSON(json, workspaceDir);
}

JSONElement clDockerComposeFile::ToJSON(const wxString& workspaceDir) const
{
    JSONElement json = clDockerBuildableFile::ToJSON(workspaceDir);
    return json;
}