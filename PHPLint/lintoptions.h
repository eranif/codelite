#ifndef LINTOPTIONS_H
#define LINTOPTIONS_H

#include "cl_config.h" // Base class: clConfigItem

class LintOptions : public clConfigItem
{
    bool m_lintOnFileLoad = true;
    bool m_lintOnFileSave = true;
    wxString m_phpcsPhar;
    wxString m_phpmdPhar;
    wxString m_phpmdRules;
    wxString m_phpstanPhar;

public:
    LintOptions();
    virtual ~LintOptions();

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    LintOptions& Load();
    LintOptions& Save();

    LintOptions& SetLintOnFileLoad(bool lintOnFileLoad)
    {
        this->m_lintOnFileLoad = lintOnFileLoad;
        return *this;
    }
    LintOptions& SetLintOnFileSave(bool lintOnFileSave)
    {
        this->m_lintOnFileSave = lintOnFileSave;
        return *this;
    }
    LintOptions& SetPhpcsPhar(const wxFileName& PhpcsPhar)
    {
        this->m_phpcsPhar = PhpcsPhar.GetFullPath();
        return *this;
    }
    LintOptions& SetPhpmdPhar(const wxFileName& PhpmdPhar)
    {
        this->m_phpmdPhar = PhpmdPhar.GetFullPath();
        return *this;
    }
    LintOptions& SetPhpmdRules(const wxFileName& PhpmdRules)
    {
        this->m_phpmdRules = PhpmdRules.GetFullPath();
        return *this;
    }
    LintOptions& SetPhpstanPhar(const wxFileName& PhpstanPhar)
    {
        this->m_phpstanPhar = PhpstanPhar.GetFullPath();
        return *this;
    }

    bool IsLintOnFileLoad() const { return m_lintOnFileLoad; }
    bool IsLintOnFileSave() const { return m_lintOnFileSave; }
    const wxString& GetPhpcsPhar() const { return m_phpcsPhar; }
    const wxString& GetPhpmdPhar() const { return m_phpmdPhar; }
    const wxString& GetPhpmdRules() const { return m_phpmdRules; }
    const wxString& GetPhpstanPhar() const { return m_phpstanPhar; }
};

#endif // LINTOPTIONS_H
