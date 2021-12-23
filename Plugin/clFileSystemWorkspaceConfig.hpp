#ifndef CLFILESYSTEMWORKSPACECONFIG_HPP
#define CLFILESYSTEMWORKSPACECONFIG_HPP

#include "JSON.h"
#include "clBacktickCache.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "macros.h"

#include <map>
#include <wx/arrstr.h>
#include <wx/sharedptr.h>
#include <wxStringHash.h>

class WXDLLIMPEXP_SDK clFileSystemWorkspaceConfig
{
public:
    enum {
        kUnused = (1 << 0),
        kEnableRemoteSync = (1 << 1),
        kRemoteBuild = (1 << 2),
    };

protected:
    size_t m_flags = 0;
    std::map<wxString, wxString> m_buildTargets;
    wxArrayString m_compileFlags;
    wxString m_fileExtensions;
    wxString m_name;
    wxString m_executable;
    wxString m_args;
    wxString m_environment;
    wxString m_compiler;
    wxString m_remoteFolder;
    wxString m_remoteAccount;
    wxString m_debugger;
    wxString m_excludeFilesPattern;
    wxString m_workingDirectory;
    wxString m_excludePaths;
    wxString m_debuggerPath;
    wxString m_debuggerCommands;
    wxArrayString m_lastExecutables;

public:
    typedef wxSharedPtr<clFileSystemWorkspaceConfig> Ptr_t;
    /**
     * @brief return 2 json nodes. The first should be stored in the shared configuration file and the other in the
     * local configuration file
     */
    std::pair<JSONItem, JSONItem> ToJSON() const;
    void FromJSONOld(const JSONItem& json);
    void FromLocalJSON(const JSONItem& json);
    void FromSharedJSON(const JSONItem& json);

    void SetExcludeFilesPattern(const wxString& excludeFilesPattern)
    {
        this->m_excludeFilesPattern = excludeFilesPattern;
    }
    const wxString& GetExcludeFilesPattern() const { return m_excludeFilesPattern; }
    void SetBuildTargets(const std::map<wxString, wxString>& buildTargets) { this->m_buildTargets = buildTargets; }
    void SetCompileFlags(const wxArrayString& compileFlags) { this->m_compileFlags = compileFlags; }
    void SetCompileFlags(const wxString& compileFlags);
    void SetFileExtensions(const wxString& fileExtensions) { this->m_fileExtensions = fileExtensions; }
    void SetFlags(size_t flags) { this->m_flags = flags; }
    void SetName(const wxString& name) { this->m_name = name; }
    const std::map<wxString, wxString>& GetBuildTargets() const { return m_buildTargets; }
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
    wxArrayString GetSearchPaths(const wxFileName& workspaceFile) const;
    bool IsRemoteEnabled() const { return m_flags & kEnableRemoteSync; }
    void SetRemoteEnabled(bool b)
    {
        if(b) {
            m_flags |= kEnableRemoteSync;
        } else {
            m_flags &= ~kEnableRemoteSync;
        }
    }
    void SetCompiler(const wxString& compiler) { this->m_compiler = compiler; }
    const wxString& GetCompiler() const { return m_compiler; }
    clFileSystemWorkspaceConfig();
    ~clFileSystemWorkspaceConfig() {}
    void SetRemoteFolder(const wxString& remoteFolder) { this->m_remoteFolder = remoteFolder; }
    const wxString& GetRemoteFolder() const { return m_remoteFolder; }
    void SetRemoteAccount(const wxString& remoteAccount) { this->m_remoteAccount = remoteAccount; }
    const wxString& GetRemoteAccount() const { return m_remoteAccount; }
    clFileSystemWorkspaceConfig::Ptr_t Clone() const;
    void SetDebugger(const wxString& debugger) { this->m_debugger = debugger; }
    const wxString& GetDebugger() const { return m_debugger; }
    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
    void SetExcludePaths(const wxString& paths) { m_excludePaths = paths; }
    const wxString& GetExecludePaths() const { return m_excludePaths; }
    bool IsRemoteBuild() const { return m_flags & kRemoteBuild; }
    void SetRemoteBuild(bool b)
    {
        if(b) {
            m_flags |= kRemoteBuild;
        } else {
            m_flags &= ~kRemoteBuild;
        }
    }

    wxArrayString ExpandUserCompletionFlags(const wxString& workingDirectory, clBacktickCache::ptr_t backticks,
                                            bool withPrefix = false) const;
    wxArrayString GetCompilerOptions(clBacktickCache::ptr_t backticks) const;
    /**
     * @brief based on the workspace directory, return list of all folders that contain
     * C/C++ header file
     */
    wxArrayString GetWorkspaceIncludes(bool withPrefix = false) const;
    void SetDebuggerCommands(const wxString& debuggerCommands) { this->m_debuggerCommands = debuggerCommands; }
    void SetDebuggerPath(const wxString& debuggerPath) { this->m_debuggerPath = debuggerPath; }
    const wxString& GetDebuggerCommands() const { return m_debuggerCommands; }
    const wxString& GetDebuggerPath() const { return m_debuggerPath; }
    void SetLastExecutables(const wxArrayString& lastExecutables);
    const wxArrayString& GetLastExecutables() const;
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
    void ToJSON(JSONItem& shared, JSONItem& local) const;
    void FromJSON(const JSONItem& shared, const JSONItem& local);

public:
    clFileSystemWorkspaceSettings();
    virtual ~clFileSystemWorkspaceSettings();

    bool Save(const wxFileName& filename, const wxFileName& localSettings = {});
    bool Load(const wxFileName& filename, const wxFileName& localSettings = {});

    bool AddConfig(const wxString& name, const wxString& copyfrom = wxEmptyString);
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

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_FSW_SETTINGS_SAVED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_FSW_SETTINGS_LOADED, clCommandEvent);
#endif // CLFILESYSTEMWORKSPACECONFIG_HPP
