#ifndef PHPOPTIONS_H
#define PHPOPTIONS_H

#include "cl_config.h" // Base class: clConfigItem
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK PhpOptions : public clConfigItem
{
    wxString m_phpExe;

public:
    PhpOptions();
    virtual ~PhpOptions();

    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    PhpOptions& Load();
    PhpOptions& Save();

    PhpOptions& SetPhpExecutable(const wxFileName& phpExecutable) {
        this->m_phpExe = phpExecutable.GetFullPath();
        return *this;
    }

    const wxString& GetPhpExecutable() const {
        return m_phpExe;
    }
};

#endif // PHPOPTIONS_H
