#include "GTKNotebook.hpp"
#include <wx/wupdlock.h>

#ifdef __WXGTK__

#include "clTabRenderer.h"
#include "editor_config.h"
#include <gtk/gtk.h>
#include <iostream>
#include <unordered_map>
#include <wx/dataobj.h>
#include <wx/defs.h>
#include <wx/dnd.h>

//===------------------
// GTK specifics
//===------------------
class wxGtkNotebookPage : public wxObject
{
public:
    GtkWidget* m_box;
    GtkWidget* m_label;
    GtkWidget* m_image;
    int m_imageIndex;
};

static void on_action_button_clicked(GtkWidget* widget, clGTKNotebook* book)
{
    wxUnusedVar(widget);
    book->GTKActionButtonMenuClicked(GTK_TOOL_ITEM(widget));
}

static void on_action_button_new_clicked(GtkWidget* widget, clGTKNotebook* book)
{
    wxUnusedVar(widget);
    book->GTKActionButtonNewClicked(GTK_TOOL_ITEM(widget));
}

static void on_button_clicked(GtkWidget* widget, gpointer* data)
{
    wxUnusedVar(widget);
    wxWindow* page = reinterpret_cast<wxWindow*>(data);
    clGTKNotebook* book = dynamic_cast<clGTKNotebook*>(page->GetParent());
    book->TabButtonClicked(page);
}

static void on_page_reordered(clGTKNotebook* notebook, GtkWidget* child, guint page_num, gpointer user_data)
{
    wxUnusedVar(notebook);
    wxUnusedVar(child);
    wxUnusedVar(page_num);
    clGTKNotebook* wxbook = reinterpret_cast<clGTKNotebook*>(user_data);
    wxbook->TabReordered();
}

static gboolean button_press_event(GtkWidget* WXUNUSED_IN_GTK3(widget), GdkEventButton* gdk_event, clGTKNotebook* win)
{
    // check the mouse button clicked
    if(gdk_event->button == 1 && gdk_event->type == GDK_2BUTTON_PRESS) {
        // wxEVT_LEFT_DCLICK
        win->GTKLeftDClick();
        return true;
    } else if(gdk_event->button == 2 && gdk_event->type == GDK_BUTTON_PRESS) {
        // wxEVT_MIDDLE_DOWN
        win->GTKMiddleDown();
        return true;
    } else if(gdk_event->button == 3 && gdk_event->type == GDK_BUTTON_PRESS) {
        // wxEVT_RIGHT_DOWN
        win->GTKRightDown();
        return true;
    } else {
        return false;
    }
}

//===------------------
//===------------------

clGTKNotebook::clGTKNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                             const wxString& name)
    : wxNotebook(parent, id, pos, size, wxBK_DEFAULT, name)
{
    Initialise(style);
}

clGTKNotebook::~clGTKNotebook()
{
    wxDELETE(m_tabContextMenu); 
    Unbind(wxEVT_NOTEBOOK_PAGE_CHANGING, &clGTKNotebook::OnPageChanging, this);
    Unbind(wxEVT_NOTEBOOK_PAGE_CHANGED, &clGTKNotebook::OnPageChanged, this);
}

bool clGTKNotebook::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                           const wxString& name)
{
    Initialise(style);
    return wxNotebook::Create(parent, id, pos, size, wxBK_DEFAULT);
}

int clGTKNotebook::SetSelection(size_t nPage)
{
    int res = wxNotebook::SetSelection(nPage);
    m_history->Push(GetCurrentPage());
    return res;
}

int clGTKNotebook::ChangeSelection(size_t nPage)
{
    int res = wxNotebook::ChangeSelection(nPage);
    m_history->Push(GetCurrentPage());
    return res;
}

void clGTKNotebook::AddPage(wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp,
                            const wxString& shortLabel)
{
    if(!page) {
        return;
    }
    if(!page->IsShown()) {
        page->Show();
    }
    if(page->GetParent() != this) {
        page->Reparent(this);
    }
    if(!wxNotebook::InsertPage(GetPageCount(), page, label, selected, wxNOT_FOUND)) {
        return;
    }
    DoFinaliseAddPage(page, shortLabel, bmp);
}

bool clGTKNotebook::InsertPage(size_t index, wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp,
                               const wxString& shortLabel)
{
    if(!page) {
        return false;
    }
    if(page->GetParent() != this) {
        page->Reparent(this);
    }
    if(!page->IsShown()) {
        page->Show();
    }
    if(!wxNotebook::InsertPage(index, page, label, selected, wxNOT_FOUND)) {
        return false;
    }
    DoFinaliseAddPage(page, shortLabel, bmp);
    return true;
}

int clGTKNotebook::GetPageIndex(wxWindow* page) const
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        if(page == GetPage(i)) {
            return static_cast<int>(i);
        }
    }
    return wxNOT_FOUND;
}

void clGTKNotebook::BindEvents()
{
    Bind(wxEVT_NOTEBOOK_PAGE_CHANGING, &clGTKNotebook::OnPageChanging, this);
    Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &clGTKNotebook::OnPageChanged, this);

    g_signal_connect(GTK_WIDGET(GetHandle()), "button_press_event", G_CALLBACK(button_press_event), this);
    g_signal_connect(GTK_NOTEBOOK(GetHandle()), "page-reordered", G_CALLBACK(on_page_reordered), this);
}

void clGTKNotebook::GTKLeftDClick()
{
    long flags = 0;
    wxPoint pt = ScreenToClient(::wxGetMousePosition());
    int pos = HitTest(pt, &flags);
    if(pos != wxNOT_FOUND && (flags & (wxBK_HITTEST_ONICON | wxBK_HITTEST_ONLABEL))) {
        // Fire event
        wxBookCtrlEvent event(wxEVT_BOOK_TAB_DCLICKED);
        event.SetEventObject(this);
        event.SetSelection(pos);
        GetEventHandler()->AddPendingEvent(event);
    } else if(flags & wxBK_HITTEST_NOWHERE) {
        // we do nothing
    }
}

void clGTKNotebook::GTKMiddleDown()
{
    long flags = 0;
    wxPoint pt = ScreenToClient(::wxGetMousePosition());

    int pos = HitTest(pt, &flags);
    if(pos == wxNOT_FOUND) {
        return;
    }
    if(m_bookStyle & kNotebook_MouseMiddleClickClosesTab) {
        // Close the tab
        DeletePage(pos);
    } else if(m_bookStyle & kNotebook_CloseButtonOnActiveTabFireEvent) {
        // Fire event
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSE_BUTTON);
        event.SetEventObject(this);
        event.SetSelection(pos);
        GetEventHandler()->ProcessEvent(event);
    }
}

void clGTKNotebook::GTKRightDown()
{
    long flags = 0;
    wxPoint pt = ScreenToClient(::wxGetMousePosition());
    int where = HitTest(pt, &flags);
    if(where == wxNOT_FOUND) {
        return;
    }

    if(m_tabContextMenu) {
        PopupMenu(m_tabContextMenu);
    } else {
        // fire an event
        wxBookCtrlEvent menuEvent(wxEVT_BOOK_TAB_CONTEXT_MENU);
        menuEvent.SetEventObject(this);
        menuEvent.SetSelection(where);
        GetEventHandler()->ProcessEvent(menuEvent);
    }
}

bool clGTKNotebook::GetPageDetails(wxWindow* page, int& curindex, wxString& label, int& imageId) const
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        if(page == GetPage(i)) {
            curindex = i;
            label = GetPageText(i);
            imageId = GetPageImage(i);
            return true;
        }
    }
    return false;
}

int clGTKNotebook::FindPageByGTKHandle(WXWidget page) const
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        if(page == GetPage(i)->GetHandle()) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

void clGTKNotebook::OnPageChanged(wxBookCtrlEvent& e)
{
    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
    event.SetEventObject(this);
    event.SetSelection(GetSelection());
    event.SetOldSelection(e.GetOldSelection());
    GetEventHandler()->ProcessEvent(event);
    m_history->Push(GetCurrentPage());
}

void clGTKNotebook::OnPageChanging(wxBookCtrlEvent& e)
{
    // wrap this event with our own event
    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGING);
    event.SetEventObject(this);
    event.SetSelection(e.GetSelection());
    event.SetOldSelection(GetSelection());
    GetEventHandler()->ProcessEvent(event);
    if(!event.IsAllowed()) {
        // Vetoed by the user, veto the original event and return
        e.Veto();
        return;
    }
    // Allow changing
    e.Skip();
}

wxWindow* clGTKNotebook::DoUpdateHistoryPreRemove(wxWindow* page)
{
    bool deletingSelection = (page == GetCurrentPage());
    wxWindow* nextSelection = nullptr;
    if(deletingSelection) {
        while(!m_history->GetHistory().empty() && !nextSelection) {
            nextSelection = m_history->PrevPage();
            if(GetPageIndex(nextSelection) == wxNOT_FOUND) {
                // The history contains a tab that no longer exists
                m_history->Pop(nextSelection);
                nextSelection = NULL;
            }
        }
        // It is OK to end up with a null next selection, we will handle it later
    }
    return nextSelection;
}

void clGTKNotebook::DoUpdateHistoryPostRemove(wxWindow* page, bool deletedSelection)
{
    // Choose a new selection
    if(deletedSelection) {
        // Always make sure we have something to select...
        if(!page && GetPageCount()) {
            page = GetPage(0);
        }

        int nextSel = FindPage(page);
        if(nextSel != wxNOT_FOUND) {
            SetSelection(nextSel);
        }
    }
}

bool clGTKNotebook::RemovePage(size_t page, bool notify)
{
    if(GetPageCount() <= page) {
        return false;
    }
    wxWindow* win = GetPage(page);
    if(!win) {
        return false;
    }

    if(notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSING);
        event.SetEventObject(this);
        event.SetSelection(page);
        GetEventHandler()->ProcessEvent(event);
        if(!event.IsAllowed()) {
            // Vetoed
            return false;
        }
    }

    m_userData.erase(win);
    m_history->Pop(win);
    bool deletingSelection = (page == GetSelection());
    wxWindow* nextSelection = DoUpdateHistoryPreRemove(win);
    wxNotebook::RemovePage(page);

    // notify about removal
    if(notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }

    DoUpdateHistoryPostRemove(nextSelection, deletingSelection);
    return true;
}

bool clGTKNotebook::RemovePage(size_t page) { return RemovePage(page, false); }

bool clGTKNotebook::DeletePage(size_t page, bool notify)
{
    if(GetPageCount() <= page) {
        return false;
    }
    wxWindow* win = GetPage(page);
    if(!win) {
        return false;
    }

    // Can we close this page?
    if(notify) {
        wxBookCtrlEvent eventClosing(wxEVT_BOOK_PAGE_CLOSING);
        eventClosing.SetEventObject(this);
        eventClosing.SetSelection(page);
        GetEventHandler()->ProcessEvent(eventClosing);
        if(!eventClosing.IsAllowed()) {
            // Vetoed
            return false;
        }
    }

    bool deletingSelection = (page == GetSelection());
    wxWindow* nextSelection = DoUpdateHistoryPreRemove(win);

    m_history->Pop(win);
    wxNotebook::DeletePage(page);
    m_userData.erase(win);

    // notify about removal
    if(notify) {
        wxBookCtrlEvent eventClosed(wxEVT_BOOK_PAGE_CLOSED);
        eventClosed.SetEventObject(this);
        GetEventHandler()->ProcessEvent(eventClosed);
    }

    DoUpdateHistoryPostRemove(nextSelection, deletingSelection);
    return true;
}

bool clGTKNotebook::DeletePage(size_t page) { return DeletePage(page, true); }

void clGTKNotebook::TabButtonClicked(wxWindow* page)
{
    int where = GetPageIndex(page);
    if(where == wxNOT_FOUND) {
        return;
    }
    if(m_bookStyle & kNotebook_CloseButtonOnActiveTabFireEvent) {
        // just fire an event
        // let the user process this
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSE_BUTTON);
        event.SetEventObject(this);
        event.SetSelection(where);
        GetEventHandler()->ProcessEvent(event);
    } else {
        // Close the tab (the below will fire events)
        DeletePage(where);
    }
}

void clGTKNotebook::Initialise(long style)
{
    m_history.reset(new clTabHistory());
    m_bookStyle = (style & ~wxWINDOW_STYLE_MASK);
    if(!(m_bookStyle & kNotebook_CloseButtonOnActiveTab)) {
        SetPadding(wxSize(5, 5));
    }

    GtkWidget* box = nullptr;
    if(m_bookStyle & (kNotebook_NewButton | kNotebook_ShowFileListButton)) {
#if defined(__WXGTK20__) && !defined(__WXGTK3__)
        box = gtk_hbox_new(true, 2);
#else
        box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
#endif
    }

    if(m_bookStyle & kNotebook_ShowFileListButton) {
        GtkToolItem* button = gtk_tool_button_new(nullptr, "\u25BE");
        gtk_box_pack_end(GTK_BOX(box), GTK_WIDGET(button), false, true, 10);
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_action_button_clicked), this);
    }

    // Add the "+" button if needed
    if(m_bookStyle & kNotebook_NewButton) {
        GtkToolItem* button = gtk_tool_button_new(nullptr, "\uFF0B");
        gtk_box_pack_end(GTK_BOX(box), GTK_WIDGET(button), true, true, 2);
        gtk_widget_show_all(GTK_WIDGET(button));
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_action_button_new_clicked), this);
    }

    if(box) {
        gtk_notebook_set_action_widget(GTK_NOTEBOOK(GetHandle()), GTK_WIDGET(box), GTK_PACK_END);
        gtk_widget_show_all(box);
    }

    BindEvents();
}

void clGTKNotebook::DoFinaliseAddPage(wxWindow* page, const wxString& shortlabel, const wxBitmap& bmp)
{
    // do we need to add buton?
    int index = GetPageIndex(page);
    if(index == wxNOT_FOUND) {
        return;
    }
    m_history->Push(page);
    if(m_userData.count(page) == 0) {
        m_userData.insert({ page, {} });
    }
    auto& data = m_userData[page];
    data.tooltip = shortlabel;
    data.bitmap = bmp;

    wxGtkNotebookPage* p = GetNotebookPage(index);
    if(m_bookStyle & kNotebook_CloseButtonOnActiveTab) {
        GtkToolItem* button = gtk_tool_button_new(nullptr, "✖");

        // remove the label and insert it back at the start
        // add nother reference before removing it, since we plan on reusing it later
        g_object_ref(p->m_label);
        gtk_container_remove(GTK_CONTAINER(p->m_box), p->m_label);

        gtk_box_pack_start(GTK_BOX(p->m_box), p->m_label, TRUE, TRUE, 20);
        gtk_box_pack_end(GTK_BOX(p->m_box), GTK_WIDGET(button), FALSE, FALSE, 0);

        gtk_widget_show_all(p->m_box);

        // connect gtk signals
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_button_clicked), page);
    }

    if(m_bookStyle & kNotebook_AllowDnD) {
        // we allow tabs to be moved around
        gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(GetHandle()), GTK_WIDGET(page->GetHandle()), true);
    }

    // wxNotebook does not fire wxEVT_NOTEBOOK_PAGE_CHANGED
    // when we are inserting/adding the first page
    // force an event here
    if(GetPageCount() == 1) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
        event.SetEventObject(this);
        event.SetSelection(GetSelection());
        event.SetOldSelection(wxNOT_FOUND);
        GetEventHandler()->ProcessEvent(event);
    }
}

void clGTKNotebook::TabReordered()
{
    // we need to update two data structures:
    // m_pagesData & m_pages

    // build a map with the correct order of pages
    int pageCount = GetPageCount();
    GtkNotebook* gbook = GTK_NOTEBOOK(GetHandle());
    std::vector<wxWindow*> modifiedPages;
    std::vector<wxGtkNotebookPage*> modifiedPagesDataList;
    modifiedPages.resize(m_pages.size());
    modifiedPagesDataList.resize(m_pages.size());
    for(int i = 0; i < pageCount; ++i) {
        GtkWidget* page = gtk_notebook_get_nth_page(gbook, i);
        int curidx = FindPageByGTKHandle(page);
        modifiedPages[i] = GetPage(curidx);
        modifiedPagesDataList[i] = m_pagesData[curidx];
    }

    // update the lists
    m_pages.clear();
    for(wxWindow* win : modifiedPages) {
        m_pages.push_back(win);
    }
    m_pagesData.clear();
    for(size_t i = 0; i < modifiedPagesDataList.size(); ++i) {
        m_pagesData.push_back(modifiedPagesDataList[i]);
    }
}

void clGTKNotebook::SetTabDirection(wxDirection d)
{
    GtkPositionType pos;
    switch(d) {
    case wxLEFT:
        pos = GTK_POS_LEFT;
        break;
    case wxRIGHT:
        pos = GTK_POS_RIGHT;
        break;
    case wxBOTTOM:
        pos = GTK_POS_BOTTOM;
        break;
    case wxTOP:
        pos = GTK_POS_TOP;
        break;
    default:
        pos = GTK_POS_TOP;
        break;
    }
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(GetHandle()), pos);
}

void clGTKNotebook::EnableStyle(NotebookStyle style, bool enable)
{
    wxUnusedVar(style);
    wxUnusedVar(enable);
}

wxWindow* clGTKNotebook::GetCurrentPage() const
{
    if(GetSelection() == wxNOT_FOUND) {
        return nullptr;
    }
    return GetPage(GetSelection());
}

bool clGTKNotebook::DeleteAllPages()
{
    if(GetPageCount() == 0) {
        return true;
    }
    while(GetPageCount()) {
        DeletePage(0, false);
    }
    m_history->Clear();
    m_userData.clear();
    return true;
}

void clGTKNotebook::SetPageBitmap(size_t index, const wxBitmap& bmp)
{
    wxWindow* win = GetPage(index);
    if(m_userData.count(win) == 0) {
        return;
    }
    m_userData[win].bitmap = bmp;
}

wxBitmap clGTKNotebook::GetPageBitmap(size_t index) const
{
    auto iter = m_userData.find(GetPage(index));
    if(iter == m_userData.end()) {
        return wxNullBitmap;
    }
    return iter->second.bitmap;
}

int clGTKNotebook::GetPageIndex(const wxString& label) const
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        if(GetPageText(i) == label) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

void clGTKNotebook::GetAllPages(std::vector<wxWindow*>& pages)
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        pages.push_back(GetPage(i));
    }
}

bool clGTKNotebook::SetPageToolTip(size_t page, const wxString& tooltip)
{
    wxWindow* win = GetPage(page);
    if(m_userData.count(win) == 0) {
        return false;
    }
    m_userData[win].tooltip = tooltip;
    return true;
}

bool clGTKNotebook::MoveActivePage(int newIndex)
{
    wxWindow* curpage = GetCurrentPage();
    if(curpage == nullptr) {
        return false;
    }

    // avoid flickering
    wxWindowUpdateLocker locker(this);
    wxString label = GetPageText(GetSelection());
    RemovePage(GetSelection());
    InsertPage(newIndex, curpage, label, true);
    return true;
}

clTabHistory::Ptr_t clGTKNotebook::GetHistory() const
{
    // return the history
    return m_history;
}

size_t clGTKNotebook::GetAllTabs(clTabInfo::Vec_t& tabs)
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        clTabInfo::Ptr_t info(new clTabInfo(nullptr, 0, GetPage(i), GetPageText(i), GetPageBitmap(i)));
        tabs.push_back(info);
    }
    return tabs.size();
}

void clGTKNotebook::GTKActionButtonMenuClicked(GtkToolItem* button)
{
    clTabInfo::Vec_t tabs;
    GetAllTabs(tabs);

    // Do we have pages opened?
    if(GetPageCount() == 0) {
        return;
    }

    const int curselection = GetSelection();
    wxMenu menu;
    const int firstTabPageID = 13457;
    int pageMenuID = firstTabPageID;

    // Optionally make a sorted view of tabs.
    std::vector<size_t> sortedIndexes;
    if(GetPageCount()) {
        sortedIndexes.resize(GetPageCount());
        {
            // std is C++11 at the moment, so no generalized capture.
            size_t index = 0;
            std::generate(sortedIndexes.begin(), sortedIndexes.end(), [&index]() { return index++; });
        }
        if(EditorConfigST::Get()->GetOptions()->IsSortTabsDropdownAlphabetically()) {
            std::sort(sortedIndexes.begin(), sortedIndexes.end(),
                      [&](size_t i1, size_t i2) { return tabs[i1]->GetLabel().CmpNoCase(tabs[i2]->GetLabel()) < 0; });
        }

        for(auto sortedIndex : sortedIndexes) {
            clTabInfo::Ptr_t tab = tabs[sortedIndex];
            wxWindow* pWindow = tab->GetWindow();
            wxString label = tab->GetLabel();
            wxMenuItem* item = new wxMenuItem(&menu, pageMenuID, label, "", wxITEM_CHECK);
            menu.Append(item);
            item->Check(tab->IsActive());
            menu.Bind(
                wxEVT_MENU,
                [=](wxCommandEvent& event) {
                    int newSelection = GetPageIndex(pWindow);
                    if(newSelection != curselection) {
                        SetSelection(newSelection);
                    }
                },
                pageMenuID);
            pageMenuID++;
        }
    }

    // Let others handle this event as well
    clContextMenuEvent menuEvent(wxEVT_BOOK_FILELIST_BUTTON_CLICKED);
    menuEvent.SetMenu(&menu);
    menuEvent.SetEventObject(this); // The clGTKNotebook
    GetEventHandler()->ProcessEvent(menuEvent);

    wxPoint pt(wxNOT_FOUND, wxNOT_FOUND);
    int width, height;
    GtkRequisition req;
#if defined(__WXGTK20__) && !defined(__WXGTK3__)
    gtk_widget_size_request(GTK_WIDGET(button), &req);
#else
    gtk_widget_get_preferred_size(GTK_WIDGET(button), NULL, &req);
#endif
    width = req.width;
    height = req.height;

    wxWindow* curpage = GetCurrentPage();
    if(curpage) {
        GtkPositionType pos = gtk_notebook_get_tab_pos(GTK_NOTEBOOK(GetHandle()));
        switch(pos) {
        case GTK_POS_BOTTOM:
            pt = curpage->GetRect().GetBottomRight();
            pt.y += height;
            pt.x -= width;
            break;
        case GTK_POS_LEFT:
            pt = curpage->GetRect().GetBottomLeft();
            pt.x -= width;
            break;
        case GTK_POS_RIGHT:
            pt = curpage->GetRect().GetBottomRight();
            break;
        case GTK_POS_TOP:
            pt = curpage->GetRect().GetTopRight();
            pt.x -= width;
            break;
        }
    }

    if(pt.x != wxNOT_FOUND) {
        PopupMenu(&menu, pt);
    } else {
        PopupMenu(&menu);
    }
}

void clGTKNotebook::GTKActionButtonNewClicked(GtkToolItem* button)
{
    wxUnusedVar(button);
    wxBookCtrlEvent event(wxEVT_BOOK_NEW_PAGE);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

#endif // __WXGTK3__
