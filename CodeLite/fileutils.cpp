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

#include "StringUtils.h"
#include "asyncprocess.h"
#include "clConsoleBase.h"
#include "cl_config.h"
#include "cl_standard_paths.h"
#include "dirsaver.h"
#include "file_logger.h"
#include "macros.h"
#include "procutils.h"
#include "wxStringHash.h"

#include <algorithm>
#include <fstream>
#include <map>
#include <wx/ffile.h>
#include <wx/file.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/string.h>
#include <wx/uri.h>
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
#include <cstring>
#include <functional>
#include <memory>
#include <wx/filename.h>

// internal helper method
namespace
{
bool write_file_content(const wxFileName& fn, const wxString& content, const wxMBConv& conv)
{
    wxFile file(fn.GetFullPath(), wxFile::write);
    if(file.IsOpened()) {
        return file.Write(content, conv);
    } else {
        return false;
    }
}

#ifndef __WXMSW__
const char ELF_STR[] = { 0x7f, 'E', 'L', 'F' };
#endif
} // namespace

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
    wxFileName tmpFile = CreateTempFileName(fn.GetPath(), "cltmp", fn.GetExt());
    // make sure we erase the temp file
    FileUtils::Deleter d(tmpFile);
    if(!write_file_content(tmpFile, content, conv)) {
        return false;
    }

    // rename tmp -> real file
    return ::wxRenameFile(tmpFile.GetFullPath(), fn.GetFullPath(), true);
}

bool FileUtils::AppendFileContent(const wxFileName& fn, const wxString& content, const wxMBConv& conv)
{
    wxFile fp(fn.GetFullPath(), wxFile::write_append);
    if(!fp.IsOpened()) {
        // failed to open the file
        return false;
    }
    if(!fp.Write(content, conv)) {
        clWARNING() << "Failed to append content to file:" << fn << endl;
    }
    fp.Close();
    return true;
}

bool FileUtils::ReadFileContent(const wxFileName& fn, wxString& data, const wxMBConv& conv)
{
    std::string rawdata;
    if(!ReadFileContentRaw(fn, rawdata)) {
        return false;
    }

    // convert
    data = wxString(rawdata.c_str(), conv, rawdata.length());
    if(data.IsEmpty() && !rawdata.empty()) {
        // Conversion failed
        data = wxString::From8BitData(rawdata.c_str(), rawdata.length());
    }
    return true;
}

bool FileUtils::ReadFileContentRaw(const wxFileName& fn, std::string& data)
{
    // fopen() may return non NULL for directories too, so we have to test it ourselves
    if(!fn.FileExists()) {
        return false;
    }

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
    data.reserve(fsize + 1);

    // use unique_ptr to auto release the buffer
    std::unique_ptr<char, std::function<void(char*)>> buffer(new char[fsize + 1], [](char* d) { delete[] d; });

    long bytes_read = fread(buffer.get(), 1, fsize, fp);
    if(bytes_read != fsize) {
        // failed to read
        clERROR() << "Failed to read file content:" << fn << "." << strerror(errno);
        fclose(fp);
        return false;
    }
    buffer.get()[fsize] = 0;
    fclose(fp);

    // Close the handle
    data = buffer.get();
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
    if(dwAttrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
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
    LOG_IF_TRACE { clDEBUG1() << "Deleting file:" << filename << "(" << context << ")"; }
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
#if defined(__FreeBSD__) || defined(__WXOSX__)
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
    std::wifstream fin(fn.GetFullPath().ToStdString(), std::ios::binary);
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
namespace
{
bool DoFindExe(const wxString& name, wxFileName& exepath, const wxArrayString& hint)
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
} // namespace

bool FileUtils::FindExe(const wxString& name, wxFileName& exepath, const wxArrayString& hint,
                        const wxArrayString& suffix_list)
{
    wxArrayString possible_suffix;
    possible_suffix.Add(wxEmptyString);
    if(!suffix_list.empty()) {
        possible_suffix.reserve(possible_suffix.size() + suffix_list.size());
        possible_suffix.insert(possible_suffix.end(), suffix_list.begin(), suffix_list.end());
    }
    for(const wxString& suffix : possible_suffix) {
        if(DoFindExe(name + suffix, exepath, hint)) {
            return true;
        }
    }
    return false;
}

wxFileName FileUtils::CreateTempFileName(const wxString& folder, const wxString& prefix, const wxString& ext)
{
    static bool srandInit = false;
    if(!srandInit) {
        srand(time(nullptr));
        srandInit = true;
    }

    static const char alphanum[] = "0123456789"
                                   "abcdefghijklmnopqrstuvwxyz"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t N = sizeof(alphanum) - 1;
    wxString full_name = prefix;
    full_name << "-";
    for(size_t i = 0; i < 8; ++i) {
        size_t index = rand() / (RAND_MAX / N + 1);
        full_name += alphanum[index];
    }
    full_name += "." + ext;
    return wxFileName(folder, full_name);
}

size_t FileUtils::FindSimilar(const wxFileName& filename, const std::vector<wxString>& extensions,
                              std::vector<wxFileName>& vout)
{
    wxFileName fn(filename);
    vout.reserve(extensions.size());
    for(const wxString& ext : extensions) {
        fn.SetExt(ext);
        if(fn.FileExists()) {
            vout.emplace_back(fn);
        }
    }
    return vout.size();
}

bool FileUtils::ParseURI(const wxString& uri, wxString& path, wxString& scheme, wxString& user, wxString& host,
                         wxString& port)
{
    if(uri.StartsWith("file://")) {
        path = uri.Mid(7);
        scheme = "file://";
        return true;
    } else if(uri.StartsWith("ssh://")) {
        // expected syntax:
        // ssh://user@host:port:/path
        scheme = "ssh://";
        wxString remainder = uri.Mid(6);
        user = remainder.BeforeFirst('@');
        remainder = remainder.AfterFirst('@');
        host = remainder.BeforeFirst(':');
        remainder = remainder.AfterFirst(':');

        // at this point we got:
        // port:/path
        // OR -
        // /path
        if(remainder.empty()) {
            return true;
        }

        if(remainder[0] == '/') {
            path = remainder;
        } else {
            port = remainder.BeforeFirst(':');
            path = remainder.AfterFirst(':');
        }
        return true;
    } else {
        return false;
    }
}

wxString FileUtils::FilePathToURI(const wxString& filepath)
{
    if(filepath.StartsWith("file://")) {
        return filepath;
    } else {
        wxString uri;
        uri << "file://";
        if(!filepath.StartsWith("/")) {
            uri << "/";
        }
        wxString file_part = filepath;
        file_part.Replace("\\", "/");
        file_part = EncodeURI(file_part);
        uri << file_part;
        return uri;
    }
}

wxString FileUtils::FilePathFromURI(const wxString& uri)
{
    wxString rest;
    if(uri.StartsWith("file://", &rest)) {
#ifdef __WXMSW__
        // check if the file path starts with /C: (Windows drive)
        wxRegEx re_windows_drive("/[a-z]{1}:", wxRE_DEFAULT | wxRE_ICASE);
        if(re_windows_drive.IsValid() && re_windows_drive.Matches(rest)) {
            rest.Remove(0, 1); // remove the leading slash
            // since we know that his is a Windows style path
            // make sure we are using backslashes
            rest.Replace("/", "\\");
        }
#endif
        rest = DecodeURI(rest);
        return rest;
    } else {
        return uri;
    }
}

namespace
{
/* Number of bytes to read at once.  */
constexpr size_t BUFLEN = (1 << 16);

uint_fast32_t const crctab[256] = {
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005, 0x2608edb8,
    0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
    0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75, 0x6a1936c8, 0x6ed82b7f, 0x639b0da6,
    0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84,
    0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d, 0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
    0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a,
    0xec7dd02d, 0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 0x7897ab07,
    0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
    0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba, 0xaca5c697, 0xa864db20, 0xa527fdf9,
    0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
    0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b,
    0xf771768c, 0xfa325055, 0xfef34de2, 0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
    0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c,
    0x774bb0eb, 0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b, 0x0315d626,
    0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
    0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b, 0xd727bbb6, 0xd3e6a601, 0xdea580d8,
    0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
    0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a,
    0x8cf30bad, 0x81b02d74, 0x857130c3, 0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
    0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093,
    0x65c52d24, 0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 0xc5a92679,
    0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
    0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 0x89b8fd09, 0x8d79e0be, 0x803ac667,
    0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
    0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

bool cksum(const std::string& file, size_t* checksum)
{
    unsigned char buf[BUFLEN];
    uint_fast32_t crc = 0;
    size_t length = 0;
    size_t bytes_read;
    FILE* fp;

    fp = fopen(file.c_str(), "rb");
    if(fp == NULL) {
        return false;
    }

    while((bytes_read = fread(buf, 1, BUFLEN, fp)) > 0) {
        unsigned char* cp = buf;
        length += bytes_read;
        while(bytes_read--) {
            crc = (crc << 8) ^ crctab[((crc >> 24) ^ *cp++) & 0xFF];
        }
        if(feof(fp)) {
            break;
        }
    }

    if(ferror(fp)) {
        return false;
    }

    if(fclose(fp) == EOF) {
        return false;
    }

    for(; length; length >>= 8) {
        crc = (crc << 8) ^ crctab[((crc >> 24) ^ length) & 0xFF];
    }

    crc = ~crc & 0xFFFFFFFF;
    *checksum = crc;
    return true;
}
} // namespace

bool FileUtils::GetChecksum(const wxString& filepath, size_t* checksum)
{
    return cksum(ToStdString(filepath), checksum);
}

bool FileUtils::IsBinaryExecutable(const wxString& filename)
{
#ifdef __WXMSW__
    // check extension first
    wxString extension = filename.AfterLast('.');
    if(extension == "exe" || extension == "dll") {
        return true;
    }

    // check the file itself
    DWORD type;
    return GetBinaryType(filename.c_str(), &type) == TRUE;
#else
    // do ELF check by comparing the first 4 bytes of the file to the ELF magic string
    char buffer[4];
    FILE* fp = ::fopen(filename.mb_str(wxConvUTF8).data(), "rb");
    if(!fp) {
        return false;
    }
    int count = ::fread(buffer, sizeof(char), sizeof(buffer), fp);
    ::fclose(fp);
    return (count == 4 && ::memcmp(buffer, ELF_STR, 4) == 0);
#endif
}
