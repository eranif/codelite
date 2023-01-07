#include "events_table_list_view.h"

#include "EventsEditorDlg.h"
#include "wxc_project_metadata.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_helpers.h"

#include <wx/wxcrtvararg.h>

namespace
{
wxString PLACE_HOLDER = "";
}

wxDECLARE_EVENT(wxEVT_EVENTS_PROPERTIES_UPDATED, wxCommandEvent);

EventsTableListView::EventsTableListView(wxWindow* parent)
    : wxPropertyGridManager(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION |
                                wxCrafter::GetControlBorder())
    , m_eventsDb(NULL)
{
    Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(EventsTableListView::OnPropertyChanged), NULL, this);
}

EventsTableListView::~EventsTableListView()
{
    Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(EventsTableListView::OnPropertyChanged), NULL, this);
}

void EventsTableListView::Construct(EventsEditorPane* dlg, wxcWidget* control, const EventsDatabase& events)
{
    wxPropertyGrid* grid = GetGrid();
    grid->Append(new wxPropertyCategory(_("Event Handlers")));

    m_dlg = dlg;
    m_eventsDb = const_cast<EventsDatabase*>(&events);
    CHECK_POINTER(control);
    m_control = control;
    EventsDatabase::MapEvents_t::const_iterator iter = events.GetEvents().begin();
    for(; iter != events.GetEvents().end(); ++iter) {
        ConnectDetails cd = iter->second;

        wxString eventName = cd.GetEventName();
        eventName.Trim().Trim(false);
        if(eventName.IsEmpty())
            continue;

        // Check to see if this event has a user defined function
        wxString fooname;
        if(control->HasEvent(eventName)) {
            fooname = control->GetEvent(eventName).GetFunctionNameAndSignature();
        } else if(!cd.GetFunctionNameAndSignature().empty()) {
            // A few events (well, just 1 atm) come with a default function
            fooname = cd.GetFunctionNameAndSignature();
        } else {
            fooname = PLACE_HOLDER;
        }

        fooname = fooname.BeforeFirst(wxT('('));
        wxPGProperty* prop = grid->Append(new wxStringProperty(eventName, wxPG_LABEL, fooname));
        SetDescription("", ""); // SetHelpString("") doesn't seem to clear any stale description, so clear it first
        prop->SetHelpString(cd.GetDescription());
    }
}

void EventsTableListView::Save()
{
    wxPropertyGrid* grid = GetGrid();

    wxPropertyGridIterator it;
    for(it = grid->GetIterator(); !it.AtEnd(); it++) {
        wxPGProperty* p = *it;
        wxString event = p->GetLabel();
        wxString func = p->GetValueAsString();

        func.Trim().Trim(false);

        // If the handler is empty delete the event
        if(func.IsEmpty() || func == PLACE_HOLDER) {
            m_control->RemoveEvent(event);

        } else if(m_eventsDb) {
            ConnectDetails cd = m_eventsDb->GetEvents().Item(event);
            cd.MakeSignatureForName(func);
            // insert or update
            m_control->AddEvent(cd);
        }

        // Out of politeness, tell our control; it might even be interested e.g. dropdown tools
        wxCommandEvent evt(wxEVT_EVENTS_PROPERTIES_UPDATED);
        evt.SetString(event);
        EventNotifier::Get()->AddPendingEvent(evt);
    }
}

void EventsTableListView::OnPropertyChanged(wxPropertyGridEvent& e) { e.Skip(); }
