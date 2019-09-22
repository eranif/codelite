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
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include <wx/datetime.h>

static wxString __get_user_name()
{
    wxString squashedname, name = wxGetUserName();

    // The wx doc says that 'name' may now be e.g. "Mr. John Smith"
    // So try to make it more suitable to be an extension
    name.MakeLower();
    name.Replace(wxT(" "), wxT("_"));
    for(size_t i = 0; i < name.Len(); ++i) {
        wxChar ch = name.GetChar(i);
        if((ch < wxT('a') || ch > wxT('z')) && ch != wxT('_')) {
            // Non [a-z_] character: skip it
        } else {
            squashedname << ch;
        }
    }

    return (squashedname.IsEmpty() ? wxString(wxT("someone")) : squashedname);
}

clStandardPaths::clStandardPaths() { IgnoreAppSubDir("bin"); }

clStandardPaths::~clStandardPaths() {}

clStandardPaths& clStandardPaths::Get()
{
    static clStandardPaths codelitePaths;
    return codelitePaths;
}

wxString clStandardPaths::GetUserDataDir() const
{
    // If the user has provided an alternative datadir, use it
    if(!m_path.empty()) { return m_path; }

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
    if(!m_dataDir.IsEmpty()) { return m_dataDir; }
    
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

wxString clStandardPaths::GetTempDir() const
{
    static bool once = true;
    static wxString tmpdir;
    if(once) {
        wxString username = __get_user_name();
#if defined(__WXGTK__) || defined(__WXOSX__)
        tmpdir << "/tmp/CodeLite." << username << ".";
#else
        tmpdir << wxStandardPaths::Get().GetTempDir() << "\\CodeLite." << username << ".";
#endif
        tmpdir << wxDateTime::Now().GetTicks();
        // Create the temp folder
        wxFileName::Mkdir(tmpdir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        once = false;
    }
    return tmpdir;
}

wxString clStandardPaths::GetDocumentsDir() const
{
    wxString path = wxStandardPaths::Get().GetDocumentsDir();
#ifdef __WXGTK__
    // On linux, according to the docs, GetDocumentsDir() return the home directory
    // but what we really want is ~/Documents
    wxFileName fp(path, "");
    fp.AppendDir("Documents");
    if(fp.DirExists()) { return fp.GetPath(); }
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

void clStandardPaths::IgnoreAppSubDir(const wxString& subdirPattern)
{
#ifdef USE_POSIX_LAYOUT
    wxStandardPaths::Get().IgnoreAppSubDir(subdirPattern);
#endif
}

void clStandardPaths::RemoveTempDir() { wxFileName::Rmdir(GetTempDir(), wxPATH_RMDIR_RECURSIVE); }
