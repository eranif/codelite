//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPLocator.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
