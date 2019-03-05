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

void clDockerComposeFile::FromJSON(const JSONItem& json, const wxString& workspaceDir)
{
    clDockerBuildableFile::FromJSON(json, workspaceDir);
}

JSONItem clDockerComposeFile::ToJSON(const wxString& workspaceDir) const
{
    JSONItem json = clDockerBuildableFile::ToJSON(workspaceDir);
    return json;
}