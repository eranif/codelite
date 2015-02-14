#include "wxCodeCompletionBox.h"
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>
#include <wx/stc/stc.h>

#define MAX_NUM_LINES 8
#define Y_SPACER 2
#define BOX_WIDTH 300

#ifdef __WXMSW__
#define DEFAULT_FACE_NAME "Consolas"
#define DEFAULT_FONT_SIZE 10
#elif defined(__WXMAC__)
#define DEFAULT_FACE_NAME "monaco"
#define DEFAULT_FONT_SIZE 12
#else // GTK, FreeBSD etc
#define DEFAULT_FACE_NAME "monospace"
#define DEFAULT_FONT_SIZE 11
#endif

wxCodeCompletionBox::wxCodeCompletionBox(wxWindow* parent)
    : wxCodeCompletionBoxBase(parent)
    , m_index(0)
    , m_stc(NULL)
{
    m_ccFont = wxFont(wxFontInfo(DEFAULT_FONT_SIZE).Family(wxFONTFAMILY_TELETYPE).FaceName(DEFAULT_FACE_NAME));
    SetCursor(wxCURSOR_HAND);
    
    // Calculate the size of the box
    int singleLineHeight = GetSingleLineHeight();
    int boxHeight = singleLineHeight * MAX_NUM_LINES;
    int boxWidth = BOX_WIDTH; // 100 pixels
    wxSize boxSize = wxSize(boxWidth, boxHeight);
    wxRect rect(boxSize);

    // Increase the size by 2 pixel for each dimension
    rect.Inflate(2, 2);
    SetSize(rect);
    m_canvas->SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_canvas->Bind(wxEVT_LEFT_DOWN, &wxCodeCompletionBox::OnLeftDClick, this);
    m_canvas->Bind(wxEVT_LEFT_DCLICK, &wxCodeCompletionBox::OnLeftDClick, this);
}

wxCodeCompletionBox::~wxCodeCompletionBox()
{
    m_canvas->Unbind(wxEVT_LEFT_DOWN, &wxCodeCompletionBox::OnLeftDClick, this);
    m_canvas->Unbind(wxEVT_LEFT_DCLICK, &wxCodeCompletionBox::OnLeftDClick, this);
    if(m_stc) {
        m_stc->Unbind(wxEVT_STC_MODIFIED, &wxCodeCompletionBox::OnUpdateList, this);
        m_stc->Unbind(wxEVT_KEY_DOWN, &wxCodeCompletionBox::OnStcKey, this);
        m_stc->Unbind(wxEVT_LEFT_DOWN, &wxCodeCompletionBox::OnStcLeftDown, this);
    }
}

void wxCodeCompletionBox::OnEraseBackground(wxEraseEvent& event) { wxUnusedVar(event); }

void wxCodeCompletionBox::OnPaint(wxPaintEvent& event)
{
    // Paint the background colour
    wxAutoBufferedPaintDC dc(m_canvas);
    
    // Invalidate all item rects before we draw them
    for(size_t i=0; i<m_entries.size(); ++i) {
        m_entries.at(i)->m_itemRect = wxRect();
    }
    
    wxColour darkBorder("rgb(54, 54, 54)");
    wxColour lightBorder("rgb(77, 77, 77)");
    wxColour bgColour("rgb(64, 64, 64)");
    wxColour textColour("rgb(200, 200, 200)");
    wxColour selection("rgb(87, 87, 87)");

    wxRect rect = GetClientRect();

    dc.SetFont(m_ccFont);

    wxSize textSize = dc.GetTextExtent("Tp");
    int singleLineHeight = GetSingleLineHeight();

    m_canvas->PrepareDC(dc);
    dc.SetBrush(lightBorder);
    dc.SetPen(lightBorder);
    dc.DrawRectangle(rect);

    rect.Deflate(2, 2);
    dc.SetBrush(bgColour);
    dc.SetPen(bgColour);
    dc.DrawRectangle(rect);
    
    int firstIndex = m_index;
    int lastIndex = m_index + MAX_NUM_LINES;
    if(lastIndex > (int)m_entries.size()) {
        lastIndex = m_entries.size();
    }
    
    // if the number of items to display is less from the number of lines
    // on the box, try prepending items from the top
    while(firstIndex > 0 && ((lastIndex - firstIndex) != MAX_NUM_LINES)) {
        --firstIndex;
    }

    // Paint the entries, starting from m_index => m_index + 7
    wxCoord y = 2;
    wxCoord x = m_bitmaps.empty() ? 2 : 20;

    // Draw all items from firstIndex => lastIndex
    for(int i = firstIndex; i < lastIndex; ++i) {
        bool isSelected = (i == m_index);
        wxRect itemRect(2, y, rect.GetWidth(), singleLineHeight);
        if(isSelected) {
            // highlight the selection
            dc.SetBrush(selection);
            dc.SetPen(selection);
            dc.DrawRectangle(itemRect);
        }

        dc.SetTextForeground(isSelected ? *wxWHITE : textColour);
        dc.SetPen(lightBorder);
        dc.DrawLine(2, y, itemRect.GetWidth() + 1, y);
        y += 1;
        y += Y_SPACER;

        // If this item has a bitmap, draw it
        wxCodeCompletionBoxEntry::Ptr_t entry = m_entries.at(i);
        if(entry->GetImgIndex() != wxNOT_FOUND && entry->GetImgIndex() < (int)m_bitmaps.size()) {
            const wxBitmap& bmp = m_bitmaps.at(entry->GetImgIndex());
            wxCoord bmpY = ((singleLineHeight - bmp.GetHeight()) / 2) + itemRect.y;
            dc.DrawBitmap(bmp, 2, bmpY);
        }
        
        // Draw the text
        dc.DrawText(entry->GetText(), x, y);
        y += textSize.y;
        y += Y_SPACER;
        dc.SetPen(darkBorder);
        dc.DrawLine(2, y, itemRect.GetWidth() + 1, y);
        y += 1;
        entry->m_itemRect = itemRect;
    }
}

void wxCodeCompletionBox::ShowCompletionBox(wxStyledTextCtrl* ctrl, const wxCodeCompletionBoxEntry::Vec_t& entries)
{
    m_index = 0;
    m_stc = ctrl;
    m_allEntries = entries;
    FilterResults();
    // If we got a single match - insert it
    if(m_entries.size() == 1) {
        // single match
        InsertSelection();
        Destroy();
        return;
    }

    int lineHeight = GetSingleLineHeight();
    wxPoint pt = m_stc->PointFromPosition(m_stc->GetCurrentPos());
    pt = m_stc->ClientToScreen(pt);
    pt.y += lineHeight;
    Move(pt);

    Show();
    if(m_stc) {
        m_stc->Bind(wxEVT_STC_MODIFIED, &wxCodeCompletionBox::OnUpdateList, this);
        m_stc->Bind(wxEVT_KEY_DOWN, &wxCodeCompletionBox::OnStcKey, this);
        m_stc->Bind(wxEVT_LEFT_DOWN, &wxCodeCompletionBox::OnStcLeftDown, this);
        // Set the focus back to the completion control
        m_stc->CallAfter(&wxStyledTextCtrl::SetFocus);
    }
}

void wxCodeCompletionBox::OnUpdateList(wxStyledTextEvent& event)
{
    event.Skip();
    FilterResults();
    if(m_entries.empty()) {
        Destroy();
    } else {
        Refresh();
    }
}

void wxCodeCompletionBox::OnStcKey(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_UP) {
        if((m_index - 1) >= 0) {
            --m_index;
            Refresh();
        }

    } else if(event.GetKeyCode() == WXK_DOWN) {
        if((m_index + 1) < (int)m_entries.size()) {
            ++m_index;
            Refresh();
        }
    } else if(event.GetKeyCode() == WXK_ESCAPE) {
        Destroy();

    } else if(event.GetKeyCode() == WXK_TAB || event.GetKeyCode() == WXK_RETURN) {
        // Insert the selection
        InsertSelection();
        Destroy();

    } else {
        event.Skip();
    }
}

int wxCodeCompletionBox::GetSingleLineHeight() const
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    memDC.SetFont(m_ccFont);
    wxSize size = memDC.GetTextExtent("Tp");

    int singleLineHeight = size.y + (2 * Y_SPACER) + 2; // the extra pixel is for the border line
    if(singleLineHeight < 16) {
        singleLineHeight = 16; // at least 16 pixels for image
    }
    return singleLineHeight;
}

void wxCodeCompletionBox::FilterResults()
{
    int start = m_stc->WordStartPosition(m_stc->GetCurrentPos(), true);
    int end = m_stc->WordEndPosition(m_stc->GetCurrentPos(), true);
    wxString word = m_stc->GetTextRange(start, end); // the current word
    if(word.IsEmpty()) {
        m_entries = m_allEntries;
        return;
    }

    m_entries.clear();
    wxString lcFilter = word.Lower();
    for(size_t i = 0; i < m_allEntries.size(); ++i) {
        wxString lcText = m_allEntries.at(i)->GetText().Lower();
        if(lcText.Contains(lcFilter)) {
            m_entries.push_back(m_allEntries.at(i));
        }
    }
    m_index = 0;
}

void wxCodeCompletionBox::InsertSelection()
{
    if((m_index >= 0 && m_index < (int)m_entries.size()) && m_stc) {
        wxCodeCompletionBoxEntry::Ptr_t match = m_entries.at(m_index);
        int start = m_stc->WordStartPosition(m_stc->GetCurrentPos(), true);
        int end = m_stc->WordEndPosition(m_stc->GetCurrentPos(), true);
        m_stc->SetSelection(start, end);
        m_stc->ReplaceSelection(match->GetText());
    }
}

void wxCodeCompletionBox::OnStcLeftDown(wxMouseEvent& event)
{
    event.Skip();
    Destroy();
}

void wxCodeCompletionBox::OnLeftDClick(wxMouseEvent& event)
{
    for(size_t i=0; i<m_entries.size(); ++i) {
        if(m_entries.at(i)->m_itemRect.Contains(event.GetPosition())) {
            m_index = i;
            InsertSelection();
            Destroy();
            return;
        }
    }
}
