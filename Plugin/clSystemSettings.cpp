#include "clSystemSettings.h"
#include "cl_config.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "theme_handler_helper.h"

bool clSystemSettings::m_useCustomColours = false;
clColours clSystemSettings::m_customColours;

wxDEFINE_EVENT(wxEVT_SYS_COLOURS_CHANGED, clCommandEvent);

clSystemSettings::clSystemSettings()
{
    m_useCustomColours = clConfig::Get().Read("UseCustomBaseColour", false);
    if(m_useCustomColours) {
        wxColour baseColour = clConfig::Get().Read("BaseColour", wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        m_customColours.InitFromColour(baseColour);
    }
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clSystemSettings::OnColoursChanged, this);
}

clSystemSettings::~clSystemSettings()
{
    EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clSystemSettings::OnColoursChanged, this);
}

wxColour clSystemSettings::GetColour(wxSystemColour index)
{
    if(m_useCustomColours) {
        switch(index) {
        case wxSYS_COLOUR_3DFACE:
        case wxSYS_COLOUR_LISTBOX:
            return m_customColours.GetBgColour();
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
            return wxSystemSettings::GetColour(index);
        }
    } else {
        return wxSystemSettings::GetColour(index);
    }
}

void clSystemSettings::OnColoursChanged(clCommandEvent& event)
{
    event.Skip();
    m_useCustomColours = clConfig::Get().Read("UseCustomBaseColour", false);
    if(m_useCustomColours) {
        wxColour baseColour = clConfig::Get().Read("BaseColour", wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        m_customColours.InitFromColour(baseColour);

        // Make sure that the notebook colours are getting updated as well
        ThemeHandlerHelper helper(EventNotifier::Get()->TopFrame());
        helper.UpdateNotebookColours(EventNotifier::Get()->TopFrame());
    }
    
    // Notify about colours changes
    clCommandEvent evtColoursChanged(wxEVT_SYS_COLOURS_CHANGED);
    EventNotifier::Get()->AddPendingEvent(evtColoursChanged);
}

clSystemSettings& clSystemSettings::Get()
{
    static clSystemSettings settings;
    return settings;
}
