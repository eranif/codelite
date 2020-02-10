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
#include "StringUtils.h"
#include "asyncprocess.h"
#include "clConsoleBase.h"
#include "cl_config.h"
#include "cl_standard_paths.h"
#include "dirsaver.h"
#include "file_logger.h"
#include "fileutils.h"
#include "macros.h"
#include "procutils.h"
#include "wx/string.h"
#include "wxStringHash.h"
#include <algorithm>
#include <fstream>
#include <map>
#include <wx/ffile.h>
#include <wx/file.h>
#include <wx/log.h>
#if wxUSE_GUI
#include <wx/msgdlg.h>
#endif
#include <string.h> // strerror
#include <wx/strconv.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>
#ifdef __WXGTK__
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <wx/filename.h>

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

void FileUtils::OpenTerminal(const wxString& path, const wxString& user_command, bool pause_when_exit)
{
    clConsoleBase::Ptr_t console = clConsoleBase::GetTerminal();
    console->SetCommand(user_command, "");
    console->SetWorkingDirectory(path);
    console->SetWaitWhenDone(pause_when_exit);
    console->Start();
}

bool FileUtils::WriteFileContent(const wxFileName& fn, const wxString& content, const wxMBConv& conv)
{
    wxFile file(fn.GetFullPath(), wxFile::write);
    if(file.IsOpened()) {
        return file.Write(content, conv);
    } else {
        return false;
    }
}

bool FileUtils::ReadFileContent(const wxFileName& fn, wxString& data, const wxMBConv& conv)
{
    wxString filename = fn.GetFullPath();
    data.clear();
    const char* cfile = filename.mb_str(wxConvUTF8).data();
    FILE* fp = fopen(cfile, "rb");
    if(!fp) {
        // Nothing to be done
        return false;
    }

    // Get the file size
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Allocate buffer for the read
    char* buffer = (char*)malloc(fsize + 1);
    long bytes_read = fread(buffer, 1, fsize, fp);
    if(bytes_read != fsize) {
        // failed to read
        clERROR() << "Failed to read file content:" << fn << "." << strerror(errno);
        fclose(fp);
        free(buffer);
        return false;
    }
    buffer[fsize] = 0;

    // Close the handle
    fclose(fp);

    // Convert it into wxString
    data = wxString(buffer, conv, fsize);
    if(data.IsEmpty() && fsize != 0) {
        // Conversion failed
        data = wxString::From8BitData(buffer, fsize);
    }

    // Release the C-buffer allocated earlier
    free(buffer);
    return true;
}

void FileUtils::OpenFileExplorerAndSelect(const wxFileName& filename)
{
#ifdef __WXMSW__
    wxString strPath = filename.GetFullPath();
    if(strPath.Contains(" ")) {
        strPath.Prepend("\"").Append("\"");
    }
    wxString cmd;
    cmd << "explorer /select," << strPath;
    ::wxExecute(cmd);
#else
    OpenFileExplorer(filename.GetPath());
#endif
}

void FileUtils::OSXOpenDebuggerTerminalAndGetTTY(const wxString& path, const wxString& appname, wxString& tty,
                                                 long& pid)
{
    tty.Clear();
    wxString command;
    wxString tmpfile;
    tmpfile << "/tmp/terminal.tty." << ::wxGetProcessId();
    wxFileName helperScript("/tmp", "codelite-lldb-helper.sh");
    wxString fileContent;
    fileContent << "#!/bin/bash\n";
    fileContent << "tty > " << tmpfile << "\n";
    fileContent << "sleep 12345";
    FileUtils::WriteFileContent(helperScript, fileContent);
    int rc = system("chmod +x /tmp/codelite-lldb-helper.sh");
    wxUnusedVar(rc);

    command << "open -a " << appname << " /tmp/codelite-lldb-helper.sh";
    clDEBUG() << "Executing: " << command;
    long res = ::wxExecute(command);
    if(res == 0) {
        clWARNING() << "Failed to execute command:" << command;
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
        clDEBUG() << "ps command output:\n" << psOutput;
        wxArrayString lines = ::wxStringTokenize(psOutput, "\n", wxTOKEN_STRTOK);
        for(size_t u = 0; u < lines.GetCount(); ++u) {
            wxString l = lines.Item(u);
            l.Trim().Trim(false);
            if(l.Contains("sleep") && l.Contains("12345")) {
                // we got a match
                clDEBUG() << "Got a match!";
                wxString ppidString = l.BeforeFirst(' ');
                ppidString.ToCLong(&pid);
                break;
            }
        }
        break;
    }
    clDEBUG() << "PID is:" << pid;
    clDEBUG() << "TTY is:" << tty;
}

void FileUtils::OpenSSHTerminal(const wxString& sshClient, const wxString& connectString, const wxString& password,
                                int port)
{
    clConsoleBase::Ptr_t console = clConsoleBase::GetTerminal();
    wxString args;
#ifdef __WXMSW__
    args << "-P " << port << " " << connectString;
    if(!password.IsEmpty()) {
        args << " -pw " << password;
    }
    console->SetExecExtraFlags(wxEXEC_HIDE_CONSOLE);
#else
    args << "-p " << port << " " << connectString;
#endif
    console->SetCommand(sshClient, args);
    console->Start();
}

static void SplitMask(const wxString& maskString, wxArrayString& includeMask, wxArrayString& excludeMask)
{
    wxString lcMask = maskString.Lower();
    wxArrayString masks = ::wxStringTokenize(lcMask, ";,", wxTOKEN_STRTOK);
    for(size_t i = 0; i < masks.size(); ++i) {
        wxString& mask = masks.Item(i);
        mask.Trim().Trim(false);
        // exclude mask starts with "!" or "-"
        if((mask[0] == '!') || (mask[0] == '-')) {
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
        if(!lcHaystack.Contains(word)) {
            return false;
        }
    }
    return true;
}

bool FileUtils::IsHidden(const wxString& filename)
{
#ifdef __WXMSW__
    DWORD dwAttrs = GetFileAttributes(filename.c_str());
    if(dwAttrs == INVALID_FILE_ATTRIBUTES)
        return false;
    return (dwAttrs & FILE_ATTRIBUTE_HIDDEN) || (wxFileName(filename).GetFullName().StartsWith("."));
#else
    // is it enough to test for file name?
    wxFileName fn(filename);
    return fn.GetFullName().StartsWith(".");
#endif
}

bool FileUtils::IsHidden(const wxFileName& filename) { return IsHidden(filename.GetFullPath()); }

bool FileUtils::WildMatch(const wxArrayString& masks, const wxString& filename)
{
    if(masks.IsEmpty()) {
        return false;
    }

    if(masks.Index("*") != wxNOT_FOUND) {
        // If one of the masks is plain "*" - we match everything
        return true;
    }

    for(size_t i = 0; i < masks.size(); ++i) {
        const wxString& pattern = masks.Item(i);
        if((!pattern.Contains("*") && filename == pattern) ||
           (pattern.Contains("*") && ::wxMatchWild(pattern, filename))) {
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
    if(stat(cname.data(), &buff) < 0) {
        return 0;
    }
    return buff.st_mtime;
}

size_t FileUtils::GetFileSize(const wxFileName& filename)
{
    struct stat b;
    wxString file_name = filename.GetFullPath();
    const char* cfile = file_name.mb_str(wxConvUTF8).data();
    if(::stat(cfile, &b) == 0) {
        return b.st_size;
    } else {
        clERROR() << "Failed to open file:" << file_name << "." << strerror(errno);
        return 0;
    }
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
    if(!workingDirectory.IsEmpty()) {
        cmd << "cd " << EscapeString(workingDirectory) << " && ";
    }
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
        std::vector<int> V = { '@', '-', '^', '%', '&', '$', '#', '@', '!', '(', ')',
                               '{', '}', '[', ']', '+', '=', ';', ',', '.', ' ' };
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
    if(offset == str.size()) {
        return false;
    }
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
        if(makeLower) {
            ch = wxTolower(ch);
        }
        word << ch;
    }

    if((start != wxString::npos) && (offset > start)) {
        return true;
    }
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
    clDEBUG1() << "Deleting file:" << filename << "(" << context << ")";
    wxLogNull NOLOG;
    return ::wxRemoveFile(filename);
}

unsigned int FileUtils::UTF8Length(const wchar_t* uptr, unsigned int tlen)
{
#define SURROGATE_LEAD_FIRST 0xD800
#define SURROGATE_TRAIL_FIRST 0xDC00
#define SURROGATE_TRAIL_LAST 0xDFFF
    unsigned int len = 0;
    for(unsigned int i = 0; i < tlen && uptr[i];) {
        unsigned int uch = uptr[i];
        if(uch < 0x80) {
            len++;
        } else if(uch < 0x800) {
            len += 2;
        } else if((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_TRAIL_LAST)) {
            len += 4;
            i++;
        } else {
            len += 3;
        }
        i++;
    }
#undef SURROGATE_LEAD_FIRST
#undef SURROGATE_TRAIL_FIRST
#undef SURROGATE_TRAIL_LAST
    return len;
}

// This is readlink on steroids: it also makes-absolute, and dereferences any symlinked dirs in the path
wxString FileUtils::RealPath(const wxString& filepath)
{
#if defined(__WXGTK__) || defined(__WXOSX__)
    if(!filepath.empty()) {
#if defined(__FreeBSD__)
        wxStructStat stbuff;
        if((::wxLstat(filepath, &stbuff) != 0) || !S_ISLNK(stbuff.st_mode)) {
            return filepath;
        }
#endif
        char* buf = realpath(filepath.mb_str(wxConvUTF8), NULL);
        if(buf != NULL) {
            wxString result(buf, wxConvUTF8);
            free(buf);
            return result;
        }
    }
#endif // defined(__WXGTK__) || defined(__WXOSX__)

    return filepath;
}

void FileUtils::OpenBuiltInTerminal(const wxString& wd, const wxString& user_command, bool pause_when_exit)
{
    wxString title(user_command);

    wxFileName fnCodeliteTerminal(clStandardPaths::Get().GetExecutablePath());
    fnCodeliteTerminal.SetFullName("codelite-terminal");

    wxString newCommand;
    newCommand << fnCodeliteTerminal.GetFullPath() << " --exit ";
    if(pause_when_exit) {
        newCommand << " --wait ";
    }
    if(wxDirExists(wd)) {
        wxString workingDirectory = wd;
        workingDirectory.Trim().Trim(false);
        if(workingDirectory.Contains(" ") && !workingDirectory.StartsWith("\"")) {
            workingDirectory.Prepend("\"").Append("\"");
        }
        newCommand << " --working-directory " << wd;
    }
    newCommand << " --cmd " << title;
    ::wxExecute(newCommand, wxEXEC_ASYNC);
}

std::string FileUtils::ToStdString(const wxString& str) { return StringUtils::ToStdString(str); }

bool FileUtils::ReadBufferFromFile(const wxFileName& fn, wxString& data, size_t bufferSize)
{
    std::wifstream fin(fn.GetFullPath().c_str(), std::ios::binary);
    if(fin.bad()) {
        clERROR() << "Failed to open file:" << fn;
        return false;
    }

    std::vector<wchar_t> buffer(bufferSize, 0);
    if(!fin.eof()) {
        fin.read(buffer.data(), buffer.size());
    }
    data.reserve(buffer.size());
    data << std::wstring(buffer.begin(), buffer.begin() + buffer.size());
    return true;
}

bool FileUtils::IsSymlink(const wxString& filename)
{
#ifdef __WXMSW__
    DWORD dwAttrs = GetFileAttributesW(filename.c_str());
    if(dwAttrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return dwAttrs & FILE_ATTRIBUTE_REPARSE_POINT;
#else
    wxStructStat buff;
    if(wxLstat(filename, &buff) != 0) {
        return false;
    }
    return S_ISLNK(buff.st_mode);
#endif
}

bool FileUtils::IsDirectory(const wxString& filename)
{
#ifdef __WXMSW__
    DWORD dwAttrs = GetFileAttributesW(filename.c_str());
    if(dwAttrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return dwAttrs & FILE_ATTRIBUTE_DIRECTORY;
#else
    wxStructStat buff;
    if(wxLstat(filename, &buff) != 0) {
        return false;
    }
    return S_ISDIR(buff.st_mode);
#endif
}

clEnvList_t FileUtils::CreateEnvironment(const wxString& envstr)
{
    wxArrayString arr = ::wxStringTokenize(envstr, "\r\n", wxTOKEN_STRTOK);
    clEnvList_t L;
    for(wxString& line : arr) {
        line.Trim().Trim(false);
        if(line.Index('=') == wxString::npos) {
            continue;
        }
        if(line.StartsWith("#")) {
            continue;
        }
        wxString name = line.BeforeFirst('=');
        wxString value = line.AfterFirst('=');
        L.push_back({ name, value });
    }
    return L;
}

bool FileUtils::FindExe(const wxString& name, wxFileName& exepath, const wxArrayString& hint)
{
    wxString path;
    if(!::wxGetEnv("PATH", &path)) {
        clWARNING() << "Could not read environment variable PATH" << clEndl;
        return false;
    }

    wxArrayString mergedPaths = hint;
    wxArrayString paths = ::wxStringTokenize(path, clPATH_SEPARATOR, wxTOKEN_STRTOK);
    for(const auto& p : paths) {
        mergedPaths.Add(p);
    }
    mergedPaths.swap(paths);

    for(size_t i = 0; i < paths.size(); ++i) {
        const wxString& curpath = paths.Item(i);
        wxFileName fnPath(curpath, name);
        if(fnPath.FileExists()) {
            exepath = fnPath;
            return true;
        }
#ifdef __WXMSW__
        wxFileName fullname("", name);
        // on Windows, an executable can have a list of known extensions defined in the
        // environment variable PATHEXT. Use this environment variable only if the user did not
        // provide a fullname (name + ext)
        wxArrayString exts;
        if(fullname.GetExt().IsEmpty()) {
            wxString pathext;
            ::wxGetEnv("PATHEXT", &pathext);
            exts = ::wxStringTokenize(pathext, ";", wxTOKEN_STRTOK);
        } else {
            exts.Add(fullname.GetExt());
        }

        for(size_t j = 0; j < exts.size(); ++j) {
            wxString ext = exts.Item(j).AfterFirst('.'); // remove the . from the extension
            wxFileName fnFileWithExt(curpath, name);
            fnFileWithExt.SetExt(ext);
            if(fnFileWithExt.FileExists()) {
                exepath = fnFileWithExt;
                return true;
            }
        }
#endif
    }
    return false;
}
