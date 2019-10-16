#include "cl_standard_paths.h"
#include "lintoptions.h"
#include "fileutils.h"
#ifndef __WXMSW__
#include "globals.h"
#endif

LintOptions::LintOptions()
    : clConfigItem("phplint")
    , m_lintOnFileLoad(true)
    , m_lintOnFileSave(true)
    , m_phpcsPhar("")
    , m_phpmdPhar("")
    , m_phpmdRules("")
    , m_phpstanPhar("")
{
    wxFileName newConfigFile = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
                               wxFileName::GetPathSeparator() + "phplint.conf";
    if(!newConfigFile.FileExists()) {
        wxFileName oldConfigFile = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
                                   wxFileName::GetPathSeparator() + "php.conf";

        // If the file does not exist, create a new one
        wxString buf;
        if(FileUtils::ReadBufferFromFile(newConfigFile, buf, 1) && (buf == "[")) {
            FileUtils::WriteFileContent(newConfigFile, "{}");
        }
        
        if(!newConfigFile.FileExists()) { FileUtils::WriteFileContent(newConfigFile, "{}"); }

        // Save it
        JSON newRoot(newConfigFile);
        JSONItem e = JSONItem::createObject(GetName());
        e.addProperty("lintOnFileSave", m_lintOnFileSave);
        e.addProperty("lintOnFileLoad", m_lintOnFileLoad);
        newRoot.toElement().append(e);
        newRoot.save(newConfigFile);
    }
}

LintOptions::~LintOptions() {}

void LintOptions::FromJSON(const JSONItem& json)
{
    m_lintOnFileLoad = json.namedObject("lintOnFileLoad").toBool(m_lintOnFileLoad);
    m_lintOnFileSave = json.namedObject("lintOnFileSave").toBool(m_lintOnFileSave);
    m_phpcsPhar = json.namedObject("phpcsPhar").toString(m_phpcsPhar);
    m_phpmdPhar = json.namedObject("phpmdPhar").toString(m_phpmdPhar);
    m_phpmdRules = json.namedObject("phpmdRules").toString(m_phpmdRules);
    m_phpstanPhar = json.namedObject("phpstanPhar").toString(m_phpstanPhar);

#ifndef __WXMSW__
    // Find an installed version of phpcs
    if(m_phpcsPhar.IsEmpty()) {
        wxFileName phpcsFile;
        ::clFindExecutable("phpcs", phpcsFile);
        SetPhpcsPhar(phpcsFile);
    }

    // Find an installed version of phpmd
    if(m_phpmdPhar.IsEmpty()) {
        wxFileName phpmdFile;
        ::clFindExecutable("phpmd", phpmdFile);
        SetPhpmdPhar(phpmdFile);
    }

    // Find an installed version of phpstan
    if(m_phpstanPhar.IsEmpty()) {
        wxFileName phpstanFile;
        ::clFindExecutable("phpstan", phpstanFile);
        SetPhpstanPhar(phpstanFile);
    }
#endif
}

JSONItem LintOptions::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("lintOnFileLoad", m_lintOnFileLoad);
    element.addProperty("lintOnFileSave", m_lintOnFileSave);
    element.addProperty("phpcsPhar", m_phpcsPhar);
    element.addProperty("phpmdPhar", m_phpmdPhar);
    element.addProperty("phpmdRules", m_phpmdRules);
    element.addProperty("phpstanPhar", m_phpstanPhar);
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
