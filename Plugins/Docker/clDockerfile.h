#pragma once

#include "JSON.h"
#include "clDockerBuildableFile.h"

class clDockerfile : public clDockerBuildableFile
{
public:
    clDockerfile(const wxString& path);
    clDockerfile();
    ~clDockerfile() override = default;

    void FromJSON(const JSONItem& json, const wxString& workspaceDir);
    JSONItem ToJSON(const wxString& workspaceDir) const;
};
