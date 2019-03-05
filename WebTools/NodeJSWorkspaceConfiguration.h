//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSWorkspaceConfiguration.h
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

#ifndef NODEJSWORKSPACECONFIGURATION_H
#define NODEJSWORKSPACECONFIGURATION_H

#include "cl_config.h"
#include <wx/arrstr.h>

class NodeJSWorkspaceConfiguration : public clConfigItem
{
    wxArrayString m_folders;
    bool m_isOk;
    bool m_showHiddenFiles;
    wxFileName m_filename;

protected:
    void ConvertToRelative(wxString& folder) const;
    void ConvertToRelative(wxArrayString& folders) const;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    NodeJSWorkspaceConfiguration(const wxFileName& filename);
    virtual ~NodeJSWorkspaceConfiguration();

    NodeJSWorkspaceConfiguration& Load();
    NodeJSWorkspaceConfiguration& Save();
    NodeJSWorkspaceConfiguration& SetFolders(const wxArrayString& folders);

    NodeJSWorkspaceConfiguration& SetIsOk(bool isOk)
    {
        this->m_isOk = isOk;
        return *this;
    }
    NodeJSWorkspaceConfiguration& SetShowHiddenFiles(bool showHiddenFiles)
    {
        this->m_showHiddenFiles = showHiddenFiles;
        return *this;
    }
    bool IsShowHiddenFiles() const { return m_showHiddenFiles; }
    wxArrayString GetFolders() const;
    bool IsOk() const { return m_isOk; }
};

#endif // NODEJSWORKSPACECONFIGURATION_H
