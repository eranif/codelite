#ifndef CLTREECTRL_H
#define CLTREECTRL_H

#include "clTreeCtrlModel.h"
#include "codelite_exports.h"
#include <wx/arrstr.h>
#include <wx/datetime.h>
#include <wx/dc.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>

class clScrollBarHelper;
class WXDLLIMPEXP_SDK clTreeCtrl : public wxPanel
{
    int m_lineHeight = 0;
    clTreeCtrlModel m_model;
#ifdef __WXOSX__
    int m_scrollTick = 2;
#else
    int m_scrollTick = 3;
#endif
    std::vector<wxBitmap> m_bitmaps;
    clTreeCtrlColours m_colours;
    long m_treeStyle = 0;
    clScrollBarHelper* m_vsb = nullptr;
    wxDirection m_lastScrollDir = wxDOWN;
    wxDateTime m_dragStartTime;
    wxPoint m_dragStartPos;

private:
    wxPoint DoFixPoint(const wxPoint& pt);
    wxTreeItemId DoGetSiblingVisibleItem(const wxTreeItemId& item, bool next) const;
    bool IsItemVisible(clTreeCtrlNode* item) const;
    void EnsureItemVisible(clTreeCtrlNode* item, bool fromTop);
    int GetNumLineCanFitOnScreen() const;
    clTreeCtrlNode* GetFirstItemOnScreen();
    void SetFirstItemOnScreen(clTreeCtrlNode* item);
    void UpdateScrollBar();
    wxTreeItemId DoScrollLines(int numLines, bool up, wxTreeItemId from, bool selectIt);

protected:
    void OnBeginDrag();

public:
    clTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clTreeCtrl();

    /**
     * @brief the drop action
     */
    void DropOnItem(const wxTreeItemId& item);

    // For internal use, dont use these two methods
    const clTreeCtrlModel& GetModel() const { return m_model; }
    clTreeCtrlModel& GetModel() { return m_model; }

    /**
     * @brief set a sorting function for this tree. The function returns true if the first element should be placed
     * before the second element
     */
    void SetSortFunction(const std::function<bool(const wxTreeItemId&, const wxTreeItemId&)>& CompareFunc);

    /**
     * @brief associate bitmap vector with this tree
     */
    void SetBitmaps(const std::vector<wxBitmap>& bitmaps);
    const std::vector<wxBitmap>& GetBitmaps() const { return m_bitmaps; }

    /**
     * @brief return the tree style
     */
    long GetTreeStyle() const { return m_treeStyle; }

    /**
     * @brief is the root hidden?
     */
    bool IsRootHidden() const { return m_treeStyle & wxTR_HIDE_ROOT; }

    /**
     * @brief set the colours used for drawing items
     */
    void SetColours(const clTreeCtrlColours& colours);

    /**
     * @brief enable style on the tree
     */
    void EnableStyle(int style, bool enable, bool refresh = true);

    /**
     * @brief does the tree has 'style' enabled?
     */
    bool HasStyle(int style) const { return m_treeStyle & style; }

    /**
     * @brief get the colours used for drawing items
     */
    const clTreeCtrlColours& GetColours() const { return m_colours; }

    /**
     * @brief return line height
     */
    int GetLineHeight() const { return m_lineHeight; }

    /**
     * @brief return bitmap at give index, return wxNullBitmap on error
     */
    const wxBitmap& GetBitmap(size_t index) const;
    /**
     * @brief Calculates which (if any) item is under the given point, returning the tree item id at this point plus
     *  extra information flags.
     *  flags is a bitlist of the following:
     *  wxTREE_HITTEST_NOWHERE: In the client area but below the last item.
     *  wxTREE_HITTEST_ONITEMBUTTON: On the button associated with an item.
     *  wxTREE_HITTEST_ONITEMICON: On the bitmap associated with an item.
     *  wxTREE_HITTEST_ONITEMLABEL: On the label (string) associated with an item.
     */
    wxTreeItemId HitTest(const wxPoint& point, int& flags) const;

    /**
     * @brief ppends an item to the end of the branch identified by parent, return a new item id.
     */
    wxTreeItemId AppendItem(const wxTreeItemId& parent, const wxString& text, int image = -1, int selImage = -1,
        wxTreeItemData* data = NULL);
    /**
     * @brief Adds the root node to the tree, returning the new item.
     */
    wxTreeItemId AddRoot(const wxString& text, int image = -1, int selImage = -1, wxTreeItemData* data = NULL);

    /**
     * @brief insert item after 'previous'
     */
    wxTreeItemId InsertItem(const wxTreeItemId& parent, const wxTreeItemId& previous, const wxString& text,
        int image = -1, int selImage = -1, wxTreeItemData* data = NULL);
    /**
     * @brief return the root item
     */
    wxTreeItemId GetRootItem() const;

    /**
     * @brief return the item's parent
     */
    wxTreeItemId GetItemParent(const wxTreeItemId& item) const;

    /**
     * @brief Expands the given item
     */
    void Expand(const wxTreeItemId& item);

    /**
     * @brief Collapses the given item
     */
    void Collapse(const wxTreeItemId& item);

    /**
     * @brief Selects the given item
     */
    void SelectItem(const wxTreeItemId& item, bool select = true);

    // Selection
    wxTreeItemId GetSelection() const;
    wxTreeItemId GetFocusedItem() const;
    size_t GetSelections(wxArrayTreeItemIds& selections) const;

    /**
     * @brief unselect all items
     */
    void UnselectAll();

    /**
     * @brief Scrolls and/or expands items to ensure that the given item is visible.
     */
    void EnsureVisible(const wxTreeItemId& item);

    /**
     * @brief is item expanded?
     */
    bool IsExpanded(const wxTreeItemId& item) const;

    /**
     * @brief Returns true if the item has children.
     */
    bool ItemHasChildren(const wxTreeItemId& item) const;

    /**
     * @brief set the item's indent size
     */
    void SetIndent(int size);

    /**
     * @brief return the current indent size
     */
    int GetIndent() const;

    /**
     * @brief is the tree has items? (root included)
     */
    bool IsEmpty() const;

    /**
     * @brief return the children count of this item
     */
    size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = true) const;

    /**
     * @brief Delete all the item's children. No events are fired
     */
    void DeleteChildren(const wxTreeItemId& item);
    /**
     * @brief Returns the first child; call GetNextChild() for the next child.
     */
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;

    /**
     * @brief for compatibility, we dont really need to call this method manually
     */
    void SortChildren(const wxTreeItemId& item) { wxUnusedVar(item); }

    /**
     * @brief set item's image index
     */
    void SetItemImage(const wxTreeItemId& item, int imageId, int openImageId = wxNOT_FOUND);

    /**
     * @brief return the associated image id with this item
     */
    int GetItemImage(const wxTreeItemId& item, bool selectedImage = false) const;
    /**
     * @brief Returns the first visible item
     */
    wxTreeItemId GetFirstVisibleItem() const;

    /**
     * @brief Returns the next visible item or an invalid item if this item is the last visible one
     */
    wxTreeItemId GetNextVisible(const wxTreeItemId& item) const;

    /**
     * @brief Returns the prev visible item or an invalid item if this item is the first visible one
     */
    wxTreeItemId GetPrevVisible(const wxTreeItemId& item) const;

    //===--------------------------------
    // Item properties
    //===--------------------------------
    wxTreeItemData* GetItemData(const wxTreeItemId& item) const;
    void SetItemData(const wxTreeItemId& item, wxTreeItemData* data);

    void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& colour);
    wxColour GetItemBackgroudColour(const wxTreeItemId& item) const;

    void SetItemTextColour(const wxTreeItemId& item, const wxColour& colour);
    wxColour GetItemTextColour(const wxTreeItemId& item) const;

    void SetItemText(const wxTreeItemId& item, const wxString& text);
    wxString GetItemText(const wxTreeItemId& item) const;

    void SetItemBold(const wxTreeItemId& item, bool bold);

    void SetItemFont(const wxTreeItemId& item, const wxFont& font);
    wxFont GetItemFont(const wxTreeItemId& item) const;

    /**
     * @brief expand this item and all its children
     */
    void ExpandAllChildren(const wxTreeItemId& item);

    /**
     * @brief expand the entire tree
     */
    void ExpandAll() { ExpandAllChildren(GetRootItem()); }

    /**
     * @brief expand this item and all its children
     */
    void CollapseAllChildren(const wxTreeItemId& item);

    /**
     * @brief expand the entire tree
     */
    void CollapAll() { CollapseAllChildren(GetRootItem()); }

    /**
     * @brief Deletes the specified item.
     * A EVT_TREE_DELETE_ITEM event will be generated.
     */
    void Delete(const wxTreeItemId& item);

    /**
     * @brief Select all the immediate children of the given parent
     */
    void SelectChildren(const wxTreeItemId& item);

    /**
     * @brief Returns the next sibling of the specified item; call GetPrevSibling() for the previous sibling
     */
    wxTreeItemId GetNextSibling(const wxTreeItemId& item) const;

    /**
     * @brief Returns the previous sibling of the specified item; call GetNextSibling() for the next sibling
     */
    wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const;

protected:
    void DoEnsureVisible(const wxTreeItemId& item);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnMotion(wxMouseEvent& event);
    void OnMouseLeftDClick(wxMouseEvent& event);
    void OnMouseScroll(wxMouseEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
    void OnEnterWindow(wxMouseEvent& event);
    void OnCharHook(wxKeyEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    /**
     * @brief scroll events sent from the scrollbar
     */
    void OnScroll(wxScrollEvent& event);
};

#endif // CLTREECTRL_H
