//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : fileutils.h
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
#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "wx/filename.h"
#include "codelite_exports.h"
#include <wx/filename.h>
#include <wx/log.h>

class WXDLLIMPEXP_CL FileUtils
{
public:
    class Deleter
    {
        wxFileName m_filename;

    public:
        Deleter(const wxFileName& filename)
            : m_filename(filename)
        {
        }

        ~Deleter()
        {
            if(m_filename.Exists()) {
                wxLogNull noLog;
                ::wxRemoveFile(m_filename.GetFullPath());
            }
        }
    };

public:
    static bool ReadFileContent(const wxFileName& fn, wxString& data, const wxMBConv& conv = wxConvUTF8);

    /**
     * @brief set the file content (replacing it)
     */
    static bool WriteFileContent(const wxFileName& fn, const wxString& content, const wxMBConv& conv = wxConvUTF8);

    /**
     * @brief open file explorer at given path
     */
    static void OpenFileExplorer(const wxString& path);

    /**
     * @brief open file explorer at given path and select the input file
     */
    static void OpenFileExplorerAndSelect(const wxFileName& filename);

    /**
     * @brief launch the OS default terminal at a given path
     */
    static void OpenTerminal(const wxString& path);

    /**
     * @brief open ssh terminal
     * @param sshClient ssh client to use (putty, ssh etc)
     * @param connectString e.g. eran@host
     * @param password the password
     * @param port ssh port
     */
    static void OpenSSHTerminal(
        const wxString& sshClient, const wxString& connectString, const wxString& password, int port = 22);

    /**
     * @brief OSX only: open Terminal and return its TTY
     * @param path working directory
     * @param [output] tty the TTY of the launched terminal
     */
    static void OSXOpenDebuggerTerminalAndGetTTY(const wxString& path, wxString& tty, long& pid);

    /**
     * @brief file masking search
     */
    static bool WildMatch(const wxString& mask, const wxString& filename);
    /**
     * @brief file masking search
     */
    static bool WildMatch(const wxString& mask, const wxFileName& filename);

    /**
     * @brief file masking search
     */
    static bool WildMatch(const wxArrayString& masks, const wxString& filename);

    /**
     * @brief return true if needle exists in the haystack. Supports fuzzy search
     * @param needle the pattern to search
     * @param haystack the string to search on
     */
    static bool FuzzyMatch(const wxString& needle, const wxString& haystack);

    /**
     * @brief decode URI using percent encoding
     */
    static wxString DecodeURI(const wxString& uri);

    /**
     * @brief encode URI using percent encoding
     */
    static wxString EncodeURI(const wxString& uri);

    /**
     * @brief is the file or folder a hidden file?
     */
    static bool IsHidden(const wxFileName& path);
    /**
     * @brief is the file or folder a hidden file?
     */
    static bool IsHidden(const wxString& path);
};
#endif // FILEUTILS_H
