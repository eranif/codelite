#include "clDataViewListCtrl.h"
#include "clHeaderItem.h"
#include <algorithm>
#include <wx/dataview.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

#define DV_ITEM(tree_item) wxDataViewItem(tree_item.GetID())
#define TREE_ITEM(dv_item) wxTreeItemId(dv_item.GetID())

wxIMPLEMENT_DYNAMIC_CLASS(clDataViewTextBitmap, wxObject);
IMPLEMENT_VARIANT_OBJECT_EXPORTED(clDataViewTextBitmap, WXDLLIMPEXP_SDK);

wxIMPLEMENT_DYNAMIC_CLASS(clDataViewCheckbox, wxObject);
IMPLEMENT_VARIANT_OBJECT_EXPORTED(clDataViewCheckbox, WXDLLIMPEXP_SDK);

wxIMPLEMENT_DYNAMIC_CLASS(clDataViewChoice, wxObject);
IMPLEMENT_VARIANT_OBJECT_EXPORTED(clDataViewChoice, WXDLLIMPEXP_SDK);

wxDEFINE_EVENT(wxEVT_DATAVIEW_SEARCH_TEXT, wxDataViewEvent);
wxDEFINE_EVENT(wxEVT_DATAVIEW_CLEAR_SEARCH, wxDataViewEvent);
wxDEFINE_EVENT(wxEVT_DATAVIEW_CHOICE_BUTTON, wxDataViewEvent);
wxDEFINE_EVENT(wxEVT_DATAVIEW_CHOICE, wxDataViewEvent);

std::unordered_map<int, int> clDataViewListCtrl::m_stylesMap;
clDataViewListCtrl::clDataViewListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                       long style)
    : clTreeCtrl(parent, id, pos, size, style)
{
    // Map clDataViewListCtrl to clTreeCtrl style
    SetShowHeader(true);
    if(m_stylesMap.empty()) {
        m_stylesMap.insert({ wxDV_ROW_LINES, wxTR_ROW_LINES });
        m_stylesMap.insert({ wxDV_MULTIPLE, wxTR_MULTIPLE });
        m_stylesMap.insert({ wxDV_ENABLE_SEARCH, wxTR_ENABLE_SEARCH });
    }

    int my_style = 0;
    if(style & wxDV_ROW_LINES) { my_style |= wxTR_ROW_LINES; }
    if(style & wxDV_MULTIPLE) { my_style |= wxTR_MULTIPLE; }
    if(style & wxDV_NO_HEADER) { SetShowHeader(false); }
    my_style |= wxTR_HIDE_ROOT;
    m_treeStyle = my_style;

    // Ignore these events
    Bind(wxEVT_TREE_ITEM_EXPANDING, [](wxTreeEvent& e) { wxUnusedVar(e); });
    Bind(wxEVT_TREE_ITEM_EXPANDED, [](wxTreeEvent& e) { wxUnusedVar(e); });
    Bind(wxEVT_TREE_ITEM_COLLAPSING, [](wxTreeEvent& e) { wxUnusedVar(e); });
    Bind(wxEVT_TREE_ITEM_COLLAPSED, [](wxTreeEvent& e) { wxUnusedVar(e); });
    Bind(wxEVT_TREE_DELETE_ITEM, [](wxTreeEvent& e) { wxUnusedVar(e); });
    Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, [](wxTreeEvent& e) { e.Skip(); });

    // Translate the following events to wxDVC events
    Bind(wxEVT_TREE_ITEM_VALUE_CHANGED, &clDataViewListCtrl::OnConvertEvent, this);
    Bind(wxEVT_TREE_CHOICE, &clDataViewListCtrl::OnConvertEvent, this);
    Bind(wxEVT_TREE_BEGIN_DRAG, &clDataViewListCtrl::OnConvertEvent, this);
    Bind(wxEVT_TREE_END_DRAG, &clDataViewListCtrl::OnConvertEvent, this);
    Bind(wxEVT_TREE_SEL_CHANGED, &clDataViewListCtrl::OnConvertEvent, this);
    Bind(wxEVT_TREE_ITEM_ACTIVATED, &clDataViewListCtrl::OnConvertEvent, this);
    Bind(wxEVT_TREE_ITEM_MENU, &clDataViewListCtrl::OnConvertEvent, this);
    Bind(wxEVT_TREE_SEARCH_TEXT, &clDataViewListCtrl::OnConvertEvent, this);
    Bind(wxEVT_TREE_CLEAR_SEARCH, &clDataViewListCtrl::OnConvertEvent, this);

    AddRoot("Hidden Root", -1, -1, nullptr);
}

clDataViewListCtrl::~clDataViewListCtrl()
{
    Unbind(wxEVT_TREE_BEGIN_DRAG, &clDataViewListCtrl::OnConvertEvent, this);
    Unbind(wxEVT_TREE_CHOICE, &clDataViewListCtrl::OnConvertEvent, this);
    Unbind(wxEVT_TREE_END_DRAG, &clDataViewListCtrl::OnConvertEvent, this);
    Unbind(wxEVT_TREE_SEL_CHANGED, &clDataViewListCtrl::OnConvertEvent, this);
    Unbind(wxEVT_TREE_ITEM_ACTIVATED, &clDataViewListCtrl::OnConvertEvent, this);
    Unbind(wxEVT_TREE_ITEM_MENU, &clDataViewListCtrl::OnConvertEvent, this);
    Unbind(wxEVT_TREE_ITEM_VALUE_CHANGED, &clDataViewListCtrl::OnConvertEvent, this);
}

void clDataViewListCtrl::AppendItem(const wxVector<wxVariant>& values, wxUIntPtr data)
{
    wxTreeItemId item = clTreeCtrl::AppendItem(GetRootItem(), "", -1, -1, nullptr);
    clRowEntry* child = m_model.ToPtr(item);
    // mark this row as a "list-view" row (i.e. it can't have children)
    child->SetListItem(true);
    child->SetData(data);
    for(size_t i = 0; i < values.size(); ++i) {
        const wxVariant& v = values[i];
        DoSetCellValue(child, i, v);
    }
    UpdateScrollBar();
}

wxDataViewColumn* clDataViewListCtrl::AppendIconTextColumn(const wxString& label, wxDataViewCellMode mode, int width,
                                                           wxAlignment align, int flags)
{
    wxUnusedVar(mode);
    wxUnusedVar(align);
    wxUnusedVar(flags);
    AddHeader(label, wxNullBitmap, width);
    return nullptr;
}

wxDataViewColumn* clDataViewListCtrl::AppendProgressColumn(const wxString& label, wxDataViewCellMode mode, int width,
                                                           wxAlignment align, int flags)
{
    wxUnusedVar(mode);
    wxUnusedVar(align);
    wxUnusedVar(flags);
    AddHeader(label, wxNullBitmap, width);
    return nullptr;
}

wxDataViewColumn* clDataViewListCtrl::AppendTextColumn(const wxString& label, wxDataViewCellMode mode, int width,
                                                       wxAlignment align, int flags)
{
    wxUnusedVar(mode);
    wxUnusedVar(align);
    wxUnusedVar(flags);
    AddHeader(label, wxNullBitmap, width);
    return nullptr;
}

int clDataViewListCtrl::GetSelections(wxDataViewItemArray& sel) const
{
    wxArrayTreeItemIds items;
    clTreeCtrl::GetSelections(items);
    for(size_t i = 0; i < items.size(); ++i) {
        sel.Add(DV_ITEM(items[i]));
    }
    return sel.size();
}

int clDataViewListCtrl::GetSelectedItemsCount() const { return m_model.GetSelectionsCount(); }

void clDataViewListCtrl::OnConvertEvent(wxTreeEvent& event)
{
    wxEventType type = wxEVT_ANY;
    wxString eventText;
    if(event.GetEventType() == wxEVT_TREE_BEGIN_DRAG) {
        type = wxEVT_DATAVIEW_ITEM_BEGIN_DRAG;
    } else if(event.GetEventType() == wxEVT_TREE_END_DRAG) {
        type = wxEVT_DATAVIEW_ITEM_DROP;
    } else if(event.GetEventType() == wxEVT_TREE_SEL_CHANGED) {
        type = wxEVT_DATAVIEW_SELECTION_CHANGED;
    } else if(event.GetEventType() == wxEVT_TREE_ITEM_ACTIVATED) {
        type = wxEVT_DATAVIEW_ITEM_ACTIVATED;
    } else if(event.GetEventType() == wxEVT_TREE_ITEM_MENU) {
        type = wxEVT_DATAVIEW_ITEM_CONTEXT_MENU;
    } else if(event.GetEventType() == wxEVT_TREE_SEARCH_TEXT) {
        type = wxEVT_DATAVIEW_SEARCH_TEXT;
        eventText = event.GetString();
    } else if(event.GetEventType() == wxEVT_TREE_CLEAR_SEARCH) {
        type = wxEVT_DATAVIEW_CLEAR_SEARCH;
    } else if(event.GetEventType() == wxEVT_TREE_ITEM_VALUE_CHANGED) {
        type = wxEVT_DATAVIEW_ITEM_VALUE_CHANGED;
    } else if(event.GetEventType() == wxEVT_TREE_CHOICE) {
        type = wxEVT_DATAVIEW_CHOICE_BUTTON;
    }
    if(type != wxEVT_ANY) { SendDataViewEvent(type, event, eventText); }
}

bool clDataViewListCtrl::SendDataViewEvent(const wxEventType& type, wxTreeEvent& treeEvent, const wxString& text)
{
#if wxCHECK_VERSION(3, 1, 0)
    wxDataViewEvent e(type, &m_dummy, DV_ITEM(treeEvent.GetItem()));
#else
    wxDataViewEvent e(type);
    e.SetItem(DV_ITEM(treeEvent.GetItem()));
#endif
    e.SetEventObject(this);
    e.SetColumn(treeEvent.GetInt());
    e.SetString(text);
    if(!GetEventHandler()->ProcessEvent(e)) {
        treeEvent.Skip();
        return false;
    }
    return true;
}

void clDataViewListCtrl::DeleteAllItems(const std::function<void(wxUIntPtr)>& deleterFunc)
{
    // If a deleter was provided, call it per user's item data
    if(deleterFunc && m_model.GetRoot()) {
        clRowEntry::Vec_t& children = m_model.GetRoot()->GetChildren();
        for(size_t i = 0; i < children.size(); ++i) {
            wxUIntPtr userData = children[i]->GetData();
            if(userData) { deleterFunc(userData); }
            children[i]->SetData(0);
        }
    }
    clTreeCtrl::DeleteAllItems();
    // DVC must always have the hidden root
    AddRoot("Hidden Root", -1, -1, nullptr);
}

wxDataViewItem clDataViewListCtrl::AppendItem(const wxString& text, int image, int selImage, wxUIntPtr data)
{
    wxTreeItemId child = clTreeCtrl::AppendItem(GetRootItem(), text, image, selImage, nullptr);
    // mark this row as a "list-view" row (i.e. it can't have children)
    m_model.ToPtr(child)->SetListItem(true);
    wxDataViewItem dvItem = DV_ITEM(child);
    SetItemData(dvItem, data);
    UpdateScrollBar();
    return dvItem;
}

wxDataViewItem clDataViewListCtrl::InsertItem(const wxDataViewItem& previous, const wxString& text, int image,
                                              int selImage, wxUIntPtr data)
{
    wxTreeItemId child =
        clTreeCtrl::InsertItem(GetRootItem(), wxTreeItemId(previous.GetID()), text, image, selImage, nullptr);
    // mark this row as a "list-view" row (i.e. it can't have children)
    m_model.ToPtr(child)->SetListItem(true);
    wxDataViewItem dvItem = DV_ITEM(child);
    SetItemData(dvItem, data);
    return dvItem;
}

wxUIntPtr clDataViewListCtrl::GetItemData(const wxDataViewItem& item) const
{
    clRowEntry* r = m_model.ToPtr(TREE_ITEM(item));
    return r->GetData();
}

void clDataViewListCtrl::SetItemData(const wxDataViewItem& item, wxUIntPtr data)
{
    clRowEntry* r = m_model.ToPtr(TREE_ITEM(item));
    r->SetData(data);
}

void clDataViewListCtrl::SetItemBackgroundColour(const wxDataViewItem& item, const wxColour& colour, size_t col)
{
    clTreeCtrl::SetItemBackgroundColour(TREE_ITEM(item), colour, col);
}

wxColour clDataViewListCtrl::GetItemBackgroundColour(const wxDataViewItem& item, size_t col) const
{
    return clTreeCtrl::GetItemBackgroundColour(TREE_ITEM(item), col);
}

void clDataViewListCtrl::SetItemTextColour(const wxDataViewItem& item, const wxColour& colour, size_t col)
{
    clTreeCtrl::SetItemTextColour(TREE_ITEM(item), colour, col);
}

wxColour clDataViewListCtrl::GetItemTextColour(const wxDataViewItem& item, size_t col) const
{
    return clTreeCtrl::GetItemTextColour(TREE_ITEM(item), col);
}

void clDataViewListCtrl::SetItemText(const wxDataViewItem& item, const wxString& text, size_t col)
{
    clTreeCtrl::SetItemText(TREE_ITEM(item), text, col);
}

wxString clDataViewListCtrl::GetItemText(const wxDataViewItem& item, size_t col) const
{
    return clTreeCtrl::GetItemText(TREE_ITEM(item), col);
}

void clDataViewListCtrl::SetItemBold(const wxDataViewItem& item, bool bold, size_t col)
{
    clTreeCtrl::SetItemBold(TREE_ITEM(item), bold, col);
}

void clDataViewListCtrl::SetItemFont(const wxDataViewItem& item, const wxFont& font, size_t col)
{
    clTreeCtrl::SetItemFont(TREE_ITEM(item), font, col);
}

wxFont clDataViewListCtrl::GetItemFont(const wxDataViewItem& item, size_t col) const
{
    return clTreeCtrl::GetItemFont(TREE_ITEM(item), col);
}

void clDataViewListCtrl::EnableStyle(int style, bool enable, bool refresh)
{
    if(m_stylesMap.count(style) == 0) { return; }
    clTreeCtrl::EnableStyle(m_stylesMap[style], enable, refresh);
}

clHeaderItem* clDataViewListCtrl::GetColumn(size_t index)
{
    if(index >= GetHeader()->size()) { return nullptr; }
    return &GetHeader()->Item(index);
}

size_t clDataViewListCtrl::GetItemCount() const
{
    clRowEntry* root = m_model.GetRoot();
    if(!root) { return 0; }
    return root->GetChildrenCount(false);
}

wxDataViewItem clDataViewListCtrl::RowToItem(size_t row) const
{
    // Since a clDataViewListCtrl is basically a tree with a single hidden node (the root)
    // A row is simply a child at a given index
    clRowEntry* root = m_model.GetRoot();
    if(!root) { return wxDataViewItem(); }
    if(row >= root->GetChildren().size()) { return wxDataViewItem(); }
    return wxDataViewItem(root->GetChildren()[row]);
}

void clDataViewListCtrl::DeleteItem(size_t row)
{
    wxDataViewItem item = RowToItem(row);
    if(!item.IsOk()) { return; }
    Delete(TREE_ITEM(item));
}

void clDataViewListCtrl::SetValue(const wxVariant& value, size_t row, size_t col)
{
    wxDataViewItem item = RowToItem(row);
    if(!item.IsOk()) { return; }
    clRowEntry* r = m_model.ToPtr(TREE_ITEM(item));
    DoSetCellValue(r, col, value);
}

void clDataViewListCtrl::DoSetCellValue(clRowEntry* row, size_t col, const wxVariant& value)
{
    wxString variantType = value.GetType();
    if(variantType == "bool") {
        row->SetChecked(value.GetBool(), wxNOT_FOUND, wxString(), col);
    } else if(variantType == "string") {
        row->SetLabel(value.GetString(), col);
    } else if(variantType == "clDataViewCheckbox") {
        clDataViewCheckbox check;
        check << value;
        row->SetChecked(check.IsChecked(), check.GetBitmapIndex(), check.GetText(), col);
    } else if(variantType == "wxDataViewIconText") {
        // Extract the iamge + text from the wxDataViewIconText class
        wxDataViewIconText iconText;
        iconText << value;
        //  update the row with the icon + text
        row->SetLabel(iconText.GetText(), col);
    } else if(variantType == "clDataViewTextBitmap") {
        // Extract the iamge + text from the wxDataViewIconText class
        clDataViewTextBitmap iconText;
        iconText << value;
        //  update the row with the icon + text
        row->SetLabel(iconText.GetText(), col);
        row->SetBitmapIndex(iconText.GetBitmapIndex(), col);
    } else if(variantType == "clDataViewChoice") {
        clDataViewChoice choice;
        choice << value;
        row->SetChoice(true, col);
        row->SetBitmapIndex(choice.GetBitmapIndex(), col);
        row->SetLabel(choice.GetLabel(), col);
    } else if(variantType == "double") {
        row->SetLabel(wxString() << value.GetDouble(), col);
    } else if(variantType == "datetime") {
        row->SetLabel(value.GetDateTime().FormatDate(), col);
    }
    // Call this to update the view + update the header bar
    clTreeCtrl::SetItemText(wxTreeItemId(row), row->GetLabel(col), col);
}

void clDataViewListCtrl::SetSortFunction(const clSortFunc_t& CompareFunc)
{
    clRowEntry* root = m_model.GetRoot();
    if(!root) { return; }

    // Disconnect the current function, if any
    m_model.SetSortFunction(nullptr);
    if(!CompareFunc) {
        // we are done
        return;
    }

    // This list ctrl is composed of a hidden root + its children
    // Step 1:
    clRowEntry::Vec_t& children = root->GetChildren();
    for(size_t i = 0; i < children.size(); ++i) {
        clRowEntry* child = children[i];
        child->SetNext(nullptr);
        child->SetPrev(nullptr);
    }

    // Step 2: disconect the root
    root->SetNext(nullptr);

    // Step 3: sort the children
    std::sort(children.begin(), children.end(), CompareFunc);

    // Now, reconnect the children, starting with the root
    clRowEntry* prev = root;
    for(size_t i = 0; i < children.size(); ++i) {
        clRowEntry* child = children[i];
        prev->SetNext(child);
        child->SetPrev(prev);
        prev = child;
    }

    // and store the new sorting method
    m_model.SetSortFunction(CompareFunc);

    Refresh();
}

int clDataViewListCtrl::ItemToRow(const wxDataViewItem& item) const
{
    clRowEntry* pItem = m_model.ToPtr(TREE_ITEM(item));
    if(!pItem) { return wxNOT_FOUND; }

    clRowEntry* root = m_model.GetRoot();
    if(!root) { return wxNOT_FOUND; }

    const clRowEntry::Vec_t& children = root->GetChildren();
    for(size_t i = 0; i < children.size(); ++i) {
        if(children[i] == pItem) { return i; }
    }
    return wxNOT_FOUND;
}

void clDataViewListCtrl::Select(const wxDataViewItem& item)
{
    if(HasStyle(wxTR_MULTIPLE)) {
        m_model.SelectItem(TREE_ITEM(item), true, true, false);
    } else {
        clTreeCtrl::SelectItem(TREE_ITEM(item), true);
    }
}

wxDataViewItem clDataViewListCtrl::FindNext(const wxDataViewItem& from, const wxString& what, size_t col,
                                            size_t searchFlags)
{
    return DV_ITEM(clTreeCtrl::FindNext(TREE_ITEM(from), what, col, searchFlags));
}

wxDataViewItem clDataViewListCtrl::FindPrev(const wxDataViewItem& from, const wxString& what, size_t col,
                                            size_t searchFlags)
{
    return DV_ITEM(clTreeCtrl::FindPrev(TREE_ITEM(from), what, col, searchFlags));
}

void clDataViewListCtrl::HighlightText(const wxDataViewItem& item, bool b)
{
    clTreeCtrl::HighlightText(TREE_ITEM(item), b);
}

void clDataViewListCtrl::ClearHighlight(const wxDataViewItem& item) { clTreeCtrl::ClearHighlight(TREE_ITEM(item)); }

void clDataViewListCtrl::EnsureVisible(const wxDataViewItem& item) { clTreeCtrl::EnsureVisible(TREE_ITEM(item)); }

void clDataViewListCtrl::ClearColumns() { GetHeader()->Clear(); }

void clDataViewListCtrl::SetItemChecked(const wxDataViewItem& item, bool checked, size_t col)
{
    clTreeCtrl::Check(TREE_ITEM(item), checked, col);
}

bool clDataViewListCtrl::IsItemChecked(const wxDataViewItem& item, size_t col) const
{
    return clTreeCtrl::IsChecked(TREE_ITEM(item), col);
}

void clDataViewListCtrl::ShowMenuForItem(const wxDataViewItem& item, wxMenu& menu, size_t col)
{
    clRowEntry* row = m_model.ToPtr(TREE_ITEM(item));
    if(!row) { return; }

    wxRect r = row->GetCellRect(col);
    PopupMenu(&menu, r.GetBottomLeft());
}

void clDataViewListCtrl::ShowStringSelectionMenu(const wxDataViewItem& item, const wxArrayString& choices, size_t col)
{
    clRowEntry* row = m_model.ToPtr(TREE_ITEM(item));
    if(!row) { return; }
    const wxString& currentSelection = row->GetLabel(col);
    wxMenu menu;
    wxString selectedString;
    std::unordered_map<int, wxString> M;
    for(const wxString& str : choices) {
        int id = wxXmlResource::GetXRCID(str);
        wxMenuItem* item = menu.Append(id, str, str, wxITEM_CHECK);
        item->Check(currentSelection == str);
        M.insert({ id, str });
    }
    menu.Bind(wxEVT_MENU,
              [&](wxCommandEvent& event) {
                  if(M.count(event.GetId())) { selectedString = M[event.GetId()]; }
              },
              wxID_ANY);
    wxRect r = row->GetCellRect(col);
    PopupMenu(&menu, r.GetBottomLeft());
    if(!selectedString.IsEmpty()) {
        // fire selection made event
#if wxCHECK_VERSION(3, 1, 0)
        wxDataViewEvent e(wxEVT_DATAVIEW_CHOICE, &m_dummy, item);
#else
        wxDataViewEvent e(wxEVT_DATAVIEW_CHOICE);
        e.SetItem(item);
#endif
        e.SetEventObject(this);
        e.SetColumn(col);
        e.SetString(selectedString);
        e.Allow(); // by default allow
        GetEventHandler()->ProcessEvent(e);
        if(e.IsAllowed()) {
            SetItemText(item, selectedString, col);
        }
    }
}
