#ifndef MSWDARKMODE_HPP
#define MSWDARKMODE_HPP

#ifdef __WXMSW__
#include "codelite_exports.h"

#include <wx/dynlib.h>
#include <wx/module.h>
#include <wx/string.h>

typedef BOOL(WINAPI* AllowDarkModeForApp_t)(BOOL allow);                       // AllowDarkModeForApp
typedef BOOL(WINAPI* AllowDarkModeForWindow_t)(HWND window, BOOL allow);       // AllowDarkModeForWindow
typedef void(WINAPI* FlushMenuThemes_t)();                                     // FlushMenuThemes
typedef HRESULT(WINAPI* DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD); // DwmSetWindowAttribute

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
    ~MSWDarkMode();

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
