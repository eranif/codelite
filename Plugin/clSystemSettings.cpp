#include "clSystemSettings.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "wx/app.h"
#include <wx/settings.h>

#ifdef __WXGTK__
#include <gtk/gtk.h>
#endif

bool clSystemSettings::m_useCustomColours = false;
clColours clSystemSettings::m_customColours;

wxDEFINE_EVENT(wxEVT_SYS_COLOURS_CHANGED, clCommandEvent);

#if 0
static GtkWidget* gs_tlw_parent = nullptr;
static GtkContainer* GetContainerWidget()
{
    static GtkContainer* s_widget;
    if(s_widget == NULL) {
        s_widget = GTK_CONTAINER(gtk_fixed_new());
        g_object_add_weak_pointer(G_OBJECT(s_widget), (void**)&s_widget);
        gs_tlw_parent = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_container_add(GTK_CONTAINER(gs_tlw_parent), GTK_WIDGET(s_widget));
    }
    return s_widget;
}

static GtkWidget* GetHeaderBarWidget()
{
    static GtkWidget* s_widget;
    if(s_widget == NULL) {
        s_widget = gtk_toolbar_new();
        g_object_add_weak_pointer(G_OBJECT(s_widget), (void**)&s_widget);
        gtk_container_add(GetContainerWidget(), s_widget);
        gtk_widget_ensure_style(s_widget);
    }
    return s_widget;
}

static const GtkStyle* GtkHeaderbarStyle() { return gtk_widget_get_style(GetHeaderBarWidget()); }

#endif

clSystemSettings::clSystemSettings()
{
    m_useCustomColours = clConfig::Get().Read("UseCustomBaseColour", false);
    if(m_useCustomColours) {
        wxColour baseColour = clConfig::Get().Read("BaseColour", wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        m_customColours.InitFromColour(baseColour);
    }
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clSystemSettings::OnColoursChanged, this);
    wxTheApp->Bind(wxEVT_SYS_COLOUR_CHANGED, &clSystemSettings::OnSystemColourChanged, this);
}

clSystemSettings::~clSystemSettings()
{
    // EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clSystemSettings::OnColoursChanged, this);
    // wxTheApp->Unbind(wxEVT_SYS_COLOUR_CHANGED, &clSystemSettings::OnSystemColourChanged, this);
}

wxColour clSystemSettings::GetColour(int index)
{
    if(m_useCustomColours) {
        if(index == wxSYS_COLOUR_TOOLBAR) {
            index = wxSYS_COLOUR_3DFACE; // fallback
        } else if(index == wxSYS_COLOUR_TOOLBARTEXT) {
            return m_customColours.GetItemTextColour();
        }
        switch(index) {
        case wxSYS_COLOUR_MENUBAR:
        case wxSYS_COLOUR_3DFACE:
        case wxSYS_COLOUR_LISTBOX:
        case wxSYS_COLOUR_WINDOW:
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
            return wxSystemSettings::GetColour((wxSystemColour)index);
        }
    } else {
        if(index == wxSYS_COLOUR_TOOLBAR) {
            return wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        } else if(index == wxSYS_COLOUR_TOOLBARTEXT) {
            return wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
#ifdef __WXMSW__
        } else if(index == wxSYS_COLOUR_WINDOW) {
            return wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        } else if(index == wxSYS_COLOUR_3DFACE) {
            return wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE).ChangeLightness(90);
#endif
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
    DoColourChangedEvent();
}

void clSystemSettings::DoColourChangedEvent()
{
    m_useCustomColours = clConfig::Get().Read("UseCustomBaseColour", false);
    if(m_useCustomColours) {
        wxColour baseColour = clConfig::Get().Read("BaseColour", wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        m_customColours.InitFromColour(baseColour);
    } else {
        m_customColours.InitFromColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    }

    // Notify about colours changes
    clCommandEvent evtColoursChanged(wxEVT_SYS_COLOURS_CHANGED);
    EventNotifier::Get()->AddPendingEvent(evtColoursChanged);
}
