#ifndef CLDOCKERFILE_H
#define CLDOCKERFILE_H

#include "clDockerBuildableFile.h"
#include "json_node.h"

class clDockerfile : public clDockerBuildableFile
{
public:
    clDockerfile(const wxString& path);
    clDockerfile();
    virtual ~clDockerfile();

    void FromJSON(const JSONElement& json, const wxString& workspaceDir);
    JSONElement ToJSON(const wxString& workspaceDir) const;
};

#endif // CLDOCKERFILE_H
