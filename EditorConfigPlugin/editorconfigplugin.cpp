#include "editorconfigplugin.h"
#include <wx/xrc/xmlres.h>
#include "event_notifier.h"
#include "codelite_events.h"
#include <wx/filename.h>

static EditorConfigPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new EditorConfigPlugin(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("EditorConfig"));
    info.SetDescription(_("Support for .editorconfig files in CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

EditorConfigPlugin::EditorConfigPlugin(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Support for .editorconfig files in CodeLite");
    m_shortName = wxT("EditorConfig");

    // Bind events
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_LOADING, &EditorConfigPlugin::OnEditorConfigLoading, this);
}

EditorConfigPlugin::~EditorConfigPlugin() {}

clToolBar* EditorConfigPlugin::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void EditorConfigPlugin::CreatePluginMenu(wxMenu* pluginsMenu) {}

void EditorConfigPlugin::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_LOADING, &EditorConfigPlugin::OnEditorConfigLoading, this);
}

void EditorConfigPlugin::OnEditorConfigLoading(clEditorConfigEvent& event)
{
    // TODO: cache the results
    event.Skip();
    clEditorConfig conf;
    clEditorConfigSection section;
    wxFileName fn(event.GetFileName());
    if(!fn.IsOk() || !fn.FileExists()) return;
    if(conf.GetSectionForFile(event.GetFileName(), section)) {
        event.Skip(false);
        event.SetEditorConfig(section);
    }
}
