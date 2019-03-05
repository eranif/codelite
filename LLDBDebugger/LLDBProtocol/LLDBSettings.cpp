//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBSettings.cpp
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

#include "LLDBSettings.h"
#include <wx/filename.h>
#include "cl_standard_paths.h"
#include <wx/ffile.h>
#include "cl_standard_paths.h"
#include <wx/utils.h>
#include "globals.h"

#ifdef __WXMAC__
static const wxString s_DefaultTypes = "type summary add wxString --summary-string \"${var.m_impl}\"\n"
                                       "type summary add wxPoint --summary-string \"x = ${var.x}, y = ${var.y}\"\n"
                                       "type summary add wxRect --summary-string \"(x = ${var.x}, y = ${var.y}) (width "
                                       "= ${var.width}, height = ${var.height})\"\n";
#else
static const wxString s_DefaultTypes = "type summary add wxString --summary-string \"${var.m_impl._M_dataplus._M_p}\"\n"
                                       "type summary add wxPoint --summary-string \"x = ${var.x}, y = ${var.y}\"\n"
                                       "type summary add wxRect --summary-string \"(x = ${var.x}, y = ${var.y}) (width "
                                       "= ${var.width}, height = ${var.height})\"\n";
#endif

LLDBSettings::LLDBSettings()
    : m_arrItems(50)
    , m_stackFrames(100)
    , m_flags(kLLDBOptionRaiseCodeLite)
    , m_proxyIp("127.0.0.1")
    , m_proxyPort(13610)
{
    m_types = s_DefaultTypes;
    wxFileName exePath;
#ifdef __WXGTK__
    // Default path to lldb-server
    wxString path;
    ::wxGetEnv("PATH", &path);
    wxArrayString lldbServerOptions;
    lldbServerOptions.Add("lldb-server"); // no version suffix
    for(size_t major = 3; major < 8; ++major) {
        for(size_t minor = 0; minor < 10; ++minor) {
            lldbServerOptions.Add(wxString() << "lldb-server-" << major << "." << minor);
        }
    }
    for(size_t i = 0; i < lldbServerOptions.size(); ++i) {
        if(::clFindExecutable(lldbServerOptions.Item(i), exePath)) {
            break;
        }
    }

#elif defined(__WXOSX__)
    exePath = wxFileName(clStandardPaths::Get().GetBinaryFullPath("debugserver"));
#endif

    if(exePath.IsOk() && exePath.FileExists()) {
        m_debugserver = exePath.GetFullPath();
    }
}

LLDBSettings::~LLDBSettings() {}

wxString LLDBSettings::LoadPerspective()
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "lldb.perspective");
    fn.AppendDir("config");

    wxFFile fp(fn.GetFullPath(), "rb");
    if(fp.IsOpened()) {
        wxString content;
        fp.ReadAll(&content, wxConvUTF8);
        return content;
    }
    return "";
}

void LLDBSettings::SavePerspective(const wxString& perspective)
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "lldb.perspective");
    fn.AppendDir("config");

    wxFFile fp(fn.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(perspective);
        fp.Close();
    }
}

void LLDBSettings::FromJSON(const JSONItem& json)
{
    m_arrItems = json.namedObject("m_maxArrayElements").toSize_t(m_arrItems);
    m_stackFrames = json.namedObject("m_maxCallstackFrames").toSize_t(m_stackFrames);
    m_flags = json.namedObject("m_flags").toSize_t(m_flags);
    m_types = json.namedObject("m_types").toString(s_DefaultTypes);
    m_proxyPort = json.namedObject("m_proxyPort").toInt(13610);
    m_proxyIp = json.namedObject("m_proxyIp").toString("127.0.0.1");
    m_lastLocalFolder = json.namedObject("m_lastLocalFolder").toString();
    m_lastRemoteFolder = json.namedObject("m_lastRemoteFolder").toString();
    m_debugserver = json.namedObject("m_debugserver").toString(m_debugserver);
}

JSONItem LLDBSettings::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("m_maxArrayElements", m_arrItems);
    json.addProperty("m_maxCallstackFrames", m_stackFrames);
    json.addProperty("m_flags", m_flags);
    json.addProperty("m_types", m_types);
    json.addProperty("m_proxyPort", m_proxyPort);
    json.addProperty("m_proxyIp", m_proxyIp);
    json.addProperty("m_lastLocalFolder", m_lastLocalFolder);
    json.addProperty("m_lastRemoteFolder", m_lastRemoteFolder);
    json.addProperty("m_debugserver", m_debugserver);
    return json;
}

LLDBSettings& LLDBSettings::Load()
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "lldb.conf");
    fn.AppendDir("config");
    if(fn.Exists()) { // check for existance, or we will get an error in the Trace tab
        // Open the file
        wxFFile fp(fn.GetFullPath(), "rb");
        if(fp.IsOpened()) {
            wxString content;
            fp.ReadAll(&content, wxConvUTF8);

            JSON root(content);
            FromJSON(root.toElement());
            fp.Close();
        }
    }
    return *this;
}

LLDBSettings& LLDBSettings::Save()
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "lldb.conf");
    fn.AppendDir("config");

    wxFFile fp(fn.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(ToJSON().format());
        fp.Close();
    }
    return *this;
}
