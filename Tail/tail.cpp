#include "tail.h"

#include "TailFrame.h"
#include "TailPanel.h"
#include "cl_command_event.h"
#include "cl_config.h"
#include "event_notifier.h"

#include <wx/xrc/xmlres.h>

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    return new Tail(manager);
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("PC"));
    info.SetName(wxT("Tail"));
    info.SetDescription(_("A Linux like tail command "));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

Tail::Tail(IManager* manager)
    : IPlugin(manager)
    , m_view(NULL)
{
    m_longName = _("A Linux like tail command ");
    m_shortName = wxT("Tail");

    // Hook our output-pane panel
    InitTailWindow(m_mgr->BookGet(PaneId::BOTTOM_BAR), true, TailData(), false);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &Tail::OnInitDone, this);
}

Tail::~Tail() {}

void Tail::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    if(clConfig::Get().Read("force-show-tail-tab", true)) {
        clCommandEvent eventShow(wxEVT_SHOW_OUTPUT_TAB);
        eventShow.SetSelected(true).SetString("Tail");
        EventNotifier::Get()->AddPendingEvent(eventShow);
    }
    clConfig::Get().Write("force-show-tail-tab", false);
}

void Tail::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void Tail::CreatePluginMenu(wxMenu* pluginsMenu) {}

void Tail::UnPlug()
{
    m_editEventsHandler = nullptr;
    // Unbind events
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &Tail::OnInitDone, this);

    // Remove our tab
    m_tabHelper.reset(NULL); // before this plugin is un-plugged we must remove the tab we added
    if(m_view && !m_view->IsDetached()) {
        DoDetachWindow();
        m_view->Destroy();
        m_view = NULL;
    } else if(m_view && m_view->GetFrame()) {
        m_view->GetFrame()->Destroy();
        m_view->SetFrame(NULL);
    }
}

void Tail::DetachTailWindow(const TailData& d)
{
    // Create new frame
    TailFrame* frame = new TailFrame(EventNotifier::Get()->TopFrame(), this);
    InitTailWindow(frame, false, d, false);
    m_view->SetIsDetached(true); // set the window as detached
    m_view->SetFrame(frame);
    frame->GetSizer()->Add(m_view, 1, wxEXPAND);
    frame->GetSizer()->Fit(frame);
    m_view->SetFrameTitle();
    frame->Show();
}

void Tail::DockTailWindow(const TailData& d)
{
    InitTailWindow(m_mgr->BookGet(PaneId::BOTTOM_BAR), true, d, true);
    m_mgr->GetDockingManager()->Update();
}

void Tail::DoDetachWindow() { m_mgr->BookRemovePage(PaneId::BOTTOM_BAR, m_view); }

void Tail::InitTailWindow(wxWindow* parent, bool isNotebook, const TailData& d, bool selectPage)
{
    TailPanel* tmpView = new TailPanel(parent, this);
    tmpView->Initialize(d);

    if(m_view) {
        // copy the settinhs from the current view
        DoDetachWindow();
        m_view->Destroy();
        m_view = NULL;
    }

    // Hook our output-pane panel
    m_view = tmpView;
    m_editEventsHandler = std::make_unique<clEditEventsHandler>(m_view->GetStc());
    if(isNotebook) {
        m_mgr->BookAddPage(PaneId::BOTTOM_BAR, m_view, "Tail");
    } else {
        m_tabHelper.reset(NULL);
    }
}
