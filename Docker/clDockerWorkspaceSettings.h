#ifndef CLDOCKERWORKSPACESETTINGS_H
#define CLDOCKERWORKSPACESETTINGS_H

#include "cl_config.h"
#include <vector>
#include <wx/string.h>

class clDockerWorkspaceSettings : public clConfigItem
{
public:
    class File
    {
    public:
        typedef std::vector<std::pair<wxString, wxString> > wxStringPairVec_t;

    protected:
        wxString m_path;
        wxString m_buildOptions;
        wxString m_runOptions;

    public:
        File(const wxString& path)
            : m_path(path)
        {
        }
        File() {}
        ~File() {}

        void SetBuildOptions(const wxString& buildOptions) { this->m_buildOptions = buildOptions; }
        void SetPath(const wxString& path) { this->m_path = path; }
        void SetRunOptions(const wxString& runOptions) { this->m_runOptions = runOptions; }
        const wxString& GetBuildOptions() const { return m_buildOptions; }
        const wxString& GetPath() const { return m_path; }
        const wxString& GetRunOptions() const { return m_runOptions; }

        void FromJSON(const JSONElement& json);
        JSONElement ToJSON() const;
        typedef std::vector<File> Vect_t;
    };

protected:
    clDockerWorkspaceSettings::File::Vect_t m_files;
    wxString m_version;

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

public:
    clDockerWorkspaceSettings();
    virtual ~clDockerWorkspaceSettings();

    clDockerWorkspaceSettings::File::Vect_t& GetFiles() { return m_files; }

    clDockerWorkspaceSettings& Load(const wxFileName& filename);
    clDockerWorkspaceSettings& Save(const wxFileName& filename);
    
    bool IsOk() const;
    
    void Clear();
};

#endif // CLDOCKERWORKSPACESETTINGS_H
