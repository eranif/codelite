#ifndef CLDOCKERBUILDABLEFILE_H
#define CLDOCKERBUILDABLEFILE_H

#include "JSON.h"
#include "wxStringHash.h"

#include <memory>
#include <unordered_map>
#include <wx/string.h>

enum class eDockerFileType {
    kDockerfile = 0,
    kDockerCompose = 1,
};

class clDockerBuildableFile
{
protected:
    wxString m_path;
    wxString m_buildOptions;
    wxString m_runOptions;
    eDockerFileType m_type = eDockerFileType::kDockerfile;

protected:
    wxString GetDockerExe() const;

public:
    using Ptr_t = std::shared_ptr<clDockerBuildableFile>;
    using Map_t = std::unordered_map<wxString, clDockerBuildableFile::Ptr_t>;

    clDockerBuildableFile(const wxString& path, eDockerFileType type);
    clDockerBuildableFile() = default;
    virtual ~clDockerBuildableFile() = default;

    void SetBuildOptions(const wxString& buildOptions) { this->m_buildOptions = buildOptions; }
    void SetPath(const wxString& path) { this->m_path = path; }
    void SetRunOptions(const wxString& runOptions) { this->m_runOptions = runOptions; }
    const wxString& GetBuildOptions() const { return m_buildOptions; }
    const wxString& GetPath() const { return m_path; }
    const wxString& GetRunOptions() const { return m_runOptions; }
    eDockerFileType GetType() const { return m_type; };

    void FromJSON(const JSONItem& json, const wxString& workspaceDir);
    JSONItem ToJSON(const wxString& workspaceDir) const;

    /**
     * @brief return the build/run base command for this file (e.g. docker build, docker-compose build)
     * @return
     */
    wxString GetBuildBaseCommand() const;
    void GetRunBaseCommand(wxString& docker, wxString &args) const;

    /**
     * @brief create new instance of clDockerBuildableFile based on the type
     */
    static clDockerBuildableFile::Ptr_t New(eDockerFileType type);
};
#endif // CLDOCKERBUILDABLEFILE_H
