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

#include "asyncprocess.h"
#include "codelite_exports.h"
#include "macros.h"
#include "wx/filename.h"

#include <wx/filename.h>
#include <wx/log.h>

#define clRemoveFile(filename) FileUtils::RemoveFile(filename, (wxString() << __FILE__ << ":" << __LINE__))

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
                clRemoveFile(m_filename);
            }
        }
    };

public:
    static bool ReadFileContent(const wxFileName& fn, wxString& data, const wxMBConv& conv = wxConvUTF8);
    static bool ReadFileContentRaw(const wxFileName& fn, std::string& data);

    /**
     * @brief attempt to read up to bufferSize from the beginning of file
     */
    static bool ReadBufferFromFile(const wxFileName& fn, wxString& data, size_t bufferSize);

    /**
     * @brief set the file content (replacing it)
     */
    static bool WriteFileContent(const wxFileName& fn, const wxString& content, const wxMBConv& conv = wxConvUTF8);
    /**
     * @brief append content to end of the file
     */
    static bool AppendFileContent(const wxFileName& fn, const wxString& content, const wxMBConv& conv = wxConvUTF8);
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
    static void OpenTerminal(const wxString& path, const wxString& user_command = "", bool pause_when_exit = false);

    /**
     * @brief open the built-in terminal
     */
    static void OpenBuiltInTerminal(const wxString& wd, const wxString& user_command, bool pause_when_exit = false);

    /**
     * @brief open ssh terminal
     * @param sshClient ssh client to use (putty, ssh etc)
     * @param connectString e.g. eran@host
     * @param password the password
     * @param port ssh port
     */
    static void OpenSSHTerminal(const wxString& sshClient, const wxString& connectString, const wxString& password,
                                int port = 22);

    /**
     * @brief OSX only: open Terminal and return its TTY
     * @param path working directory
     * @param [output] tty the TTY of the launched terminal
     */
    static void OSXOpenDebuggerTerminalAndGetTTY(const wxString& path, const wxString& appname, wxString& tty,
                                                 long& pid);

    /**
     * @brief return the command needed to open OSX terminal at a given directory and launch a command
     */
    static wxString GetOSXTerminalCommand(const wxString& command, const wxString& workingDirectory);

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
     * @brief an efficient way to tokenize string into words (separated by SPACE and/or TAB)
     * @code
     * wxString str = "My String That Requires Tokenize";
     * wxString word; // The output
     * size_t offset = 0;
     * while(clNextWord(str, offset, word)) {
     *      // Do something with "word" here
     * }
     * @codeend
     * @param str the string to tokenize
     * @param offset used internally, allocate one on the stack and initialise it to 0
     * @param word [output]
     * @return true if a word was found
     */
    static bool NextWord(const wxString& str, size_t& offset, wxString& word, bool makeLower = false);

    /**
     * @brief split a string by whitespace
     */
    static size_t SplitWords(const wxString& str, wxStringSet_t& outputSet, bool makeLower = false);

    /**
     * @brief decode URI using percent encoding
     */
    static wxString DecodeURI(const wxString& uri);

    /**
     * @brief encode URI using percent encoding
     */
    static wxString EncodeURI(const wxString& uri);

    /**
     * @brief escape string. Each space and double quotes marker is escaped with backslash
     */
    static wxString EscapeString(const wxString& str);

    /**
     * @brief is the file or folder a hidden file?
     */
    static bool IsHidden(const wxFileName& path);
    /**
     * @brief is the file or folder a hidden file?
     */
    static bool IsHidden(const wxString& path);

    /**
     * @brief return true if filename is a symlink
     */
    static bool IsSymlink(const wxFileName& filename) { return IsSymlink(filename.GetFullPath()); }
    /**
     * @brief return true if filename is a symlink
     */
    static bool IsSymlink(const wxString& filename);

    /**
     * @brief return true if filename is a symlink
     */
    static bool IsDirectory(const wxFileName& filename) { return IsDirectory(filename.GetFullPath()); }
    /**
     * @brief return true if filename is a symlink
     */
    static bool IsDirectory(const wxString& filename);

    /**
     * @brief is `filename` an executable?
     * on Linux/macOS, we check if the file is an ELF file, on Windows, we check if the
     * file has the extension `.exe` or `.dll`
     */
    static bool IsBinaryExecutable(const wxString& filename);
    static bool IsBinaryExecutable(const wxFileName& filename) { return IsBinaryExecutable(filename.GetFullPath()); }

    /**
     * @brief set permissions to filename
     */
    static bool SetFilePermissions(const wxFileName& filename, mode_t perm);

    /**
     * @brief get file permissions
     */
    static bool GetFilePermissions(const wxFileName& filename, mode_t& perm);

    /**
     * @brief return the file modification time
     */
    static time_t GetFileModificationTime(const wxFileName& filename);

    /**
     * @brief return the file size, in bytes
     */
    static size_t GetFileSize(const wxFileName& filename);

    /**
     * @brief replace any unwanted characters with underscore
     * The chars that we replace are:
     * @-^%&$#@!(){}[]+=;,.
     * @param name
     * @return modified name excluding the above chars (will be replaced with _)
     */
    static wxString NormaliseName(const wxString& name);

    /**
     * @brief remove a file from the file system
     * @param filename file name to remove
     * @param context unique context string which will be logged to the log file
     */
    static bool RemoveFile(const wxFileName& filename, const wxString& context = "")
    {
        return RemoveFile(filename.GetFullPath(), context);
    }
    /**
     * @brief same as above
     */
    static bool RemoveFile(const wxString& filename, const wxString& context = "");

    static unsigned int UTF8Length(const wchar_t* uptr, unsigned int tlen);

    /**
     * @brief (on Linux) makes-absolute filepath, and dereferences it and any symlinked dirs in the path
     */
    static wxString RealPath(const wxString& filepath);

    /**
     * @brief convert string into std::string
     */
    static std::string ToStdString(const wxString& str);

    /**
     * @brief create an environment list from string in the format of:
     * ...
     * key=value
     * key1=value2
     * ...
     */
    static clEnvList_t CreateEnvironment(const wxString& envstr);

    /**
     * @brief Check if the file 'name' is findable on the user's system
     * @param name the name of the file to locate
     * @param exepath will contain its filepath if successfully located
     * @param hint extra paths to search
     * @param list of suffixes. On Linux, some files may have number attached to them like: lldb-10, lldb-9
     * passing suffix_list = {"-10", "-9"...} will also check for these files (in order)
     * @return true if a filepath was found
     */
    static bool FindExe(const wxString& name, wxFileName& exepath, const wxArrayString& hint = {},
                        const wxArrayString& suffix_list = {});

    /**
     * @brief create a temporary file *name* in a given folder with a given name prefix and an extension
     * note that this function does not actually create the file, but only generates a name
     */
    static wxFileName CreateTempFileName(const wxString& folder, const wxString& prefix, const wxString& ext);

    /**
     * @brief find a file(s) with similar name and path, but with a different extension
     * @param filename
     * @param extensions
     * @param [out] the files found
     */
    static size_t FindSimilar(const wxFileName& filename, const std::vector<wxString>& extensions,
                              std::vector<wxFileName>& vout);

    /**
     * @brief given URI, parse it into its basic parts
     */
    static bool ParseURI(const wxString& uri, wxString& path, wxString& scheme, wxString& user, wxString& host,
                         wxString& port);

    /**
     * @brief covnert path to uri
     * /home/eran/file.cpp -> file:///home/eran/file.cpp
     * file:///home/eran/file.cpp -> file:///home/eran/file.cpp
     */
    static wxString FilePathToURI(const wxString& filepath);

    /**
     * @brief convert uri to file path
     * file:///home/eran/file.cpp -> /home/eran/file.cpp
     * /home/eran/file.cpp -> /home/eran/file.cpp
     */
    static wxString FilePathFromURI(const wxString& uri);

    /**
     * @brief calculate checksum of a given file
     */
    static bool GetChecksum(const wxString& filepath, size_t* checksum);
};
#endif // FILEUTILS_H
