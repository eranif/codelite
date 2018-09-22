#include "clScrollBar.h"
#include "clTreeCtrl.h"
#include "clTreeCtrlModel.h"
#include "clTreeNodeVisitor.h"
#include <algorithm>
#include <cmath>
#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/dnd.h>
#include <wx/log.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/utils.h>
#include <wx/wupdlock.h>

#define CHECK_PTR_RET(p) \
    if(!p) { return; }

#define CHECK_ITEM_RET(item) \
    if(!item.IsOk()) { return; }

#define CHECK_ITEM_RET_INVALID_ITEM(item) \
    if(!item.IsOk()) { return wxTreeItemId(); }

#define CHECK_ITEM_RET_FALSE(item) \
    if(!item.IsOk()) { return false; }

#define CHECK_ROOT_RET() \
    if(!m_model.GetRoot()) { return; }

clTreeCtrl::clTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clControlWithItems(parent, wxID_ANY, pos, size, wxWANTS_CHARS)
    , m_model(this)
{
    m_treeStyle = style;
    DoInitialize();
}

clTreeCtrl::clTreeCtrl()
    : m_model(this)
{
}

bool clTreeCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    m_treeStyle = style;
    if(!clControlWithItems::Create(parent, id, pos, size, wxWANTS_CHARS)) { return false; }
    DoInitialize();
    return true;
}

void clTreeCtrl::DoInitialize()
{
    wxSize textSize = GetTextSize("Tp");
    SetLineHeight(clRowEntry::Y_SPACER + textSize.GetHeight() + clRowEntry::Y_SPACER);
    SetIndent(GetLineHeight());

    Bind(wxEVT_IDLE, &clTreeCtrl::OnIdle, this);
    Bind(wxEVT_PAINT, &clTreeCtrl::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent& event) { wxUnusedVar(event); });
    Bind(wxEVT_LEFT_DOWN, &clTreeCtrl::OnMouseLeftDown, this);
    Bind(wxEVT_LEFT_UP, &clTreeCtrl::OnMouseLeftUp, this);
    Bind(wxEVT_LEFT_DCLICK, &clTreeCtrl::OnMouseLeftDClick, this);
    Bind(wxEVT_MOUSEWHEEL, &clTreeCtrl::OnMouseScroll, this);
    Bind(wxEVT_LEAVE_WINDOW, &clTreeCtrl::OnLeaveWindow, this);
    Bind(wxEVT_ENTER_WINDOW, &clTreeCtrl::OnEnterWindow, this);
    Bind(wxEVT_CONTEXT_MENU, &clTreeCtrl::OnContextMenu, this);
    Bind(wxEVT_RIGHT_DOWN, &clTreeCtrl::OnRightDown, this);

    // Initialise default colours
    GetColours().InitDefaults();

    // There is always a header
    clHeaderBar header(this);
    header.Add("");
    SetHeader(header);
    SetShowHeader(false);
}

clTreeCtrl::~clTreeCtrl()
{
    Unbind(wxEVT_IDLE, &clTreeCtrl::OnIdle, this);
    Unbind(wxEVT_PAINT, &clTreeCtrl::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent& event) { wxUnusedVar(event); });
    Unbind(wxEVT_LEFT_DOWN, &clTreeCtrl::OnMouseLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &clTreeCtrl::OnMouseLeftUp, this);
    Unbind(wxEVT_LEFT_DCLICK, &clTreeCtrl::OnMouseLeftDClick, this);
    Unbind(wxEVT_MOUSEWHEEL, &clTreeCtrl::OnMouseScroll, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clTreeCtrl::OnLeaveWindow, this);
    Unbind(wxEVT_ENTER_WINDOW, &clTreeCtrl::OnEnterWindow, this);
    Unbind(wxEVT_CONTEXT_MENU, &clTreeCtrl::OnContextMenu, this);
    Unbind(wxEVT_RIGHT_DOWN, &clTreeCtrl::OnRightDown, this);
}

void clTreeCtrl::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC pdc(this);
    PrepareDC(pdc);

    wxGCDC dc(pdc);

    // Call the parent's Render method
    Render(dc);

    if(!m_model.GetRoot()) {
        // Reset the various items
        SetFirstItemOnScreen(nullptr);
        clRowEntry::Vec_t items;
        m_model.SetOnScreenItems(items);
        return;
    }

    size_t maxItems = GetNumLineCanFitOnScreen();
    bool needToUpdateScrollbar = false;
    if(!GetFirstItemOnScreen()) {
        SetFirstItemOnScreen(m_model.GetRoot());
        needToUpdateScrollbar = true;
    }
    clRowEntry* firstItem = GetFirstItemOnScreen();
    if(!firstItem) { return; }

    // Get list of items to draw
    clRowEntry::Vec_t items;
    m_model.GetNextItems(firstItem, maxItems, items);

    // Always try to get maximum entries to draw on the canvas
    while(items.size() < maxItems) {
        firstItem = m_model.GetRowBefore(firstItem, true);
        if(!firstItem) { break; }
        items.insert(items.begin(), firstItem);
        needToUpdateScrollbar = true;
    }
    
    // Update the first item on screen
    SetFirstItemOnScreen(firstItem);
    
    // Draw the items
    RenderItems(dc, items);

    // Keep the visible items
    m_model.SetOnScreenItems(items); // Keep track of the visible items
    if(needToUpdateScrollbar) { CallAfter(&clTreeCtrl::UpdateScrollBar); }
}

wxTreeItemId clTreeCtrl::InsertItem(const wxTreeItemId& parent, const wxTreeItemId& previous, const wxString& text,
                                    int image, int selImage, wxTreeItemData* data)
{
    wxTreeItemId item = m_model.InsertItem(parent, previous, text, image, selImage, data);
    DoUpdateHeader(item);
    return item;
}

wxTreeItemId clTreeCtrl::AppendItem(const wxTreeItemId& parent, const wxString& text, int image, int selImage,
                                    wxTreeItemData* data)
{
    wxTreeItemId item = m_model.AppendItem(parent, text, image, selImage, data);
    DoUpdateHeader(item);
    return item;
}

wxTreeItemId clTreeCtrl::AddRoot(const wxString& text, int image, int selImage, wxTreeItemData* data)
{
    wxTreeItemId root = m_model.AddRoot(text, image, selImage, data);
    DoUpdateHeader(root);
    return root;
}

wxTreeItemId clTreeCtrl::GetRootItem() const { return m_model.GetRootItem(); }

void clTreeCtrl::Expand(const wxTreeItemId& item)
{
    CHECK_ITEM_RET(item);
    clRowEntry* child = m_model.ToPtr(item);
    if(!child) return;
    child->SetExpanded(true);
    DoUpdateHeader(item);
    UpdateScrollBar();
    Refresh();
}

void clTreeCtrl::Collapse(const wxTreeItemId& item)
{
    CHECK_ITEM_RET(item);
    clRowEntry* child = m_model.ToPtr(item);
    if(!child) return;
    child->SetExpanded(false);
    UpdateScrollBar();
    DoUpdateHeader(item);
    Refresh();
}

void clTreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
{
    CHECK_ITEM_RET(item);
    if((select && m_model.IsItemSelected(item)) || (!select && !m_model.IsItemSelected(item))) { return; }
    m_model.SelectItem(item, select, false, true);
    Refresh();
}

void clTreeCtrl::OnMouseLeftDown(wxMouseEvent& event)
{
    event.Skip();
    CHECK_ROOT_RET();
    int flags = 0;
    wxPoint pt = DoFixPoint(event.GetPosition());
    wxTreeItemId where = HitTest(pt, flags);
    if(where.IsOk()) {
        if(flags & wxTREE_HITTEST_ONITEMBUTTON) {
            if(IsExpanded(where)) {
                Collapse(where);
            } else {
                Expand(where);
            }
        } else {
            clRowEntry* pNode = m_model.ToPtr(where);
            if(HasStyle(wxTR_MULTIPLE)) {
                if(event.ControlDown()) {
                    // Toggle the selection
                    m_model.SelectItem(where, !pNode->IsSelected(), true);
                } else if(event.ShiftDown()) {
                    // Range selection
                    clRowEntry::Vec_t range;
                    m_model.GetRange(pNode, m_model.ToPtr(m_model.GetSingleSelection()), range);
                    std::for_each(range.begin(), range.end(),
                                  [&](clRowEntry* p) { m_model.AddSelection(wxTreeItemId(p)); });
                } else {
                    // The default, single selection
                    bool has_multiple_selection = (m_model.GetSelectionsCount() > 1);
                    if(!has_multiple_selection && pNode->IsSelected()) {
                        // Nothing to be done here
                    } else if(!has_multiple_selection && !pNode->IsSelected()) {
                        // We got other selection (1), but not this one, select it
                        m_model.SelectItem(where, true, false, true);
                    } else if(has_multiple_selection && !pNode->IsSelected()) {
                        // we got multiple selections and we want to select a new item, what we do
                        // is we clear the current selections and select the new item
                        m_model.SelectItem(where, true, false, true);
                    }
                }

            } else {
                if(GetSelection() != wxTreeItemId(pNode)) {
                    // Select it
                    SelectItem(wxTreeItemId(pNode));
                }
            }
        }
        Refresh();
    }
}

void clTreeCtrl::OnMouseLeftUp(wxMouseEvent& event)
{
    event.Skip();
    int flags = 0;
    wxPoint pt = DoFixPoint(event.GetPosition());
    wxTreeItemId where = HitTest(pt, flags);
    if(where.IsOk() && (flags & wxTREE_HITTEST_ONITEM)) {
        bool has_multiple_selection = (m_model.GetSelectionsCount() > 1);
        clRowEntry* pNode = m_model.ToPtr(where);
        if(has_multiple_selection && pNode->IsSelected() && !event.HasAnyModifiers()) {
            // Select this item while clearing the others
            m_model.SelectItem(where, true, false, true);
            Refresh();
        }
    }
}

wxTreeItemId clTreeCtrl::HitTest(const wxPoint& point, int& flags) const
{
    if(!m_model.GetRoot()) { return wxTreeItemId(); }
    flags = 0;
    for(size_t i = 0; i < m_model.GetOnScreenItems().size(); ++i) {
        const clRowEntry* item = m_model.GetOnScreenItems()[i];
        wxRect buttonRect = item->GetButtonRect();
        // Adjust the coordiantes incase we got h-scroll
        buttonRect.SetX(buttonRect.GetX() - GetFirstColumn());
        if(buttonRect.Contains(point)) {
            flags |= wxTREE_HITTEST_ONITEMBUTTON;
            return wxTreeItemId(const_cast<clRowEntry*>(item));
        }
        if(item->GetItemRect().Contains(point)) {
            flags |= wxTREE_HITTEST_ONITEM;
            return wxTreeItemId(const_cast<clRowEntry*>(item));
        }
    }
    return wxTreeItemId();
}

void clTreeCtrl::UnselectAll()
{
    if(!m_model.GetRoot()) { return; }
    m_model.UnselectAll();
    Refresh();
}

wxPoint clTreeCtrl::DoFixPoint(const wxPoint& pt)
{
    wxPoint point = pt;
    return point;
}

void clTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    if(!item.IsOk()) { return; }
    // Make sure that all parents of ítem are expanded
    if(!m_model.ExpandToItem(item)) { return; }
    Refresh();
    CallAfter(&clTreeCtrl::DoEnsureVisible, item);
}

void clTreeCtrl::DoEnsureVisible(const wxTreeItemId& item)
{
    // scroll to the item
    if(!item.IsOk()) { return; }
    clRowEntry* pNode = m_model.ToPtr(item);
    if(IsItemVisible(pNode)) { return; }
    EnsureItemVisible(pNode, false); // make it visible at the bottom
    UpdateScrollBar();               // Make sure that the scrollbar fits the view
    Refresh();
}

void clTreeCtrl::OnMouseLeftDClick(wxMouseEvent& event)
{
    event.Skip();
    CHECK_ROOT_RET();

    int flags = 0;
    wxPoint pt = DoFixPoint(event.GetPosition());
    wxTreeItemId where = HitTest(pt, flags);
    if(where.IsOk()) {
        SelectItem(where, true);

        // Let sublclasses handle this first
        wxTreeEvent evt(wxEVT_TREE_ITEM_ACTIVATED);
        evt.SetEventObject(this);
        evt.SetItem(where);
        if(GetEventHandler()->ProcessEvent(evt)) { return; }

        // Process the default action
        if(ItemHasChildren(where)) {
            if(IsExpanded(where)) {
                Collapse(where);
            } else {
                Expand(where);
            }
        }
    }
}

bool clTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    if(!item.GetID()) return false;
    clRowEntry* child = m_model.ToPtr(item);
    if(!child) return false;
    return child->IsExpanded();
}

bool clTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    if(!item.GetID()) return false;
    clRowEntry* child = m_model.ToPtr(item);
    if(!child) return false;
    return child->HasChildren();
}

void clTreeCtrl::SetIndent(int size)
{
    clControlWithItems::SetIndent(size);
    m_model.SetIndentSize(size);
    Refresh();
}

int clTreeCtrl::GetIndent() const { return m_model.GetIndentSize(); }

bool clTreeCtrl::IsEmpty() const { return m_model.IsEmpty(); }

size_t clTreeCtrl::GetChildrenCount(const wxTreeItemId& item, bool recursively) const
{
    if(!item.GetID()) return 0;
    clRowEntry* node = m_model.ToPtr(item);
    return node->GetChildrenCount(recursively);
}

void clTreeCtrl::DeleteChildren(const wxTreeItemId& item)
{
    if(!item.GetID()) return;
    clRowEntry* node = m_model.ToPtr(item);
    node->DeleteAllChildren();
    UpdateScrollBar();
    Refresh();
}

wxTreeItemId clTreeCtrl::GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const
{
    if(!item.GetID()) return wxTreeItemId();
    clRowEntry* node = m_model.ToPtr(item);
    const clRowEntry::Vec_t& children = node->GetChildren();
    if(children.empty()) return wxTreeItemId(); // No children
    int* pidx = (int*)&cookie;
    int& idx = (*pidx);
    idx = 1; // the next item
    return wxTreeItemId(children[0]);
}

wxTreeItemId clTreeCtrl::GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const
{
    if(!item.GetID()) return wxTreeItemId();
    int* pidx = (int*)&cookie;
    int& idx = (*pidx);
    clRowEntry* node = m_model.ToPtr(item);
    const clRowEntry::Vec_t& children = node->GetChildren();
    if(idx >= (int)children.size()) return wxTreeItemId();
    wxTreeItemId child(children[idx]);
    idx++;
    return child;
}

wxString clTreeCtrl::GetItemText(const wxTreeItemId& item, size_t col) const
{
    if(!item.GetID()) return "";
    clRowEntry* node = m_model.ToPtr(item);
    return node->GetLabel(col);
}

wxTreeItemData* clTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
    if(!item.GetID()) return nullptr;
    clRowEntry* node = m_model.ToPtr(item);
    return node->GetClientObject();
}

void clTreeCtrl::OnMouseScroll(wxMouseEvent& event)
{
    event.Skip();
    CHECK_ROOT_RET();
    if(!GetFirstItemOnScreen()) { return; }

    // Ignore the first tick (should fix an annoyance on OSX)
    wxDirection direction = (event.GetWheelRotation() > 0) ? wxUP : wxDOWN;
    if(direction != m_lastScrollDir) {
        // Changing direction
        m_lastScrollDir = direction;
        return;
    }

    const clRowEntry::Vec_t& onScreenItems = m_model.GetOnScreenItems();
    if(onScreenItems.empty()) { return; }
    clRowEntry* lastItem = onScreenItems.back();
    clRowEntry* firstItem = onScreenItems.front();

    // Can we scroll any further?
    wxTreeItemId nextItem;
    if(event.GetWheelRotation() > 0) { // Scrolling up
        nextItem = m_model.GetItemAfter(firstItem, true);
    } else {
        nextItem = m_model.GetItemAfter(lastItem, true);
    }
    if(!nextItem.IsOk()) {
        // No more items to draw
        return;
    }
    clRowEntry::Vec_t items;
    if(event.GetWheelRotation() > 0) { // Scrolling up
        m_model.GetPrevItems(GetFirstItemOnScreen(), GetScrollTick(), items);
        if(items.empty()) { return; }
        SetFirstItemOnScreen(items.front()); // first item
        UpdateScrollBar();
    } else {
        m_model.GetNextItems(GetFirstItemOnScreen(), GetScrollTick(), items);
        if(items.empty()) { return; }
        SetFirstItemOnScreen(items.back()); // the last item
        UpdateScrollBar();
    }
    Refresh();
}

void clTreeCtrl::DoBitmapAdded()
{
    // Also, we adjust the indent size
    const std::vector<wxBitmap>& bmps = GetBitmaps();
    int heighestBitmap = 0;
    for(size_t i = 0; i < bmps.size(); ++i) {
        heighestBitmap = wxMax(heighestBitmap, bmps[i].GetScaledHeight());
    }
    heighestBitmap += 2 * clRowEntry::Y_SPACER;
    SetLineHeight(wxMax(heighestBitmap, GetLineHeight()));
    SetIndent(GetLineHeight());
}

void clTreeCtrl::SetBitmaps(const std::vector<wxBitmap>& bitmaps)
{
    clControlWithItems::SetBitmaps(bitmaps);
    DoBitmapAdded();
    Refresh();
}

int clTreeCtrl::AddBitmap(const wxBitmap& bmp)
{
    GetBitmaps().push_back(bmp);
    DoBitmapAdded();
    int new_index = (GetBitmaps().size() - 1);
    return new_index;
}

void clTreeCtrl::ProcessIdle()
{
    if(HasStyle(wxTR_FULL_ROW_HIGHLIGHT)) {
        CHECK_ROOT_RET();
        int flags = 0;
        wxPoint pt = ScreenToClient(::wxGetMousePosition());
        wxTreeItemId item = HitTest(pt, flags);
        if(item.IsOk()) {
            clRowEntry::Vec_t& items = m_model.GetOnScreenItems();
            clRowEntry* hoveredNode = m_model.ToPtr(item);
            bool refreshNeeded = false;
            for(size_t i = 0; i < items.size(); ++i) {
                bool new_state = hoveredNode == items[i];
                bool old_state = items[i]->IsHovered();
                if(!refreshNeeded) { refreshNeeded = (new_state != old_state); }
                items[i]->SetHovered(hoveredNode == items[i]);
            }
            if(refreshNeeded) { Refresh(); }
        }
    }
}

void clTreeCtrl::OnLeaveWindow(wxMouseEvent& event)
{
    event.Skip();
    CHECK_ROOT_RET();
    clRowEntry::Vec_t& items = m_model.GetOnScreenItems();
    for(size_t i = 0; i < items.size(); ++i) {
        items[i]->SetHovered(false);
    }
    Update();
}

void clTreeCtrl::ExpandAllChildren(const wxTreeItemId& item)
{
    wxBusyCursor bc;
    m_model.ExpandAllChildren(item);
    UpdateScrollBar();
    DoUpdateHeader(item);
    Refresh();
}

void clTreeCtrl::CollapseAllChildren(const wxTreeItemId& item)
{
    wxBusyCursor bc;
    m_model.CollapseAllChildren(item);
    SetFirstItemOnScreen(m_model.ToPtr(item));
    SelectItem(item);
    UpdateScrollBar();
    DoUpdateHeader(item);
    Refresh();
}

wxTreeItemId clTreeCtrl::GetFirstVisibleItem() const
{
    const clRowEntry::Vec_t& items = m_model.GetOnScreenItems();
    if(items.empty()) { return wxTreeItemId(); }
    return wxTreeItemId(items[0]);
}

wxTreeItemId clTreeCtrl::GetNextVisible(const wxTreeItemId& item) const { return DoGetSiblingVisibleItem(item, true); }
wxTreeItemId clTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const { return DoGetSiblingVisibleItem(item, false); }

wxTreeItemId clTreeCtrl::DoGetSiblingVisibleItem(const wxTreeItemId& item, bool next) const
{
    if(!item.IsOk()) { return wxTreeItemId(); }
    const clRowEntry::Vec_t& items = m_model.GetOnScreenItems();
    if(items.empty()) { return wxTreeItemId(); }
    clRowEntry* from = m_model.ToPtr(item);
    clRowEntry::Vec_t::const_iterator iter =
        std::find_if(items.begin(), items.end(), [&](clRowEntry* p) { return p == from; });
    if(next && (iter == items.end())) { return wxTreeItemId(); }
    if(!next && (iter == items.begin())) { return wxTreeItemId(); }
    if(next) {
        ++iter;
        if(iter == items.end()) { return wxTreeItemId(); }
    } else {
        --iter;
        if(iter == items.begin()) { return wxTreeItemId(); }
    }
    return wxTreeItemId(*iter);
}

wxTreeItemId clTreeCtrl::GetSelection() const { return m_model.GetSingleSelection(); }

wxTreeItemId clTreeCtrl::GetFocusedItem() const { return GetSelection(); }

size_t clTreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
{
    const clRowEntry::Vec_t& items = m_model.GetSelections();
    if(items.empty()) { return 0; }
    std::for_each(items.begin(), items.end(), [&](clRowEntry* item) { selections.Add(wxTreeItemId(item)); });
    return selections.size();
}

bool clTreeCtrl::DoKeyDown(const wxKeyEvent& event)
{
    // Let the user chance to process this first
    wxTreeEvent evt(wxEVT_TREE_KEY_DOWN);
    evt.SetEventObject(this);
    evt.SetKeyEvent(event);
    evt.SetItem(GetSelection()); // can be an invalid item
    if(GetEventHandler()->ProcessEvent(evt)) { return true; }

    // Let the parent process this
    if(clControlWithItems::DoKeyDown(event)) { return false; }

    if(!m_model.GetRoot()) {
        // we didnt process this event, carry on
        return true;
    }
    wxTreeItemId selectedItem = GetSelection();
    if(!selectedItem.IsOk()) { return true; }

    if(event.GetKeyCode() == WXK_LEFT) {
        if(m_model.ToPtr(selectedItem)->IsExpanded()) {
            Collapse(selectedItem);
            return true;
        }
    } else if(event.GetKeyCode() == WXK_RIGHT) {
        if(!m_model.ToPtr(selectedItem)->IsExpanded()) {
            Expand(selectedItem);
            return true;
        }
    } else if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
        wxTreeEvent evt(wxEVT_TREE_ITEM_ACTIVATED);
        evt.SetEventObject(this);
        evt.SetItem(selectedItem);
        GetEventHandler()->ProcessEvent(evt);
        return true;
    }
    // We didnt process this event, continue on processing it
    return false;
}

bool clTreeCtrl::IsItemVisible(clRowEntry* item) const
{
    const clRowEntry::Vec_t& onScreenItems = m_model.GetOnScreenItems();
    return (std::find_if(onScreenItems.begin(), onScreenItems.end(), [&](clRowEntry* p) { return p == item; }) !=
            onScreenItems.end());
}

void clTreeCtrl::EnsureItemVisible(clRowEntry* item, bool fromTop)
{
    CHECK_PTR_RET(item)
    if(IsItemVisible(item)) { return; }
    if(fromTop) {
        SetFirstItemOnScreen(item);
    } else {
        int max_lines_on_screen = GetNumLineCanFitOnScreen();
        clRowEntry::Vec_t items;
        m_model.GetPrevItems(item, max_lines_on_screen, items);
        if(items.empty()) { return; }
        SetFirstItemOnScreen(items[0]);
    }
}

void clTreeCtrl::Delete(const wxTreeItemId& item)
{
    CHECK_ITEM_RET(item);
    // delete the item + its children
    // fires event
    m_model.DeleteItem(item);
    UpdateScrollBar();
    Refresh();
}

void clTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData* data)
{
    clRowEntry* node = m_model.ToPtr(item);
    CHECK_PTR_RET(node);
    node->SetClientData(data);
}

void clTreeCtrl::SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& colour, size_t col)
{
    clRowEntry* node = m_model.ToPtr(item);
    CHECK_PTR_RET(node);
    node->SetBgColour(colour, col);
    Refresh();
}

wxColour clTreeCtrl::GetItemBackgroundColour(const wxTreeItemId& item, size_t col) const
{
    clRowEntry* node = m_model.ToPtr(item);
    if(!node) { return wxNullColour; }
    return node->GetBgColour(col);
}

void clTreeCtrl::SetItemTextColour(const wxTreeItemId& item, const wxColour& colour, size_t col)
{
    clRowEntry* node = m_model.ToPtr(item);
    CHECK_PTR_RET(node);
    node->SetTextColour(colour, col);
    Refresh();
}

wxColour clTreeCtrl::GetItemTextColour(const wxTreeItemId& item, size_t col) const
{
    clRowEntry* node = m_model.ToPtr(item);
    if(!node) { return wxNullColour; }
    return node->GetTextColour(col);
}

void clTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text, size_t col)
{
    clRowEntry* node = m_model.ToPtr(item);
    CHECK_PTR_RET(node);
    node->SetLabel(text, col);
    DoUpdateHeader(item);
    Refresh();
}

void clTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold, size_t col)
{
    clRowEntry* node = m_model.ToPtr(item);
    CHECK_PTR_RET(node);
    wxFont f = node->GetFont(col);
    if(!f.IsOk()) { f = GetDefaultFont(); }
    f.SetWeight(bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    node->SetFont(f, col);

    // Changing font can change the width of the text, so update the header if needed
    DoUpdateHeader(item);
    Refresh();
}

void clTreeCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font, size_t col)
{
    clRowEntry* node = m_model.ToPtr(item);
    CHECK_PTR_RET(node);
    node->SetFont(font, col);
    Refresh();
}

wxFont clTreeCtrl::GetItemFont(const wxTreeItemId& item, size_t col) const
{
    clRowEntry* node = m_model.ToPtr(item);
    if(!node) { return wxNullFont; }
    return node->GetFont(col);
}

void clTreeCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    event.Skip();
    CHECK_ROOT_RET();
    int flags = 0;
    wxPoint pt = ScreenToClient(::wxGetMousePosition());
    wxTreeItemId item = HitTest(pt, flags);
    if(item.IsOk()) {
        SelectItem(item, true);
        wxTreeEvent evt(wxEVT_TREE_ITEM_MENU);
        evt.SetItem(item);
        evt.SetEventObject(this);
        GetEventHandler()->ProcessEvent(evt);
    }
}

void clTreeCtrl::OnRightDown(wxMouseEvent& event)
{
    event.Skip();
    CHECK_ROOT_RET();
    int flags = 0;
    wxPoint pt = DoFixPoint(event.GetPosition());
    wxTreeItemId where = HitTest(pt, flags);
    if(where.IsOk()) {
        wxTreeEvent evt(wxEVT_TREE_ITEM_RIGHT_CLICK);
        evt.SetEventObject(this);
        evt.SetItem(where);
        event.Skip(false);
        if(GetEventHandler()->ProcessEvent(evt)) { return; }
        event.Skip();
    }
}

clRowEntry* clTreeCtrl::GetFirstItemOnScreen() { return m_model.GetFirstItemOnScreen(); }

void clTreeCtrl::SetFirstItemOnScreen(clRowEntry* item) { m_model.SetFirstItemOnScreen(item); }

void clTreeCtrl::SetSortFunction(const clSortFunc_t& CompareFunc) { m_model.SetSortFunction(CompareFunc); }

void clTreeCtrl::ScrollToRow(int firstLine)
{
    clRowEntry* newTopLine = nullptr;
    // Thumbtrack sends the top-line index in event.GetPosition()
    newTopLine = m_model.GetItemFromIndex(firstLine);
    if(newTopLine) {
        if(newTopLine->IsHidden()) { newTopLine = newTopLine->GetFirstChild(); }
        SetFirstItemOnScreen(m_model.ToPtr(newTopLine));
    }
    UpdateScrollBar();
    Refresh();
}

void clTreeCtrl::ScrollRows(int steps, wxDirection direction)
{
    // Process the rest of the scrolling events here
    wxTreeItemId nextSelection;
    bool fromTop = false;
    if(steps == 0) {
        // Top or Bottom
        if(direction == wxUP) {
            if(IsRootHidden()) {
                nextSelection = wxTreeItemId(m_model.ToPtr(GetRootItem())->GetFirstChild());
            } else {
                nextSelection = GetRootItem();
            }
            fromTop = true;
        } else {
            // Find the last item, it does not matter if the root is hidden
            clRowEntry* node = m_model.ToPtr(GetRootItem());
            while(node->GetLastChild()) {
                node = node->GetLastChild();
            }
            nextSelection = wxTreeItemId(node);
        }
    } else {
        nextSelection = DoScrollLines(steps, direction == wxUP, GetFocusedItem(), false);
        fromTop = (direction == wxUP);
    }

    if(::wxGetKeyState(WXK_SHIFT) && HasStyle(wxTR_MULTIPLE)) {
        m_model.AddSelection(nextSelection);
    } else {
        SelectItem(nextSelection);
    }
    EnsureItemVisible(m_model.ToPtr(nextSelection), fromTop);
    Refresh();
    UpdateScrollBar();
}

void clTreeCtrl::SelectChildren(const wxTreeItemId& item)
{
    CHECK_ITEM_RET(item);
    if(!(GetTreeStyle() & wxTR_MULTIPLE)) {
        // Can only be used with multiple selection trees
        return;
    }
    m_model.SelectChildren(item);
    Refresh();
}

wxTreeItemId clTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    CHECK_ITEM_RET_INVALID_ITEM(item);
    return m_model.GetNextSibling(m_model.ToPtr(item));
}

wxTreeItemId clTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    CHECK_ITEM_RET_INVALID_ITEM(item);
    return m_model.GetPrevSibling(m_model.ToPtr(item));
}

wxTreeItemId clTreeCtrl::DoScrollLines(int numLines, bool up, wxTreeItemId from, bool selectIt)
{
    wxTreeItemId selectedItem = from;
    if(!selectedItem.IsOk()) { return wxTreeItemId(); }

    int counter = 0;
    wxTreeItemId nextItem = selectedItem;
    while(nextItem.IsOk() && (counter < numLines)) {
        if(up) {
            nextItem = m_model.GetItemBefore(selectedItem, true);
        } else {
            nextItem = m_model.GetItemAfter(selectedItem, true);
        }
        if(nextItem.IsOk()) { selectedItem = nextItem; }
        counter++;
    }
    if(selectIt) { SelectItem(selectedItem); }
    return selectedItem;
}

void clTreeCtrl::EnableStyle(int style, bool enable, bool refresh)
{
    if(enable) {
        m_treeStyle |= style;
    } else {
        m_treeStyle &= ~style;
    }

    if(style == wxTR_ENABLE_SEARCH) { GetSearch().SetEnabled(enable); }

    // From this point on, we require a root item
    if(!m_model.GetRoot()) { return; }

    // When changing the wxTR_HIDE_ROOT style
    // we need to fix the indentation for each node in the tree
    if(style == wxTR_HIDE_ROOT) {
        m_model.GetRoot()->SetHidden(IsRootHidden());
        std::function<bool(clRowEntry*, bool)> UpdateIndentsFunc = [=](clRowEntry* node, bool visibleItem) {
            wxUnusedVar(visibleItem);
            if(node->GetParent()) { node->SetIndentsCount(node->GetParent()->GetIndentsCount() + 1); }
            return true;
        };
        clTreeNodeVisitor V;
        V.Visit(m_model.GetRoot(), false, UpdateIndentsFunc);
        wxTreeItemId newRoot(m_model.GetRoot()->GetFirstChild());
        if(newRoot) { DoUpdateHeader(newRoot); }
    }
    if(refresh) { Refresh(); }
}

wxTreeItemId clTreeCtrl::GetItemParent(const wxTreeItemId& item) const
{
    CHECK_ITEM_RET_INVALID_ITEM(item);
    return wxTreeItemId(m_model.ToPtr(item)->GetParent());
}

void clTreeCtrl::SetItemImage(const wxTreeItemId& item, int imageId, int openImageId, size_t col)
{
    clRowEntry* node = m_model.ToPtr(item);
    CHECK_PTR_RET(node);
    node->SetBitmapIndex(imageId, col);
    node->SetBitmapSelectedIndex(openImageId, col);
    Refresh();
}

int clTreeCtrl::GetItemImage(const wxTreeItemId& item, bool selectedImage, size_t col) const
{
    if(!item.GetID()) { return wxNOT_FOUND; }
    clRowEntry* node = m_model.ToPtr(item);
    return selectedImage ? node->GetBitmapSelectedIndex(col) : node->GetBitmapIndex(col);
}

void clTreeCtrl::OnEnterWindow(wxMouseEvent& event) { event.Skip(); }

void clTreeCtrl::DoUpdateHeader(const wxTreeItemId& item) { clControlWithItems::DoUpdateHeader(m_model.ToPtr(item)); }

bool clTreeCtrl::IsVisible(const wxTreeItemId& item) const { return m_model.IsVisible(item); }

bool clTreeCtrl::IsSelected(const wxTreeItemId& item) const { return m_model.IsItemSelected(item); }

wxTreeItemId clTreeCtrl::GetRow(const wxPoint& pt) const
{
    int flags = 0;
    wxTreeItemId item = HitTest(pt, flags);
    if(item.IsOk() && (flags & wxTREE_HITTEST_ONITEM)) { return item; }
    return wxTreeItemId();
}
int clTreeCtrl::GetFirstItemPosition() const { return m_model.GetItemIndex(m_model.GetFirstItemOnScreen()); }

int clTreeCtrl::GetRange() const { return m_model.GetExpandedLines(); }

void clTreeCtrl::DeleteAllItems()
{
    m_model.EnableEvents(false);
    Delete(GetRootItem());
    m_model.EnableEvents(true);
    DoUpdateHeader(nullptr);
}

wxTreeItemId clTreeCtrl::GetNextItem(const wxTreeItemId& item) const { return m_model.GetItemAfter(item, true); }

wxTreeItemId clTreeCtrl::GetPrevItem(const wxTreeItemId& item) const { return m_model.GetItemBefore(item, true); }

wxTreeItemId clTreeCtrl::FindNext(const wxTreeItemId& from, const wxString& what, size_t col, size_t searchFlags)
{
    return wxTreeItemId(DoFind(m_model.ToPtr(from), what, col, searchFlags, true));
}

wxTreeItemId clTreeCtrl::FindPrev(const wxTreeItemId& from, const wxString& what, size_t col, size_t searchFlags)
{
    return wxTreeItemId(DoFind(m_model.ToPtr(from), what, col, searchFlags, false));
}

void clTreeCtrl::HighlightText(const wxTreeItemId& item, bool b)
{
    if(!item.IsOk()) { return; }
    m_model.ToPtr(item)->SetHighlight(true);
}

void clTreeCtrl::ClearHighlight(const wxTreeItemId& item)
{
    if(!item.IsOk()) { return; }
    clRowEntry* row = m_model.ToPtr(item);
    row->SetHighlight(false);
    row->SetHighlightInfo({});
    Refresh();
}

clRowEntry* clTreeCtrl::DoFind(clRowEntry* from, const wxString& what, size_t col, size_t searchFlags, bool next)
{
    clRowEntry* curp = nullptr;
    if(!from) {
        curp = m_model.GetRoot();
    } else {
        if(searchFlags & wxTR_SEARCH_INCLUDE_CURRENT_ITEM) {
            curp = from;
        } else {
            curp = next ? m_model.GetRowAfter(m_model.ToPtr(from), searchFlags & wxTR_SEARCH_VISIBLE_ITEMS)
                        : m_model.GetRowBefore(m_model.ToPtr(from), searchFlags & wxTR_SEARCH_VISIBLE_ITEMS);
        }
    }
    while(curp) {
        const wxString& haystack = curp->GetLabel(col);
        clMatchResult res;
        if(clSearchText::Matches(what, col, haystack, searchFlags, &res)) {
            curp->SetHighlightInfo(res);
            curp->SetHighlight(true);
            return curp;
        }
        curp = next ? m_model.GetRowAfter(curp, searchFlags & wxTR_SEARCH_VISIBLE_ITEMS)
                    : m_model.GetRowBefore(curp, searchFlags & wxTR_SEARCH_VISIBLE_ITEMS);
    }
    return nullptr;
}

void clTreeCtrl::ClearAllHighlights()
{
    clTreeNodeVisitor V;
    std::function<bool(clRowEntry*, bool)> Foo = [&](clRowEntry* r, bool visible) {
        wxUnusedVar(visible);
        r->SetHighlightInfo({});
        r->SetHighlight(false);
        return true;
    };
    V.Visit(m_model.GetRoot(), false, Foo);
    Refresh();
}
