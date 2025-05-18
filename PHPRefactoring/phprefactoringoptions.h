#ifndef PHPREFACTORINGOPTIONS_H
#define PHPREFACTORINGOPTIONS_H

#include "cl_config.h" // Base class: clConfigItem

class PHPRefactoringOptions : public clConfigItem
{
    wxString m_phprefactoringPhar;
    bool m_skipPreview;

public:
    PHPRefactoringOptions();
    virtual ~PHPRefactoringOptions();

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    PHPRefactoringOptions& Load();
    PHPRefactoringOptions& Save();

    PHPRefactoringOptions& SetPhprefactoringPhar(const wxFileName& PhprefactoringPhar)
    {
        this->m_phprefactoringPhar = PhprefactoringPhar.GetFullPath();
        return *this;
    }

    const wxString& GetPhprefactoringPhar() const { return m_phprefactoringPhar; }

    PHPRefactoringOptions& SetSkipPreview(bool SkipPreview)
    {
        this->m_skipPreview = SkipPreview;
        return *this;
    }

    bool GetSkipPreview() const { return m_skipPreview; }
};

#endif // PHPREFACTORINGOPTIONS_H
