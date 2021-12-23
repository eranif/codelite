//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : configtool.cpp
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
#include "configtool.h"

#include "conffilelocator.h"
#include "editor_config.h"
#include "globals.h"
#include "serialized_object.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#include "wx_xml_compatibility.h"
#include "xmlutils.h"

ConfigTool::ConfigTool()
    : m_fileName(wxEmptyString)
{
}

ConfigTool::~ConfigTool() {}

bool ConfigTool::Load(const wxString& basename, const wxString& version)
{
    wxString initialSettings = ConfFileLocator::Instance()->Locate(basename);
    bool loaded = m_doc.Load(initialSettings);
    wxString xmlVersion;
    if(loaded) {
        xmlVersion = m_doc.GetRoot()->GetPropVal(wxT("Version"), wxEmptyString);
    }

    if(xmlVersion != version) {
        loaded = m_doc.Load(ConfFileLocator::Instance()->GetDefaultCopy(basename));
    }
    m_fileName = ConfFileLocator::Instance()->GetLocalCopy(basename);
    return loaded;
}

bool ConfigTool::WriteObject(const wxString& name, SerializedObject* obj)
{
    if(m_doc.IsOk() == false) {
        return false;
    }

    if(!XmlUtils::StaticWriteObject(m_doc.GetRoot(), name, obj))
        return false;
    return ::SaveXmlToFile(&m_doc, m_fileName);
}

bool ConfigTool::ReadObject(const wxString& name, SerializedObject* obj)
{
    if(m_doc.IsOk() == false) {
        return false;
    }

    return XmlUtils::StaticReadObject(m_doc.GetRoot(), name, obj);
}
