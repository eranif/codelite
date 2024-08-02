//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : subversion_password_db.cpp
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

#include "subversion_password_db.h"

#include "cl_standard_paths.h"
#include "md5/wxmd5.h"
#include "xor_string.h"

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>

SubversionPasswordDb::SubversionPasswordDb()
{
    // disable logging
    wxFileName passwordIni(clStandardPaths::Get().GetUserDataDir(), "passwords.ini");
    passwordIni.AppendDir("subversion");
    wxFileName::Mkdir(passwordIni.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    m_fileConfig = new wxFileConfig(
        wxEmptyString, wxEmptyString, passwordIni.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
}

SubversionPasswordDb::~SubversionPasswordDb()
{
    m_fileConfig->Flush();
    wxDELETE(m_fileConfig);
}

bool SubversionPasswordDb::GetLogin(const wxString& url, wxString& user, wxString& password)
{
    wxString escapedUrl(wxMD5::GetDigest(url));
    if(m_fileConfig->HasGroup(escapedUrl) == false)
        return false;
    
    // read and decrypt the password/username
    m_fileConfig->Read(escapedUrl + wxT("/user"), &user);
    XORString userXor(user);
    user = userXor.Decrypt();
    
    m_fileConfig->Read(escapedUrl + wxT("/password"), &password);
    XORString passXor(password);
    password = passXor.Decrypt();
    return true;
}

void SubversionPasswordDb::SetLogin(const wxString& url, const wxString& user, const wxString& password)
{
    wxString escapedUrl(wxMD5::GetDigest(url));
    if(m_fileConfig->HasGroup(escapedUrl)) {
        m_fileConfig->DeleteGroup(escapedUrl);
    }

    m_fileConfig->Write(escapedUrl + wxT("/user"), XORString(user).Encrypt());
    m_fileConfig->Write(escapedUrl + wxT("/password"), XORString(password).Encrypt());
    m_fileConfig->Flush();
}

void SubversionPasswordDb::DeleteLogin(const wxString& url)
{
    wxString escapedUrl(wxMD5::GetDigest(url));
    if(m_fileConfig->HasGroup(escapedUrl)) {
        m_fileConfig->DeleteGroup(escapedUrl);
    }
    m_fileConfig->Flush();
}
