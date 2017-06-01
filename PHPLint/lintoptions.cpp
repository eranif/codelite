#include "lintoptions.h"
#ifndef __WXMSW__
#include "globals.h"
#endif

LintOptions::LintOptions()
    : clConfigItem("phplint")
    , m_lintOnFileLoad(false)
    , m_lintOnFileSave(false)
    , m_phpcsPhar("")
    , m_phpmdPhar("")
    , m_phpmdRules("")
{
}

LintOptions::~LintOptions() {}

void LintOptions::FromJSON(const JSONElement& json)
{
    m_lintOnFileLoad = json.namedObject("lintOnFileLoad").toBool(m_lintOnFileLoad);
    m_lintOnFileSave = json.namedObject("lintOnFileSave").toBool(m_lintOnFileSave);
    m_phpcsPhar = json.namedObject("phpcsPhar").toString(m_phpcsPhar);
    m_phpmdPhar = json.namedObject("phpmdPhar").toString(m_phpmdPhar);
    m_phpmdRules = json.namedObject("phpmdRules").toString(m_phpmdRules);

#ifndef __WXMSW__
    // Find an installed version of phpcs
    if (m_phpcsPhar.IsEmpty()) {
        wxFileName phpcsFile;
        clFindExecutable("phpcs", phpcsFile);
        SetPhpcsPhar(phpcsFile);
    }

    // Find an installed version of phpmd
    if (m_phpmdPhar.IsEmpty()) {
        wxFileName phpmdFile;
        clFindExecutable("phpmd", phpmdFile);
        SetPhpmdPhar(phpmdFile);
    }
#endif
}

JSONElement LintOptions::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("lintOnFileLoad", m_lintOnFileLoad);
    element.addProperty("lintOnFileSave", m_lintOnFileSave);
    element.addProperty("phpcsPhar", m_phpcsPhar);
    element.addProperty("phpmdPhar", m_phpmdPhar);
    element.addProperty("phpmdRules", m_phpmdRules);
    return element;
}

LintOptions& LintOptions::Load()
{
    clConfig config("phplint.conf");
    config.ReadItem(this);
    return *this;
}

LintOptions& LintOptions::Save()
{
    clConfig config("phplint.conf");
    config.WriteItem(this);
    return *this;
}
