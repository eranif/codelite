//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_strings.h
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

#ifndef PHP_STRINGS_H
#define PHP_STRINGS_H

#include <wx/string.h>
#include <wx/frame.h>
#include <wx/app.h>

#define FRAME static_cast<wxFrame*>(static_cast<wxApp*>(wxApp::GetInstance())->GetTopWindow())

namespace PHPStrings {

const wxString PHP_WORKSPACE_EXT        = wxT("workspace");
const wxString PHP_WORKSPACE_VIEW_TITLE = wxT("PHP");
const wxString PHP_WORKSPACE_VIEW_LABEL = _("PHP");

};

#endif // PHP_STRINGS_H

