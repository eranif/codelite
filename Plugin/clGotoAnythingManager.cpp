#include "GotoAnythingDlg.h"
#include "clGotoAnythingManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include <algorithm>
#include <wx/xrc/xmlres.h>

clGotoAnythingManager::clGotoAnythingManager()
{
    // Register the common actions
    m_coreActions[_("Preferences")] = wxID_PREFERENCES;
    m_coreActions[_("Colours and Fonts")] = XRCID("syntax_highlight");
    m_coreActions[_("Keyboard Shortcuts")] = XRCID("configure_accelerators");
    m_coreActions[_("Environment Variables")] = XRCID("add_envvar");
    m_coreActions[_("Build Settings")] = XRCID("advance_settings");
    m_coreActions[_("GDB settings")] = XRCID("debuger_settings");
    m_coreActions[_("Code Completion Settings")] = XRCID("tags_options");

    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SELECTED, &clGotoAnythingManager::OnActionSelected, this);
}

clGotoAnythingManager::~clGotoAnythingManager()
{
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SELECTED, &clGotoAnythingManager::OnActionSelected, this);
}

void clGotoAnythingManager::Add(const wxString& entry) { m_pluginActions.insert(entry); }

void clGotoAnythingManager::Delete(const wxString& entry) { m_pluginActions.erase(entry); }

clGotoAnythingManager& clGotoAnythingManager::Get()
{
    static clGotoAnythingManager manager;
    return manager;
}

void clGotoAnythingManager::OnActionSelected(clCommandEvent& e)
{
    e.Skip();
    if(m_coreActions.count(e.GetString())) {
        e.Skip(false);

        // Trigger the action
        wxCommandEvent evtAction(wxEVT_MENU, m_coreActions[e.GetString()]);
        EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evtAction);
    }
}

void clGotoAnythingManager::ShowDialog()
{
    GotoAnythingDlg dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

std::vector<wxString> clGotoAnythingManager::GetActions() const
{
    std::vector<wxString> actions;
    std::for_each(m_coreActions.begin(), m_coreActions.end(),
                  [&](const std::unordered_map<wxString, int>::value_type& vt) { actions.push_back(vt.first); });
    std::for_each(m_pluginActions.begin(), m_pluginActions.end(),
                  [&](const wxString& action) { actions.push_back(action); });
    std::sort(actions.begin(), actions.end());
    return actions;
}
