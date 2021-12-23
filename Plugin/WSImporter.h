//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : WSImporter.h
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

#ifndef WSIMPORTER_H
#define WSIMPORTER_H

#include "GenericImporter.h"
#include "codelite_exports.h"

#include <memory>
#include <set>
#include <vector>
#include <wx/string.h>

class WXDLLIMPEXP_SDK WSImporter
{
public:
    WSImporter();
    ~WSImporter();

    void Load(const wxString& filename, const wxString& defaultCompiler);
    bool Import(wxString& errMsg);

protected:
    void AddImporter(std::shared_ptr<GenericImporter> importer);

private:
    bool ContainsEnvVar(std::vector<wxString> elems);
    std::set<wxString> GetListEnvVarName(std::vector<wxString> elems);
    wxString GetVPath(const wxString& filename, const wxString& virtualPath);

    wxString filename, defaultCompiler;
    std::vector<std::shared_ptr<GenericImporter>> importers;
};

#endif // WSIMPORTER_H
