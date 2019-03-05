#include "phprefactoringoptions.h"
#include "cl_standard_paths.h"
#include <wx/filename.h>

PHPRefactoringOptions::PHPRefactoringOptions()
    : clConfigItem("phprefactoring")
    , m_phprefactoringPhar("")
    , m_skipPreview(false)
{
    // Set default PHAR file
    wxFileName pharFile(clStandardPaths::Get().GetDataDir(), "phprefactor.phar");
    if(pharFile.FileExists()) {
        m_phprefactoringPhar = pharFile.GetFullPath();
    }
}

PHPRefactoringOptions::~PHPRefactoringOptions() {}

void PHPRefactoringOptions::FromJSON(const JSONItem& json)
{
    m_phprefactoringPhar = json.namedObject("phprefactoringPhar").toString(m_phprefactoringPhar);
    m_skipPreview = json.namedObject("skipPreview").toBool(m_skipPreview);
}

JSONItem PHPRefactoringOptions::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("phprefactoringPhar", m_phprefactoringPhar);
    element.addProperty("skipPreview", m_skipPreview);
    return element;
}

PHPRefactoringOptions& PHPRefactoringOptions::Load()
{
    clConfig config("phprefactoring.conf");
    config.ReadItem(this);
    return *this;
}

PHPRefactoringOptions& PHPRefactoringOptions::Save()
{
    clConfig config("phprefactoring.conf");
    config.WriteItem(this);
    return *this;
}
