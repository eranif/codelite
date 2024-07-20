//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_utils.h
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

#ifndef PHP_UTILS_H
#define PHP_UTILS_H

#include "ieditor.h"

#include <wx/wx.h>

/**
 * @brief return true of the editor holds a file of type PHP
 */
bool IsPHPFile(IEditor* editor);

/**
 * @brief return true if this is a PHP file
 */
bool IsPHPFileByExt(const wxString& fileName);

bool IsPHPSection(int styleAtPos );
bool IsPHPCommentOrString(int styleAtPos);

wxString GetResourceDirectory();

#define FILE_SCHEME "file://"

/**
 * @brief convert file in the URI format: file:///path/to/file to -> /path/to/file
 */
wxString URIToFileName(const wxString &uriFileName);

/**
 * @brief convert filename to URI file format 
 */
wxString FileNameToURI(const wxString &filename);

/**
 * @brief encode a string into base64
 */
wxString Base64Encode(const wxString &str);

wxString MapRemoteFileToLocalFile(const wxString &remoteFile);

#endif // PHP_UTILS

