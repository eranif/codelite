#ifndef CLDOCKERBUILDABLEFILE_H
#define CLDOCKERBUILDABLEFILE_H

#include "JSON.h"
#include <wx/sharedptr.h>
#include <wx/string.h>
#include <wxStringHash.h>

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
    typedef wxSharedPtr<clDockerBuildableFile> Ptr_t;
    typedef std::unordered_map<wxString, clDockerBuildableFile::Ptr_t> Map_t;

    clDockerBuildableFile(const wxString& path, eDockerFileType type);
    clDockerBuildableFile();
    virtual ~clDockerBuildableFile();

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
    wxString GetRunBaseCommand() const;

    /**
     * @brief create new instance of clDockerBuildableFile based on the type
     */
    static clDockerBuildableFile::Ptr_t New(eDockerFileType type);
};
#endif // CLDOCKERBUILDABLEFILE_H
