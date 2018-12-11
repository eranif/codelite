#ifndef CLDOCKERCOMPOSEFILE_H
#define CLDOCKERCOMPOSEFILE_H

#include "clDockerBuildableFile.h"

class clDockerComposeFile : public clDockerBuildableFile
{
public:
    clDockerComposeFile(const wxString& path);
    clDockerComposeFile();
    virtual ~clDockerComposeFile();
    void FromJSON(const JSONElement& json, const wxString& workspaceDir);
    JSONElement ToJSON(const wxString& workspaceDir) const;
};

#endif // CLDOCKERCOMPOSEFILE_H
