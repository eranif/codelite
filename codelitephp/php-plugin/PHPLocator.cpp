#include "PHPLocator.h"
#include <wx/tokenzr.h>
#include "globals.h"

PHPLocator::PHPLocator() {}

PHPLocator::~PHPLocator() {}

bool PHPLocator::Locate()
{
    return DoLocateBinary() && DoLocateIniFile();
}

bool PHPLocator::DoLocateBinary()
{   
    // test for /usr/local/zend/bin/php
    if(wxFileName("/usr/local/zend/bin/php").Exists()) {
        // Use this PHP
        m_phpExe = "/usr/local/zend/bin/php";
        return true;
    }
    
    // Try to locate /usr/bin/php
    if(wxFileName("/usr/bin/php").Exists()) {
        // use this PHP
        m_phpExe = "/usr/bin/php";
        return true;
    }
    
    // Windows
#ifdef __WXMSW__
    // HKEY_LOCAL_MACHINE\SOFTWARE\codelite\settings
    wxRegKey regZendServer(wxRegKey::HKLM, "SOFTWARE\\Zend Technologies\\ZendServer");
    wxString zendServerInstallPath;
    if ( regZendServer.QueryValue("InstallLocation", zendServerInstallPath) && wxDirExists(zendServerInstallPath)) {
        m_phpExe = wxFileName(zendServerInstallPath, "php.exe");
        m_phpExe.AppendDir("ZendServer");
        m_phpExe.AppendDir("bin");
        return m_phpExe.Exists();
    }
#endif
    return false;
}

bool PHPLocator::DoLocateIniFile() 
{
    // We locate PHP ini first
    // this is done by running php --ini in the command line
    wxString phpExe = m_phpExe.GetFullPath();
    ::WrapWithQuotes(phpExe);
    wxString cmdOuptut = ProcUtils::SafeExecuteCommand(wxString() << phpExe << " --ini");
    // Search for the line:
    // Loaded Configuration File:         /etc/php5/cli/php.ini
    wxArrayString lines = ::wxStringTokenize(cmdOuptut, "\n", wxTOKEN_STRTOK);
    for(size_t i=0; i<lines.GetCount(); ++i) {
        wxString lineLower = lines.Item(i).Lower();
        if(lineLower.Contains("loaded configuration file")) {
            // we got a match
            wxString configFile = lines.Item(i).AfterFirst(':');
            configFile.Trim().Trim(false);
            m_iniFile = configFile;
            return true;
        }
    }
    return false;
}
