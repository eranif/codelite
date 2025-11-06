#ifndef MSWDARKMODE_HPP
#define MSWDARKMODE_HPP

#ifdef __WXMSW__
#include "codelite_exports.h"

#include <wx/dynlib.h>
#include <wx/module.h>
#include <wx/msw/wrapwin.h>
#include <wx/string.h>
#include <wx/window.h>

using AllowDarkModeForApp_t = BOOL(WINAPI*)(BOOL allow);                       // AllowDarkModeForApp
using AllowDarkModeForWindow_t = BOOL(WINAPI*)(HWND window, BOOL allow);       // AllowDarkModeForWindow
using FlushMenuThemes_t = void(WINAPI*)();                                     // FlushMenuThemes
using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD); // DwmSetWindowAttribute

class WXDLLIMPEXP_SDK MSWDarkMode
{
    wxDynamicLibrary m_dllDWM;
    wxDynamicLibrary m_dllUxTheme;

    // functions
    AllowDarkModeForApp_t m_pfnAllowDarkModeForApp = nullptr;
    AllowDarkModeForWindow_t m_pfnAllowDarkModeForWindow = nullptr;
    DwmSetWindowAttribute_t m_pfnDwmSetWindowAttribute = nullptr;
    FlushMenuThemes_t m_pfnFlushMenuThemes = nullptr;

private:
    MSWDarkMode();
    ~MSWDarkMode() = default;

    void Initialise();

    void SetWindowThemeRecurse(wxWindow* win, BOOL useDarkMode);

public:
    static MSWDarkMode& Get();

    /**
     * @brief set dark mode for a window based on the current theme
     */
    void SetDarkMode(wxWindow* win);
};
#endif
#endif // MSWDARKMODE_HPP
