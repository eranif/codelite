#include "EditorConfigSettings.h"
#include "EditorConfigSettingsDlg.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "editorconfigplugin.h"
#include "event_notifier.h"
#include "file_logger.h"
#include <wx/filename.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

static EditorConfigPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) { thePlugin = new EditorConfigPlugin(manager); }
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

    // Load the settings
    m_settings.Load();

    // Bind events
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_LOADING, &EditorConfigPlugin::OnEditorConfigLoading, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &EditorConfigPlugin::OnActiveEditorChanged, this);
}

EditorConfigPlugin::~EditorConfigPlugin() {}

void EditorConfigPlugin::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void EditorConfigPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(new wxMenuItem(menu, XRCID("editor_config_settings"), _("Settings...")));
    pluginsMenu->Append(wxID_ANY, "EditorConfig", menu);
    menu->Bind(wxEVT_MENU, &EditorConfigPlugin::OnSettings, this, XRCID("editor_config_settings"));
}

void EditorConfigPlugin::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_LOADING, &EditorConfigPlugin::OnEditorConfigLoading, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &EditorConfigPlugin::OnActiveEditorChanged, this);
}

void EditorConfigPlugin::OnEditorConfigLoading(clEditorConfigEvent& event)
{
    event.Skip();
    if(!m_settings.IsEnabled()) {
        clDEBUG() << "EditorConfig is disabled." << clEndl;
        return;
    }

    clEditorConfigSection section;
    wxFileName fn(event.GetFileName());
    if(!DoGetEditorConfigForFile(fn, section)) { return; }

    event.Skip(false);
    event.SetEditorConfig(section);
}

void EditorConfigPlugin::OnActiveEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    if(!m_settings.IsEnabled()) {
        clDEBUG1() << "EditorConfig is disabled." << clEndl;
        return;
    }

    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    OptionsConfigPtr conf = editor->GetOptions();
    CHECK_PTR_RET(conf);

    clEditorConfigSection section;
    if(!DoGetEditorConfigForFile(editor->GetFileName(), section)) return;
    conf->UpdateFromEditorConfig(section);
    editor->ApplyEditorConfig();
}

bool EditorConfigPlugin::DoGetEditorConfigForFile(const wxFileName& filename, clEditorConfigSection& section)
{
    // Try the cache first
    if(m_cache.Get(filename, section)) {
        clDEBUG1() << "Using EditorConfig settings for file:" << filename << clEndl;
        section.PrintToLog();
        return true;
    }

    // Sanity
    if(!filename.IsOk() || !filename.FileExists()) {
        clDEBUG1() << "No EditorConfig file found for file:" << filename << clEndl;
        return false;
    }

    clEditorConfig conf;
    if(!conf.GetSectionForFile(filename, section)) {
        // Update the cache
        clDEBUG1() << "No EditorConfig file found for file:" << filename << clEndl;
        return false;
    }

    m_cache.Add(filename, section);
    return true;
}

void EditorConfigPlugin::OnSettings(wxCommandEvent& event)
{
    EditorConfigSettingsDlg dlg(wxTheApp->GetTopWindow());
    if(dlg.ShowModal() == wxID_OK) {
        // Store the settings
        m_settings.SetEnabled(dlg.IsEnabled());
        m_settings.Save();
    }
}
