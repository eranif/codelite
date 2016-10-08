#include "tail.h"
#include <wx/xrc/xmlres.h>
#include "TailPanel.h"
#include "cl_config.h"
#include "cl_command_event.h"
#include "event_notifier.h"

static Tail* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new Tail(manager);
    }
    return thePlugin;
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
{
    m_longName = _("A Linux like tail command ");
    m_shortName = wxT("Tail");

    // Hook our output-pane panel
    wxBitmap bmp = m_mgr->GetStdIcons()->LoadBitmap("mime-txt");
    m_view = new TailPanel(m_mgr->GetOutputPaneNotebook());
    m_editEventsHandler.Reset(new clEditEventsHandler(m_view->GetStc()));

    m_mgr->GetOutputPaneNotebook()->AddPage(m_view, "Tail", false, bmp);
    m_tabHelper.reset(new clTabTogglerHelper("Tail", m_view, "", NULL));
    m_tabHelper->SetOutputTabBmp(bmp);

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

clToolBar* Tail::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void Tail::CreatePluginMenu(wxMenu* pluginsMenu) {}

void Tail::UnPlug()
{
    m_editEventsHandler.Reset(NULL);
    // Unbind events
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &Tail::OnInitDone, this);

    // Remove our tab
    m_tabHelper.reset(NULL); // before this plugin is un-plugged we must remove the tab we added
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if(m_view == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            m_view->Destroy();
            break;
        }
    }
}
