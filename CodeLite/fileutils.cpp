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
#include "cl_standard_paths.h"
#include "dirsaver.h"
#include "file_logger.h"
#include "fileutils.h"
#include "macros.h"
#include "procutils.h"
#include "wx/string.h"
#include "wxStringHash.h"
#include <map>
#include <wx/ffile.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/strconv.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>

#ifdef __WXGTK__
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

void FileUtils::OpenFileExplorer(const wxString& path)
{
    // Wrap the path with quotes if needed
    wxString strPath = path;
    if(strPath.Contains(" ")) { strPath.Prepend("\"").Append("\""); }
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

#ifdef __WXGTK__
struct TerminalCookie {
    size_t idx = 0;
    bool IsOk(const std::vector<std::pair<wxString, wxString> >& terminals) const { return (idx < terminals.size()); }
};

static wxString GTKGetTerminal(const wxString& command, TerminalCookie& cookie)
{
    static std::vector<std::pair<wxString, wxString> > Terminals;
    if(Terminals.empty()) {
        // Try to locate gnome-terminal
        if(wxFileName::FileExists("/usr/bin/lxterminal")) {
            wxString cmd = "lxterminal";
            wxString titlePattern = "-e \"$COMMAND\"";
            Terminals.push_back({ cmd, titlePattern });
        }

        if(wxFileName::FileExists("/usr/bin/konsole")) {
            wxString cmd = "konsole -p font=\"Monospace,12\"";
            wxString titlePattern = "-e $COMMAND";
            Terminals.push_back({ cmd, titlePattern });
        }

        if(wxFileName::FileExists("/usr/bin/gnome-terminal")) {
            wxString cmd = "/usr/bin/gnome-terminal";
            wxString titlePattern = "-e \"$COMMAND\"";
            Terminals.push_back({ cmd, titlePattern });
        }

        if(wxFileName::FileExists("/usr/bin/xterm")) {
            wxString cmd = "xterm";
            wxString titlePattern = "-e \"$COMMAND\"";
            Terminals.push_back({ cmd, titlePattern });
        }

        if(wxFileName::FileExists("/usr/bin/uxterm")) {
            wxString cmd = "uxterm";
            wxString titlePattern = "-e \"$COMMAND\"";
            Terminals.push_back({ cmd, titlePattern });
        }
    }
    if(!cookie.IsOk(Terminals)) { return ""; }
    wxString cmd = Terminals[cookie.idx].first;
    wxString extra = Terminals[cookie.idx].second;
    ++cookie.idx;
    if(!command.IsEmpty()) {
        extra.Replace("$COMMAND", command);
        return cmd + " " + extra;
    } else {
        return cmd;
    }
}

static void GTKOpenTerminal(const wxString& command, const wxString& path)
{
    DirSaver ds;
    if(!path.IsEmpty() && wxDirExists(path)) { ::wxSetWorkingDirectory(path); }

    TerminalCookie cookie;
    while(true) {
        wxString cmd = GTKGetTerminal(command, cookie);
        if(cmd.IsEmpty()) { return; }
        clDEBUG() << "Trying terminal" << cmd;
        long PID = ::wxExecute(cmd);
        if(PID != wxNOT_FOUND) {
            // Successful launch
            wxThread::Sleep(150);
            // Check that the process is actually running
            if(::kill(PID, 0) == 0) {
                clDEBUG() << "Launched terminal (PID=" << PID << "):" << cmd;
                break;
            } else {
                // The process terminated
                clDEBUG() << "Failed to launch terminal:" << cmd;
            }
        }
        // Try another terminal
    }
}

#endif

void FileUtils::OpenTerminal(const wxString& path)
{
    wxString strPath = path;
    if(strPath.Contains(" ")) { strPath.Prepend("\"").Append("\""); }

    wxString cmd;
#ifdef __WXMSW__
    cmd << "cmd";
    DirSaver ds;
    ::wxSetWorkingDirectory(path);

#elif defined(__WXGTK__)
    GTKOpenTerminal("", path);
    return;

#elif defined(__WXMAC__)
    strPath = path;
    if(strPath.Contains(" ")) { strPath.Prepend("\\\"").Append("\\\""); }
    // osascript -e 'tell app "Terminal" to do script "echo hello"'
    cmd << "osascript -e 'tell app \"Terminal\" to do script \"cd " << strPath << "\"'";
    CL_DEBUG(cmd);
    ::system(cmd.mb_str(wxConvUTF8).data());
    return;
#endif
    if(cmd.IsEmpty()) return;
    ::wxExecute(cmd);
}

bool FileUtils::WriteFileContent(const wxFileName& fn, const wxString& content, const wxMBConv& conv)
{
    wxFFile file(fn.GetFullPath(), wxT("w+b"));
    if(!file.IsOpened()) { return false; }

    if(!file.Write(content, conv)) { return false; }
    return true;
}

bool FileUtils::ReadFileContent(const wxFileName& fn, wxString& data, const wxMBConv& conv)
{
    wxString filename = fn.GetFullPath();
    wxFFile file(filename, wxT("rb"));
    if(file.IsOpened() == false) {
        // Nothing to be done
        return false;
    }
    return file.ReadAll(&data, conv);
}

void FileUtils::OpenFileExplorerAndSelect(const wxFileName& filename)
{
#ifdef __WXMSW__
    wxString strPath = filename.GetFullPath();
    if(strPath.Contains(" ")) { strPath.Prepend("\"").Append("\""); }
    wxString cmd;
    cmd << "explorer /select," << strPath;
    ::wxExecute(cmd);
#else
    OpenFileExplorer(filename.GetPath());
#endif
}

void FileUtils::OSXOpenDebuggerTerminalAndGetTTY(const wxString& path, wxString& tty, long& pid)
{
    tty.Clear();
    wxString command;
    wxString tmpfile;
    wxString escapedPath = path;
    if(escapedPath.Contains(" ")) { escapedPath.Prepend("\"").Append("\""); }
    tmpfile << "/tmp/terminal.tty." << ::wxGetProcessId();
    command << "osascript -e 'tell app \"Terminal\" to do script \"tty > " << tmpfile << " && clear && sleep 12345\"'";
    CL_DEBUG("Executing: %s", command);
    long res = ::wxExecute(command);
    if(res == 0) {
        CL_WARNING("Failed to execute command:\n%s", command);
        return;
    }

    // Read the tty from the file, wait for it up to 10 seconds
    wxFileName ttyFile(tmpfile);
    pid = wxNOT_FOUND;
    for(size_t i = 0; i < 10; ++i) {
        if(!ttyFile.Exists()) {
            ::wxSleep(1);
            continue;
        }
        ReadFileContent(ttyFile, tty);
        tty.Trim().Trim(false);

        // Remove the file
        clRemoveFile(ttyFile.GetFullPath());

        // Get the parent process ID (we want the parent PID and not
        // the sleep command PID)
        wxString psCommand;
        psCommand << "ps -A -o ppid,command";
        wxString psOutput = ProcUtils::SafeExecuteCommand(psCommand);
        CL_DEBUG("PS output:\n%s\n", psOutput);
        wxArrayString lines = ::wxStringTokenize(psOutput, "\n", wxTOKEN_STRTOK);
        for(size_t u = 0; u < lines.GetCount(); ++u) {
            wxString l = lines.Item(u);
            l.Trim().Trim(false);
            if(l.Contains("sleep") && l.Contains("12345")) {
                // we got a match
                CL_DEBUG("Got a match!");
                wxString ppidString = l.BeforeFirst(' ');
                ppidString.ToCLong(&pid);
                break;
            }
        }
        break;
    }
    CL_DEBUG("PID is: %d\n", (int)pid);
    CL_DEBUG("TTY is: %s\n", tty);
}

void FileUtils::OpenSSHTerminal(const wxString& sshClient, const wxString& connectString, const wxString& password,
                                int port)
{
#ifdef __WXMSW__
    wxString command;
    wxFileName putty(sshClient);
    if(!putty.Exists()) {
        wxMessageBox(_("Can't launch PuTTY. Don't know where it is ...."), "CodeLite", wxOK | wxCENTER | wxICON_ERROR);
        return;
    }

    wxString puttyClient = putty.GetFullPath();
    if(puttyClient.Contains(" ")) { puttyClient.Prepend("\"").Append("\""); }

    if(password.IsEmpty()) {
        command << "cmd /C \"" << puttyClient << " -P " << port << " " << connectString << "\"";
    } else {
        command << "cmd /C \"" << puttyClient << " -P " << port << " " << connectString << " -pw " << password << "\"";
    }
    ::wxExecute(command, wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE);

#elif defined(__WXGTK__)
    // Linux, we can't pass the password in the command line
    wxString command;
    command << sshClient << " -p " << port << " " << connectString;
    GTKOpenTerminal(command, "");
#else
    // OSX
    wxString command;
    command << "osascript -e 'tell app \"Terminal\" to do script \"" << sshClient << " " << connectString << " -p "
            << port << "\"'";
    ::wxExecute(command);
#endif
}

static void SplitMask(const wxString& maskString, wxArrayString& includeMask, wxArrayString& excludeMask)
{
    wxString lcMask = maskString.Lower();
    wxArrayString masks = ::wxStringTokenize(lcMask, ";,", wxTOKEN_STRTOK);
    for(size_t i = 0; i < masks.size(); ++i) {
        wxString& mask = masks.Item(i);
        mask.Trim().Trim(false);
        if(mask[0] == '!') {
            mask.Remove(0, 1);
            excludeMask.Add(mask);
        } else {
            includeMask.Add(mask);
        }
    }
}

bool FileUtils::WildMatch(const wxString& mask, const wxFileName& filename)
{

    wxArrayString incMasks;
    wxArrayString excMasks;
    SplitMask(mask, incMasks, excMasks);

    if(incMasks.Index("*") != wxNOT_FOUND) {
        // If one of the masks is plain "*" - we match everything
        return true;
    }

    wxString lcFilename = filename.GetFullName().Lower();
    // Try to the "exclude" masking first
    for(size_t i = 0; i < excMasks.size(); ++i) {
        const wxString& pattern = excMasks.Item(i);
        if((!pattern.Contains("*") && lcFilename == pattern) ||
           (pattern.Contains("*") && ::wxMatchWild(pattern, lcFilename))) {
            // use exact match
            return false;
        }
    }

    for(size_t i = 0; i < incMasks.size(); ++i) {
        const wxString& pattern = incMasks.Item(i);
        if((!pattern.Contains("*") && lcFilename == pattern) ||
           (pattern.Contains("*") && ::wxMatchWild(pattern, lcFilename))) {
            // use exact match
            return true;
        }
    }
    return false;
}

bool FileUtils::WildMatch(const wxString& mask, const wxString& filename)
{
    return WildMatch(mask, wxFileName(filename));
}

wxString FileUtils::DecodeURI(const wxString& uri)
{
    static wxStringMap_t T = { { "%20", " " }, { "%21", "!" }, { "%23", "#" }, { "%24", "$" }, { "%26", "&" },
                               { "%27", "'" }, { "%28", "(" }, { "%29", ")" }, { "%2A", "*" }, { "%2B", "+" },
                               { "%2C", "," }, { "%3B", ";" }, { "%3D", "=" }, { "%3F", "?" }, { "%40", "@" },
                               { "%5B", "[" }, { "%5D", "]" } };
    wxString decodedString;
    wxString escapeSeq;
    int state = 0;
    for(size_t i = 0; i < uri.size(); ++i) {
        wxChar ch = uri[i];
        switch(state) {
        case 0: // Normal
            switch(ch) {
            case '%':
                state = 1;
                escapeSeq << ch;
                break;
            default:
                decodedString << ch;
                break;
            }
            break;
        case 1: // Escaping mode
            escapeSeq << ch;
            if(escapeSeq.size() == 3) {
                // Try to decode it
                wxStringMap_t::iterator iter = T.find(escapeSeq);
                if(iter != T.end()) {
                    decodedString << iter->second;
                } else {
                    decodedString << escapeSeq;
                }
                state = 0;
                escapeSeq.Clear();
            }
            break;
        }
    }
    return decodedString;
}

wxString FileUtils::EncodeURI(const wxString& uri)
{
    static std::unordered_map<int, wxString> sEncodeMap = {
        { (int)'!', "%21" }, { (int)'#', "%23" }, { (int)'$', "%24" }, { (int)'&', "%26" }, { (int)'\'', "%27" },
        { (int)'(', "%28" }, { (int)')', "%29" }, { (int)'*', "%2A" }, { (int)'+', "%2B" }, { (int)',', "%2C" },
        { (int)';', "%3B" }, { (int)'=', "%3D" }, { (int)'?', "%3F" }, { (int)'@', "%40" }, { (int)'[', "%5B" },
        { (int)']', "%5D" }, { (int)' ', "%20" }
    };

    wxString encoded;
    for(size_t i = 0; i < uri.length(); ++i) {
        wxChar ch = uri[i];
        std::unordered_map<int, wxString>::iterator iter = sEncodeMap.find((int)ch);
        if(iter != sEncodeMap.end()) {
            encoded << iter->second;
        } else {
            encoded << ch;
        }
    }
    return encoded;
}

bool FileUtils::FuzzyMatch(const wxString& needle, const wxString& haystack)
{
    wxString word;
    size_t offset = 0;
    wxString lcHaystack = haystack.Lower();
    while(NextWord(needle, offset, word, true)) {
        if(!lcHaystack.Contains(word)) { return false; }
    }
    return true;
}

bool FileUtils::IsHidden(const wxFileName& filename)
{
#ifdef __WXMSW__
    DWORD dwAttrs = GetFileAttributes(filename.GetFullPath().c_str());
    if(dwAttrs == INVALID_FILE_ATTRIBUTES) return false;
    return (dwAttrs & FILE_ATTRIBUTE_HIDDEN) || (filename.GetFullName().StartsWith("."));
#else
    // is it enough to test for file name?
    return filename.GetFullName().StartsWith(".");
#endif
}

bool FileUtils::IsHidden(const wxString& filename) { return IsHidden(filename); }

bool FileUtils::WildMatch(const wxArrayString& masks, const wxString& filename)
{
    if(masks.Index("*") != wxNOT_FOUND) {
        // If one of the masks is plain "*" - we match everything
        return true;
    }

    wxString lcFilename = filename.Lower();
    for(size_t i = 0; i < masks.size(); ++i) {
        const wxString& pattern = masks.Item(i);
        if((!pattern.Contains("*") && lcFilename == pattern) ||
           (pattern.Contains("*") && ::wxMatchWild(pattern, lcFilename))) {
            // use exact match
            return true;
        }
    }
    return false;
}

bool FileUtils::SetFilePermissions(const wxFileName& filename, mode_t perm)
{
    wxString strFileName = filename.GetFullPath();
    return (::chmod(strFileName.mb_str(wxConvUTF8).data(), perm & 07777) != 0);
}

bool FileUtils::GetFilePermissions(const wxFileName& filename, mode_t& perm)
{
    struct stat b;
    wxString strFileName = filename.GetFullPath();
    if(::stat(strFileName.mb_str(wxConvUTF8).data(), &b) == 0) {
        perm = b.st_mode;
        return true;
    }
    return false;
}

time_t FileUtils::GetFileModificationTime(const wxFileName& filename)
{
    wxString file = filename.GetFullPath();
    struct stat buff;
    const wxCharBuffer cname = file.mb_str(wxConvUTF8);
    if(stat(cname.data(), &buff) < 0) { return 0; }
    return buff.st_mtime;
}

size_t FileUtils::GetFileSize(const wxFileName& filename)
{
    wxFFile fp(filename.GetFullPath(), "rb");
    if(fp.IsOpened()) { return fp.Length(); }
    return 0;
}

wxString FileUtils::EscapeString(const wxString& str)
{
    wxString modstr = str;
    modstr.Replace(" ", "\\ ");
    modstr.Replace("\"", "\\\"");
    return modstr;
}

wxString FileUtils::GetOSXTerminalCommand(const wxString& command, const wxString& workingDirectory)
{
    wxFileName script(clStandardPaths::Get().GetBinFolder(), "osx-terminal.sh");

    wxString cmd;
    cmd << EscapeString(script.GetFullPath()) << " \"";
    if(!workingDirectory.IsEmpty()) { cmd << "cd " << EscapeString(workingDirectory) << " && "; }
    cmd << EscapeString(command) << "\"";
    clDEBUG() << "GetOSXTerminalCommand returned:" << cmd << clEndl;
    return cmd;
}

wxString FileUtils::NormaliseName(const wxString& name)
{
    static bool initialised = false;
    static int invalidChars[256];
    if(!initialised) {
        memset(invalidChars, 0, sizeof(invalidChars));
        std::vector<int> V = { '@', '-', '^', '%', '&', '$', '#', '@', '!', '(',
                               ')', '{', '}', '[', ']', '+', '=', ';', ',', '.' };
        for(size_t i = 0; i < V.size(); ++i) {
            invalidChars[V[i]] = 1;
        }
        initialised = true;
    }

    wxString normalisedName;
    for(size_t i = 0; i < name.size(); ++i) {
        if(invalidChars[name[i]]) {
            // an invalid char was found
            normalisedName << "_";
        } else {
            normalisedName << name[i];
        }
    }
    return normalisedName;
}

bool FileUtils::NextWord(const wxString& str, size_t& offset, wxString& word, bool makeLower)
{
    if(offset == str.size()) { return false; }
    size_t start = wxString::npos;
    word.Clear();
    for(; offset < str.size(); ++offset) {
        wxChar ch = str[offset];
        bool isWhitespace = ((ch == ' ') || (ch == '\t'));
        if(isWhitespace && (start != wxString::npos)) {
            // we found a trailing whitespace
            break;
        } else if(isWhitespace && (start == wxString::npos)) {
            // skip leading whitespace
            continue;
        } else if(start == wxString::npos) {
            start = offset;
        }
        if(makeLower) { ch = wxTolower(ch); }
        word << ch;
    }

    if((start != wxString::npos) && (offset > start)) { return true; }
    return false;
}

size_t FileUtils::SplitWords(const wxString& str, wxStringSet_t& outputSet, bool makeLower)
{
    size_t offset = 0;
    wxString word;
    outputSet.clear();
    while(NextWord(str, offset, word, makeLower)) {
        outputSet.insert(word);
    }
    return outputSet.size();
}

bool FileUtils::RemoveFile(const wxString& filename, const wxString& context)
{
    clDEBUG() << "Deleting file:" << filename << "(" << context << ")";
    wxLogNull NOLOG;
    return ::wxRemoveFile(filename);
}
