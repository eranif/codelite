//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : outline_settings.cpp
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

#include "outline_settings.h"
#include <wx/stdpaths.h>
#include "JSON.h"

OutlineSettings::OutlineSettings()
{
}

OutlineSettings::~OutlineSettings()
{
}

void OutlineSettings::Load()
{
/*    wxFileName fn(clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config"), wxT("outline.conf"));
    JSON json(fn);
    if( json.isOk() ) {
        
    }*/
}

void OutlineSettings::Save()
{
/*    wxFileName fn(clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config"), wxT("outline.conf"));
    JSON json(cJSON_Object);
   
    json.save(fn);*/
}
