#include "clScrollBar.h"
#include "clTreeCtrl.h"
#include "clTreeCtrlModel.h"
#include "clTreeNodeVisitor.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
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

#ifdef __WXMSW__
#include <uxtheme.h>
#endif

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

wxDEFINE_EVENT(wxEVT_TREE_ITEM_VALUE_CHANGED, wxTreeEvent);
wxDEFINE_EVENT(wxEVT_TREE_CHOICE, wxTreeEvent);

static void MSWSetNativeTheme(wxWindow* win)
{
#if defined(__WXMSW__) && defined(_WIN64)
    SetWindowTheme((HWND)win->GetHWND(), wxT("Explorer"), NULL);
#endif
}

clTreeCtrl::clTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clControlWithItems(parent, id, pos, size, style | wxWANTS_CHARS)
    , m_model(this)
{
    m_treeStyle = style & ~wxWINDOW_STYLE_MASK; // remove the non window style
    DoInitialize();
}

clTreeCtrl::clTreeCtrl()
    : m_model(this)
{
}

bool clTreeCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    m_treeStyle = style & ~wxWINDOW_STYLE_MASK;
    if(!clControlWithItems::Create(parent, id, pos, size, style | wxWANTS_CHARS)) { return false; }
    DoInitialize();
    return true;
}

void clTreeCtrl::UpdateLineHeight()
{
    wxMemoryDC tmpDC;
    wxBitmap bmp(1, 1);
    tmpDC.SelectObject(bmp);
    wxGCDC gcdc(tmpDC);

    gcdc.SetFont(GetDefaultFont());
    wxSize textSize = gcdc.GetTextExtent("Tp");

    SetLineHeight(clRowEntry::Y_SPACER + textSize.GetHeight() + clRowEntry::Y_SPACER);
    SetIndent(GetLineHeight());
}

void clTreeCtrl::DoInitialize()
{
    UpdateLineHeight();
    Bind(wxEVT_IDLE, &clTreeCtrl::OnIdle, this);
    Bind(wxEVT_PAINT, &clTreeCtrl::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent& event) { wxUnusedVar(event); });
    Bind(wxEVT_LEFT_DOWN, &clTreeCtrl::OnMouseLeftDown, this);
    Bind(wxEVT_LEFT_UP, &clTreeCtrl::OnMouseLeftUp, this);
    Bind(wxEVT_LEFT_DCLICK, &clTreeCtrl::OnMouseLeftDClick, this);
    Bind(wxEVT_LEAVE_WINDOW, &clTreeCtrl::OnLeaveWindow, this);
    Bind(wxEVT_ENTER_WINDOW, &clTreeCtrl::OnEnterWindow, this);
    Bind(wxEVT_CONTEXT_MENU, &clTreeCtrl::OnContextMenu, this);
    Bind(wxEVT_RIGHT_DOWN, &clTreeCtrl::OnRightDown, this);

    // Initialise default colours
    GetColours().InitDefaults();

    // There is always a header
    GetHeader()->Add("");
    SetShowHeader(false);
    MSWSetNativeTheme(this);
}

clTreeCtrl::~clTreeCtrl()
{
    Unbind(wxEVT_IDLE, &clTreeCtrl::OnIdle, this);
    Unbind(wxEVT_PAINT, &clTreeCtrl::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent& event) { wxUnusedVar(event); });
    Unbind(wxEVT_LEFT_DOWN, &clTreeCtrl::OnMouseLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &clTreeCtrl::OnMouseLeftUp, this);
    Unbind(wxEVT_LEFT_DCLICK, &clTreeCtrl::OnMouseLeftDClick, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clTreeCtrl::OnLeaveWindow, this);
    Unbind(wxEVT_ENTER_WINDOW, &clTreeCtrl::OnEnterWindow, this);
    Unbind(wxEVT_CONTEXT_MENU, &clTreeCtrl::OnContextMenu, this);
    Unbind(wxEVT_RIGHT_DOWN, &clTreeCtrl::OnRightDown, this);
}

void clTreeCtrl::OnPaint(wxPaintEvent& event)
{
    wxUnusedVar(event);
    wxAutoBufferedPaintDC pdc(this);
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

    bool needToUpdateScrollbar = false;
    if(!GetFirstItemOnScreen()) {
        SetFirstItemOnScreen(m_model.GetRoot());
        needToUpdateScrollbar = true;
    }
    clRowEntry* firstItem = GetFirstItemOnScreen();
    if(!firstItem) { return; }

    // Get list of items to draw
    clRowEntry::Vec_t items;
    size_t maxItems = GetNumLineCanFitOnScreen();
    if(maxItems == 0) { return; }
    m_model.GetNextItems(firstItem, maxItems, items);

    // Always try to get maximum entries to draw on the canvas
    if(items.empty()) { return; }
    bool canScrollDown = GetVScrollBar()->CanScollDown();

    // An action took that requires us to try to maximimze the list
    if(m_maxList) {
        while(
            (canScrollDown &&
             (items.size() < maxItems)) || // While can move the scroll thumb a bit further down, increase the list size
            (!canScrollDown && (items.size() < (maxItems - 1)))) { // the scroll thumb cant be moved further down, so it
                                                                   // makes no sense on hiding the last item (we wont be
                                                                   // able to reach it), so make sure we extend the list
                                                                   // up to max-items -1, this means that the last item
                                                                   // is always fully visible
            firstItem = m_model.GetRowBefore(firstItem, true);
            if(!firstItem) { break; }
            items.insert(items.begin(), firstItem);
            needToUpdateScrollbar = true;
        }
    }
    m_maxList = false;
    // So we increased the list to display as much as items as we can.
    // However, if the last item in the view is not visible, make it so
    clRowEntry* lastRow = items.back();
    if(lastRow && lastRow->IsSelected()) {
        AssignRects(items);
        if(!IsItemFullyVisible(lastRow)) {
            // Remove the first item from the list and append new item at the bottom
            // basically what we are doing here is sliding the window (visible items) 1 row up
            items.erase(items.begin());
            clRowEntry* new_last_item = m_model.GetRowAfter(lastRow, true);
            if(new_last_item) { items.push_back(new_last_item); }
        }
    }

    // Update the first item on screen
    SetFirstItemOnScreen(firstItem);

    // Draw the items
    wxRect clientRect = GetItemsRect();
    // Set the width of the clipping region to match the header's width
    clientRect.SetWidth(clientRect.GetWidth() + m_firstColumn + 1);
    dc.SetClippingRegion(clientRect);
    RenderItems(dc, items);
    dc.DestroyClippingRegion();

    // Keep the visible items
    m_model.SetOnScreenItems(items); // Keep track of the visible items
    if(needToUpdateScrollbar) { CallAfter(&clTreeCtrl::UpdateScrollBar); }

    // Update the header as well
    if(GetHeader() && GetHeader()->IsShown()) { GetHeader()->Update(); }
}

wxTreeItemId clTreeCtrl::InsertItem(const wxTreeItemId& parent, const wxTreeItemId& previous, const wxString& text,
                                    int image, int selImage, wxTreeItemData* data)
{
    wxTreeItemId item = m_model.InsertItem(parent, previous, text, image, selImage, data);
    DoUpdateHeader(item);
    if(IsExpanded(parent)) { UpdateScrollBar(); }
    return item;
}

wxTreeItemId clTreeCtrl::AppendItem(const wxTreeItemId& parent, const wxString& text, int image, int selImage,
                                    wxTreeItemData* data)
{
    wxTreeItemId item = m_model.AppendItem(parent, text, image, selImage, data);
    DoUpdateHeader(item);
    if(IsExpanded(parent)) { UpdateScrollBar(); }
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
    m_maxList = true;
    DoUpdateHeader(item);
    UpdateScrollBar();
    Refresh();
    if(GetVScrollBar() && GetVScrollBar()->IsShown()) { GetVScrollBar()->CallAfter(&clScrollBar::Update); }
}

void clTreeCtrl::Collapse(const wxTreeItemId& item)
{
    CHECK_ITEM_RET(item);
    clRowEntry* child = m_model.ToPtr(item);
    if(!child) return;
    child->SetExpanded(false);
    m_maxList = true;
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
    int column = wxNOT_FOUND;
    wxPoint pt = DoFixPoint(event.GetPosition());
    wxTreeItemId where = HitTest(pt, flags, column);
    if(where.IsOk()) {
        if(flags & wxTREE_HITTEST_ONITEMBUTTON) {
            if(IsExpanded(where)) {
                Collapse(where);
            } else {
                Expand(where);
            }
        } else {
            clRowEntry* pNode = m_model.ToPtr(where);
            if(flags & wxTREE_HITTEST_ONITEMSTATEICON) {
                // Change the state
                Check(where, !IsChecked(where, column), column);
            }
            bool has_multiple_selection = (m_model.GetSelectionsCount() > 1);
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
                    if(!has_multiple_selection && pNode->IsSelected()) {
                        // Nothing to be done here
                    } else if(!has_multiple_selection && !pNode->IsSelected()) {
                        // We got other selection (1), but not this one, select it
                        m_model.SelectItem(where, true, false, true);
                        EnsureVisible(where);
                    } else if(has_multiple_selection && !pNode->IsSelected()) {
                        // we got multiple selections and we want to select a new item, what we do
                        // is we clear the current selections and select the new item
                        m_model.SelectItem(where, true, false, true);
                        EnsureVisible(where);
                    }
                }

            } else {
                if(GetSelection() != wxTreeItemId(pNode)) {
                    // Select it
                    SelectItem(wxTreeItemId(pNode));
                    EnsureVisible(where);
                }
            }

            if((flags & wxTREE_HITTEST_ONDROPDOWNARROW) && !has_multiple_selection) {
                wxTreeEvent evt(wxEVT_TREE_CHOICE);
                evt.SetInt(column);
                evt.SetEventObject(this);
                evt.SetItem(where);
                GetEventHandler()->ProcessEvent(evt);
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
    int column = wxNOT_FOUND;
    wxTreeItemId where = HitTest(pt, flags, column);
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

wxTreeItemId clTreeCtrl::HitTest(const wxPoint& point, int& flags, int& column) const
{
    column = wxNOT_FOUND;
    flags = 0;
    if(!m_model.GetRoot()) { return wxTreeItemId(); }
    for(size_t i = 0; i < m_model.GetOnScreenItems().size(); ++i) {
        const clRowEntry* item = m_model.GetOnScreenItems()[i];

        {
            wxRect buttonRect = item->GetButtonRect();
            // Adjust the coordiantes incase we got h-scroll
            buttonRect.SetX(buttonRect.GetX() - GetFirstColumn());
            if(buttonRect.Contains(point)) {
                flags = wxTREE_HITTEST_ONITEMBUTTON;
                // The button is always on column 0
                column = 0;
                return wxTreeItemId(const_cast<clRowEntry*>(item));
            }
        }
        if(item->GetItemRect().Contains(point)) {
            flags = wxTREE_HITTEST_ONITEM;
            if(GetHeader() && !GetHeader()->empty()) {
                for(size_t col = 0; col < GetHeader()->size(); ++col) {
                    // Check which column was clicked
                    wxRect cellRect = item->GetCellRect(col);
                    // We need to fix the x-axis to reflect any horizontal scrollbar
                    cellRect.SetX(cellRect.GetX() - GetFirstColumn());
                    if(cellRect.Contains(point)) {
                        // Check if click was made on the checkbox ("state icon")
                        wxRect checkboxRect = item->GetCheckboxRect(col);
                        wxRect dropDownRect = item->GetChoiceRect(col);
                        if(!checkboxRect.IsEmpty()) {
                            // Adjust the coordiantes incase we got h-scroll
                            checkboxRect.SetX(checkboxRect.GetX() - GetFirstColumn());
                            if(checkboxRect.Contains(point)) { flags |= wxTREE_HITTEST_ONITEMSTATEICON; }
                        } else if(!dropDownRect.IsEmpty()) {
                            dropDownRect.SetX(dropDownRect.GetX() - GetFirstColumn());
                            if(dropDownRect.Contains(point)) { flags |= wxTREE_HITTEST_ONDROPDOWNARROW; }
                        }
                        column = col;
                        break;
                    }
                }
                if(column == wxNOT_FOUND) {
                    // assume its the last column (the last column expands on the remainder of the row
                    column = (GetHeader()->size() - 1);
                }
            }
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

    // We need to paint to take place now so all the items will be assigned with the updated geometry
    Update();
    DoEnsureVisible(item);
}

void clTreeCtrl::DoEnsureVisible(const wxTreeItemId& item)
{
    // scroll to the item
    if(!item.IsOk()) { return; }
    clRowEntry* pNode = m_model.ToPtr(item);
    if(IsItemVisible(pNode) && IsItemFullyVisible(pNode)) { return; }
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
    int column = wxNOT_FOUND;
    wxTreeItemId where = HitTest(pt, flags, column);
    if(where.IsOk()) {
        SelectItem(where, true);

        // Let sublclasses handle this first
        wxTreeEvent evt(wxEVT_TREE_ITEM_ACTIVATED);
        evt.SetEventObject(this);
        evt.SetItem(where);
        evt.SetInt(column);
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

void clTreeCtrl::DoMouseScroll(const wxMouseEvent& event)
{
    CHECK_ROOT_RET();
    if(!GetFirstItemOnScreen()) { return; }

    const clRowEntry::Vec_t& onScreenItems = m_model.GetOnScreenItems();
    if(onScreenItems.empty()) { return; }
    clRowEntry* lastItem = onScreenItems.back();
    clRowEntry* firstItem = onScreenItems.front();

    // Can we scroll any further?
    wxTreeItemId nextItem;
    bool scrolling_down = !(event.GetWheelRotation() > 0);
    if(scrolling_down) { // Scrolling up
        nextItem = m_model.GetItemAfter(lastItem, true);
    } else {
        nextItem = m_model.GetItemAfter(firstItem, true);
    }

    if(!nextItem.IsOk() && (scrolling_down && !IsItemFullyVisible(lastItem))) {
        nextItem = wxTreeItemId(lastItem);
    } else if(!nextItem.IsOk()) {
        // No more items to draw
        m_scrollLines = 0;
        return;
    }

    clRowEntry::Vec_t items;
    m_scrollLines += event.GetWheelRotation();
    int lines = m_scrollLines / event.GetWheelDelta();
    int remainder = m_scrollLines % event.GetWheelDelta();

    if(lines != 0) {
        m_scrollLines = remainder;
    } else {
        return;
    }
    if(event.GetWheelRotation() > 0) { // Scrolling up
        m_model.GetPrevItems(GetFirstItemOnScreen(), std::abs((double)lines), items, false);
        if(items.empty()) { return; }
        SetFirstItemOnScreen(items.front()); // first item
        UpdateScrollBar();
    } else {
        m_model.GetNextItems(GetFirstItemOnScreen(), std::abs((double)lines), items, false);
        if(items.empty()) { return; }
        SetFirstItemOnScreen(items.back()); // the last item
        UpdateScrollBar();
    }
    Refresh();
}

void clTreeCtrl::DoBitmapAdded()
{
    // Also, we adjust the indent size
    if(!GetBitmaps()) { return; }
    int heighestBitmap = 0;
    for(size_t i = 0; i < GetBitmaps()->size(); ++i) {
        const wxBitmap& bmp = GetBitmaps()->at(i);
        if(bmp.IsOk()) { heighestBitmap = wxMax(heighestBitmap, bmp.GetScaledHeight()); }
    }
    heighestBitmap += 2 * clRowEntry::Y_SPACER;
    SetLineHeight(wxMax(heighestBitmap, GetLineHeight()));
    SetIndent(GetLineHeight());
}

void clTreeCtrl::SetBitmaps(BitmapVec_t* bitmaps)
{
    clControlWithItems::SetBitmaps(bitmaps);
    DoBitmapAdded();
    Refresh();
}

void clTreeCtrl::ProcessIdle()
{
    if(HasStyle(wxTR_FULL_ROW_HIGHLIGHT)) {
        CHECK_ROOT_RET();
        int flags = 0;
        wxPoint pt = ScreenToClient(::wxGetMousePosition());
        int column = wxNOT_FOUND;
        wxTreeItemId item = HitTest(pt, flags, column);
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

    clRowEntry* row = m_model.ToPtr(selectedItem);
    if(event.GetKeyCode() == WXK_LEFT) {
        if(row->IsExpanded()) {
            Collapse(selectedItem);
            return true;
        } else if(row->GetParent()) {
            SelectItem(GetItemParent(selectedItem), true);
            return true;
        }
    } else if(event.GetKeyCode() == WXK_RIGHT) {
        if(!row->IsExpanded()) {
            Expand(selectedItem);
            return true;
        } else if(row->GetChildrenCount(false)) {
            // this item has children, select the first child
            wxTreeItemIdValue cookie;
            SelectItem(GetFirstChild(selectedItem, cookie), true);
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
    clRowEntry::Vec_t::const_iterator iter =
        std::find_if(onScreenItems.begin(), onScreenItems.end(), [&](clRowEntry* p) { return p == item; });
    return (iter != onScreenItems.end());
}

bool clTreeCtrl::IsItemFullyVisible(clRowEntry* item) const
{
    const wxRect& itemRect = item->GetItemRect();
    const wxRect clientRect = GetItemsRect();
    return clientRect.Contains(itemRect);
}

void clTreeCtrl::EnsureItemVisible(clRowEntry* item, bool fromTop)
{
    CHECK_PTR_RET(item)
    if(m_model.GetOnScreenItems().empty()) {
        // requesting to ensure item visibility before we drawn anyting on the screen
        // to reduce any strange behaviours (e.g. 1/2 screen is displayed while we can display more items)
        // turn the m_maxList flag to ON
        m_maxList = true;
    }

    if(IsItemFullyVisible(item)) { return; }
    if(fromTop) {
        SetFirstItemOnScreen(item);
    } else {
        int max_lines_on_screen = GetNumLineCanFitOnScreen();
        clRowEntry::Vec_t items;
        m_model.GetPrevItems(item, max_lines_on_screen, items);
        if(items.empty()) { return; }
        if(!IsItemFullyVisible(item) && (items.size() > 1)) { items.erase(items.begin()); }
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
    int column = wxNOT_FOUND;
    wxPoint pt = ScreenToClient(::wxGetMousePosition());
    wxTreeItemId item = HitTest(pt, flags, column);
    if(item.IsOk()) {
        SelectItem(item, true);
        wxTreeEvent evt(wxEVT_TREE_ITEM_MENU);
        evt.SetItem(item);
        evt.SetInt(column); // pass the column
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
    int column = wxNOT_FOUND;
    wxTreeItemId where = HitTest(pt, flags, column);
    if(where.IsOk()) {
        wxTreeEvent evt(wxEVT_TREE_ITEM_RIGHT_CLICK);
        evt.SetEventObject(this);
        evt.SetItem(where);
        evt.SetInt(column);
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
        SetFirstItemOnScreen(newTopLine);
        if(!GetVScrollBar()->CanScollDown()) {
            // we cant scroll down anymore
            // Get list of items to draw
            clRowEntry::Vec_t items;
            size_t maxItems = GetNumLineCanFitOnScreen();
            m_model.GetNextItems(newTopLine, maxItems, items);
            AssignRects(items);
            if(!items.empty() && !IsItemFullyVisible(items.back()) && (firstLine != 0)) {
                newTopLine = m_model.GetRowAfter(newTopLine, true);
                if(newTopLine) { SetFirstItemOnScreen(newTopLine); }
            }
        }
    }
#if CL_USE_NATIVE_SCROLLBAR
    UpdateScrollBar();
#endif
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
    int column = wxNOT_FOUND;
    wxTreeItemId item = HitTest(pt, flags, column);
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
    m_scrollLines = 0;
    SetFirstColumn(0);
    UpdateScrollBar();
    Refresh();
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
    m_model.ToPtr(item)->SetHighlight(b);
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

void clTreeCtrl::UpdateScrollBar()
{
    clControlWithItems::UpdateScrollBar();
    m_scrollLines = 0;
}

void clTreeCtrl::AddHeader(const wxString& label, const wxBitmap& bmp, int width) { DoAddHeader(label, bmp, width); }

void clTreeCtrl::DoAddHeader(const wxString& label, const wxBitmap& bmp, int width)
{
    wxUnusedVar(bmp);
    if(m_needToClearDefaultHeader) {
        m_needToClearDefaultHeader = false;
        GetHeader()->Clear();
    }
    clHeaderItem& col = GetHeader()->Add(label);
    if(width > 0) { col.SetWidthValue(width); }
}

void clTreeCtrl::Check(const wxTreeItemId& item, bool check, size_t col)
{
    clRowEntry* row = m_model.ToPtr(item);
    if(!row) { return; }
    row->SetChecked(check, row->GetBitmapIndex(col), row->GetLabel(col), col);

    // Fire value changed event
    wxTreeEvent evt(wxEVT_TREE_ITEM_VALUE_CHANGED);
    evt.SetInt(col);
    evt.SetEventObject(this);
    evt.SetItem(item);
    GetEventHandler()->ProcessEvent(evt);
    Refresh();
}

bool clTreeCtrl::IsChecked(const wxTreeItemId& item, size_t col) const
{
    clRowEntry* row = m_model.ToPtr(item);
    if(!row) { return false; }
    return row->IsChecked(col);
}

void clTreeCtrl::SetImageList(wxImageList* images)
{
    clControlWithItems::SetImageList(images);
    DoBitmapAdded();
    Refresh();
}

void clTreeCtrl::LineUp() { ScrollRows(1, wxUP); }

void clTreeCtrl::LineDown() { ScrollRows(1, wxDOWN); }

void clTreeCtrl::PageDown() { ScrollRows(GetNumLineCanFitOnScreen(), wxDOWN); }

void clTreeCtrl::PageUp() { ScrollRows(GetNumLineCanFitOnScreen(), wxUP); }

void clTreeCtrl::SetDefaultFont(const wxFont& font)
{
    m_defaultFont = font;
    UpdateLineHeight();
    Refresh();
}

wxFont clTreeCtrl::GetDefaultFont() const
{
    if(m_defaultFont.IsOk()) { return m_defaultFont; }
    return clScrolledPanel::GetDefaultFont();
}
