#ifndef CLHEADERBAR_H
#define CLHEADERBAR_H

#include "clHeaderItem.h"
#include "codelite_exports.h"
#include <vector>

class WXDLLIMPEXP_SDK clHeaderBar
{
    clHeaderItem::Vect_t m_columns;
    bool m_hideHeaders = false;
    int m_firstColumn = 0;

protected:
    void DoUpdateSize();
    wxSize GetTextSize(const wxString& label) const;

public:
    clHeaderBar();
    virtual ~clHeaderBar();

    /**
     * @brief update the column width, but only if the new width is greater than the current one, unless 'force' is set
     * to 'true'
     * @param col
     * @param width
     * @param force set the new width, no matter what...
     */
    void UpdateColWidthIfNeeded(size_t col, size_t width, bool force = false);
    clHeaderItem& Item(int index) { return m_columns[index]; }
    const clHeaderItem& Item(int index) const { return m_columns[index]; }
    clHeaderItem& operator[](int index) { return Item(index); }
    const clHeaderItem& operator[](int index) const { return Item(index); }

    size_t size() const { return m_columns.size(); }
    size_t GetCount() const { return size(); }

    /**
     * @brief Return the header bar height, taking into consideration all columns
     */
    size_t GetHeight() const;

    /**
     * @brief do we have any columns?
     */
    bool empty() const { return m_columns.empty(); }
    /**
     * @brief do we have any columns?
     */
    bool IsEmpty() const { return empty(); }

    /**
     * @brief add new column to the header bar
     */
    void push_back(const clHeaderItem& item);
    /**
     * @brief add new column to the header bar
     */
    void Add(const clHeaderItem& item) { push_back(item); }
    /**
     * @brief add new column to the header bar
     */
    void Add(const wxString& label, const wxBitmap& bitmap = wxNullBitmap) { push_back(clHeaderItem(label, bitmap)); }

    /**
     * @brief draw the header bar using dc and colours
     */
    void Render(wxDC& dc, const wxRect& rect, const clColours& colours);

    /**
     * @brief hide the columns headers
     */
    void SetHideHeaders(bool b);
    bool IsHideHeaders() const { return m_hideHeaders; }

    size_t GetWidth() const;

    void ScrollToColumn(int firstColumn);
};

#endif // CLHEADERBAR_H
