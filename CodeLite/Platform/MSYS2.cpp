#include "MSYS2.hpp"

#include "cl_standard_paths.h"
#include "file_logger.h"

#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

namespace
{
constexpr const char* kMSYS2RegistryNameV1 = "MSYS2 64bit";
constexpr const char* kMSYS2RegistryNameV2 = "MSYS2";
} // namespace

std::optional<wxString> MSYS2::FindInstallDir()
{
    if (m_checked_for_install_dir) {
        return m_install_dir;
    }

    m_checked_for_install_dir = true;

    wxString reg_install_path;
#ifdef __WXMSW__
    wxRegKey uninstall(wxRegKey::HKCU, R"(SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall)");
    wxString appname;
    long dummy;
    bool cont = uninstall.GetFirstKey(appname, dummy);
    while (cont) {
        wxString display_name;
        wxRegKey appkey(wxRegKey::HKCU, R"(SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\)" + appname);
        if (appkey.QueryValue("DisplayName", display_name) &&
            (display_name == kMSYS2RegistryNameV1 || display_name == kMSYS2RegistryNameV2)) {
            appkey.QueryValue("InstallLocation", reg_install_path);
            break;
        }
        cont = uninstall.GetNextKey(appname, dummy);
    }
#endif

    if (!reg_install_path.empty()) {
        m_install_dir = reg_install_path;
        return m_install_dir;
    } else {
        // try common paths
        std::vector<wxString> vpaths = {R"(C:\msys64)", R"(C:\msys2)", R"(C:\msys)"};
        for (const wxString& path : vpaths) {
            if (wxFileName::DirExists(path)) {
                m_install_dir = path;
                return m_install_dir;
            }
        }
    }
    return std::nullopt;
}

std::optional<wxString> MSYS2::FindHomeDir()
{
    const auto msyspath = FindInstallDir();
    if (!msyspath) {
        return std::nullopt;
    }

    if (m_checked_for_home_dir) {
        return m_home_dir;
    }

    m_checked_for_home_dir = true;

    wxFileName cargo_dir{*msyspath, wxEmptyString};
    cargo_dir.AppendDir("home");
    cargo_dir.AppendDir(::wxGetUserId());

    if (cargo_dir.DirExists()) {
        m_home_dir = cargo_dir.GetPath();
    }
    return m_home_dir;
}

std::optional<wxString> MSYS2::Which(const wxString& command)
{
    wxString path = GetPath(m_flags & SEARCH_PATH_ENV).value_or("");

    wxArrayString paths_to_try = ::wxStringTokenize(path, ";", wxTOKEN_STRTOK);
    static const wxString exts[] = {".exe", ".cmd"};
    // at the point, the order of search is:
    // MSYS2 -> Executable path -> PATH paths
    for (const auto& path : paths_to_try) {
        if (!wxFileName::DirExists(path)) {
            continue;
        }
        for (const wxString& ext : exts) {
            wxString exepath = path;
            exepath << "\\" << command << ext;
            if (wxFileName::FileExists(exepath)) {
                return exepath;
            }
        }
    }
    return std::nullopt;
}

std::optional<wxString> MSYS2::WhichWithVersion(const wxString& command, const std::vector<int>& versions)
{
    return PlatformCommon::WhichWithVersion(command, versions);
}

void MSYS2::SetChroot(const wxString& chroot)
{
    m_chroots.clear();
    m_chroots.Add(chroot);

    // exclude PATH from the search path
    m_flags &= ~SEARCH_PATH_ENV;
}

namespace
{
thread_local MSYS2 instance;
}

MSYS2* MSYS2::Get() { return &instance; }

MSYS2::MSYS2()
{
    // last entry -> most important (reverse order)
    m_chroots.Add(R"(\usr)");
    m_chroots.Add(R"(\mingw64)");
    m_chroots.Add(R"(\clang64)");
}

std::optional<wxString> MSYS2::GetPath(bool useSystemPath)
{
    const auto msyspath = FindInstallDir();

    wxArrayString paths_to_try;

    // next in order are is the PATH environment variable
    if (useSystemPath) {
        wxString pathenv;
        wxGetEnv("PATH", &pathenv);
        paths_to_try = ::wxStringTokenize(pathenv, ";", wxTOKEN_STRTOK);
    }

    // add the executable path (this is how windows work: we first look at the executable path)
    paths_to_try.Insert(wxFileName(clStandardPaths::Get().GetExecutablePath()).GetPath(), 0);

    // if we have msys2 installed, add the bin folder (we place them at start)
    if (msyspath) {
        for (const auto& root : m_chroots) {
            paths_to_try.Insert(*msyspath + root + R"(\bin)", 0);
        }

        // cargo (MSYS2 path)
        wxFileName cargo_msys_bin{*msyspath, wxEmptyString};
        cargo_msys_bin.AppendDir("home");
        cargo_msys_bin.AppendDir(::wxGetUserId());
        cargo_msys_bin.AppendDir(".cargo");
        cargo_msys_bin.AppendDir("bin");
        if (cargo_msys_bin.DirExists()) {
            paths_to_try.Add(cargo_msys_bin.GetPath());
        }
    }

    // cargo (Windows native path)
    // e.g. C:\Users\user\.cargo\bin
    static const wxString kWindowsUsersBaseDir = R"(C:\Users)";
    wxFileName cargo_native_bin{kWindowsUsersBaseDir, wxEmptyString};
    cargo_native_bin.AppendDir(::wxGetUserId());
    cargo_native_bin.AppendDir(".cargo");
    cargo_native_bin.AppendDir("bin");

    if (cargo_native_bin.DirExists()) {
        paths_to_try.Add(cargo_native_bin.GetPath());
    }

    // local (Windows native path)
    // e.g. C:\Users\user\.local\bin
    wxFileName local_native_bin{kWindowsUsersBaseDir, wxEmptyString};
    local_native_bin.AppendDir(::wxGetUserId());
    local_native_bin.AppendDir(".local");
    local_native_bin.AppendDir("bin");

    if (local_native_bin.DirExists()) {
        paths_to_try.Add(local_native_bin.GetPath());
    }
    return ::wxJoin(paths_to_try, ';');
}
