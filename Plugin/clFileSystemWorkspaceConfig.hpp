#ifndef CLFILESYSTEMWORKSPACECONFIG_HPP
#define CLFILESYSTEMWORKSPACECONFIG_HPP

#include "codelite_exports.h"
#include "macros.h"
#include <wx/arrstr.h>
#include "JSON.h"
#include <wx/sharedptr.h>
#include <map>
#include <wxStringHash.h>

class WXDLLIMPEXP_SDK clFileSystemWorkspaceConfig
{
public:
    enum {
        kGenerateCompileFlags = (1 << 0),
    };

protected:
    size_t m_flags = kGenerateCompileFlags;
    wxStringMap_t m_buildTargets;
    wxArrayString m_compileFlags;
    wxString m_fileExtensions;
    wxString m_name;
    wxString m_executable;
    wxString m_args;
    wxString m_environment;
    wxString m_compiler;

public:
    JSONItem ToJSON() const;
    void FromJSON(const JSONItem& json);

    void SetBuildTargets(const wxStringMap_t& buildTargets) { this->m_buildTargets = buildTargets; }
    void SetCompileFlags(const wxArrayString& compileFlags) { this->m_compileFlags = compileFlags; }
    void SetCompileFlags(const wxString& compileFlags);
    void SetFileExtensions(const wxString& fileExtensions) { this->m_fileExtensions = fileExtensions; }
    void SetFlags(size_t flags) { this->m_flags = flags; }
    void SetName(const wxString& name) { this->m_name = name; }
    const wxStringMap_t& GetBuildTargets() const { return m_buildTargets; }
    const wxArrayString& GetCompileFlags() const { return m_compileFlags; }
    wxString GetCompileFlagsAsString() const;
    const wxString& GetFileExtensions() const { return m_fileExtensions; }
    size_t GetFlags() const { return m_flags; }
    const wxString& GetName() const { return m_name; }
    void SetArgs(const wxString& args) { this->m_args = args; }
    void SetEnvironment(const wxString& environment) { this->m_environment = environment; }
    void SetExecutable(const wxString& executable) { this->m_executable = executable; }
    const wxString& GetArgs() const { return m_args; }
    const wxString& GetEnvironment() const { return m_environment; }
    const wxString& GetExecutable() const { return m_executable; }
    wxArrayString GetSearchPaths(const wxFileName& workspaceFile, wxString& compile_flags_txt) const;

    void SetCompiler(const wxString& compiler) { this->m_compiler = compiler; }
    const wxString& GetCompiler() const { return m_compiler; }
    clFileSystemWorkspaceConfig();
    ~clFileSystemWorkspaceConfig() {}

    bool ShouldCreateCompileFlags() const { return m_flags & kGenerateCompileFlags; }
    void SetCreateCompileFlags(bool b)
    {
        if(b) {
            m_flags |= kGenerateCompileFlags;
        } else {
            m_flags &= ~kGenerateCompileFlags;
        }
    }
    typedef wxSharedPtr<clFileSystemWorkspaceConfig> Ptr_t;
};

class WXDLLIMPEXP_SDK clFileSystemWorkspaceSettings
{
    // Workspace settings
    // we use map to keep the entries sorted
    std::map<wxString, clFileSystemWorkspaceConfig::Ptr_t> m_configsMap;
    wxString m_selectedConfig;
    wxString m_workspaceType;
    wxString m_name;

protected:
    JSONItem ToJSON(JSONItem& item) const;
    void FromJSON(const JSONItem& json);

public:
    clFileSystemWorkspaceSettings();
    virtual ~clFileSystemWorkspaceSettings();

    bool Save(const wxFileName& filename);
    bool Load(const wxFileName& filename);

    bool AddConfig(const wxString& name);
    bool DeleteConfig(const wxString& name);
    bool SetSelectedConfig(const wxString& name);
    clFileSystemWorkspaceConfig::Ptr_t GetSelectedConfig() const;
    clFileSystemWorkspaceConfig::Ptr_t GetConfig(const wxString& name) const;
    const std::map<wxString, clFileSystemWorkspaceConfig::Ptr_t>& GetConfigsMap() const { return m_configsMap; }
    wxArrayString GetConfigs() const;
    const wxString& GetName() const { return m_name; }
    void SetName(const wxString& name) { this->m_name = name; }
    void Clear();

    /**
     * @brief check if the file is a valid workspace
     */
    static bool IsOk(const wxFileName& filename);
};

#endif // CLFILESYSTEMWORKSPACECONFIG_HPP
