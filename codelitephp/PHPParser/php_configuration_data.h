#ifndef PHPCONFIGURATIONDATA_H
#define PHPCONFIGURATIONDATA_H

#include <wx/arrstr.h>
#include <wx/string.h>
#include <cl_config.h>

class PHPConfigurationData : public clConfigItem
{
protected:
    wxArrayString m_includePaths;
    wxString      m_phpExe;
    wxString      m_errorReporting;
    size_t        m_xdebugPort;
    wxArrayString m_ccIncludePath;
    size_t        m_flags;
    wxString      m_xdebugIdeKey;

public:
    enum {
        kDontPromptForMissingFileMapping = 0x00000001,
    };

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    PHPConfigurationData& Load();
    void Save();

    PHPConfigurationData();
    virtual ~PHPConfigurationData();

    PHPConfigurationData& EnableFlag( size_t flag, bool b ) {
        if ( b ) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
        return *this;
    }

    bool HasFlag( size_t flag ) const {
        return m_flags & flag;
    }

    
    
    // ----------------------------------------------------
    // Setters
    // ----------------------------------------------------
    PHPConfigurationData& SetCcIncludePath(const wxArrayString& ccIncludePath) {
        this->m_ccIncludePath = ccIncludePath;
        return *this;
    }
    PHPConfigurationData& SetErrorReporting(const wxString& errorReporting) {
        this->m_errorReporting = errorReporting;
        return *this;
    }
    PHPConfigurationData& SetIncludePaths(const wxArrayString& includePaths) {
        this->m_includePaths = includePaths;
        return *this;
    }
    PHPConfigurationData& SetPhpExe(const wxString& phpExe) {
        this->m_phpExe = phpExe;
        return *this;
    }
    PHPConfigurationData& SetXdebugPort(size_t xdebugPort) {
        this->m_xdebugPort = xdebugPort;
        return *this;
    }
    PHPConfigurationData& SetFlags(size_t flags) {
        this->m_flags = flags;
        return *this;
    }
    PHPConfigurationData& SetXdebugIdeKey(const wxString& xdebugIdeKey) {
        this->m_xdebugIdeKey = xdebugIdeKey;
        return *this;
    }
    
    // ----------------------------------------------------
    // Getters
    // ----------------------------------------------------

    const wxArrayString& GetCcIncludePath() const {
        return m_ccIncludePath;
    }

    const wxString& GetErrorReporting() const {
        return m_errorReporting;
    }
    const wxArrayString& GetIncludePaths() const {
        return m_includePaths;
    }
    wxString GetIncludePathsAsString() const;
    wxString GetCCIncludePathsAsString() const;
    const wxString& GetPhpExe() const {
        return m_phpExe;
    }
    size_t GetXdebugPort() const {
        return m_xdebugPort;
    }

    size_t GetFlags() const {
        return m_flags;
    }
    const wxString& GetXdebugIdeKey() const {
        return m_xdebugIdeKey;
    }
};

#endif // PHPCONFIGURATIONDATA_H
