#include "phprefactoringoptions.h"
#include "cl_standard_paths.h"

PHPRefactoringOptions::PHPRefactoringOptions()
    : clConfigItem("phprefactoring")
    , m_phprefactoringPhar("")
{
}

PHPRefactoringOptions::~PHPRefactoringOptions() {}

void PHPRefactoringOptions::FromJSON(const JSONElement& json)
{
    m_phprefactoringPhar = json.namedObject("phprefactoringPhar").toString(m_phprefactoringPhar);
}

JSONElement PHPRefactoringOptions::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("phprefactoringPhar", m_phprefactoringPhar);
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
