#include "smartcompletion.h"
#include <wx/xrc/xmlres.h>
#include "event_notifier.h"
#include "codelite_events.h"
#include "macros.h"
#include <queue>
#include "SmartCompletionsSettingsDlg.h"
#include <wx/menu.h>
#include <algorithm>

static SmartCompletion* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new SmartCompletion(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("PC"));
    info.SetName(wxT("SmartCompletion"));
    info.SetDescription(_("Make the default Code Completion smarter and better"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

SmartCompletion::SmartCompletion(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Make the default Code Completion smarter and better");
    m_shortName = wxT("SmartCompletion");
    EventNotifier::Get()->Bind(wxEVT_CCBOX_SELECTION_MADE, &SmartCompletion::OnCodeCompletionSelectionMade, this);
    EventNotifier::Get()->Bind(wxEVT_CCBOX_SHOWING, &SmartCompletion::OnCodeCompletionShowing, this);
    m_config.Load();
    m_pWeight = &m_config.GetWeightTable();
}

SmartCompletion::~SmartCompletion() {}

clToolBar* SmartCompletion::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void SmartCompletion::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("smart_completion_settings"), _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, wxT("SmartCompletions"), menu);

    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &SmartCompletion::OnSettings, this, XRCID("smart_completion_settings"));
}

void SmartCompletion::UnPlug()
{
    m_config.Save();
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SELECTION_MADE, &SmartCompletion::OnCodeCompletionSelectionMade, this);
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SHOWING, &SmartCompletion::OnCodeCompletionShowing, this);
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &SmartCompletion::OnSettings, this, XRCID("smart_completion_settings"));
}

void SmartCompletion::OnCodeCompletionSelectionMade(clCodeCompletionEvent& event)
{
    event.Skip();
    if(!m_config.IsEnabled()) return;

    CHECK_PTR_RET(event.GetEntry());

    // Collect info about this match
    TagEntryPtr tag = event.GetEntry()->GetTag();
    if(tag) {
        // we have an associated tag
        wxString k = tag->GetScope() + "::" + tag->GetName();
        (*m_pWeight)[k]++;
        m_config.GetUsageDb().StoreUsage(k, (*m_pWeight)[k]);
    }
}

void SmartCompletion::OnCodeCompletionShowing(clCodeCompletionEvent& event)
{
    event.Skip();
    if(!m_config.IsEnabled()) return;

    // Sort the entries by their weight
    wxCodeCompletionBoxEntry::Vec_t& entries = event.GetEntries();

    // We dont want to mess with the default sorting. We just want to place the onse with weight at the top
    // so we split the list into 2: entries with weight geater than 0 and 0
    wxCodeCompletionBoxEntry::Vec_t importantEntries;
    wxCodeCompletionBoxEntry::Vec_t normalEntries;
    wxCodeCompletionBoxEntry::Vec_t::iterator iter = entries.begin();
    for(; iter != entries.end(); ++iter) {
        wxCodeCompletionBoxEntry::Ptr_t entry = (*iter);
        if(entry->GetTag()) {
            wxString k = entry->GetTag()->GetScope() + "::" + entry->GetTag()->GetName();
            if(m_pWeight->count(k)) {
                entry->SetWeight((*m_pWeight)[k]);
                importantEntries.push_back(entry);
            } else {
                normalEntries.push_back(entry);
            }
        } else {
            normalEntries.push_back(entry);
        }
    }

    entries.swap(normalEntries);
    // Step 2: sort the important entries, based on their weight
    std::sort(importantEntries.begin(), importantEntries.end(),
              [&](wxCodeCompletionBoxEntry::Ptr_t a, wxCodeCompletionBoxEntry::Ptr_t b) {
                  // Sort in desecnding order
                  return a->GetWeight() > b->GetWeight();
              });

    // Step 3: prepend the important entries
    entries.insert(entries.begin(), importantEntries.begin(), importantEntries.end());
}

void SmartCompletion::OnSettings(wxCommandEvent& e)
{
    SmartCompletionsSettingsDlg dlg(EventNotifier::Get()->TopFrame(), m_config);
    dlg.ShowModal();
}
