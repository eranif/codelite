#ifndef CLDOCKERFILE_H
#define CLDOCKERFILE_H

#include "JSON.h"
#include "clDockerBuildableFile.h"

class clDockerfile : public clDockerBuildableFile
{
public:
    clDockerfile(const wxString& path);
    clDockerfile();
    virtual ~clDockerfile() = default;

    void FromJSON(const JSONItem& json, const wxString& workspaceDir);
    JSONItem ToJSON(const wxString& workspaceDir) const;
};

#endif // CLDOCKERFILE_H
