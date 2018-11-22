#ifndef CLTREECTRL_H
#define CLTREECTRL_H

#include "clControlWithItems.h"
#include "clHeaderBar.h"
#include "clScrolledPanel.h"
#include "clTreeCtrlModel.h"
#include "codelite_exports.h"
#include <wx/arrstr.h>
#include <wx/datetime.h>
#include <wx/dc.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/headercol.h>

#define wxTR_ENABLE_SEARCH 0x4000

class clScrollBar;
class WXDLLIMPEXP_SDK clTreeCtrl : public clControlWithItems
{
protected:
    clTreeCtrlModel m_model;
    bool m_needToClearDefaultHeader = true;
    long m_treeStyle = 0;
    int m_scrollLines = 0;
    
private:
    wxPoint DoFixPoint(const wxPoint& pt);
    wxTreeItemId DoGetSiblingVisibleItem(const wxTreeItemId& item, bool next) const;
    bool IsItemVisible(clRowEntry* item) const;
    bool IsItemFullyVisible(clRowEntry* item) const;
    void EnsureItemVisible(clRowEntry* item, bool fromTop);
    clRowEntry* GetFirstItemOnScreen();
    void SetFirstItemOnScreen(clRowEntry* item);
    wxTreeItemId DoScrollLines(int numLines, bool up, wxTreeItemId from, bool selectIt);

    /**
     * @brief bitmap file was added, re-calculate the line heights
     */
    void DoBitmapAdded();

    /**
     * @brief update the header size
     */
    void DoUpdateHeader(const wxTreeItemId& item);

    void DoInitialize();
    clRowEntry* DoFind(clRowEntry* from, const wxString& what, size_t col, size_t searchFlags, bool next);
    
protected:
    void UpdateScrollBar();
    void DoAddHeader(const wxString& label, const wxBitmap& bmp, int width = wxCOL_WIDTH_AUTOSIZE);
    
public:
    virtual int GetFirstItemPosition() const;
    virtual int GetRange() const;
    clTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = 0);
    clTreeCtrl();
    virtual ~clTreeCtrl();
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0);

    //===--------------------
    // Search support
    //===--------------------
    /**
     * @brief search for an item from pos that matches the "what" string.
     * Pass an invalid item to start the search from the root
     */
    wxTreeItemId FindNext(const wxTreeItemId& from, const wxString& what, size_t col = 0,
                          size_t searchFlags = wxTR_SEARCH_DEFAULT);
    /**
     * @brief search for an item from pos that matches the "what" string.
     * Pass an invalid item to start the search from the root
     */
    wxTreeItemId FindPrev(const wxTreeItemId& from, const wxString& what, size_t col = 0,
                          size_t searchFlags = wxTR_SEARCH_DEFAULT);

    /**
     * @brief highlight matched string of an item. This call should be called after a successfull call to
     * FindNext or FindPrev
     */
    void HighlightText(const wxTreeItemId& item, bool b);

    /**
     * @brief clear highlighted text
     */
    void ClearHighlight(const wxTreeItemId& item);

    /**
     * @brief clear all highlighted text from all the items
     */
    void ClearAllHighlights();
    
    //===--------------------
    // table view support
    //===--------------------
    
    void AddHeader(const wxString& label, const wxBitmap& bmp = wxNullBitmap, int width = wxCOL_WIDTH_AUTOSIZE);
    
    // For internal use, dont use these two methods
    const clTreeCtrlModel& GetModel() const { return m_model; }
    clTreeCtrlModel& GetModel() { return m_model; }

    /**
     * @brief set a sorting function for this tree. The function returns true if the first element should be placed
     * before the second element
     */
    virtual void SetSortFunction(const clSortFunc_t& CompareFunc);

    /**
     * @brief associate bitmap vector with this tree. The bitmaps array must exists as long as this control exists
     */
    virtual void SetBitmaps(BitmapVec_t* bitmaps);

    /**
     * @brief return the tree style
     */
    long GetTreeStyle() const { return m_treeStyle; }

    /**
     * @brief is the root hidden?
     */
    bool IsRootHidden() const { return m_treeStyle & wxTR_HIDE_ROOT; }

    /**
     * @brief enable style on the tree
     */
    virtual void EnableStyle(int style, bool enable, bool refresh = true);

    /**
     * @brief does the tree has 'style' enabled?
     */
    bool HasStyle(int style) const { return m_treeStyle & style; }

    /**
     * @brief Calculates which (if any) item is under the given point, returning the tree item id at this point plus
     *  extra information flags.
     *  flags is a bitlist of the following (the bits are output returned by this function):
     *  wxTREE_HITTEST_NOWHERE: In the client area but below the last item.
     *  wxTREE_HITTEST_ONITEMBUTTON: On the button associated with an item.
     *  wxTREE_HITTEST_ONITEM
     */
    wxTreeItemId HitTest(const wxPoint& point, int& flags, int& column) const;

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
     * @brief check a given item (if it has checkbox)
     * This method fires 'wxEVT_TREE_ITEM_VALUE_CHANGED' event
     */
    void Check(const wxTreeItemId& item, bool check, size_t col = 0);
    
    /**
     * @brief is the state of an item is checked?
     */
    bool IsChecked(const wxTreeItemId& item, size_t col = 0) const;
    
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
    bool HasChildren(const wxTreeItemId& item) const { return ItemHasChildren(item); }

    /**
     * @brief set the item's indent size
     */
    void SetIndent(int size);

    /**
     * @brief return the current indent size
     */
    virtual int GetIndent() const;

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
     * @brief get the next item in tree (as if you were clicking DOWN in the keyboard)
     */
    wxTreeItemId GetNextItem(const wxTreeItemId& item) const;

    /**
     * @brief get the prev item in tree (as if you were clicking DOWN in the keyboard)
     */
    wxTreeItemId GetPrevItem(const wxTreeItemId& item) const;

    /**
     * @brief for compatibility, we dont really need to call this method manually
     */
    void SortChildren(const wxTreeItemId& item) { wxUnusedVar(item); }

    /**
     * @brief set item's image index
     */
    void SetItemImage(const wxTreeItemId& item, int imageId, int openImageId = wxNOT_FOUND, size_t col = 0);

    /**
     * @brief return the associated image id with this item
     */
    int GetItemImage(const wxTreeItemId& item, bool selectedImage = false, size_t col = 0) const;
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

    void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& colour, size_t col = 0);
    wxColour GetItemBackgroundColour(const wxTreeItemId& item, size_t col = 0) const;

    void SetItemTextColour(const wxTreeItemId& item, const wxColour& colour, size_t col = 0);
    wxColour GetItemTextColour(const wxTreeItemId& item, size_t col = 0) const;

    void SetItemText(const wxTreeItemId& item, const wxString& text, size_t col = 0);
    wxString GetItemText(const wxTreeItemId& item, size_t col = 0) const;

    void SetItemBold(const wxTreeItemId& item, bool bold, size_t col = 0);

    void SetItemFont(const wxTreeItemId& item, const wxFont& font, size_t col = 0);
    wxFont GetItemFont(const wxTreeItemId& item, size_t col = 0) const;

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
    void CollapseAll() { CollapseAllChildren(GetRootItem()); }

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

    /**
     * @brief delete all items in tree
     */
    virtual void DeleteAllItems();

    /**
     * @brief is this item visible?
     */
    bool IsVisible(const wxTreeItemId& item) const;
    /**
     * @brief is item selected?
     */
    bool IsSelected(const wxTreeItemId& item) const;

protected:
    virtual bool DoKeyDown(const wxKeyEvent& event);
    void DoEnsureVisible(const wxTreeItemId& item);
    void OnPaint(wxPaintEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnMouseLeftDClick(wxMouseEvent& event);
    void DoMouseScroll(const wxMouseEvent& event);
    void ProcessIdle();
    void OnLeaveWindow(wxMouseEvent& event);
    void OnEnterWindow(wxMouseEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);

    void ScrollRows(int steps, wxDirection direction);
    void ScrollToRow(int firstLine);

    wxTreeItemId GetRow(const wxPoint& pt) const;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TREE_ITEM_VALUE_CHANGED, wxTreeEvent);
#endif // CLTREECTRL_H
