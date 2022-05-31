//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : build_system.cpp
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

#include "build_system.h"

#include "wx_xml_compatibility.h"
#include "xmlutils.h"

#include <wx/thread.h>

BuilderConfig::BuilderConfig(wxXmlNode* node)
    : m_isActive(false)
{
    if(node) {
        m_name = XmlUtils::ReadString(node, wxT("Name"));
        if(m_name == "CodeLite Make Generator") {
            m_name = "CodeLite Makefile Generator";
        }
        m_toolPath = XmlUtils::ReadString(node, wxT("ToolPath"));
        m_toolOptions = XmlUtils::ReadString(node, wxT("Options"));
        m_toolJobs = wxString() << wxThread::GetCPUCount();
        m_isActive = XmlUtils::ReadBool(node, wxT("Active"), m_isActive);
    }
}

BuilderConfig::~BuilderConfig() {}

wxXmlNode* BuilderConfig::ToXml() const
{
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("BuildSystem"));
    node->AddAttribute(wxT("Name"), m_name);
    node->AddAttribute(wxT("ToolPath"), m_toolPath);
    node->AddAttribute(wxT("Options"), m_toolOptions);
    node->AddAttribute(wxT("Jobs"), m_toolJobs);
    node->AddAttribute(wxT("Active"), m_isActive ? wxString(wxT("yes")) : wxString(wxT("no")));
    return node;
}
