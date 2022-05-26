#ifndef CLHEADERBAR_H
#define CLHEADERBAR_H

#include "clHeaderItem.h"
#include "codelite_exports.h"

#include <vector>
#include <wx/panel.h>

class clControlWithItems;
class WXDLLIMPEXP_SDK clHeaderBar : public wxPanel
{
    clHeaderItem::Vect_t m_columns;
    const clColours& m_colours;
    int m_flags = 0;
    bool m_isDragging = false;
    int m_draggedCol = wxNOT_FOUND;
    wxCursor m_previousCursor;
    wxFont m_headerFont = wxNullFont;

protected:
    void DoUpdateSize();
    wxSize GetTextSize(const wxString& label) const;
    int HitBorder(int x) const;
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void DoCancelDrag();

public:
    clHeaderBar(clControlWithItems* parent, const clColours& colours);
    virtual ~clHeaderBar();

    void SetHeaderFont(const wxFont& headerFont) { this->m_headerFont = headerFont; }
    const wxFont& GetHeaderFont() const { return m_headerFont; }
    bool Show(bool show = true);
    /**
     * @brief set drawing native header
     */
    void SetNative(bool b)
    {
        if(b) {
            m_flags |= kHeaderNative;
        } else {
            m_flags &= ~kHeaderNative;
        }
        Refresh();
    }

    /**
     * @brief update the column width, but only if the new width is greater than the current one, unless 'force' is set
     * to 'true'
     * @param col
     * @param width
     * @param force set the new width, no matter what...
     */
    void UpdateColWidthIfNeeded(size_t col, int width, bool force = false);
    clHeaderItem& Item(int index) { return m_columns[index]; }
    const clHeaderItem& Item(int index) const { return m_columns[index]; }
    clHeaderItem& operator[](int index) { return Item(index); }
    const clHeaderItem& operator[](int index) const { return Item(index); }

    size_t size() const { return m_columns.size(); }
    size_t GetCount() const { return size(); }

    void SetColumnsWidth(const std::vector<size_t>& v_width);

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
    clHeaderItem& Add(const clHeaderItem& item)
    {
        push_back(item);
        return Last();
    }
    /**
     * @brief add new column to the header bar
     */
    clHeaderItem& Add(const wxString& label, const wxBitmap& bitmap = wxNullBitmap)
    {
        push_back(clHeaderItem(m_parent, label, bitmap));
        return Last();
    }

    void Clear();
    const clHeaderItem& Last() const;
    clHeaderItem& Last();
    /**
     * @brief draw the header bar using dc and colours
     */
    void Render(wxDC& dc, const clColours& colours);
    size_t GetWidth() const;

    /**
     * @brief are we dragging a column?
     */
    bool IsDragging() { return m_isDragging; };

    /**
     * @brief process a left-down event
     */
    void OnMouseLeftDown(wxMouseEvent& event);
    /**
     * @brief process motion events
     */
    void OnMotion(wxMouseEvent& event);
    /**
     * @brief process a left-up event
     */
    void OnMouseLeftUp(wxMouseEvent& event);
};

#endif // CLHEADERBAR_H
