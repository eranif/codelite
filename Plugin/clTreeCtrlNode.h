#ifndef CLTREECTRLNODE_H
#define CLTREECTRLNODE_H

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
};

struct WXDLLIMPEXP_SDK clTreeCtrlColours {
    wxColour hoverBgColour;       // Background colour of an hovered item
    wxColour itemTextColour;      // item text colour
    wxColour itemBgColour;        // item bg colour
    wxColour selItemTextColour;   // text colour for the selected item
    wxColour selItemBgColour;     // selected item background colour
    wxColour buttonColour;        // expand/collapse button colour
    wxColour bgColour;            // background colour for the control
    wxColour scrolBarButton;      // The scrollbar thumb button colour
    wxColour scrollBarBgColour;   // The scrollbar background colour
    wxColour alternateColourOdd;  // Colour to draw odd items background (wxTR_ROW_LINES)
    wxColour alternateColourEven; // Colour to draw even items background (wxTR_ROW_LINES)
    clTreeCtrlColours() { InitDefaults(); }
    void InitDefaults();
    void InitDarkDefaults();
};

class WXDLLIMPEXP_SDK clTreeCtrlNode
{
public:
    typedef std::vector<clTreeCtrlNode*> Vec_t;
#ifdef __WXOSX__
    static const int Y_SPACER = 1;
    static const int X_SPACER = 1;
#else
    static const int Y_SPACER = 2;
    static const int X_SPACER = 2;
#endif
protected:
    clTreeCtrl* m_tree = nullptr;
    clTreeCtrlModel* m_model = nullptr;
    wxString m_label;
    int m_bitmapIndex = wxNOT_FOUND;
    int m_bitmapSelectedIndex = wxNOT_FOUND;
    size_t m_flags = 0;
    wxTreeItemData* m_clientData = nullptr;
    clTreeCtrlNode* m_parent = nullptr;
    clTreeCtrlNode::Vec_t m_children;
    clTreeCtrlNode* m_next = nullptr;
    clTreeCtrlNode* m_prev = nullptr;
    int m_indentsCount = 0;
    wxRect m_itemRect;
    wxRect m_buttonRect;
    wxFont m_font;
    wxColour m_textColour;
    wxColour m_bgColour;

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
    clTreeCtrlNode* GetVisibleItem(int index);

public:
    clTreeCtrlNode* GetLastChild() const;
    clTreeCtrlNode* GetFirstChild() const;

    clTreeCtrlNode(clTreeCtrl* tree);
    clTreeCtrlNode(
        clTreeCtrl* tree, const wxString& label, int bitmapIndex = wxNOT_FOUND, int bitmapSelectedIndex = wxNOT_FOUND);
    ~clTreeCtrlNode();

    clTreeCtrlNode* GetNext() const { return m_next; }
    clTreeCtrlNode* GetPrev() const { return m_prev; }

    void SetHidden(bool b);
    bool IsHidden() const { return HasFlag(kNF_Hidden); }

    bool IsVisible() const;
    void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
    void SetFont(const wxFont& font) { this->m_font = font; }
    void SetTextColour(const wxColour& textColour) { this->m_textColour = textColour; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    const wxFont& GetFont() const { return m_font; }
    const wxColour& GetTextColour() const { return m_textColour; }
    /**
     * @brief remove and delete a single child
     * @param child
     */
    void DeleteChild(clTreeCtrlNode* child);
    /**
     * @brief remove all children items
     */
    void DeleteAllChildren();
    void Render(wxDC& dc, const clTreeCtrlColours& colours, int visibileIndex);
    void SetHovered(bool b) { SetFlag(kNF_Hovered, b); }
    bool IsHovered() const { return m_flags & kNF_Hovered; }

    void ClearRects();
    void SetRects(const wxRect& rect, const wxRect& buttonRect)
    {
        m_itemRect = rect;
        m_buttonRect = buttonRect;
    }
    const wxRect& GetItemRect() const { return m_itemRect; }
    const wxRect& GetButtonRect() const { return m_buttonRect; }

    void AddChild(clTreeCtrlNode* child);

    /**
     * @brief insert item at 'where'. The new item is placed after 'prev'
     */
    void InsertChild(clTreeCtrlNode* child, clTreeCtrlNode* prev);

    /**
     * @brief insert this node between first and second
     */
    void ConnectNodes(clTreeCtrlNode* first, clTreeCtrlNode* second);

    bool IsBold() const { return HasFlag(kNF_FontBold); }
    void SetBold(bool b) { SetFlag(kNF_FontBold, b); }

    bool IsItalic() const { return HasFlag(kNF_FontItalic); }
    void SetItalic(bool b) { SetFlag(kNF_FontItalic, b); }

    bool IsExpanded() const { return HasFlag(kNF_Expanded) || HasFlag(kNF_Hidden); }
    bool SetExpanded(bool b);
    bool IsRoot() const { return GetParent() == nullptr; }
    void SetBitmapIndex(int bitmapIndex) { this->m_bitmapIndex = bitmapIndex; }
    int GetBitmapIndex() const { return m_bitmapIndex; }
    void SetBitmapSelectedIndex(int bitmapIndex) { this->m_bitmapSelectedIndex = bitmapIndex; }
    int GetBitmapSelectedIndex() const { return m_bitmapSelectedIndex; }

    const std::vector<clTreeCtrlNode*>& GetChildren() const { return m_children; }
    wxTreeItemData* GetClientObject() const { return m_clientData; }
    void SetParent(clTreeCtrlNode* parent);
    clTreeCtrlNode* GetParent() const { return m_parent; }
    bool HasChildren() const { return !m_children.empty(); }
    void SetClientData(wxTreeItemData* clientData)
    {
        wxDELETE(m_clientData);
        this->m_clientData = clientData;
    }
    void SetLabel(const wxString& label) { this->m_label = label; }
    const wxString& GetLabel() const { return m_label; }
    size_t GetChildrenCount(bool recurse) const;
    int GetExpandedLines() const;
    void GetNextItems(int count, clTreeCtrlNode::Vec_t& items);
    void GetPrevItems(int count, clTreeCtrlNode::Vec_t& items);
    void SetIndentsCount(int count) { this->m_indentsCount = count; }
    int GetIndentsCount() const { return m_indentsCount; }

    bool IsSelected() const { return HasFlag(kNF_Selected); }
    void SetSelected(bool b) { SetFlag(kNF_Selected, b); }
    void UnselectAll();
    bool IsSorted() const { return HasFlag(kNF_SortItems); }
};

#endif // CLTREECTRLNODE_H
