//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : configurationtoolbase.cpp
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
#include "configurationtoolbase.h"

#include "cl_standard_paths.h"
#include "editor_config.h"
#include "globals.h"
#include "serialized_object.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "xmlutils.h"

#include <wx/stdpaths.h>

ConfigurationToolBase::ConfigurationToolBase()
    : m_fileName(wxEmptyString)
{
}

ConfigurationToolBase::~ConfigurationToolBase() {}

bool ConfigurationToolBase::Load(const wxString& fileName)
{
    // Try to open the file from the user data settings first
    wxFileName userFile(clStandardPaths::Get().GetUserDataDir() + wxT("/") + fileName);
    m_fileName = userFile.GetFullPath();

    if(userFile.FileExists() == false) {
        // no such file, create an empty file
        wxString content;
        content << wxT("<") << GetRootName() << wxT("/>");
        wxFFile file;
        file.Open(userFile.GetFullPath(), wxT("w+b"));
        if(file.IsOpened()) {
            file.Write(content);
            file.Close();
        }
    }
    return m_doc.Load(m_fileName);
}

bool ConfigurationToolBase::WriteObject(const wxString& name, SerializedObject* obj)
{
    if(m_doc.IsOk() == false) {
        return false;
    }

    if(!XmlUtils::StaticWriteObject(m_doc.GetRoot(), name, obj))
        return false;
    return ::SaveXmlToFile(&m_doc, m_fileName);
}

bool ConfigurationToolBase::ReadObject(const wxString& name, SerializedObject* obj)
{
    if(m_doc.IsOk() == false) {
        return false;
    }

    return XmlUtils::StaticReadObject(m_doc.GetRoot(), name, obj);
}
