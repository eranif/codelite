#ifndef CLDATAVIEWLISTCTRL_H
#define CLDATAVIEWLISTCTRL_H

#include "clTreeCtrl.h"
#include <unordered_map>
#include <wx/dataview.h>

/**
 * @brief a thin wrapper around clTreeCtrl which provides basic compatiblity API (such as adding columns)
 * This is mainly for code generators like wxCrafter
 */
class WXDLLIMPEXP_SDK clDataViewListCtrl : public clTreeCtrl
{
    bool m_needToClearDefaultHeader = true;
    wxDataViewListCtrl m_dummy; // Needed for generating wxDV compatible events

    static std::unordered_map<int, int> m_stylesMap;

protected:
    void DoAddHeader(const wxString& label, int width);
    void OnConvertEvent(wxTreeEvent& event);
    bool SendDataViewEvent(const wxEventType& type, wxTreeEvent& treeEvent);
    void DoSetCellValue(clRowEntry* row, size_t col, const wxVariant& value);

public:
    clDataViewListCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clDataViewListCtrl();

    /**
     * @brief how many items can scroll in the view?
     */
    int GetRange() const { return GetItemCount(); }
    bool IsEmpty() const { return GetItemCount() == 0; }

    ///===--------------------
    /// wxDV compatilibty API
    ///===--------------------

    /**
     * @brief appends an item to the end of the list
     */
    wxDataViewItem AppendItem(const wxString& text, int image = -1, int selImage = -1, wxUIntPtr data = 0);

    /**
     * @brief insert item after 'previous'
     */
    wxDataViewItem InsertItem(const wxDataViewItem& previous, const wxString& text, int image = -1, int selImage = -1,
                              wxUIntPtr data = 0);

    void AppendItem(const wxVector<wxVariant>& values, wxUIntPtr data = 0);

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
    void DeleteAllItems();

    wxUIntPtr GetItemData(const wxDataViewItem& item) const;
    void SetItemData(const wxDataViewItem& item, wxUIntPtr data);

    void SetItemBackgroundColour(const wxDataViewItem& item, const wxColour& colour, size_t col = 0);
    wxColour GetItemBackgroundColour(const wxDataViewItem& item, size_t col = 0) const;

    void SetItemTextColour(const wxDataViewItem& item, const wxColour& colour, size_t col = 0);
    wxColour GetItemTextColour(const wxDataViewItem& item, size_t col = 0) const;

    void SetItemText(const wxDataViewItem& item, const wxString& text, size_t col = 0);
    wxString GetItemText(const wxDataViewItem& item, size_t col = 0) const;

    void SetItemBold(const wxDataViewItem& item, bool bold, size_t col = 0);

    void SetItemFont(const wxDataViewItem& item, const wxFont& font, size_t col = 0);
    wxFont GetItemFont(const wxDataViewItem& item, size_t col = 0) const;

    virtual void EnableStyle(int style, bool enable, bool refresh = true);

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
    void SetSortFunction(const clSortFunc_t& CompareFunc);
};

// Helper class passing bitmap index + text
class WXDLLIMPEXP_SDK clDataViewTextBitmap : public wxObject
{
private:
    wxString m_text;
    int      m_bitmapIndex;
    
public:
    clDataViewTextBitmap( const wxString &text = wxEmptyString,
                        int bitmapIndex = wxNOT_FOUND )
        : m_text(text),
          m_bitmapIndex(bitmapIndex)
    { }

    clDataViewTextBitmap( const clDataViewTextBitmap &other )
        : wxObject(),
          m_text(other.m_text),
          m_bitmapIndex(other.m_bitmapIndex)
    { }
    
    virtual ~clDataViewTextBitmap() {}
    
    void SetText( const wxString &text ) { m_text = text; }
    wxString GetText() const             { return m_text; }
    void SetBitmapIndex(int index) { m_bitmapIndex = index; }
    int GetBitmapIndex() const { return m_bitmapIndex; }

    bool IsSameAs(const clDataViewTextBitmap& other) const
    {
        return m_text == other.m_text && m_bitmapIndex == other.m_bitmapIndex;
    }

    bool operator==(const clDataViewTextBitmap& other) const
    {
        return IsSameAs(other);
    }

    bool operator!=(const clDataViewTextBitmap& other) const
    {
        return !IsSameAs(other);
    }

    wxDECLARE_DYNAMIC_CLASS(clDataViewTextBitmap);
};

DECLARE_VARIANT_OBJECT_EXPORTED(clDataViewTextBitmap, WXDLLIMPEXP_SDK)

#endif // CLDATAVIEWLISTCTRL_H
