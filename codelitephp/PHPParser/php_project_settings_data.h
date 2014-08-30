#ifndef PHPPROJECTSETTINGSDATA_H
#define PHPPROJECTSETTINGSDATA_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include <json_node.h>
#include <macros.h>
#include "XDebugBreakpoint.h"

class PHPProjectSettingsData
{
protected:
    int      m_runAs;
    wxString m_phpExe;
    wxString m_indexFile;
    wxString m_args;
    wxString m_workingDirectory;
    wxString m_projectURL;
    wxString m_includePath;
    wxString m_ccIncludePath;
    wxString m_phpIniFile;
    size_t   m_flags;
    JSONElement::wxStringMap_t m_fileMapping;

public:
    enum {
        kOpt_PauseWhenExeTermiantes  = 0x00000001,
        kOpt_UseSystemDefaultBrowser = 0x00000002,
    };

    enum {
        kRunAsCLI       = 0,
        kRunAsWebsite   = 1,
    };

public:
    PHPProjectSettingsData();
    virtual ~PHPProjectSettingsData();

    void FromJSON( const JSONElement &ele );
    JSONElement ToJSON() const;

    void EnableFlag( int flag, bool b ) {
        if ( b ) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

    bool HasFlag(int flag) const {
        return m_flags & flag;
    }

    /**
     * @brief copy the global settings and append them to the project settings
     */
    void MergeWithGlobalSettings();

    /**
     * @brief convert path to its mapped path based on the
     * fileMapping of the project
     */
    wxString GetMappdPath(const wxString& sourcePath, bool useUrlScheme, const wxStringMap_t& additionalMapping) const;
    const JSONElement::wxStringMap_t& GetFileMapping() const {
        return m_fileMapping;
    }
    void SetFileMapping(const JSONElement::wxStringMap_t& fileMapping) {
        this->m_fileMapping = fileMapping;
    }
    void SetPhpIniFile(const wxString& phpIniFile) {
        this->m_phpIniFile = phpIniFile;
    }
    const wxString& GetPhpIniFile() const {
        return m_phpIniFile;
    }
    void SetPauseWhenExeTerminates(bool pauseWhenExeTerminates) {
        EnableFlag(kOpt_PauseWhenExeTermiantes, pauseWhenExeTerminates);
    }
    bool IsPauseWhenExeTerminates() const {
        return HasFlag(kOpt_PauseWhenExeTermiantes);
    }
    bool IsUseSystemBrowser() const {
        return HasFlag(kOpt_UseSystemDefaultBrowser);
    }

    void SetUseSystemBrowser(bool b) {
        return EnableFlag(kOpt_UseSystemDefaultBrowser, b);
    }

    void SetCcIncludePath(const wxString& ccIncludePath) {
        this->m_ccIncludePath = ccIncludePath;
    }
    const wxString& GetCcIncludePath() const {
        return m_ccIncludePath;
    }
    void SetArgs(const wxString& args) {
        this->m_args = args;
    }
    void SetIncludePath(const wxString& includePath) {
        this->m_includePath = includePath;
    }
    void SetIndexFile(const wxString& indexFile) {
        this->m_indexFile = indexFile;
    }
    void SetPhpExe(const wxString& phpExe) {
        this->m_phpExe = phpExe;
    }
    void SetProjectURL(const wxString& projectURL) {
        this->m_projectURL = projectURL;
    }
    void SetRunAs(int runAs) {
        this->m_runAs = runAs;
    }
    void SetWorkingDirectory(const wxString& workingDirectory) {
        this->m_workingDirectory = workingDirectory;
    }
    const wxString& GetArgs() const {
        return m_args;
    }
    const wxString& GetIncludePath() const {
        return m_includePath;
    }
    wxArrayString GetIncludePathAsArray() const;
    wxArrayString GetCCIncludePathAsArray() const;
    static wxArrayString GetAllIncludePaths();

    const wxString& GetIndexFile() const {
        return m_indexFile;
    }
    const wxString& GetPhpExe() const {
        return m_phpExe;
    }
    const wxString& GetProjectURL() const {
        return m_projectURL;
    }
    int GetRunAs() const {
        return m_runAs;
    }
    const wxString& GetWorkingDirectory() const {
        return m_workingDirectory;
    }
};

#endif // PHPPROJECTSETTINGSDATA_H
