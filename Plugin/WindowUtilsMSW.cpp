#ifdef __WXMSW__
#include "WindowUtils.hpp"

#include <wx/msw/wrapwin.h>
#include <wx/window.h>

namespace
{
// Defined here, so we do not depend on the version of the SDK headers (and we do not link with dwmapi.lib)
constexpr DWORD kDwmWindowCornerPreference = 33; // DWMWA_WINDOW_CORNER_PREFERENCE (Windows 11)
constexpr int kDwmCornerRound = 2;               // DWMWCP_ROUND (about 8 pixels at 100% scaling)
constexpr int kDwmCornerRoundSmall = 3;          // DWMWCP_ROUNDSMALL (about 4 pixels at 100% scaling)

struct DwmMargins {
    int left;
    int right;
    int top;
    int bottom;
};

using DwmSetWindowAttributeFn = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
using DwmExtendFrameIntoClientAreaFn = HRESULT(WINAPI*)(HWND, const DwmMargins*);

struct DwmApi {
    DwmSetWindowAttributeFn setWindowAttribute = nullptr;
    DwmExtendFrameIntoClientAreaFn extendFrameIntoClientArea = nullptr;
};

const DwmApi& GetDwmApi()
{
    static const DwmApi api = []() {
        DwmApi a;
        // The DLL stays loaded for the life of the process
        HMODULE dll = ::LoadLibraryW(L"dwmapi.dll");
        if (dll != nullptr) {
            a.setWindowAttribute = reinterpret_cast<DwmSetWindowAttributeFn>(
                reinterpret_cast<void*>(::GetProcAddress(dll, "DwmSetWindowAttribute")));
            a.extendFrameIntoClientArea = reinterpret_cast<DwmExtendFrameIntoClientAreaFn>(
                reinterpret_cast<void*>(::GetProcAddress(dll, "DwmExtendFrameIntoClientArea")));
        }
        return a;
    }();
    return api;
}
} // namespace

void StylePopupWindow(wxWindow* popup, double radius)
{
    if (popup == nullptr) {
        return;
    }

    HWND hwnd = reinterpret_cast<HWND>(popup->GetHandle());
    if (hwnd == nullptr) {
        return;
    }

    // Windows 11: let the system draw the rounded corners (anti-aliased). The system supports two fixed radii only, so
    // use the closest one
    const DwmApi& dwm = GetDwmApi();
    if (dwm.setWindowAttribute != nullptr) {
        const int preference = radius < 6.0 ? kDwmCornerRoundSmall : kDwmCornerRound;
        if (SUCCEEDED(dwm.setWindowAttribute(hwnd, kDwmWindowCornerPreference, &preference, sizeof(preference)))) {
            // A frame of 1 pixel makes the system draw a drop shadow around a borderless popup
            if (dwm.extendFrameIntoClientArea != nullptr) {
                const DwmMargins margins{1, 1, 1, 1};
                dwm.extendFrameIntoClientArea(hwnd, &margins);
            }
            return;
        }
    }

    // Older versions of Windows do not know the corner preference (the call above fails): clip the window to a rounded
    // rectangle. The edges are not anti-aliased and there is no shadow. The region is not updated when the popup is
    // resized, so call this function again after changing the size
    const wxSize size = popup->GetSize();
    const int ellipse = popup->FromDIP(static_cast<int>(radius * 2.0));
    HRGN region = ::CreateRoundRectRgn(0, 0, size.x + 1, size.y + 1, ellipse, ellipse);
    if (region != nullptr && ::SetWindowRgn(hwnd, region, TRUE) == 0) {
        // On success the system owns the region. On failure, we do
        ::DeleteObject(region);
    }
}
#endif
