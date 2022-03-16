#include "clFileName.hpp"

#include "file_logger.h"
#include "fileutils.h"
#include "procutils.h"

namespace
{
/// helper method:
/// run `uname -s` command and cache the output
const wxString& __uname()
{
    static wxString uname_output;
    static bool firstTime = true;

    if(firstTime) {
        firstTime = false;
        wxFileName uname;
        if(FileUtils::FindExe("uname", uname)) {
            firstTime = false;
            clDEBUG() << "Running `uname -s`..." << endl;
            wxString cmd;
            cmd << uname.GetFullPath();
            if(cmd.Contains(" ")) {
                cmd.Prepend("\"").Append("\"");
            }
            cmd << " -s";
            uname_output = ProcUtils::SafeExecuteCommand(cmd);
            clDEBUG() << uname_output << endl;
        }
    }
    return uname_output;
}

bool is_cygwin_env()
{
#ifdef __WXMSW__
    wxString uname_output = __uname();
    return uname_output.Lower().Contains("CYGWIN_NT");
#else
    return false;
#endif
}
} // namespace

clFileName::clFileName() {}

clFileName::clFileName(const wxString& fullpath)
    : wxFileName(FromCygwin(fullpath))
{
}

clFileName::clFileName(const wxString& dir, const wxString& name)
    : wxFileName(FromCygwin(dir + wxFileName::GetPathSeparator() + name))
{
}

clFileName::~clFileName() {}

wxString clFileName::FromCygwin(const wxString& fullpath)
{
    if(!is_cygwin_env()) {
        return fullpath;
    }
    wxString tmppath;
    if(fullpath.StartsWith("/cygdrive/")) {
        // "/cygdrive/c/Users/eran/devl/codelite/CodeLite/clFileName.cpp"
        tmppath = fullpath;
        tmppath.Replace("/cygdrive/", "");
        if(tmppath.IsEmpty()) {
            return fullpath;
        }

        // c/Users/eran/devl/codelite/CodeLite/clFileName.cpp
        wxString drive = tmppath[0];
        tmppath.erase(0, 1);
        // /Users/eran/devl/codelite/CodeLite/clFileName.cpp
        tmppath.Prepend(drive.Upper() + ":");
        // C:/Users/eran/devl/codelite/CodeLite/clFileName.cpp
        return wxFileName(tmppath).GetFullPath();
    }
    return fullpath;
}

wxString clFileName::ToCygwin(const wxString& fullpath) { return ToCygwin(wxFileName(fullpath)); }
wxString clFileName::ToCygwin(const wxFileName& fullpath)
{
    if(!is_cygwin_env()) {
        return fullpath.GetFullPath();
    }
    wxFileName fn(fullpath);
    if(fn.GetVolume().empty()) {
        // non windows path
        return fullpath.GetFullPath();
    }
    wxString drive = fn.GetVolume();
    wxString cygwin_path = fn.GetFullPath();
    if(cygwin_path.size() < 2) {
        return fullpath.GetFullPath();
    }

    cygwin_path.erase(0, 2); // C:
    cygwin_path.Replace("\\", "/");
    cygwin_path.Prepend("/cygdrive/" + drive.Lower());
    return cygwin_path;
}
