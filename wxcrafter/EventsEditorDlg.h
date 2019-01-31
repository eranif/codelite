#ifndef EVENTSEDITORDLG_H
#define EVENTSEDITORDLG_H

#include "events_database.h"
#include "wxcrafter.h"
#include <entry.h>

class wxcWidget;
class wxCrafterPlugin;

class EventsEditorPane : public EventsEditorPaneBase
{
    wxcWidget* m_control;
    wxCrafterPlugin* m_plugin;

public:
    static wxString PANE_NAME;

protected:
    void DoInitialize();
    TagEntryPtr DoLocateFunction(const wxString& kind, const wxString& fooname);

protected:
    void OnWidgetSelected(wxCommandEvent& e);
    void OnProjectClosed(wxCommandEvent& e);
    void OnUpdatePanes(wxCommandEvent& e); // Called when the current control has been altered
    void OnValueChanged(wxPropertyGridEvent& e);
    void OnDoubleClick(wxPropertyGridEvent& e);

public:
    EventsEditorPane(wxWindow* parent, wxcWidget* control, wxCrafterPlugin* plugin);
    virtual ~EventsEditorPane();
    void OpenFunction(const wxString& kind, const wxString& fooname);
    void Save();
    void InitEventsForWidget(wxcWidget* control);
    void Clear();
    void SplitterPositionChanged();
};
#endif // EVENTSEDITORDLG_H
