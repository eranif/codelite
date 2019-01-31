#ifndef EVENTSTABLELISTVIEW_H
#define EVENTSTABLELISTVIEW_H

#include "events_database.h"
#include "wxc_widget.h"
#include <wx/bitmap.h>
#include <wx/button.h>
#include <wx/dc.h>
#include <wx/propgrid/manager.h>
#include <wx/renderer.h>

class EventsEditorPane;
class EventsTableListView : public wxPropertyGridManager
{
    wxcWidget* m_control;
    EventsDatabase* m_eventsDb;
    EventsEditorPane* m_dlg;
    wxBitmap m_dropDownBmp;
    wxString m_gotoFunctionName;

protected:
    void OnPropertyChanged(wxPropertyGridEvent& e);

public:
    EventsTableListView(wxWindow* parent);
    virtual ~EventsTableListView();

    void Construct(EventsEditorPane* dlg, wxcWidget* control, const EventsDatabase& events);
    void Save();
};

#endif // EVENTSTABLELISTVIEW_H
