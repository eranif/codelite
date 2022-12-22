#include "MSWDarkMode.hpp"

#include "clSystemSettings.h"
#include "drawingutils.h"

#ifdef __WXMSW__
#include <UxTheme.h>
#include <unordered_set>
#include <wx/headerctrl.h>

#define DWMWA_USE_IMMERSIVE_DARK_MODE 20

MSWDarkMode::MSWDarkMode() { Initialise(); }

MSWDarkMode::~MSWDarkMode() {}

void MSWDarkMode::Initialise()
{
    static bool initialized = false;
    if(initialized) {
        return;
    }
    initialized = true;

    // load the methods
    if(m_dllDWM.Load(wxS("dwmapi.dll"), wxDL_VERBATIM | wxDL_QUIET)) {
        wxDL_INIT_FUNC(m_pfn, DwmSetWindowAttribute, m_dllDWM);
    }

    if(m_dllUxTheme.Load(wxS("uxtheme.dll"), wxDL_VERBATIM | wxDL_QUIET)) {
        m_pfnAllowDarkModeForApp =
            (AllowDarkModeForApp_t)GetProcAddress(m_dllUxTheme.GetLibHandle(), MAKEINTRESOURCEA(135));
        m_pfnAllowDarkModeForWindow =
            (AllowDarkModeForWindow_t)GetProcAddress(m_dllUxTheme.GetLibHandle(), MAKEINTRESOURCEA(133));
        m_pfnFlushMenuThemes = (FlushMenuThemes_t)GetProcAddress(m_dllUxTheme.GetLibHandle(), MAKEINTRESOURCEA(136));
    }
}

MSWDarkMode& MSWDarkMode::Get()
{
    static MSWDarkMode msw_darkMode;
    return msw_darkMode;
}

void MSWDarkMode::SetDarkMode(wxWindow* win)
{
    BOOL useDarkMode = DrawingUtils::IsDark(clSystemSettings::GetDefaultPanelColour()) ? TRUE : FALSE;
    auto handle = win->GetHWND();

    if(m_pfnDwmSetWindowAttribute) {
        HRESULT hr =
            m_pfnDwmSetWindowAttribute(handle, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
        wxUnusedVar(hr);
    }

    if(m_pfnAllowDarkModeForApp) {
        m_pfnAllowDarkModeForApp(useDarkMode);
    }

    if(m_pfnAllowDarkModeForWindow) {
        m_pfnAllowDarkModeForWindow(handle, useDarkMode);
        SetWindowThemeRecurse(win, useDarkMode);
    }

    if(m_pfnFlushMenuThemes) {
        m_pfnFlushMenuThemes();
    }
}

void MSWDarkMode::SetWindowThemeRecurse(wxWindow* win, BOOL useDarkMode)
{
    if(dynamic_cast<wxHeaderCtrlBase*>(win)) {
        return;
    }
    SetWindowTheme(win->GetHWND(), useDarkMode ? L"DarkMode_Explorer" : L"Explorer", nullptr);
    auto children = win->GetChildren();
    for(auto child : children) {
        SetWindowThemeRecurse(child, useDarkMode);
    }
}
#endif
