#include "clProfileHandler.h"

#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"

clProfileHandler::clProfileHandler()
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clProfileHandler::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clProfileHandler::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_GOING_DOWN, &clProfileHandler::OnGoingDown, this);

    m_cxxOutputTabs = {
        "UnitTest++",
        "Trace",
        "CppCheck",
        "MemCheck",
        "CScope",
        "BuildQ",
    };
    m_cxxWorkspaceTabs = {"CMake", "wxCrafter", "Groups"};
}

clProfileHandler::~clProfileHandler()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clProfileHandler::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clProfileHandler::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_GOING_DOWN, &clProfileHandler::OnGoingDown, this);
}

void clProfileHandler::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    RestoreTabs(m_cxxOutputTabsToRestore, wxEVT_SHOW_OUTPUT_TAB);
    RestoreTabs(m_cxxWorkspaceTabsToRestore, wxEVT_SHOW_WORKSPACE_TAB);
}

void clProfileHandler::OnWorkspaceLoaded(clWorkspaceEvent& e)
{
    e.Skip();
    if (::clIsCxxWorkspaceOpened()) {
        // we just opened a C++ workspace, restore all C++ related tabs
        HandleOutputTabs(true);
        HandleWorkspaceTabs(true);
    } else {
        // Hide any C++ related tabs
        HandleOutputTabs(false);
        HandleWorkspaceTabs(false);
    }
}

clProfileHandler& clProfileHandler::Get()
{
    static clProfileHandler handler;
    return handler;
}

void clProfileHandler::HandleWorkspaceTabs([[maybe_unused]] bool show) {}

void clProfileHandler::HandleOutputTabs([[maybe_unused]] bool show) {}

bool clProfileHandler::IsPageExistsInBook(PaneId pane_id, const wxString& label) const
{
    return clGetManager()->BookGetPage(pane_id, label) != nullptr;
}

void clProfileHandler::RestoreTabs(wxStringSet_t& tabs, wxEventType eventType)
{
    for (const wxString& tab : tabs) {
        clCommandEvent eventShow(eventType);
        eventShow.SetSelected(true).SetString(tab);
        EventNotifier::Get()->AddPendingEvent(eventShow);
    }
    tabs.clear();
}

void clProfileHandler::HideTabs([[maybe_unused]] const wxStringSet_t& candidates,
                                [[maybe_unused]] PaneId pane_id,
                                [[maybe_unused]] wxEventType eventType,
                                [[maybe_unused]] wxStringSet_t& tabsHidden)
{
}

void clProfileHandler::OnGoingDown(clCommandEvent& e) { e.Skip(); }
