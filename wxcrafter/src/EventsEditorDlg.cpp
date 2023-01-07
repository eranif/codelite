#include "EventsEditorDlg.h"

#include "allocator_mgr.h"
#include "wxc_project_metadata.h"
#include "wxc_widget.h"
#include "wxcrafter_plugin.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"

#include <ctags_manager.h>
#include <event_notifier.h>
#include <imanager.h>
#include <istorage.h>
#include <windowattrmanager.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>

wxString EventsEditorPane::PANE_NAME = "Control Events";

EventsEditorPane::EventsEditorPane(wxWindow* parent, wxcWidget* control, wxCrafterPlugin* plugin)
    : EventsEditorPaneBase(parent)
    , m_control(control)
    , m_plugin(plugin)
{
    DoInitialize();
    EventNotifier::Get()->Connect(wxEVT_TREE_ITEM_SELECTED, wxCommandEventHandler(EventsEditorPane::OnWidgetSelected),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_UPDATE_EVENTSEDITORPANE, wxCommandEventHandler(EventsEditorPane::OnUpdatePanes),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WXC_CLOSE_PROJECT, wxCommandEventHandler(EventsEditorPane::OnProjectClosed),
                                  NULL, this);
    m_controlEventsTable->Connect(wxEVT_PG_DOUBLE_CLICK, wxPropertyGridEventHandler(EventsEditorPane::OnDoubleClick),
                                  NULL, this);
    m_inheritedEventsTable->Connect(wxEVT_PG_DOUBLE_CLICK, wxPropertyGridEventHandler(EventsEditorPane::OnDoubleClick),
                                    NULL, this);
    m_controlEventsTable->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(EventsEditorPane::OnValueChanged), NULL,
                                  this);
    m_inheritedEventsTable->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(EventsEditorPane::OnValueChanged),
                                    NULL, this);
}

EventsEditorPane::~EventsEditorPane()
{

    EventNotifier::Get()->Disconnect(wxEVT_TREE_ITEM_SELECTED,
                                     wxCommandEventHandler(EventsEditorPane::OnWidgetSelected), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_UPDATE_EVENTSEDITORPANE,
                                     wxCommandEventHandler(EventsEditorPane::OnUpdatePanes), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WXC_CLOSE_PROJECT, wxCommandEventHandler(EventsEditorPane::OnProjectClosed),
                                     NULL, this);

    m_controlEventsTable->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(EventsEditorPane::OnValueChanged),
                                     NULL, this);
    m_controlEventsTable->Disconnect(wxEVT_PG_DOUBLE_CLICK, wxPropertyGridEventHandler(EventsEditorPane::OnDoubleClick),
                                     NULL, this);

    m_inheritedEventsTable->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(EventsEditorPane::OnValueChanged),
                                       NULL, this);
    m_inheritedEventsTable->Disconnect(wxEVT_PG_DOUBLE_CLICK,
                                       wxPropertyGridEventHandler(EventsEditorPane::OnDoubleClick), NULL, this);
}

void EventsEditorPane::DoInitialize()
{
    if(m_control) {
        m_staticTextEventsPaneTitle->SetLabel(_("Showing events for - ") + m_control->GetName());
    }

    m_controlEventsTable->GetGrid()->Clear();
    m_inheritedEventsTable->GetGrid()->Clear();

    if(m_control) {
        m_controlEventsTable->Construct(this, m_control, m_control->GetControlEvents());
        m_inheritedEventsTable->Construct(this, m_control, Allocator::GetCommonEvents());
    }
}

void EventsEditorPane::SplitterPositionChanged()
{
    if(m_control) {
        wxString label = m_control->GetName();
        // Without this rewrite, a name that's too long to display fully disappears forever if the splitter-sash is
        // moved :/
        m_staticTextEventsPaneTitle->SetLabel(_("Showing events for - ") + label);
    }

    // Try not to let the Description section of the wxPG entirely disappear. Or if it already has, try to show it
    // again.
    int charHt = m_controlEventsTable->GetCharHeight() * 3;
    if((m_controlEventsTable->GetDescBoxHeight() < charHt) && (m_controlEventsTable->GetClientSize().GetY() > charHt)) {
        m_controlEventsTable->SetDescBoxHeight(charHt / 2);
    }
}

void EventsEditorPane::Save()
{
    m_controlEventsTable->Save();
    if(m_inheritedEventsTable) {
        m_inheritedEventsTable->Save();
    }
}

void EventsEditorPane::OpenFunction(const wxString& kind, const wxString& fooname)
{
    wxUnusedVar(kind);
    wxUnusedVar(fooname);
}

void EventsEditorPane::OnWidgetSelected(wxCommandEvent& e)
{
    e.Skip();

    // Load the new item
    GUICraftItemData* selection = GUICraftMainPanel::m_MainPanel->GetSelItemData();
    CHECK_PTR_RET(selection);
    Clear();

    CHECK_PTR_RET(selection->m_wxcWidget);
    if(selection->m_wxcWidget->IsEventHandler()) {
        InitEventsForWidget(selection->m_wxcWidget);
    }
}

void EventsEditorPane::OnUpdatePanes(wxCommandEvent& e) // Called when the current control has been altered
{
    e.Skip();

    CHECK_POINTER(m_control);
    DoInitialize();
}

void EventsEditorPane::InitEventsForWidget(wxcWidget* control)
{
    m_control = control;
    DoInitialize();
}

void EventsEditorPane::OnProjectClosed(wxCommandEvent& e)
{
    e.Skip();
    Clear();
}

void EventsEditorPane::Clear()
{
    m_control = NULL;
    m_controlEventsTable->GetGrid()->Clear();
    m_inheritedEventsTable->GetGrid()->Clear();
    m_staticTextEventsPaneTitle->SetLabel("");
}

void EventsEditorPane::OnValueChanged(wxPropertyGridEvent& e)
{
    e.Skip();
    Save();
    wxcEditManager::Get().PushState("events updated");
}

void EventsEditorPane::OnDoubleClick(wxPropertyGridEvent& e)
{
    e.Skip();
    // Generate function name using the following format:
    // On<ControlName><EventName>
    wxString eventName = e.GetPropertyName();
    eventName.Replace("wxEVT_COMMAND_", "");
    eventName.Replace("wxEVT_", "");
    wxArrayString parts = ::wxStringTokenize(eventName, "_", wxTOKEN_STRTOK);
    wxString eventHandler = "On";

    // for non top level window, append the control name as well
    if(!m_control->IsTopWindow()) {
        wxString controlName = m_control->GetName();
        controlName.Replace("m_", "");
        if(controlName.StartsWith("_")) {
            controlName = controlName.Mid(1);
        }
        controlName.MakeCapitalized();
        eventHandler << controlName;
    }

    for(size_t i = 0; i < parts.GetCount(); ++i) {
        wxString part = parts.Item(i);
        part.MakeLower();
        part.MakeCapitalized();
        eventHandler << part;
    }
    e.GetProperty()->SetValue(eventHandler);
    Save();
    wxcEditManager::Get().PushState("events updated");
}
