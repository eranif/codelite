#ifndef CLTREECTRLNODE_H
#define CLTREECTRLNODE_H

#include "clCellValue.h"
#include "clColours.h"
#include "codelite_exports.h"

#include <array>
#include <unordered_map>
#include <vector>
#include <wx/colour.h>
#include <wx/gdicmn.h>
#include <wx/sharedptr.h>
#include <wx/string.h>
#include <wx/treebase.h>

class clSearchText;
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
    kNF_HighlightText = (1 << 8),
};

typedef std::array<wxString, 3> Str3Arr_t;
struct WXDLLIMPEXP_SDK clMatchResult {
    std::unordered_map<size_t, Str3Arr_t> matches;

    bool Get(size_t col, Str3Arr_t& arr) const
    {
        if(matches.count(col) == 0) {
            return false;
        }
        arr = matches.find(col)->second;
        return true;
    }
    void Add(size_t col, const Str3Arr_t& arr)
    {
        matches.erase(col);
        matches[col] = arr;
    }

    void Clear() { matches.clear(); }
};

class WXDLLIMPEXP_SDK clRowEntry
{
public:
    typedef std::vector<clRowEntry*> Vec_t;
    static int Y_SPACER;
    static int X_SPACER;

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
    clMatchResult m_higlightInfo;

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
    void DrawSimpleSelection(wxWindow* win, wxDC& dc, const wxRect& rect, const clColours& colours);
    void RenderText(wxWindow* win, wxDC& dc, const clColours& colours, const wxString& text, int x, int y, size_t col);
    void RenderTextSimple(wxWindow* win, wxDC& dc, const clColours& colours, const wxString& text, int x, int y,
                          size_t col);
    void RenderCheckBox(wxWindow* win, wxDC& dc, const clColours& colours, const wxRect& rect, bool checked);
    int GetCheckBoxWidth(wxWindow* win);

public:
    clRowEntry* GetLastChild() const;
    clRowEntry* GetFirstChild() const;

    size_t GetColumnCount() const { return m_cells.size(); }

    const clCellValue& GetColumn(size_t col = 0) const;
    clCellValue& GetColumn(size_t col = 0);
    clRowEntry(clTreeCtrl* tree, const wxString& label, int bitmapIndex = wxNOT_FOUND,
               int bitmapSelectedIndex = wxNOT_FOUND);
    clRowEntry(clTreeCtrl* tree, bool checked, const wxString& label, int bitmapIndex = wxNOT_FOUND,
               int bitmapSelectedIndex = wxNOT_FOUND);
    ~clRowEntry();
    /**
     * @brief return the item rectangle
     * @return
     */
    wxRect GetCellRect(size_t col = 0) const;
    clRowEntry* GetNext() const { return m_next; }
    clRowEntry* GetPrev() const { return m_prev; }
    void SetNext(clRowEntry* next) { this->m_next = next; }
    void SetPrev(clRowEntry* prev) { this->m_prev = prev; }
    void SetHighlightInfo(const clMatchResult& info) { m_higlightInfo = info; }
    const clMatchResult& GetHighlightInfo() const { return m_higlightInfo; }
    void SetHidden(bool b);
    bool IsHidden() const { return HasFlag(kNF_Hidden); }
    void SetHighlight(bool b) { SetFlag(kNF_HighlightText, b); }
    bool IsHighlight() const { return HasFlag(kNF_HighlightText); }
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
    void Render(wxWindow* win, wxDC& dc, const clColours& c, int row_index, clSearchText* searcher);
    void RenderBackground(wxDC& dc, long tree_style, const clColours& c, int row_index);
    std::vector<size_t> GetColumnWidths(wxWindow* win, wxDC& dc);
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
    const wxRect& GetCheckboxRect(size_t col = 0) const;
    const wxRect& GetCellButtonRect(size_t col) const;

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
    /**
     * @brief add button to the right of the cell
     */
    void SetHasButton(eCellButtonType button_type, const wxString& unicode_symbol, size_t col = 0);
    bool HasButton(size_t col) const;

    /**
     * @brief mark cell at column as a button cell
     * @param label the button label
     */
    void SetIsButton(const wxString& label, size_t col = 0);
    /**
     * @brief is cell at given column is a button?
     */
    bool IsButton(size_t col) const;

    void SetColour(const wxColour& colour, size_t col = 0);
    bool IsColour(size_t col) const;

    // Set this cell as "checkable" cell with possible label
    void SetChecked(bool checked, int bitmapIndex, const wxString& label, size_t col = 0);
    bool IsChecked(size_t col = 0) const;

    int GetBitmapIndex(size_t col = 0) const;
    int GetBitmapSelectedIndex(size_t col = 0) const;
    const wxString& GetLabel(size_t col = 0) const;

    const std::vector<clRowEntry*>& GetChildren() const { return m_children; }
    std::vector<clRowEntry*>& GetChildren() { return m_children; }
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
    void GetNextItems(int count, clRowEntry::Vec_t& items, bool selfIncluded = true);
    void GetPrevItems(int count, clRowEntry::Vec_t& items, bool selfIncluded = true);
    void SetIndentsCount(int count) { this->m_indentsCount = count; }
    int GetIndentsCount() const { return m_indentsCount; }

    bool IsSelected() const { return HasFlag(kNF_Selected); }
    void SetSelected(bool b) { SetFlag(kNF_Selected, b); }
    void UnselectAll();
    bool IsSorted() const { return HasFlag(kNF_SortItems); }
};

#endif // CLTREECTRLNODE_H
