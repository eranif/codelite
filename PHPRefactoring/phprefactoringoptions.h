#ifndef PHPREFACTORINGOPTIONS_H
#define PHPREFACTORINGOPTIONS_H

#include "cl_config.h" // Base class: clConfigItem

class PHPRefactoringOptions : public clConfigItem
{
    wxString m_phprefactoringPhar;

public:
    PHPRefactoringOptions();
    virtual ~PHPRefactoringOptions();

    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    PHPRefactoringOptions& Load();
    PHPRefactoringOptions& Save();

    PHPRefactoringOptions& SetPhprefactoringPhar(const wxFileName& PhprefactoringPhar)
    {
        this->m_phprefactoringPhar = PhprefactoringPhar.GetFullPath();
        return *this;
    }

    const wxString& GetPhprefactoringPhar() const
    {
        return m_phprefactoringPhar;
    }
};

#endif // PHPREFACTORINGOPTIONS_H
