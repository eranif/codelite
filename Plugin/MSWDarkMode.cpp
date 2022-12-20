#include "MSWDarkMode.hpp"

#include "clSystemSettings.h"
#include "drawingutils.h"

#ifdef __WXMSW__
#include <UxTheme.h>
#include <unordered_set>

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
    bool current_theme_is_dark = DrawingUtils::IsDark(clSystemSettings::GetDefaultPanelColour());
    BOOL useDarkMode = current_theme_is_dark ? TRUE : FALSE;
    auto handle = win->GetHWND();

    if(m_pfnDwmSetWindowAttribute) {
        HRESULT hr =
            m_pfnDwmSetWindowAttribute(handle, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
        wxUnusedVar(hr);
    }

    if(m_pfnAllowDarkModeForApp && m_pfnAllowDarkModeForWindow) {
        m_pfnAllowDarkModeForApp(useDarkMode);

        // bfs the windows
        std::vector<wxWindow*> Q;
        std::unordered_set<wxWindow*> V; // visited
        Q.push_back(win);
        while(!Q.empty()) {
            wxWindow* w = Q.front();
            Q.erase(Q.begin());

            if(!V.insert(w).second) {
                // already visited this window (how can this be true??)
                continue;
            }

            BOOL use_dark = current_theme_is_dark ? TRUE : FALSE;
            if(dynamic_cast<wxTextCtrl*>(w)) {
                use_dark = FALSE; // don't allow dark mode for text controls
            }

            m_pfnAllowDarkModeForWindow(w->GetHandle(), use_dark);
            SetWindowTheme(w->GetHandle(), use_dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);

            if(m_pfnFlushMenuThemes) {
                m_pfnFlushMenuThemes();
            }

            InvalidateRect(w->GetHandle(), nullptr, FALSE); // HACK
            const auto& children = w->GetChildren();
            for(auto c : children) {
                Q.push_back(c);
            }
        }
    }
}
#endif