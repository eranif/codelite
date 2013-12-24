#include "bookmark_manager.h"
#include "event_notifier.h"
#include "plugin.h"
#include "editor_config.h"

BookmarkManager::BookmarkManager()
    : m_activeBookmarkType(smt_bookmark1)
{
    for(int i=smt_FIRST_BMK_TYPE; i<=smt_LAST_BMK_TYPE; ++i) {
        wxString label;
        switch(i) {
        case smt_bookmark1:
            label << _("Normal bookmark");
            break;
        case smt_find_bookmark:
            label << _("Find bookmark");
            break;
        default:
            label << "Bookmark Type " << (i-smt_FIRST_BMK_TYPE);
            break;
        }
        m_markerLabels.insert( std::make_pair(i, label) );
    }
    EventNotifier::Get()->Connect(wxEVT_EDITOR_SETTINGS_CHANGED, wxCommandEventHandler(BookmarkManager::OnEditorSettingsChanged), NULL, this);
}

BookmarkManager::~BookmarkManager()
{
    EventNotifier::Get()->Disconnect(wxEVT_EDITOR_SETTINGS_CHANGED, wxCommandEventHandler(BookmarkManager::OnEditorSettingsChanged), NULL, this);
}

BookmarkManager& BookmarkManager::Get()
{
    static BookmarkManager mgr;
    return mgr;
}

void BookmarkManager::SetMarkerLabel(const wxString& label, int index)
{
    if ( m_markerLabels.count(index) ) {
        m_markerLabels[index] = label;
    }
}

void BookmarkManager::OnEditorSettingsChanged(wxCommandEvent& e)
{
    m_markerLabels.clear();
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    for(int i=smt_FIRST_BMK_TYPE; i<=smt_LAST_BMK_TYPE; ++i) {
        m_markerLabels.insert( std::make_pair(i,  options->GetBookmarkLabel(i-smt_FIRST_BMK_TYPE)) );
    }
}

wxString BookmarkManager::GetMarkerLabel(int index) const
{
    if ( m_markerLabels.count(index) ) {
        return m_markerLabels.find(index)->second;
    }
    return wxEmptyString;
}

wxMenu* BookmarkManager::CreateBookmarksSubmenu(wxMenu* parentMenu)
{
    wxMenu* menu = new wxMenu;
    menu->Append(XRCID("next_bookmark"), _("Next Bookmark"));
    menu->Append(XRCID("previous_bookmark"), _("Previous Bookmark"));
    menu->AppendSeparator();

    wxMenu* submenu = new wxMenu; // For the individual BM types
    static int bmktypes = smt_LAST_BMK_TYPE - smt_FIRST_BMK_TYPE + 1;

    int current = BookmarkManager::Get().GetActiveBookmarkType();
    wxCHECK_MSG(current >= smt_FIRST_BMK_TYPE && current < smt_find_bookmark, menu, "Out-of-range standard bookmarktype");
    
    for (int bmt=1; bmt < bmktypes; ++bmt) { // Not <= as we don't want smt_find_bookmark here
        wxMenuItem* item = submenu->AppendRadioItem(XRCID("BookmarkTypes[start]") + bmt, GetMarkerLabel((sci_marker_types)(smt_FIRST_BMK_TYPE + bmt-1)));
        if (bmt == (current - smt_FIRST_BMK_TYPE + 1)) {
            item->Check();
        }
    }

    wxMenuItem* item = new wxMenuItem(menu, XRCID("change_active_bookmark_type"), _("Change Active Bookmark Type..."), "", wxITEM_NORMAL, submenu);
    menu->Append(item);

    menu->AppendSeparator();
    menu->Append(XRCID("removeall_current_bookmarks"), _("Remove All Currently-Active Bookmarks"));

    if (parentMenu) {
        item = new wxMenuItem(parentMenu, XRCID("more_bookmark_options"), _("More..."), "", wxITEM_NORMAL, menu);
        parentMenu->Append(item);
    }
    
    return menu;
}
