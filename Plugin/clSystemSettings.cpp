#include "clSystemSettings.h"

#include "cl_config.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"

#include <mutex>
#include <optional>
#include <wx/app.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/settings.h>

wxColour clSystemSettings::btn_face;
wxColour clSystemSettings::panel_face;

#ifdef __WXMSW__
#define IS_GTK 0
#elif defined(__WXOSX__)
#define IS_GTK 0
#else
#define IS_GTK 1
#endif

namespace
{
/// keep the initial startup background colour
/// we use this to detect any theme changes done to the system
/// the checks are done in the OnAppAcitvated event
wxColour startupBackgroundColour;

#ifdef __WXGTK__
double GetLuminance(const wxColour& color)
{
    double r = color.Red();
    double g = color.Green();
    double b = color.Blue();

    double luma = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    return luma;
}
#endif
} // namespace

wxDEFINE_EVENT(wxEVT_SYS_COLOURS_CHANGED, clCommandEvent);

clSystemSettings::clSystemSettings()
{
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clSystemSettings::OnColoursChanged, this);
    wxTheApp->Bind(wxEVT_SYS_COLOUR_CHANGED, &clSystemSettings::OnSystemColourChanged, this);
    wxTheApp->Bind(wxEVT_ACTIVATE_APP, &clSystemSettings::OnAppActivated, this);

    /// keep the initial background colour
    startupBackgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
}

clSystemSettings::~clSystemSettings() {}

wxColour clSystemSettings::GetColour(int index)
{
#ifdef __WXOSX__
    if (!panel_face.IsOk()) {
        wxDialog* __p = new wxDialog(nullptr, wxID_ANY, "", wxPoint(-1000, -1000), wxSize(1, 1));
        panel_face = __p->GetBackgroundColour();
        wxDELETE(__p);
    }
    switch (index) {
    case wxSYS_COLOUR_TOOLBAR:
    case wxSYS_COLOUR_3DFACE:
        return panel_face;
    case wxSYS_COLOUR_TOOLBARTEXT:
        return wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT);
    default:
        return wxSystemSettings::GetColour((wxSystemColour)index);
    }
#else
    bool is_dark = DrawingUtils::IsDark(wxSYS_COLOUR_3DFACE);
    wxColour bg_colur = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#ifdef __WXGTK__
    if (is_dark) {
        wxColour window_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        wxColour face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        if (GetLuminance(window_colour) < GetLuminance(face_colour)) {
            // window colour is darker than the face colour, use the face colour
            bg_colur = window_colour;
        } else {
            // use button face colour
            bg_colur = face_colour;
        }
    }
#endif
    if (index == wxSYS_COLOUR_TOOLBAR) {
        return GetDefaultPanelColour();
    } else if (index == wxSYS_COLOUR_TOOLBARTEXT) {
        return wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
    } else if (index == wxSYS_COLOUR_WINDOW || index == wxSYS_COLOUR_3DFACE) {
        return bg_colur;
    } else if (index == wxSYS_COLOUR_LISTBOX) {
        return GetDefaultPanelColour().ChangeLightness(is_dark ? 110 : 90);
    } else {
        return wxSystemSettings::GetColour((wxSystemColour)index);
    }
#endif
}

void clSystemSettings::OnColoursChanged(clCommandEvent& event)
{
    event.Skip();
    DoColourChangedEvent();
}

clSystemSettings& clSystemSettings::Get()
{
    static clSystemSettings settings;
    return settings;
}

void clSystemSettings::OnSystemColourChanged(wxSysColourChangedEvent& event)
{
    event.Skip();
    clDEBUG() << "system colour changed!" << endl;
    DoColourChangedEvent();
}

void clSystemSettings::DoColourChangedEvent()
{
    // Notify about colours changes
    clCommandEvent evtColoursChanged(wxEVT_SYS_COLOURS_CHANGED);
    EventNotifier::Get()->AddPendingEvent(evtColoursChanged);
}

namespace
{
class MyDialog : public wxDialog
{
public:
    MyDialog(wxWindow* parent)
        : wxDialog(parent, wxID_ANY, wxEmptyString)
    {
        Hide();
    }
    ~MyDialog() override = default;
};
std::once_flag once;
} // namespace

wxColour clSystemSettings::GetDefaultPanelColour()
{
    wxColour panel_colour;
#if defined(__WXMAC__) || defined(__WXMSW__)
    static wxColour dlg_bg_colour;
    std::call_once(once, []() {
        MyDialog* dlg = new MyDialog(wxTheApp->GetTopWindow());
        dlg_bg_colour = dlg->GetBackgroundColour();
    });
    panel_colour = dlg_bg_colour;
#else
    panel_colour = GetColour(wxSYS_COLOUR_WINDOW);
#endif
    return panel_colour;
}

void clSystemSettings::OnAppActivated(wxActivateEvent& event)
{
    event.Skip();
    // check for external theme detection
    wxColour currentBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    if (currentBgColour != startupBackgroundColour) {
        /// and update the new background colour
        startupBackgroundColour = currentBgColour;

        /// external theme change detected, fire an event
        DoColourChangedEvent();
    }
}

bool clSystemSettings::IsDark()
{
#if wxCHECK_VERSION(3, 3, 0)
    return GetAppearance().IsDark();
#else
    static bool isDark = false;
    static bool once = true;
    if (once) {
        once = false;
        isDark = DrawingUtils::IsDark(GetDefaultPanelColour());
    }
    return isDark;
#endif
}
