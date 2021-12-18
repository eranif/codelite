#include "smartcompletion.h"
#include "SmartCompletionsSettingsDlg.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "macros.h"
#include <algorithm>
#include <queue>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

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
    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SORT_NEEDED, &SmartCompletion::OnGotoAnythingSort, this);
    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SELECTED, &SmartCompletion::OnGotoAnythingSelectionMade, this);
    m_config.Load();
    m_pCCWeight = &m_config.GetCCWeightTable();
    m_pGTAWeight = &m_config.GetGTAWeightTable();
}

SmartCompletion::~SmartCompletion() {}

void SmartCompletion::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void SmartCompletion::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("smart_completion_settings"), _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, _("SmartCompletions"), menu);

    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &SmartCompletion::OnSettings, this, XRCID("smart_completion_settings"));
}

void SmartCompletion::UnPlug()
{
    m_config.Save();
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SELECTION_MADE, &SmartCompletion::OnCodeCompletionSelectionMade, this);
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SHOWING, &SmartCompletion::OnCodeCompletionShowing, this);
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SORT_NEEDED, &SmartCompletion::OnGotoAnythingSort, this);
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SELECTED, &SmartCompletion::OnGotoAnythingSelectionMade, this);
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &SmartCompletion::OnSettings, this, XRCID("smart_completion_settings"));
}

void SmartCompletion::OnCodeCompletionSelectionMade(clCodeCompletionEvent& event)
{
    event.Skip();
    if(!m_config.IsEnabled())
        return;

    CHECK_PTR_RET(event.GetEntry());

    // Collect info about this match
    WeightTable_t& T = *m_pCCWeight;
    // we have an associated tag
    wxString k = event.GetEntry()->GetText();
    if(T.count(k) == 0) {
        T[k] = 1;
    } else {
        T[k]++;
    }
    m_config.GetUsageDb().StoreCCUsage(k, T[k]);
}

void SmartCompletion::OnCodeCompletionShowing(clCodeCompletionEvent& event)
{
    event.Skip();
    if(!m_config.IsEnabled())
        return;

    // Sort the entries by their weight
    wxCodeCompletionBoxEntry::Vec_t& entries = event.GetEntries();

    // We dont want to mess with the default sorting. We just want to place the onse with weight at the top
    // so we split the list into 2: entries with weight geater than 0 and 0
    wxCodeCompletionBoxEntry::Vec_t importantEntries;
    wxCodeCompletionBoxEntry::Vec_t normalEntries;
    wxCodeCompletionBoxEntry::Vec_t::iterator iter = entries.begin();
    for(; iter != entries.end(); ++iter) {
        wxCodeCompletionBoxEntry::Ptr_t entry = (*iter);
        wxString k = entry->GetText();
        if(m_pCCWeight->count(k)) {
            entry->SetWeight((*m_pCCWeight)[k]);
            importantEntries.push_back(entry);
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

void SmartCompletion::OnGotoAnythingSort(clGotoEvent& event)
{
    event.Skip();
    if(!m_config.IsEnabled())
        return;

    // Sort the entries by their weight
    clGotoEntry::Vec_t& entries = event.GetEntries();
    WeightTable_t& T = *m_pGTAWeight;
    // We dont want to mess with the default sorting. We just want to place the ones with weight at the top
    // so we split the list into 2: entries with weight geater than 0 and 0
    std::vector<std::pair<int, clGotoEntry>> importantEntries;
    clGotoEntry::Vec_t normalEntries;
    std::for_each(entries.begin(), entries.end(), [&](const clGotoEntry& entry) {
        if(T.count(entry.GetDesc())) {
            // This item has weight
            int weight = T[entry.GetDesc()];
            importantEntries.push_back({ weight, entry });
        } else {
            normalEntries.push_back(entry);
        }
    });

    // the list should now contains all the list *wihtout* weight
    entries.swap(normalEntries);

    // Step 2: sort the important entries - the sorting is DESC (lower first)
    std::sort(
        importantEntries.begin(), importantEntries.end(),
        [&](const std::pair<int, clGotoEntry>& a, const std::pair<int, clGotoEntry>& b) { return a.first < b.first; });

    // Step 3: prepend the important entries (it actually reverse the sorting)
    std::for_each(importantEntries.begin(), importantEntries.end(),
                  [&](const std::pair<int, clGotoEntry>& p) { entries.insert(entries.begin(), p.second); });
}

void SmartCompletion::OnGotoAnythingSelectionMade(clGotoEvent& event)
{
    event.Skip();
    if(!m_config.IsEnabled())
        return;

    // Collect info about this match
    WeightTable_t& T = *m_pGTAWeight;

    const wxString& key = event.GetEntry().GetDesc();
    if(T.count(key) == 0) {
        T[key] = 1;
    } else {
        T[key]++;
    }
    m_config.GetUsageDb().StoreGTAUsage(key, T[key]);
}
