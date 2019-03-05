#ifndef CLDOCKERFILE_H
#define CLDOCKERFILE_H

#include "clDockerBuildableFile.h"
#include "JSON.h"

class clDockerfile : public clDockerBuildableFile
{
public:
    clDockerfile(const wxString& path);
    clDockerfile();
    virtual ~clDockerfile();

    void FromJSON(const JSONItem& json, const wxString& workspaceDir);
    JSONItem ToJSON(const wxString& workspaceDir) const;
};

#endif // CLDOCKERFILE_H
