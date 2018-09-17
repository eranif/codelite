#ifndef CLTREECTRLNODE_H
#define CLTREECTRLNODE_H

#include "clCellValue.h"
#include "clColours.h"
#include "codelite_exports.h"
#include <vector>
#include <wx/colour.h>
#include <wx/gdicmn.h>
#include <wx/sharedptr.h>
#include <wx/string.h>
#include <wx/treebase.h>

class clTreeCtrlModel;
class clTreeCtrl;
enum clTreeCtrlNodeFlags {
    kNF_FontBold = (1 << 0),
    kNF_FontItalic = (1 << 1),
    kNF_SortItems = (1 << 2),
    kNF_Expanded = (1 << 3),
    kNF_Selected = (1 << 4),
    kNF_Hovered = (1 << 5),
    kNF_Hidden = (1 << 6),
    kNF_LisItem = (1 << 7),
};

class WXDLLIMPEXP_SDK clRowEntry
{

public:
    typedef std::vector<clRowEntry*> Vec_t;
    static const int Y_SPACER = 1;
    static const int X_SPACER = 3;

protected:
    clTreeCtrl* m_tree = nullptr;
    clTreeCtrlModel* m_model = nullptr;
    clCellValue::Vect_t m_cells;
    size_t m_flags = 0;
    wxTreeItemData* m_clientObject = nullptr;
    wxUIntPtr m_data = 0;
    clRowEntry* m_parent = nullptr;
    clRowEntry::Vec_t m_children;
    clRowEntry* m_next = nullptr;
    clRowEntry* m_prev = nullptr;
    int m_indentsCount = 0;
    wxRect m_rowRect;
    wxRect m_buttonRect;

protected:
    void SetFlag(clTreeCtrlNodeFlags flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

    bool HasFlag(clTreeCtrlNodeFlags flag) const { return m_flags & flag; }

    /**
     * @brief return the nth visible item
     */
    clRowEntry* GetVisibleItem(int index);
    clCellValue& GetColumn(size_t col = 0);
    const clCellValue& GetColumn(size_t col = 0) const;
    void DrawSelection(bool focused, wxDC& dc, const wxRect& rect, const clColours& colours);
    void DrawSimpleSelection(bool focused, wxDC& dc, const wxRect& rect, const clColours& colours);
    void DrawNativeSelection(bool focused, wxDC& dc, const wxRect& rect, const clColours& colours);

public:
    clRowEntry* GetLastChild() const;
    clRowEntry* GetFirstChild() const;

    clRowEntry(clTreeCtrl* tree, const wxString& label, int bitmapIndex = wxNOT_FOUND,
               int bitmapSelectedIndex = wxNOT_FOUND);
    ~clRowEntry();

    clRowEntry* GetNext() const { return m_next; }
    clRowEntry* GetPrev() const { return m_prev; }

    void SetHidden(bool b);
    bool IsHidden() const { return HasFlag(kNF_Hidden); }

    void SetData(wxUIntPtr data) { this->m_data = data; }
    wxUIntPtr GetData() { return m_data; }

    /**
     * @brief using wxDC, calculate the item's width
     */
    int CalcItemWidth(wxDC& dc, int rowHeight, size_t col = 0);
    bool IsListItem() const { return m_flags & kNF_LisItem; }
    void SetListItem(bool b) { SetFlag(kNF_LisItem, b); }
    bool IsVisible() const;
    void SetBgColour(const wxColour& bgColour, size_t col = 0);
    void SetFont(const wxFont& font, size_t col = 0);
    void SetTextColour(const wxColour& textColour, size_t col = 0);
    const wxColour& GetBgColour(size_t col = 0) const;
    const wxFont& GetFont(size_t col = 0) const;
    const wxColour& GetTextColour(size_t col = 0) const;
    /**
     * @brief remove and delete a single child
     * @param child
     */
    void DeleteChild(clRowEntry* child);
    /**
     * @brief remove all children items
     */
    void DeleteAllChildren();
    void Render(wxWindow* win, wxDC& dc, const clColours& colours, int row_index);
    void SetHovered(bool b) { SetFlag(kNF_Hovered, b); }
    bool IsHovered() const { return m_flags & kNF_Hovered; }

    void ClearRects();
    void SetRects(const wxRect& rect, const wxRect& buttonRect)
    {
        m_rowRect = rect;
        m_buttonRect = buttonRect;
    }
    const wxRect& GetItemRect() const { return m_rowRect; }
    const wxRect& GetButtonRect() const { return m_buttonRect; }

    void AddChild(clRowEntry* child);

    /**
     * @brief insert item at 'where'. The new item is placed after 'prev'
     */
    void InsertChild(clRowEntry* child, clRowEntry* prev);

    /**
     * @brief insert this node between first and second
     */
    void ConnectNodes(clRowEntry* first, clRowEntry* second);

    bool IsBold() const { return HasFlag(kNF_FontBold); }
    void SetBold(bool b) { SetFlag(kNF_FontBold, b); }

    bool IsItalic() const { return HasFlag(kNF_FontItalic); }
    void SetItalic(bool b) { SetFlag(kNF_FontItalic, b); }

    bool IsExpanded() const { return HasFlag(kNF_Expanded) || HasFlag(kNF_Hidden); }
    bool SetExpanded(bool b);
    bool IsRoot() const { return GetParent() == nullptr; }

    // Cell accessors
    void SetBitmapIndex(int bitmapIndex, size_t col = 0);
    void SetBitmapSelectedIndex(int bitmapIndex, size_t col = 0);
    void SetLabel(const wxString& label, size_t col = 0);
    int GetBitmapIndex(size_t col = 0) const;
    int GetBitmapSelectedIndex(size_t col = 0) const;
    const wxString& GetLabel(size_t col = 0) const;

    const std::vector<clRowEntry*>& GetChildren() const { return m_children; }
    wxTreeItemData* GetClientObject() const { return m_clientObject; }
    void SetParent(clRowEntry* parent);
    clRowEntry* GetParent() const { return m_parent; }
    bool HasChildren() const { return !m_children.empty(); }
    void SetClientData(wxTreeItemData* clientData)
    {
        wxDELETE(m_clientObject);
        this->m_clientObject = clientData;
    }
    size_t GetChildrenCount(bool recurse) const;
    int GetExpandedLines() const;
    void GetNextItems(int count, clRowEntry::Vec_t& items);
    void GetPrevItems(int count, clRowEntry::Vec_t& items);
    void SetIndentsCount(int count) { this->m_indentsCount = count; }
    int GetIndentsCount() const { return m_indentsCount; }

    bool IsSelected() const { return HasFlag(kNF_Selected); }
    void SetSelected(bool b) { SetFlag(kNF_Selected, b); }
    void UnselectAll();
    bool IsSorted() const { return HasFlag(kNF_SortItems); }
};

#endif // CLTREECTRLNODE_H
