#include "AutoSaveDlg.h"
#include "AutoSaveSettings.h"
#include "autosave.h"
#include "event_notifier.h"
#include <algorithm>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

static AutoSave* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) { thePlugin = new AutoSave(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("PC"));
    info.SetName(wxT("AutoSave"));
    info.SetDescription(_("Automatically save modified source files"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

AutoSave::AutoSave(IManager* manager)
    : IPlugin(manager)
    , m_timer(NULL)
{
    m_longName = _("Automatically save modified source files");
    m_shortName = wxT("AutoSave");

    UpdateTimers();
    wxTheApp->Bind(wxEVT_MENU, &AutoSave::OnSettings, this, XRCID("auto_save_settings"));
}

AutoSave::~AutoSave() {}

void AutoSave::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void AutoSave::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(new wxMenuItem(menu, XRCID("auto_save_settings"), _("Settings...")));
    pluginsMenu->Append(wxID_ANY, _("Auto Save"), menu);
}

void AutoSave::UnPlug()
{
    DeleteTimer();
    wxTheApp->Unbind(wxEVT_MENU, &AutoSave::OnSettings, this, XRCID("auto_save_settings"));
}

void AutoSave::OnSettings(wxCommandEvent& event)
{
    AutoSaveDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        // Update the settings
        UpdateTimers();
    }
}

void AutoSave::UpdateTimers()
{
    DeleteTimer();
    AutoSaveSettings conf = AutoSaveSettings::Load();
    if(!conf.HasFlag(AutoSaveSettings::kEnabled)) { return; }

    m_timer = new wxTimer(this, XRCID("auto_save_timer"));
    m_timer->Start((conf.GetCheckInterval() * 1000), true);
    Bind(wxEVT_TIMER, &AutoSave::OnTimer, this);
}

void AutoSave::OnTimer(wxTimerEvent& event)
{
    IEditor::List_t editors;
    m_mgr->GetAllEditors(editors);

    // Save every modified editor
    std::for_each(editors.begin(), editors.end(), [&](IEditor* editor) {
        // Save modified files. However, don't attempt to try and save an "Untitled" document :/
        if(editor->IsEditorModified() && editor->GetFileName().Exists()) {

            // Don't auto-save remote files marked with "SFTP"
            if(!editor->GetClientData("sftp")) { editor->Save(); }
        }
    });

    // Restart the timer
    AutoSaveSettings conf = AutoSaveSettings::Load();
    m_timer->Start((conf.GetCheckInterval() * 1000), true);
}

void AutoSave::DeleteTimer()
{
    if(m_timer) {
        Unbind(wxEVT_TIMER, &AutoSave::OnTimer, this);
        m_timer->Stop();
    }
    wxDELETE(m_timer);
}
