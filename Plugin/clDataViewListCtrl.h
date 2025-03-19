#ifndef CLDATAVIEWLISTCTRL_H
#define CLDATAVIEWLISTCTRL_H

#include "clCellValue.h"
#include "clTreeCtrl.h"

#include <unordered_map>
#include <wx/dataview.h>

// Extra styles supported by this class
#define wxDV_ENABLE_SEARCH wxTR_ENABLE_SEARCH
#define wxDV_COLUMN_WIDTH_NEVER_SHRINKS wxTR_COLUMN_WIDTH_NEVER_SHRINKS

// Search flags
// See wxTR_SEARCH* for more info
#define wxDV_SEARCH_METHOD_EXACT wxTR_SEARCH_METHOD_EXACT
#define wxDV_SEARCH_METHOD_CONTAINS wxTR_SEARCH_METHOD_CONTAINS
#define wxDV_SEARCH_VISIBLE_ITEMS wxTR_SEARCH_VISIBLE_ITEMS
#define wxDV_SEARCH_ICASE wxTR_SEARCH_ICASE
#define wxDV_SEARCH_INCLUDE_CURRENT_ITEM wxTR_SEARCH_INCLUDE_CURRENT_ITEM
#define wxDV_SEARCH_DEFAULT wxTR_SEARCH_DEFAULT

enum class CellType {
    UNKNOWN = -1,
    TEXT = 0,
    COLOUR = 1,
    CHECKBOX_TEXT = 2,
    TEXT_EDIT = 3,
    TEXT_OPTIONS = 4,
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DATAVIEW_SELECTION_CHANGING, wxDataViewEvent);

/**
 * @brief a thin wrapper around clTreeCtrl which provides basic compatiblity API (such as adding columns)
 * This is mainly for code generators like wxCrafter
 */
class WXDLLIMPEXP_SDK clDataViewListCtrl : public clTreeCtrl
{
    wxDataViewListCtrl m_dummy; // Needed for generating wxDV compatible events

    static std::unordered_map<int, int> m_stylesMap;

protected:
    void OnConvertEvent(wxTreeEvent& event);
    bool SendDataViewEvent(const wxEventType& type, wxTreeEvent& treeEvent, const wxString& text = "");
    void DoSetCellValue(clRowEntry* row, size_t col, const wxVariant& value);

public:
    clDataViewListCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clDataViewListCtrl();

    void ScrollToBottom();

    /**
     * @brief return the cell data type
     */
    CellType GetCellDataType(size_t row, size_t col) const;
    CellType GetCellDataType(const wxDataViewItem& item, size_t col) const;

    /**
     * @brief make row the first visible row in the view
     */
    void SetFirstVisibleRow(size_t row);

    /**
     * @brief center row in the view
     */
    void CenterRow(size_t row);

    /**
     * @brief how many items can scroll in the view?
     */
    int GetRange() const override { return GetItemCount(); }
    bool IsEmpty() const override { return GetItemCount() == 0; }

    ///===--------------------
    /// Search support
    ///===--------------------
    /**
     * @brief search for an item from pos that matches the "what" string.
     * Pass an invalid item to start the search from the root
     */
    wxDataViewItem FindNext(const wxDataViewItem& from, const wxString& what, size_t col = 0,
                            size_t searchFlags = wxDV_SEARCH_DEFAULT);
    /**
     * @brief search for an item from pos that matches the "what" string.
     * Pass an invalid item to start the search from the root
     */
    wxDataViewItem FindPrev(const wxDataViewItem& from, const wxString& what, size_t col = 0,
                            size_t searchFlags = wxDV_SEARCH_DEFAULT);

    /**
     * @brief highlight matched string of an item. This call should be called after a successful call to
     * FindNext or FindPrev
     */
    void HighlightText(const wxDataViewItem& item, bool b);

    /**
     * @brief clear highlighted text
     */
    void ClearHighlight(const wxDataViewItem& item);

    ///===--------------------
    /// wxDV compatibility API
    ///===--------------------

    /**
     * @brief Scrolls to ensure that the given item is visible.
     */
    void EnsureVisible(const wxDataViewItem& item);

    /**
     * @brief appends an item to the end of the list
     */
    wxDataViewItem AppendItem(const wxString& text, int image = -1, int selImage = -1, wxUIntPtr data = 0);

    /**
     * @brief insert item after 'previous'
     */
    wxDataViewItem InsertItem(const wxDataViewItem& previous, const wxString& text, int image = -1, int selImage = -1,
                              wxUIntPtr data = 0);

    wxDataViewItem AppendItem(const wxVector<wxVariant>& values, wxUIntPtr data = 0);

    wxDataViewColumn* AppendIconTextColumn(const wxString& label, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                           int width = -1, wxAlignment align = wxALIGN_LEFT,
                                           int flags = wxDATAVIEW_COL_RESIZABLE);

    wxDataViewColumn* AppendProgressColumn(const wxString& label, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                           int width = -1, wxAlignment align = wxALIGN_LEFT,
                                           int flags = wxDATAVIEW_COL_RESIZABLE);

    wxDataViewColumn* AppendTextColumn(const wxString& label, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                       int width = -1, wxAlignment align = wxALIGN_LEFT,
                                       int flags = wxDATAVIEW_COL_RESIZABLE);

    wxDataViewItem GetSelection() const { return wxDataViewItem(clTreeCtrl::GetSelection().GetID()); }
    wxDataViewItem GetCurrentItem() const { return wxDataViewItem(GetFocusedItem().GetID()); }
    void Select(const wxDataViewItem& item);
    int GetSelections(wxDataViewItemArray& sel) const;
    int GetSelectedItemsCount() const;
    /**
     * @brief delete all items in the view. If a "deleterFunc" is provided, it will be called per item data
     */
    void DeleteAllItems(const std::function<void(wxUIntPtr)>& deleterFunc = nullptr);

    wxUIntPtr GetItemData(const wxDataViewItem& item) const;
    void SetItemData(const wxDataViewItem& item, wxUIntPtr data);

    void SetItemBackgroundColour(const wxDataViewItem& item, const wxColour& colour, size_t col = 0);
    wxColour GetItemBackgroundColour(const wxDataViewItem& item, size_t col = 0) const;

    void SetItemTextColour(const wxDataViewItem& item, const wxColour& colour, size_t col = 0);
    wxColour GetItemTextColour(const wxDataViewItem& item, size_t col = 0) const;

    void SetItemText(const wxDataViewItem& item, const wxString& text, size_t col = 0);
    wxString GetItemText(const wxDataViewItem& item, size_t col = 0) const;

    void SetItemChecked(const wxDataViewItem& item, bool checked, size_t col = 0);
    bool IsItemChecked(const wxDataViewItem& item, size_t col = 0) const;

    void SetItemBold(const wxDataViewItem& item, bool bold, size_t col = 0);

    void SetItemFont(const wxDataViewItem& item, const wxFont& font, size_t col = 0);
    wxFont GetItemFont(const wxDataViewItem& item, size_t col = 0) const;

    void EnableStyle(int style, bool enable, bool refresh = true) override;

    clHeaderItem* GetColumn(size_t index);
    size_t GetItemCount() const;

    /**
     * @brief return item at a given row. This function is executed in O(1)
     */
    wxDataViewItem RowToItem(size_t row) const;

    /**
     * @brief return row number from item. This function is executed in O(N)
     */
    int ItemToRow(const wxDataViewItem& item) const;

    /**
     * @brief Delete the row at position row.
     */
    void DeleteItem(size_t row);
    /**
     * @brief Sets the value of a given row/col (i.e. cell)
     */
    void SetValue(const wxVariant& value, size_t row, size_t col);

    /**
     * @brief set sorting function AND apply it
     */
    void SetSortFunction(const clSortFunc_t& CompareFunc) override;

    /**
     * @brief remove all columns from the control
     */
    void ClearColumns();

    /**
     * @brief display a menu for a given item & column ("cell")
     */
    void ShowMenuForItem(const wxDataViewItem& item, wxMenu& menu, size_t col = 0);

    /**
     * @brief display a string selection menu for a given cell. The selection string is set as the cell text
     */
    void ShowStringSelectionMenu(const wxDataViewItem& item, const wxArrayString& choices, size_t col = 0);

    /**
     * @brief Returns index of the selected row or wxNOT_FOUND.
     */
    int GetSelectedRow() const;

    /**
     * @brief Selects given row.
     */
    void SelectRow(size_t row);

    /**
     * @brief Unselects given row
     */
    void UnselectRow(size_t row);

    /**
     * @brief Returns true if row is selected
     */
    bool IsRowSelected(size_t row) const;
};

// Helper class passing bitmap index + text
class WXDLLIMPEXP_SDK clDataViewTextBitmap : public wxObject
{
private:
    wxString m_text;
    int m_bitmapIndex;

public:
    clDataViewTextBitmap(const wxString& text = wxEmptyString, int bitmapIndex = wxNOT_FOUND)
        : m_text(text)
        , m_bitmapIndex(bitmapIndex)
    {
    }

    clDataViewTextBitmap(const clDataViewTextBitmap& other)
        : wxObject()
        , m_text(other.m_text)
        , m_bitmapIndex(other.m_bitmapIndex)
    {
    }

    virtual ~clDataViewTextBitmap() {}

    void SetText(const wxString& text) { m_text = text; }
    wxString GetText() const { return m_text; }
    void SetBitmapIndex(int index) { m_bitmapIndex = index; }
    int GetBitmapIndex() const { return m_bitmapIndex; }

    bool IsSameAs(const clDataViewTextBitmap& other) const
    {
        return m_text == other.m_text && m_bitmapIndex == other.m_bitmapIndex;
    }

    bool operator==(const clDataViewTextBitmap& other) const { return IsSameAs(other); }

    bool operator!=(const clDataViewTextBitmap& other) const { return !IsSameAs(other); }

    wxDECLARE_DYNAMIC_CLASS(clDataViewTextBitmap);
};

DECLARE_VARIANT_OBJECT_EXPORTED(clDataViewTextBitmap, WXDLLIMPEXP_SDK)

// Helper class passing bitmap bool + label with possible bitmap index
class WXDLLIMPEXP_SDK clDataViewCheckbox : public wxObject
{
private:
    bool m_checked = false;
    wxString m_label;
    int m_bitmapIndex = wxNOT_FOUND;

public:
    clDataViewCheckbox(bool checked, int bitmapIndex = wxNOT_FOUND, const wxString& label = wxEmptyString)
        : m_checked(checked)
        , m_label(label)
        , m_bitmapIndex(bitmapIndex)
    {
    }

    clDataViewCheckbox(const clDataViewCheckbox& other)
        : wxObject()
        , m_checked(other.m_checked)
        , m_label(other.m_label)
        , m_bitmapIndex(other.m_bitmapIndex)
    {
    }

    clDataViewCheckbox() {}

    virtual ~clDataViewCheckbox() {}

    void SetChecked(bool checked) { this->m_checked = checked; }
    bool IsChecked() const { return m_checked; }
    void SetText(const wxString& text) { m_label = text; }
    wxString GetText() const { return m_label; }
    void SetBitmapIndex(int index) { m_bitmapIndex = index; }
    int GetBitmapIndex() const { return m_bitmapIndex; }

    bool IsSameAs(const clDataViewCheckbox& other) const
    {
        return m_label == other.m_label && m_bitmapIndex == other.m_bitmapIndex && m_checked == other.m_checked;
    }

    bool operator==(const clDataViewCheckbox& other) const { return IsSameAs(other); }

    bool operator!=(const clDataViewCheckbox& other) const { return !IsSameAs(other); }

    wxDECLARE_DYNAMIC_CLASS(clDataViewCheckbox);
};

DECLARE_VARIANT_OBJECT_EXPORTED(clDataViewCheckbox, WXDLLIMPEXP_SDK)

// Helper class passing bitmap bool + label with possible bitmap index
class WXDLLIMPEXP_SDK clDataViewTextWithButton : public wxObject
{
private:
    wxString m_label;
    int m_bitmapIndex = wxNOT_FOUND;
    eCellButtonType m_button_kind = eCellButtonType::BT_NONE;

public:
    clDataViewTextWithButton(const wxString& label, eCellButtonType button, int bitmapIndex)
        : m_label(label)
        , m_bitmapIndex(bitmapIndex)
        , m_button_kind(button)
    {
    }

    clDataViewTextWithButton() {}
    virtual ~clDataViewTextWithButton() {}

    void SetBitmapIndex(int index) { m_bitmapIndex = index; }
    int GetBitmapIndex() const { return m_bitmapIndex; }

    eCellButtonType GetButtonType() const { return m_button_kind; }

    void SetLabel(const wxString& label) { this->m_label = label; }
    const wxString& GetLabel() const { return m_label; }
    const wxString& GetButtonUnicodeSymbol() const;

    bool IsSameAs(const clDataViewTextWithButton& other) const
    {
        return m_label == other.m_label && m_bitmapIndex == other.m_bitmapIndex;
    }
    bool operator==(const clDataViewTextWithButton& other) const { return IsSameAs(other); }
    bool operator!=(const clDataViewTextWithButton& other) const { return !IsSameAs(other); }
    wxDECLARE_DYNAMIC_CLASS(clDataViewTextWithButton);
};

DECLARE_VARIANT_OBJECT_EXPORTED(clDataViewTextWithButton, WXDLLIMPEXP_SDK)

// Helper class passing button as a cell content
class WXDLLIMPEXP_SDK clDataViewButton : public wxObject
{
private:
    wxString m_label;
    int m_bitmapIndex = wxNOT_FOUND;

public:
    clDataViewButton(const wxString& label, int bitmapIndex)
        : m_label(label)
        , m_bitmapIndex(bitmapIndex)
    {
    }

    clDataViewButton() {}
    virtual ~clDataViewButton() {}

    void SetBitmapIndex(int index) { m_bitmapIndex = index; }
    int GetBitmapIndex() const { return m_bitmapIndex; }

    void SetLabel(const wxString& label) { this->m_label = label; }
    const wxString& GetLabel() const { return m_label; }

    bool IsSameAs(const clDataViewButton& other) const
    {
        return m_label == other.m_label && m_bitmapIndex == other.m_bitmapIndex;
    }

    bool operator==(const clDataViewButton& other) const { return IsSameAs(other); }
    bool operator!=(const clDataViewButton& other) const { return !IsSameAs(other); }
    wxDECLARE_DYNAMIC_CLASS(clDataViewButton);
};

DECLARE_VARIANT_OBJECT_EXPORTED(clDataViewButton, WXDLLIMPEXP_SDK)

// Helper class passing bitmap bool + label with possible bitmap index
class WXDLLIMPEXP_SDK clDataViewColour : public wxObject
{
private:
    wxColour m_colour;

public:
    clDataViewColour(const wxColour& colour)
        : m_colour(colour)
    {
    }

    clDataViewColour(const clDataViewColour& other)
        : wxObject()
        , m_colour(other.m_colour)
    {
    }

    clDataViewColour() {}
    virtual ~clDataViewColour() {}

    void SetColour(const wxColour& colour) { this->m_colour = colour; }
    const wxColour& GetColour() const { return m_colour; }
    bool IsSameAs(const clDataViewColour& other) const { return m_colour.IsSameAs(other.m_colour); }
    bool operator==(const clDataViewColour& other) const { return IsSameAs(other); }
    bool operator!=(const clDataViewColour& other) const { return !IsSameAs(other); }
    wxDECLARE_DYNAMIC_CLASS(clDataViewColour);
};

DECLARE_VARIANT_OBJECT_EXPORTED(clDataViewColour, WXDLLIMPEXP_SDK)

class WXDLLIMPEXP_SDK clDataViewControl : public wxObject
{
private:
    wxControl* m_ctrl = nullptr;

public:
    clDataViewControl(wxControl* ctrl)
        : m_ctrl(ctrl)
    {
    }

    clDataViewControl(const clDataViewControl& other)
        : wxObject()
        , m_ctrl(other.m_ctrl)
    {
    }

    clDataViewControl() {}
    // we do not delete the control
    virtual ~clDataViewControl() {}

    void SetControl(wxControl* ctrl) { this->m_ctrl = ctrl; }
    wxControl* GetControl() { return m_ctrl; }
    bool IsSameAs(const clDataViewControl& other) const { return m_ctrl == other.m_ctrl; }
    bool operator==(const clDataViewControl& other) const { return IsSameAs(other); }
    bool operator!=(const clDataViewControl& other) const { return !IsSameAs(other); }
    wxDECLARE_DYNAMIC_CLASS(clDataViewControl);
};

DECLARE_VARIANT_OBJECT_EXPORTED(clDataViewControl, WXDLLIMPEXP_SDK)

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DATAVIEW_SEARCH_TEXT, wxDataViewEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DATAVIEW_CLEAR_SEARCH, wxDataViewEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DATAVIEW_ACTION_BUTTON, wxDataViewEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DATAVIEW_CHOICE, wxDataViewEvent);

#endif // CLDATAVIEWLISTCTRL_H
