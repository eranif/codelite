#ifndef CLDOCKERWORKSPACESETTINGS_H
#define CLDOCKERWORKSPACESETTINGS_H

#include "cl_config.h"
#include <vector>
#include <algorithm>
#include <wx/string.h>
#include <wxStringHash.h>

class clDockerfile
{
protected:
    wxString m_path;
    wxString m_buildOptions;
    wxString m_runOptions;

public:
    clDockerfile(const wxString& path)
        : m_path(path)
    {
    }
    clDockerfile() {}
    ~clDockerfile() {}

    void SetBuildOptions(const wxString& buildOptions) { this->m_buildOptions = buildOptions; }
    void SetPath(const wxString& path) { this->m_path = path; }
    void SetRunOptions(const wxString& runOptions) { this->m_runOptions = runOptions; }
    const wxString& GetBuildOptions() const { return m_buildOptions; }
    const wxString& GetPath() const { return m_path; }
    const wxString& GetRunOptions() const { return m_runOptions; }

    void FromJSON(const JSONElement& json, const wxString& workspaceDir);
    JSONElement ToJSON(const wxString& workspaceDir) const;
    typedef std::unordered_map<wxString, clDockerfile> Map_t;
};

class clDockerWorkspaceSettings : public clConfigItem
{
protected:
    wxFileName m_workspaceFile;
    clDockerfile::Map_t m_files;
    wxString m_version;

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

public:
    clDockerWorkspaceSettings();
    virtual ~clDockerWorkspaceSettings();

    clDockerfile::Map_t& GetFiles() { return m_files; }

    clDockerWorkspaceSettings& Load(const wxFileName& filename);
    clDockerWorkspaceSettings& Save(const wxFileName& filename);

    bool IsOk() const;
    /**
     * @brief get file info for a given file
     */
    bool GetFileInfo(const wxFileName& file, clDockerfile& info) const;
    void SetFileInfo(const wxFileName& file, const clDockerfile& info);
    void Clear();
};

#endif // CLDOCKERWORKSPACESETTINGS_H
