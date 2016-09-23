//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_standard_paths.cpp
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

#include "cl_standard_paths.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>

clStandardPaths::clStandardPaths()
{
    IgnoreAppSubDir("bin");
}

clStandardPaths::~clStandardPaths() {}

clStandardPaths& clStandardPaths::Get()
{
    static clStandardPaths codelitePaths;
    return codelitePaths;
}

wxString clStandardPaths::GetUserDataDir() const
{
    // If the user has provided an alternative datadir, use it
    if(!m_path.empty()) {
        return m_path;
    }

#ifdef __WXGTK__

#ifndef NDEBUG

    // Debug mode
    wxFileName fn(wxStandardPaths::Get().GetUserDataDir());
    fn.SetFullName(fn.GetFullName() + "-dbg");
    return fn.GetFullPath();

#else
    // Release mode
    return wxStandardPaths::Get().GetUserDataDir();

#endif
#else // Windows / OSX
    return wxStandardPaths::Get().GetUserDataDir();
#endif
}

void clStandardPaths::SetUserDataDir(const wxString& path) { m_path = path; }

wxString clStandardPaths::GetPluginsDirectory() const
{
#ifdef __WXGTK__
    wxString pluginsDir = PLUGINS_DIR;
#else
#ifdef USE_POSIX_LAYOUT
    wxFileName path(GetInstallDir() + wxT(PLUGINS_DIR), "");
#else
    wxFileName path(GetDataDir(), "");
    path.AppendDir("plugins");
#endif
    wxString pluginsDir = path.GetPath();
#endif
    return pluginsDir;
}

wxString clStandardPaths::GetDataDir() const
{
#ifdef USE_POSIX_LAYOUT
    wxFileName path(wxStandardPaths::Get().GetDataDir() + wxT(INSTALL_DIR), "");
    return path.GetPath();
#else
    return wxStandardPaths::Get().GetDataDir();
#endif
}

wxString clStandardPaths::GetBinFolder() const
{
    wxFileName fnExe(wxStandardPaths::Get().GetExecutablePath());
    return fnExe.GetPath();
}

wxString clStandardPaths::GetBinaryFullPath(const wxString& toolname) const
{
    wxFileName binary(GetBinFolder(), toolname);
    binary.SetExt(wxEmptyString);
#ifdef __WXMSW__
    binary.SetExt("exe");
#endif
    return binary.GetFullPath();
}

wxString clStandardPaths::GetLexersDir() const
{
    wxFileName fn(GetDataDir(), "");
    fn.AppendDir("lexers");
    return fn.GetPath();
}

wxString clStandardPaths::GetProjectTemplatesDir() const
{
    wxFileName fn(GetDataDir(), "");
    fn.AppendDir("templates");
    fn.AppendDir("projects");
    return fn.GetPath();
}

wxString clStandardPaths::GetUserLexersDir() const
{
    wxFileName fn(GetUserDataDir(), "");
    fn.AppendDir("lexers");
    return fn.GetPath();
}

wxString clStandardPaths::GetUserProjectTemplatesDir() const
{
    wxFileName fn(GetUserDataDir(), "");
    fn.AppendDir("templates");
    fn.AppendDir("projects");
    return fn.GetPath();
}

wxString clStandardPaths::GetExecutablePath() const { return wxStandardPaths::Get().GetExecutablePath(); }

wxString clStandardPaths::GetTempDir() const { return wxStandardPaths::Get().GetTempDir(); }

wxString clStandardPaths::GetDocumentsDir() const
{
    wxString path = wxStandardPaths::Get().GetDocumentsDir();
#ifdef __WXGTK__
    // On linux, according to the docs, GetDocumentsDir() return the home directory
    // but what we really want is ~/Documents
    wxFileName fp(path, "");
    fp.AppendDir("Documents");
    if(fp.DirExists()) {
        return fp.GetPath();
    }
#endif
    return path;
}

wxString clStandardPaths::GetInstallDir() const
{
#ifdef __WXGTK__
    return GetBinFolder();
#else
#ifdef USE_POSIX_LAYOUT
    wxFileName path(wxStandardPaths::Get().GetDataDir(), "");
    return path.GetPath();
#else
    return GetBinFolder();
#endif
#endif
}

void clStandardPaths::IgnoreAppSubDir(const wxString & subdirPattern)
{
#ifdef USE_POSIX_LAYOUT
    wxStandardPaths::Get().IgnoreAppSubDir(subdirPattern);
#endif
}
