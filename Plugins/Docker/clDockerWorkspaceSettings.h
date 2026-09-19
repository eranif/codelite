#ifndef CLDOCKERWORKSPACESETTINGS_H
#define CLDOCKERWORKSPACESETTINGS_H

#include "clDockerBuildableFile.h"
#include "cl_config.h"

#include <wx/string.h>

class clDockerWorkspaceSettings : public clConfigItem
{
protected:
    wxFileName m_workspaceFile;
    clDockerBuildableFile::Map_t m_files;
    wxString m_version;

public:
    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON() const override;

public:
    clDockerWorkspaceSettings();
    ~clDockerWorkspaceSettings() override = default;

    clDockerBuildableFile::Map_t& GetFiles() { return m_files; }

    clDockerWorkspaceSettings& Load(const wxFileName& filename);
    clDockerWorkspaceSettings& Save(const wxFileName& filename);

    bool IsOk() const;
    /**
     * @brief get file info for a given file
     */
    clDockerBuildableFile::Ptr_t GetFileInfo(const wxFileName& file) const;
    void SetFileInfo(const wxFileName& file, clDockerBuildableFile::Ptr_t info);
    void Clear();
};

#endif // CLDOCKERWORKSPACESETTINGS_H
