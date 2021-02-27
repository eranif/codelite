#include "DockerOutputPane.h"
#include "DockerSettingsDlg.h"
#include "bitmap_loader.h"
#include "clDockerWorkspace.h"
#include "clWorkspaceManager.h"
#include "docker.h"
#include "event_notifier.h"
#include "wxterminal.h"
#include <imanager.h>
#include <wx/xrc/xmlres.h>

static Docker* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new Docker(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("Docker"));
    info.SetDescription(_("Docker for CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

Docker::Docker(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Docker for CodeLite");
    m_shortName = wxT("Docker");
    m_driver.reset(new clDockerDriver(this));

    clWorkspaceManager::Get().RegisterWorkspace(new clDockerWorkspace(false, nullptr, m_driver));
    clDockerWorkspace::Initialise(this);
    clDockerWorkspace::Get(); // Make sure that the workspace instance is up and all events are hooked

    Notebook* book = m_mgr->GetOutputPaneNotebook();
    auto images = book->GetBitmaps();
    m_outputView = new DockerOutputPane(book, m_driver);
    book->AddPage(m_outputView, _("Docker"), false, images->Add("docker"));

    m_tabToggler.reset(new clTabTogglerHelper(_("Docker"), m_outputView, "", NULL));
    m_tabToggler->SetOutputTabBmp(images->Add("docker"));
}

Docker::~Docker() {}

void Docker::CreateToolBar(clToolBar* toolbar)
{
    // You can add items to the main toolbar here
    wxUnusedVar(toolbar);
}

void Docker::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(XRCID("ID_DOCKER_SETTINGS"), _("Settings"));
    pluginsMenu->Append(wxID_ANY, _("Docker"), menu);
    menu->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& event) {
            DockerSettingsDlg dlg(EventNotifier::Get()->TopFrame());
            if(dlg.ShowModal() == wxID_OK) {}
        },
        XRCID("ID_DOCKER_SETTINGS"));
}

void Docker::UnPlug()
{
    clDockerWorkspace::Shutdown();

    // before this plugin is un-plugged we must remove the tab we added
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if(m_outputView == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            m_outputView->Destroy();
            break;
        }
    }
}
