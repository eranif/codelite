#include "clFileName.hpp"

#include "file_logger.h"
#include "procutils.h"

namespace
{
bool is_cygwin_env()
{
#ifdef __WXMSW__
    static bool is_cygwin = false;
    static bool once = true;

    if(once) {
        once = false;
        wxString out = ProcUtils::SafeExecuteCommand("uname -s");
        if(out.empty()) {
            is_cygwin = false;
        } else {
            is_cygwin = out.StartsWith("CYGWIN_NT");
        }
        if(is_cygwin) {
            clSYSTEM() << "Cygwin environment detected" << endl;
        }
    }

    return is_cygwin;
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
