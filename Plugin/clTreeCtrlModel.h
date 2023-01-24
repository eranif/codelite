#ifndef CLTREECTRLMODEL_H
#define CLTREECTRLMODEL_H

#include "clRowEntry.h"
#include "codelite_exports.h"

#include <functional>
#include <vector>
#include <wx/colour.h>
#include <wx/sharedptr.h>
#include <wx/string.h>
#include <wx/treebase.h>

class clTreeCtrl;
typedef std::function<bool(clRowEntry*, clRowEntry*)> clSortFunc_t;
class WXDLLIMPEXP_SDK clTreeCtrlModel
{
    clTreeCtrl* m_tree = nullptr;
    clRowEntry* m_root = nullptr;
    clRowEntry::Vec_t m_selectedItems;
    clRowEntry::Vec_t m_onScreenItems;
    clRowEntry* m_firstItemOnScreen = nullptr;
    int m_indentSize = 16;
    bool m_shutdown = false;
    clSortFunc_t m_shouldInsertBeforeFunc = nullptr;

protected:
    void DoExpandAllChildren(const wxTreeItemId& item, bool expand);
    bool IsSingleSelection() const;
    bool IsMultiSelection() const;
    bool SendEvent(wxEvent& event);

public:
    clTreeCtrlModel(clTreeCtrl* tree);
    ~clTreeCtrlModel();

    void SetFirstItemOnScreen(clRowEntry* firstItemOnScreen) { this->m_firstItemOnScreen = firstItemOnScreen; }
    clRowEntry* GetFirstItemOnScreen() const { return m_firstItemOnScreen; }

    void SetSortFunction(const clSortFunc_t& CompareFunc) { m_shouldInsertBeforeFunc = CompareFunc; }
    clSortFunc_t GetSortFunction() const { return m_shouldInsertBeforeFunc; }

    void ExpandAllChildren(const wxTreeItemId& item);
    void CollapseAllChildren(const wxTreeItemId& item);

    // Notifications from the node
    void NodeDeleted(clRowEntry* node);
    void NodeExpanded(clRowEntry* node, bool expanded);
    bool NodeExpanding(clRowEntry* node, bool expanding);

    void GetNextItems(clRowEntry* from, int count, clRowEntry::Vec_t& items, bool selfIncluded = true) const;
    void GetPrevItems(clRowEntry* from, int count, clRowEntry::Vec_t& items, bool selfIncluded = true) const;
    wxTreeItemId AddRoot(const wxString& text, int image, int selImage, wxTreeItemData* data);
    wxTreeItemId AppendItem(const wxTreeItemId& parent, const wxString& text, int image, int selImage,
                            wxTreeItemData* data);
    wxTreeItemId InsertItem(const wxTreeItemId& parent, const wxTreeItemId& previous, const wxString& text, int image,
                            int selImage, wxTreeItemData* data);
    wxTreeItemId GetRootItem() const;

    void SetIndentSize(int indentSize) { this->m_indentSize = indentSize; }
    int GetIndentSize() const { return m_indentSize; }

    /**
     * @brief clear the selection from all the items
     */
    void UnselectAll();

    wxTreeItemId GetItemBefore(const wxTreeItemId& item, bool visibleItem) const;
    wxTreeItemId GetItemAfter(const wxTreeItemId& item, bool visibleItem) const;
    clRowEntry* GetRowBefore(clRowEntry* item, bool visibleItem) const;
    clRowEntry* GetRowAfter(clRowEntry* item, bool visibleItem) const;
    clRowEntry* ToPtr(const wxTreeItemId& item) const
    {
        if(!m_root || !item.IsOk()) {
            return nullptr;
        }
        return reinterpret_cast<clRowEntry*>(item.GetID());
    }

    /**
     * @brief select a given item
     */
    void SelectItem(const wxTreeItemId& item, bool select = true, bool addSelection = false,
                    bool clear_old_selection = false);
    /**
     * @brief add an item to the selection list
     * this function fires an event
     */
    void AddSelection(const wxTreeItemId& item);

    /**
     * @brief clear all selections, return true on sucess, this function fires the changing event
     */
    bool ClearSelections(bool notify);

    bool IsItemSelected(const wxTreeItemId& item) const { return IsItemSelected(ToPtr(item)); }
    bool IsItemSelected(const clRowEntry* item) const;

    bool IsVisible(const wxTreeItemId& item) const;

    /**
     * @brief select the children of 'item' this functin fires the changing and changed events
     */
    void SelectChildren(const wxTreeItemId& item);

    void Clear();

    void SetOnScreenItems(const clRowEntry::Vec_t& items);

    const clRowEntry::Vec_t& GetOnScreenItems() const { return m_onScreenItems; }
    clRowEntry::Vec_t& GetOnScreenItems() { return m_onScreenItems; }
    const clRowEntry::Vec_t& GetSelections() const { return m_selectedItems; }
    bool ExpandToItem(const wxTreeItemId& item);
    wxTreeItemId GetSingleSelection() const;
    size_t GetSelectionsCount() const { return m_selectedItems.size(); }

    /**
     * @brief do we have items in this tree? (root included)
     */
    bool IsEmpty() const { return m_root == nullptr; }

    clRowEntry* GetRoot() const { return m_root; }

    /**
     * @brief delete subtree starting from 'item', including item
     * fires event wxEVT_TREE_DELETE_ITEM
     * @param item
     */
    void DeleteItem(const wxTreeItemId& item);
    int GetItemIndex(clRowEntry* item) const;
    clRowEntry* GetItemFromIndex(int index) const;

    /**
     * @brief return the last visible item
     * NOTE: this does not check if the item is actually is displayed on the screen
     * it only checks if the item is a state that allows it to be displayed on screen
     */
    clRowEntry* GetLastVisibleItem() const;

    /**
     * @brief get range of items from -> to
     * Or from: to->from (incase 'to' has a lower index)
     */
    bool GetRange(clRowEntry* from, clRowEntry* to, clRowEntry::Vec_t& items) const;

    size_t GetExpandedLines() const;

    clRowEntry* GetNextSibling(clRowEntry* item) const;
    clRowEntry* GetPrevSibling(clRowEntry* item) const;

    void EnableEvents(bool enable) { m_shutdown = !enable; }
};

#endif // CLTREECTRLMODEL_H
