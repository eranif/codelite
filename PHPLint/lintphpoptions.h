#ifndef LINTPHPOPTIONS_H
#define LINTPHPOPTIONS_H

#include "cl_config.h" // Base class: clConfigItem

class LintPhpOptions : public clConfigItem
{
    wxString m_phpExe;

public:
    LintPhpOptions();
    virtual ~LintPhpOptions();

    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    LintPhpOptions& Load();
    LintPhpOptions& Save();

    LintPhpOptions& SetPhpExecutable(const wxFileName& phpExecutable) {
        this->m_phpExe = phpExecutable.GetFullPath();
        return *this;
    }

    const wxString& GetPhpExecutable() const {
        return m_phpExe;
    }
};

#endif // LINTPHPOPTIONS_H
