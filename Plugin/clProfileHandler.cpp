#include "clProfileHandler.h"

#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "Notebook.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "workspace.h"

#include <algorithm>

clProfileHandler::clProfileHandler()
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clProfileHandler::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clProfileHandler::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_GOING_DOWN, &clProfileHandler::OnGoingDown, this);

    m_cxxOutputTabs = {
        "UnitTest++", "Trace", "CppCheck", "MemCheck", "CScope", "BuildQ",
    };
    m_cxxWorkspaceTabs = { "CMake", "wxCrafter", "Groups" };
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
    if(::clIsCxxWorkspaceOpened()) {
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

void clProfileHandler::HandleWorkspaceTabs(bool show)
{
    if(show) {
        RestoreTabs(m_cxxWorkspaceTabsToRestore, wxEVT_SHOW_WORKSPACE_TAB);
    } else {
        HideTabs(m_cxxWorkspaceTabs, PaneId::SIDE_BAR, wxEVT_SHOW_WORKSPACE_TAB, m_cxxWorkspaceTabsToRestore);
    }
}

void clProfileHandler::HandleOutputTabs(bool show)
{
    if(show) {
        RestoreTabs(m_cxxOutputTabsToRestore, wxEVT_SHOW_OUTPUT_TAB);
    } else {
        HideTabs(m_cxxOutputTabs, PaneId::BOTTOM_BAR, wxEVT_SHOW_OUTPUT_TAB, m_cxxOutputTabsToRestore);
    }
}

bool clProfileHandler::IsPageExistsInBook(PaneId pane_id, const wxString& label) const
{
    return clGetManager()->BookGetPage(pane_id, label) != nullptr;
}

void clProfileHandler::RestoreTabs(wxStringSet_t& tabs, wxEventType eventType)
{
    std::for_each(tabs.begin(), tabs.end(), [&](const wxString& tab) {
        clCommandEvent eventShow(eventType);
        eventShow.SetSelected(true).SetString(tab);
        EventNotifier::Get()->AddPendingEvent(eventShow);
    });
    tabs.clear();
}

void clProfileHandler::HideTabs(const wxStringSet_t& candidates, PaneId pane_id, wxEventType eventType,
                                wxStringSet_t& tabsHidden)
{
    tabsHidden.clear();
    for(const auto& tab : candidates) {
        if(IsPageExistsInBook(pane_id, tab)) {
            tabsHidden.insert(tab);
            clCommandEvent eventHide(eventType);
            eventHide.SetSelected(false).SetString(tab);
            EventNotifier::Get()->AddPendingEvent(eventHide);
        }
    }
}

void clProfileHandler::OnGoingDown(clCommandEvent& e)
{
    e.Skip();
    RestoreTabs(m_cxxOutputTabsToRestore, wxEVT_SHOW_OUTPUT_TAB);
    RestoreTabs(m_cxxWorkspaceTabsToRestore, wxEVT_SHOW_WORKSPACE_TAB);
}
