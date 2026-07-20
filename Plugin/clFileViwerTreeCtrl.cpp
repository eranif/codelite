#include "clFileViwerTreeCtrl.h"

#include <wx/settings.h>
#include <wx/time.h>

namespace
{
int DoubleClickIntervalMs()
{
    int msec = wxSystemSettings::GetMetric(wxSYS_DCLICK_MSEC);
    return msec > 0 ? msec : 400;
}
} // namespace

clFileViewerTreeCtrl::clFileViewerTreeCtrl(
    wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxDataViewTreeCtrl(parent, id, pos, size, style)
{
    // wxDataViewTreeCtrl::Create() sets up its only column as wxDATAVIEW_CELL_EDITABLE.
    // We don't support inline rename here, so force the cell back to non-editable.
    if (wxDataViewColumn* col = GetColumn(0)) {
        if (wxDataViewRenderer* renderer = col->GetRenderer()) {
            renderer->SetMode(wxDATAVIEW_CELL_INERT);
        }
    }

    // On macOS, clicking a row that is already part of a multi-selection is
    // ambiguous to the native NSOutlineView (is this the start of a drag of the
    // whole selection, or a plain click to collapse the selection to this row?).
    // It waits out the OS click/drag disambiguation delay before resolving this,
    // which shows up as a multi-hundred-ms stall. We don't support dragging items
    // out of this tree, so pre-empt the native handling for this specific case:
    // detect it ourselves and consume the event before it reaches the native control.
    Bind(wxEVT_LEFT_DOWN, &clFileViewerTreeCtrl::OnLeftDown, this);
    // Right-click must update selection/current item before the context menu runs;
    // otherwise GTK may only move the visual cursor while commands still use the
    // previous selection (e.g. "Open Shell").
    Bind(wxEVT_RIGHT_DOWN, &clFileViewerTreeCtrl::OnRightDown, this);
}

void clFileViewerTreeCtrl::SelectItemForContext(const wxDataViewItem& item)
{
    if (!item.IsOk()) {
        return;
    }

    // Standard tree UX: right-click outside the selection selects only that item.
    // Right-click inside an existing multi-selection keeps the multi-selection.
    if (!IsSelected(item)) {
        UnselectAll();
        Select(item);
    }
    SetCurrentItem(item);

    wxDataViewEvent selectionEvent(wxEVT_DATAVIEW_SELECTION_CHANGED, this, item);
    ProcessWindowEvent(selectionEvent);
    SetFocus();
}

void clFileViewerTreeCtrl::OnRightDown(wxMouseEvent& event)
{
    wxDataViewItem item;
    wxDataViewColumn* column = nullptr;
    HitTest(event.GetPosition(), item, column);
    if (item.IsOk()) {
        SelectItemForContext(item);
    }
    event.Skip();
}

void clFileViewerTreeCtrl::OnLeftDown(wxMouseEvent& event)
{
    wxDataViewItem item;
    wxDataViewColumn* column = nullptr;
    HitTest(event.GetPosition(), item, column);

    if (item.IsOk()) {
        // Capture expand state only on the first click of a double-click sequence.
        // The second LEFT_DOWN of a double-click must keep the original snapshot;
        // otherwise activate can see a mid-sequence state and "skip" a toggle.
        const wxLongLong now = wxGetLocalTimeMillis();
        const bool newSequence =
            !m_lastClickValid || item != m_lastClickItem || (now - m_lastClickTime) > DoubleClickIntervalMs();
        if (newSequence) {
            m_lastClickItem = item;
            m_lastClickWasExpanded = IsExpanded(item);
            m_lastClickValid = true;
            m_lastClickTime = now;
        }
    } else {
        ClearLastClickSnapshot();
    }

    if (event.CmdDown() || event.ShiftDown() || event.AltDown()) {
        // let modifier-driven range/toggle selection go through the native control
        event.Skip();
        return;
    }

    if (!item.IsOk()) {
        event.Skip();
        return;
    }

    if (!IsSelected(item) || GetSelectedItemsCount() <= 1) {
        // nothing ambiguous here - let the native control handle it normally
        event.Skip();
        return;
    }

    // The clicked item is already part of a larger selection: collapse the
    // selection to just this item ourselves, and consume the event so it never
    // reaches the native control's own (slow, in this scenario) click handling.
    UnselectAll();
    Select(item);
    SetCurrentItem(item);

    wxDataViewEvent selectionEvent(wxEVT_DATAVIEW_SELECTION_CHANGED, this, item);
    ProcessWindowEvent(selectionEvent);

    SetFocus();
}

void clFileViewerTreeCtrl::SetBitmaps(std::vector<wxBitmap>* bitmaps)
{
    wxWithImages::Images images;
    if (bitmaps) {
        images.reserve(bitmaps->size());
        for (const auto& bmp : *bitmaps) {
            images.push_back(wxBitmapBundle::FromBitmap(bmp));
        }
    }
    SetImages(images);
}

bool clFileViewerTreeCtrl::ShouldComeBefore(clTreeCtrlData* a, clTreeCtrlData* b) const
{
    auto GetItemScore = [](clTreeCtrlData* item) -> size_t {
        size_t score = 0;
        if (item->IsFolder() || item->IsDummy()) {
            score += 100;
        }

        if (!item->GetName().empty()) {
            auto ch = item->GetName()[0];
            if (ch == '_' || ch == '.') {
                // Hidden file
                score += 10;
            }
        }
        return score;
    };

    size_t scoreA = GetItemScore(a);
    size_t scoreB = GetItemScore(b);
    if (scoreA != scoreB) {
        return scoreA > scoreB;
    }
    return a->GetName().CmpNoCase(b->GetName()) < 0;
}

wxDataViewItem clFileViewerTreeCtrl::InsertSorted(const wxDataViewItem& parent,
                                                  const wxString& text,
                                                  int icon,
                                                  int expandedIcon,
                                                  bool isContainer,
                                                  wxClientData* data)
{
    clTreeCtrlData* newData = static_cast<clTreeCtrlData*>(data);
    int count = GetChildCount(parent);

    // Despite its name, wxDataViewTreeStore::InsertItem(parent, "previous", ...) inserts the new
    // item immediately BEFORE "previous" in the child list. Find the first existing child that
    // should come after the new item and use it as that anchor.
    wxDataViewItem nextItem;
    for (int i = 0; i < count; ++i) {
        wxDataViewItem child = GetNthChild(parent, i);
        clTreeCtrlData* childData = GetItemData(child);
        if (childData && ShouldComeBefore(newData, childData)) {
            nextItem = child;
            break;
        }
    }

    if (!nextItem.IsOk()) {
        // the new item goes at the end (this also covers the "no children yet" case)
        return isContainer ? AppendContainer(parent, text, icon, expandedIcon, data)
                           : AppendItem(parent, text, icon, data);
    }
    return isContainer ? InsertContainer(parent, nextItem, text, icon, expandedIcon, data)
                       : InsertItem(parent, nextItem, text, icon, data);
}

wxDataViewItem clTreeNodeIndex::Find(const wxString& path)
{
#ifdef __WXMSW__
    wxString lcpath = path.Lower();
    if (m_children.count(lcpath)) {
        return m_children.find(lcpath)->second;
    }
#else
    if (m_children.count(path)) {
        return m_children.find(path)->second;
    }
#endif
    return wxDataViewItem();
}

void clTreeNodeIndex::Add(const wxString& path, const wxDataViewItem& item)
{
    m_children.insert({
#ifdef __WXMSW__
        path.Lower()
#else
        path
#endif
            ,
        item});
}

void clTreeNodeIndex::Delete(const wxString& name)
{
    m_children.erase(
#ifdef __WXMSW__
        name.Lower()
#else
        name
#endif
    );
}
