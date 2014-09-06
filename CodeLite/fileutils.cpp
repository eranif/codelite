//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : fileutils.cpp
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
#include "fileutils.h"
#include <wx/ffile.h>
#include "wx/string.h"
#include <wx/strconv.h>
#include <wx/utils.h>

void FileUtils::OpenFileExplorer(const wxString& path)
{
    // Wrap the path with quotes if needed
    wxString strPath = path;
    if(strPath.Contains(" ")) {
        strPath.Prepend("\"").Append("\"");
    }
    wxString cmd;

#ifdef __WXMSW__
    cmd << "explorer ";
#elif defined(__WXGTK__)
    cmd << "xdg-open ";
#elif defined(__WXMAC__)
    cmd << "open ";
#endif
    if(!cmd.IsEmpty()) {
        cmd << strPath;
        ::wxExecute(cmd);
    }
}

void FileUtils::OpenTerminal(const wxString& path)
{
    wxString strPath = path;
    if(strPath.Contains(" ")) {
        strPath.Prepend("\"").Append("\"");
    }

    wxString cmd;
#ifdef __WXMSW__
    cmd << "cmd";
#elif defined(__WXGTK__)
    // Try to locate gnome-terminal
    if(wxFileName::FileExists("/usr/bin/gnome-terminal")) {
        cmd << "/usr/bin/gnome-terminal";
    } else if(wxFileName::FileExists("/usr/bin/konsole")) {
        cmd << "/usr/bin/konsole";
    } else if(wxFileName::FileExists("/usr/bin/xterm")) {
        cmd << "/usr/bin/xterm";
    } else if(wxFileName::FileExists("/usr/bin/uxterm")) {
        cmd << "/usr/bin/uxterm";
    }
#elif defined(__WXMAC__)
    // osascript -e 'tell app "Terminal" to do script "echo hello"'
    cmd << "osascript -e 'tell app \"Terminal\" to do script \"cd " << strPath << "\"'";
#endif
    if(cmd.IsEmpty()) return;
    ::wxExecute(cmd);
}

bool FileUtils::WriteFileContent(const wxFileName& fn, const wxString& content, const wxMBConv& conv) 
{
    wxFFile file(fn.GetFullPath(), wxT("w+b"));
    if(!file.IsOpened()) {
        return false;
    }
    
    if(!file.Write(content, conv)) {
        return false;
    }
    return true;
}

bool FileUtils::ReadFileContent(const wxFileName& fn, wxString& data, const wxMBConv& conv)
{
    wxFFile file(fn.GetFullPath().GetData(), wxT("rb"));
    if(file.IsOpened() == false) {
        // Nothing to be done
        return false;
    }
    return file.ReadAll(&data, conv);
}
