#ifndef PHPLOCATOR_H
#define PHPLOCATOR_H

#include <wx/filename.h>
#include <wx/arrstr.h>

class PHPLocator
{
    wxFileName m_phpExe;
    wxArrayString m_includePaths;
    wxFileName m_iniFile;

protected:
    bool DoLocateBinary();
    bool DoLocateIniFile();
    bool DoLocateIncludePaths();
    
public:
    PHPLocator();
    ~PHPLocator();

    /**
     * @brief locate the PHP on this machine
     */
    bool Locate();
    
    const wxArrayString& GetIncludePaths() const { return m_includePaths; }
    const wxFileName& GetIniFile() const { return m_iniFile; }
    const wxFileName& GetPhpExe() const { return m_phpExe; }
};

#endif // PHPLOCATOR_H
