#include "clSystemSettings.h"

#include "cl_config.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "wx/app.h"

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/settings.h>

bool clSystemSettings::m_useCustomColours = false;
clColours clSystemSettings::m_customColours;
wxColour clSystemSettings::btn_face;
wxColour clSystemSettings::panel_face;

#ifdef __WXMSW__
#define IS_MSW 1
#define IS_MAC 0
#define IS_GTK 0
#elif defined(__WXMAC__)
#define IS_MSW 0
#define IS_MAC 1
#define IS_GTK 0
#else
#define IS_MSW 0
#define IS_MAC 0
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
    m_useCustomColours = clConfig::Get().Read("UseCustomBaseColour", false);
    if(m_useCustomColours) {
        wxColour baseColour = clConfig::Get().Read("BaseColour", wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        m_customColours.InitFromColour(baseColour);
    }
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clSystemSettings::OnColoursChanged, this);
    wxTheApp->Bind(wxEVT_SYS_COLOUR_CHANGED, &clSystemSettings::OnSystemColourChanged, this);
    wxTheApp->Bind(wxEVT_ACTIVATE_APP, &clSystemSettings::OnAppActivated, this);

    /// keep the initial background colour
    startupBackgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
}

clSystemSettings::~clSystemSettings() {}

wxColour clSystemSettings::GetColour(int index)
{
    if(m_useCustomColours) {
        bool is_dark = DrawingUtils::IsDark(m_customColours.GetBgColour());

        if(index == wxSYS_COLOUR_TOOLBAR) {
            index = wxSYS_COLOUR_3DFACE; // fallback
        } else if(index == wxSYS_COLOUR_TOOLBARTEXT) {
            return m_customColours.GetItemTextColour();
        }
        switch(index) {
        case wxSYS_COLOUR_MENUBAR:
        case wxSYS_COLOUR_3DFACE:
        case wxSYS_COLOUR_WINDOW:
            return m_customColours.GetBgColour();
        case wxSYS_COLOUR_LISTBOX:
            return m_customColours.GetBgColour().ChangeLightness(is_dark ? 105 : 100);
        case wxSYS_COLOUR_3DSHADOW:
            return m_customColours.GetBorderColour();
        case wxSYS_COLOUR_3DDKSHADOW:
            return m_customColours.GetDarkBorderColour();
        case wxSYS_COLOUR_WINDOWTEXT:
        case wxSYS_COLOUR_BTNTEXT:
            return m_customColours.GetItemTextColour();
        case wxSYS_COLOUR_GRAYTEXT:
            return m_customColours.GetGrayText();
        default:
            return wxSystemSettings::GetColour((wxSystemColour)index);
        }
    } else {
        bool is_dark = DrawingUtils::IsDark(wxSYS_COLOUR_3DFACE);
        wxColour bg_colur = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#ifdef __WXGTK__
        if(is_dark) {
            wxColour window_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
            wxColour face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
            if(GetLuminance(window_colour) < GetLuminance(face_colour)) {
                // window colour is darker than the face colour, use the face colour
                bg_colur = window_colour;
            } else {
                // use button face colour
                bg_colur = face_colour;
            }
        }
#endif
        if(index == wxSYS_COLOUR_TOOLBAR) {
            return GetDefaultPanelColour();
        } else if(index == wxSYS_COLOUR_TOOLBARTEXT) {
            return wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
        } else if(index == wxSYS_COLOUR_WINDOW || index == wxSYS_COLOUR_3DFACE) {
            return bg_colur;
        } else if(index == wxSYS_COLOUR_LISTBOX) {
            return GetDefaultPanelColour().ChangeLightness(is_dark ? 110 : 90);
        } else {
            return wxSystemSettings::GetColour((wxSystemColour)index);
        }
    }
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
    m_useCustomColours = clConfig::Get().Read("UseCustomBaseColour", false);
    if(m_useCustomColours) {
        wxColour baseColour = clConfig::Get().Read("BaseColour", wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        if(DrawingUtils::IsDark(baseColour)) {
            baseColour = baseColour.ChangeLightness(110);
        } else {
            baseColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        }
        m_customColours.InitFromColour(baseColour);

    } else {
        m_customColours.InitFromColour(GetDefaultPanelColour());
    }

    // Notify about colours changes
    clCommandEvent evtColoursChanged(wxEVT_SYS_COLOURS_CHANGED);
    EventNotifier::Get()->AddPendingEvent(evtColoursChanged);
}

wxColour clSystemSettings::GetDefaultPanelColour()
{
    wxColour panel_colour;
    panel_colour = GetColour(IS_GTK ? wxSYS_COLOUR_WINDOW : wxSYS_COLOUR_3DFACE);
#ifdef __WXGTK__
    if(!m_useCustomColours && !DrawingUtils::IsDark(panel_colour)) {
        panel_colour = panel_colour.ChangeLightness(95);
    }
#endif
    return panel_colour;
}

void clSystemSettings::OnAppActivated(wxActivateEvent& event)
{
    event.Skip();
    // check for external theme detection
    wxColour currentBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    if(currentBgColour != startupBackgroundColour) {
        /// and update the new background colour
        startupBackgroundColour = currentBgColour;

        /// external theme change detected, fire an event
        DoColourChangedEvent();
    }
}

void clSystemSettings::SampleColoursFromControls() {}
