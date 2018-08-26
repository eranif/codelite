#include "wxCodeCompletionBox.h"
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>
#include <wx/stc/stc.h>
#include "imanager.h"
#include "globals.h"
#include "bitmap_loader.h"
#include "wxCodeCompletionBoxManager.h"
#include "codelite_events.h"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "cc_box_tip_window.h"
#include "file_logger.h"
#include "drawingutils.h"
#include <wx/font.h>
#include "macros.h"
#include <wx/stc/stc.h>
#include "ieditor.h"
#include "imanager.h"
#include "CxxTemplateFunction.h"
#include <wx/app.h>

static int LINES_PER_PAGE = 8;
static int Y_SPACER = 2;
static int SCROLLBAR_WIDTH = 12;
static int BOX_WIDTH = 400 + SCROLLBAR_WIDTH;

wxCodeCompletionBox::BmpVec_t wxCodeCompletionBox::m_defaultBitmaps;

wxCodeCompletionBox::wxCodeCompletionBox(wxWindow* parent, wxEvtHandler* eventObject, size_t flags)
    : wxCodeCompletionBoxBase(parent)
    , m_index(0)
    , m_stc(NULL)
    , m_startPos(wxNOT_FOUND)
    , m_useLightColours(false)
    , m_eventObject(eventObject)
    , m_tipWindow(NULL)
    , m_flags(flags)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_ccFont = DrawingUtils::GetDefaultFixedFont();
    m_ccFont.SetPointSize(m_ccFont.GetPointSize() - 1);
    SetCursor(wxCURSOR_HAND);

    // Update the BOX_WIDTH to contains at least 30 chars
    static int once = false;
    if(!once) {
        once = true;
        wxMemoryDC memDC;
        wxBitmap bmp(1, 1);
        memDC.SelectObject(bmp);
        memDC.SetFont(m_ccFont);
        wxString sampleString('X', 50);
        wxSize sz = memDC.GetTextExtent(sampleString);
        BOX_WIDTH = sz.GetWidth() + SCROLLBAR_WIDTH;
    }

    // Calculate the size of the box
    int singleLineHeight = GetSingleLineHeight();
    int boxHeight = singleLineHeight * LINES_PER_PAGE;
    int boxWidth = BOX_WIDTH; // 100 pixels
    wxSize boxSize = wxSize(boxWidth, boxHeight);
    wxRect rect(boxSize);

    // Set the default bitmap list
    BitmapLoader* bmpLoader = clGetManager()->GetStdIcons();
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/class"));              // 0
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/struct"));             // 1
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/namespace"));          // 2
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_public"));      // 3
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/typedef"));            // 4
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_private"));     // 5
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_public"));      // 6
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_protected"));   // 7
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/function_private"));   // 8
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/function_public"));    // 9
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/function_protected")); // 10
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/typedef"));            // 11
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/enum"));               // 12
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/enumerator"));         // 13
    m_bitmaps.push_back(bmpLoader->LoadBitmap("mime/16/cpp"));              // 14
    m_bitmaps.push_back(bmpLoader->LoadBitmap("mime/16/h"));                // 15
    m_bitmaps.push_back(bmpLoader->LoadBitmap("mime/16/text"));             // 16
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/cpp_keyword"));        // 17
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cc/16/enum"));               // 18

    InitializeDefaultBitmaps();

    // Increase the size by 2 pixel for each dimension
    rect.Inflate(2, 2);
    SetSize(rect);
    m_canvas->SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_canvas->Bind(wxEVT_LEFT_DOWN, &wxCodeCompletionBox::OnLeftDClick, this);
    m_canvas->Bind(wxEVT_LEFT_DCLICK, &wxCodeCompletionBox::OnLeftDClick, this);
    m_canvas->Bind(wxEVT_MOUSEWHEEL, &wxCodeCompletionBox::OnMouseScroll, this);

    // Default colorus (dark theme)
    clColourPalette palette = DrawingUtils::GetColourPalette();

    // Default colours
    wxColour baseColour = DrawingUtils::GetPanelBgColour();
    m_bgColour = palette.bgColour;
    m_penColour = palette.penColour;
    m_separatorColour = m_bgColour.ChangeLightness(98);
    m_textColour = palette.textColour;
    m_selectedTextColour = palette.selecteTextColour;
    m_selectedTextBgColour = palette.selectionBgColour;
    m_useLightColours = true;

    IManager* manager = ::clGetManager();
    if(manager) {
        IEditor* editor = manager->GetActiveEditor();
        if(editor) {
            wxColour bgColour = editor->GetCtrl()->StyleGetBackground(0);
            if(DrawingUtils::IsDark(bgColour)) {
                // Dark colour
                m_separatorColour = m_bgColour.ChangeLightness(102);
                m_useLightColours = false;
            }
        }
    }

    m_bmpDown = wxXmlResource::Get()->LoadBitmap("cc-box-down");
    m_bmpDownEnabled = m_bmpDown.ConvertToDisabled();

    m_bmpUp = wxXmlResource::Get()->LoadBitmap("cc-box-up");
    m_bmpUpEnabled = m_bmpUp.ConvertToDisabled();

    if(m_useLightColours) {
        // swap between the disabled and enabeld bitmaps
        {
            wxBitmap tmpBitmap;
            tmpBitmap = m_bmpDown;
            m_bmpDown = m_bmpDownEnabled;
            m_bmpDownEnabled = tmpBitmap;
        }

        {
            wxBitmap tmpBitmap;
            tmpBitmap = m_bmpUp;
            m_bmpUp = m_bmpUpEnabled;
            m_bmpUpEnabled = tmpBitmap;
        }
    }
}

wxCodeCompletionBox::~wxCodeCompletionBox()
{
    m_canvas->Unbind(wxEVT_LEFT_DOWN, &wxCodeCompletionBox::OnLeftDClick, this);
    m_canvas->Unbind(wxEVT_LEFT_DCLICK, &wxCodeCompletionBox::OnLeftDClick, this);
    m_canvas->Unbind(wxEVT_MOUSEWHEEL, &wxCodeCompletionBox::OnMouseScroll, this);
    DoDestroyTipWindow();
}

void wxCodeCompletionBox::OnEraseBackground(wxEraseEvent& event) { wxUnusedVar(event); }

void wxCodeCompletionBox::OnPaint(wxPaintEvent& event)
{
    // Paint the background colour
    wxAutoBufferedPaintDC dc(m_canvas);

    // Invalidate all item rects before we draw them
    for(size_t i = 0; i < m_entries.size(); ++i) {
        m_entries.at(i)->m_itemRect = wxRect();
    }

    wxRect rect = GetClientRect();
    m_scrollArea = wxRect(rect.GetWidth() - SCROLLBAR_WIDTH + rect.GetTopLeft().x, rect.GetTopLeft().y, SCROLLBAR_WIDTH,
                          rect.GetHeight());

    dc.SetFont(m_ccFont);
    int singleLineHeight = GetSingleLineHeight();

    m_canvas->PrepareDC(dc);
    // Draw the entire box with single solid colour
    dc.SetBrush(m_bgColour);
    dc.SetPen(m_penColour);
#ifdef __WXOSX__
    rect.Inflate(1, 1);
#endif
    dc.DrawRectangle(rect);

    // Shrink the rectangle by 2 to provide a 2 pixle
    // border
    rect.Deflate(1, 1);
    dc.SetBrush(m_bgColour);
    dc.SetPen(m_bgColour);
    dc.DrawRectangle(rect);

    // We want to have a scrollbar alike. We do this by reducing the size of the
    // items area by 5 pixels
    rect.SetWidth(rect.GetWidth() - SCROLLBAR_WIDTH);
    int firstIndex = m_index;
    int lastIndex = m_index + LINES_PER_PAGE;
    if(lastIndex > (int)m_entries.size()) {
        lastIndex = m_entries.size();
    }

    // if the number of items to display is less from the number of lines
    // on the box, try prepending items from the top
    while(firstIndex > 0 && ((lastIndex - firstIndex) != LINES_PER_PAGE)) {
        --firstIndex;
    }

    // Paint the entries, starting from m_index => m_index + 7
    wxCoord y = 1; // Initial coord for drawing the first line
    wxCoord textX = m_bitmaps.empty() ? 2 : clGetScaledSize(20);
    wxCoord bmpX = clGetScaledSize(2);

    // Draw all items from firstIndex => lastIndex
    for(int i = firstIndex; i < lastIndex; ++i) {
        bool isSelected = (i == m_index);
        bool isLast = ((firstIndex + 1) == lastIndex);
        wxRect itemRect(0, y, rect.GetWidth(), singleLineHeight);
        if(isSelected) {
            // highlight the selection
            dc.SetBrush(m_selectedTextBgColour);
            dc.SetPen(m_selectedTextBgColour);
            dc.DrawRectangle(itemRect);
        }

        dc.SetPen(m_separatorColour);

        // If this item has a bitmap, draw it
        wxCodeCompletionBoxEntry::Ptr_t entry = m_entries.at(i);
        if(entry->GetImgIndex() != wxNOT_FOUND && entry->GetImgIndex() < (int)m_bitmaps.size()) {
            // Use a stock bitmap image
            const wxBitmap& bmp = m_bitmaps.at(entry->GetImgIndex());
            wxCoord bmpY = ((singleLineHeight - bmp.GetScaledHeight()) / 2) + itemRect.y;

            dc.DrawBitmap(bmp, bmpX, bmpY);
        } else if(entry->GetAlternateBitmap().IsOk()) {
            // We have an alternate bitmap, use it
            const wxBitmap& bmp = entry->GetAlternateBitmap();
            wxCoord bmpY = ((singleLineHeight - bmp.GetScaledHeight()) / 2) + itemRect.y;
            dc.DrawBitmap(bmp, bmpX, bmpY);
        }

        // Draw the text
        dc.SetClippingRegion(itemRect);

        // Truncate the text to fit the screen
        wxString choppedText;
        DrawingUtils::TruncateText(entry->GetText(), itemRect.GetWidth(), dc, choppedText);

        wxSize itemTextSize = dc.GetTextExtent(choppedText);
        wxCoord textY = ((singleLineHeight - itemTextSize.GetHeight()) / 2) + itemRect.y;
        dc.SetTextForeground(isSelected ? m_selectedTextColour : m_textColour);
        dc.DrawText(choppedText, textX, textY);
        dc.DestroyClippingRegion();
        y += singleLineHeight;
        dc.SetPen(m_separatorColour);
        if(!isLast) {
            dc.DrawLine(2, y, itemRect.GetWidth() + 2, y);
        }
        entry->m_itemRect = itemRect;
    }

    //===================--------------------------------------
    // draw the scrollbar
    //===================--------------------------------------

    wxRect scrollRect = m_scrollArea;
    dc.SetPen(m_penColour);
    dc.SetBrush(m_penColour);
    dc.DrawRectangle(scrollRect);
    DoDrawBottomScrollButton(dc);
    DoDrawTopScrollButton(dc);
}

void wxCodeCompletionBox::ShowCompletionBox(wxStyledTextCtrl* ctrl, const wxCodeCompletionBoxEntry::Vec_t& entries)
{
    m_index = 0;
    m_stc = ctrl;
    m_allEntries = entries;

    // Keep the start position
    if(m_startPos == wxNOT_FOUND) {
        m_startPos = m_stc->WordStartPosition(m_stc->GetCurrentPos(), true);
    }

    // Fire "Showing" event
    if(!(m_flags & kNoShowingEvent)) {
        clCodeCompletionEvent ccEvent(wxEVT_CCBOX_SHOWING);
        ccEvent.SetEntries(m_allEntries);
        ccEvent.SetEventObject(this);
        ccEvent.SetWord(GetFilter());
        EventNotifier::Get()->ProcessEvent(ccEvent);
        m_allEntries.swap(ccEvent.GetEntries());
    }
    // Filter all duplicate entries from the list (based on simple string match)
    RemoveDuplicateEntries();

    // Filter results based on user input
    FilterResults();

    // If we got a single match - insert it
    if((m_entries.size() == 1) && (m_flags & kInsertSingleMatch)) {
        // single match
        InsertSelection();
        DoDestroy();
        return;
    }

    // Let the plugins modify the list of the entries
    int start = m_startPos;
    int end = m_stc->GetCurrentPos();

    wxString word = m_stc->GetTextRange(start, end); // the current word
    if(m_entries.empty()) {
        // no entries to display
        DoDestroy();
        return;
    }

    DoShowCompletionBox();

    if(m_stc) {
        // Set the focus back to the completion control
        m_stc->CallAfter(&wxStyledTextCtrl::SetFocus);
    }

    // Display the help window
    DoDisplayTipWindow();
}

void wxCodeCompletionBox::DoDisplayTipWindow()
{
    // Display the tooltip
    if(m_index >= 0 && m_index < (int)m_entries.size()) {
        wxString docComment = m_entries.at(m_index)->GetComment();
        docComment.Trim().Trim(false);
        if(docComment.IsEmpty() && m_entries.at(m_index)->m_tag) {
            // Format the comment on demand if the origin was a tag entry
            docComment = m_entries.at(m_index)->m_tag->FormatComment();
        }

        if(docComment.IsEmpty()) {
            // No tip to display
            DoDestroyTipWindow();

        } else if(!docComment.IsEmpty() && docComment != m_displayedTip) {
            // destroy old tip window
            DoDestroyTipWindow();

            // keep the old tip
            m_displayedTip = docComment;

            // Construct a new tip window and display the tip
            m_tipWindow = new CCBoxTipWindow(GetParent(), docComment, 1, false);
            m_tipWindow->PositionRelativeTo(this, m_stc->PointFromPosition(m_stc->GetCurrentPos()));

            // restore focus to the editor
            m_stc->CallAfter(&wxStyledTextCtrl::SetFocus);
        }

    } else {
        // Nothing to display, just destroy the old tooltip
        DoDestroyTipWindow();
    }
}

void wxCodeCompletionBox::StcCharAdded(wxStyledTextEvent& event)
{
    event.Skip();
    int keychar = m_stc->GetCharAt(m_stc->PositionBefore(m_stc->GetCurrentPos()));
    if(((keychar >= 65) && (keychar <= 90)) ||  // A-Z
       ((keychar >= 97) && (keychar <= 122)) || // a-z
       ((keychar >= 48) && (keychar <= 57)) ||  // 0-9
       (keychar == 95) ||                       // _
       (keychar == 33))                         // !
    {
        DoUpdateList();
    } else {
        DoDestroy();
    }
}

void wxCodeCompletionBox::StcModified(wxStyledTextEvent& event)
{
    event.Skip();
    DoUpdateList();
}

void wxCodeCompletionBox::StcKeyDown(wxKeyEvent& event)
{
    switch(event.GetKeyCode()) {
    case WXK_UP:
        DoScrollUp();
        break;
    case WXK_DOWN:
        DoScrollDown();
        break;
    case WXK_PAGEDOWN: {
        DoPgDown();
        break;
    }
    case WXK_PAGEUP: {
        DoPgUp();
        break;
    }
    case WXK_ESCAPE:
    case WXK_LEFT:
    case WXK_RIGHT:
    case WXK_HOME:
    case WXK_END:
    case WXK_DELETE:
    case WXK_NUMPAD_DELETE:
    case WXK_ALT:
    case WXK_WINDOWS_LEFT:
    case WXK_WINDOWS_RIGHT:
    case WXK_CONTROL:
        DoDestroy();
        event.Skip();
        break;
    case WXK_TAB:
    case WXK_RETURN:
    case WXK_NUMPAD_ENTER:
        // Insert the selection
        InsertSelection();
        DoDestroy();
        break;
    default:
        event.Skip();
        break;
    }
}

int wxCodeCompletionBox::GetSingleLineHeight() const
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    memDC.SetFont(m_ccFont);
    m_canvas->PrepareDC(memDC);
    wxSize size = memDC.GetTextExtent("Tp");
    int singleLineHeight = size.y + (2 * Y_SPACER) + clGetScaledSize(2); // the extra pixel is for the border line
    if(singleLineHeight < clGetScaledSize(16)) {
        singleLineHeight = clGetScaledSize(16) + clGetScaledSize(2); // at least 16 pixels for image
    }
    return singleLineHeight;
}

bool wxCodeCompletionBox::FilterResults()
{
    wxString word = GetFilter();
    if(word.IsEmpty()) {
        m_entries = m_allEntries;
        return false;
    }

    m_entries.clear();
    wxString lcFilter = word.Lower();
    // Smart sorting:
    // We preare the list of matches in the following order:
    // Exact matches
    // Starts with
    // Contains
    wxCodeCompletionBoxEntry::Vec_t exactMatches, exactMatchesI, startsWith, startsWithI, contains, containsI;
    for(size_t i = 0; i < m_allEntries.size(); ++i) {
        wxString entryText = m_allEntries.at(i)->GetText().BeforeFirst('(');
        entryText.Trim().Trim(false);
        wxString lcEntryText = entryText.Lower();

        // Exact match:
        if(word == entryText) {
            exactMatches.push_back(m_allEntries.at(i));

        } else if(lcEntryText == lcFilter) {
            exactMatchesI.push_back(m_allEntries.at(i));

        } else if(entryText.StartsWith(word)) {
            startsWith.push_back(m_allEntries.at(i));

        } else if(lcEntryText.StartsWith(lcFilter)) {
            startsWithI.push_back(m_allEntries.at(i));

        } else if(entryText.Contains(word)) {
            contains.push_back(m_allEntries.at(i));

        } else if(lcEntryText.Contains(lcFilter)) {
            containsI.push_back(m_allEntries.at(i));
        }
    }

    // Merge the results
    m_entries.insert(m_entries.end(), exactMatches.begin(), exactMatches.end());
    m_entries.insert(m_entries.end(), exactMatchesI.begin(), exactMatchesI.end());
    m_entries.insert(m_entries.end(), startsWith.begin(), startsWith.end());
    m_entries.insert(m_entries.end(), startsWithI.begin(), startsWithI.end());
    m_entries.insert(m_entries.end(), contains.begin(), contains.end());
    m_entries.insert(m_entries.end(), containsI.begin(), containsI.end());
    m_index = 0;
    return exactMatches.empty() && exactMatchesI.empty() && startsWith.empty() && startsWithI.empty();
}

void wxCodeCompletionBox::InsertSelection()
{
    if((m_index >= 0 && m_index < (int)m_entries.size()) && m_stc) {
        wxCodeCompletionBoxEntry::Ptr_t match = m_entries.at(m_index);
        // Let the owner override the default behavior
        clCodeCompletionEvent e(wxEVT_CCBOX_SELECTION_MADE);
        e.SetWord(match->GetText());
        e.SetEventObject(m_eventObject);
        e.SetEntry(match);
        if(!EventNotifier::Get()->ProcessEvent(e)) {
            // execute the default behavior
            if(match->m_tag && match->m_tag->IsTemplateFunction()) {
                CxxTemplateFunction tf(match->m_tag);
                if(!tf.CanTemplateArgsDeduced()) {
                    // Insert a template function
                    wxCodeCompletionBoxManager::Get().CallAfter(
                        &wxCodeCompletionBoxManager::InsertSelectionTemplateFunction, match->GetText());
                    return;
                }
            }
            wxCodeCompletionBoxManager::Get().CallAfter(&wxCodeCompletionBoxManager::InsertSelection, match->GetText());
        }
    }
}

void wxCodeCompletionBox::StcLeftDown(wxMouseEvent& event)
{
    event.Skip();
    DoDestroy();
}

void wxCodeCompletionBox::OnLeftDClick(wxMouseEvent& event)
{
    // Check the scroll area
    if(m_scrollBottomRect.Contains(event.GetPosition())) {
        DoScrollDown();
    } else if(m_scrollTopRect.Contains(event.GetPosition())) {
        DoScrollUp();
    } else {
        for(size_t i = 0; i < m_entries.size(); ++i) {
            if(m_entries.at(i)->m_itemRect.Contains(event.GetPosition())) {
                m_index = i;
                InsertSelection();
                CallAfter(&wxCodeCompletionBox::DoDestroy);
                return;
            }
        }
    }
}

wxCodeCompletionBoxEntry::Vec_t wxCodeCompletionBox::TagsToEntries(const TagEntryPtrVector_t& tags)
{
    wxCodeCompletionBoxEntry::Vec_t entries;
    for(size_t i = 0; i < tags.size(); ++i) {
        TagEntryPtr tag = tags.at(i);
        wxString text = tag->GetDisplayName().Trim().Trim(false);
        int imgIndex = GetImageId(tag);
        wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(text, imgIndex);
        entry->m_tag = tag;
        entries.push_back(entry);
    }
    return entries;
}

int wxCodeCompletionBox::GetImageId(TagEntryPtr entry)
{
    wxString kind = entry->GetKind();
    wxString access = entry->GetAccess();
    if(kind == wxT("class")) return 0;
    if(kind == wxT("struct")) return 1;
    if(kind == wxT("namespace")) return 2;
    if(kind == wxT("variable")) return 3;
    if(kind == wxT("typedef")) return 4;
    if(kind == wxT("member") && access.Contains(wxT("private"))) return 5;
    if(kind == wxT("member") && access.Contains(wxT("public"))) return 6;
    if(kind == wxT("member") && access.Contains(wxT("protected"))) return 7;
    // member with no access? (maybe part of namespace??)
    if(kind == wxT("member")) return 6;
    if((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("private"))) return 8;
    if((kind == wxT("function") || kind == wxT("prototype")) && (access.Contains(wxT("public")) || access.IsEmpty()))
        return 9;
    if((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("protected"))) return 10;
    if(kind == wxT("macro")) return 11;
    if(kind == wxT("enum")) return 12;
    if(kind == wxT("enumerator")) return 13;
    if(kind == wxT("cpp_keyword")) return 17;
    if(kind == "cenum") return 18;
    return wxNOT_FOUND;
}

void wxCodeCompletionBox::ShowCompletionBox(wxStyledTextCtrl* ctrl, const TagEntryPtrVector_t& tags)
{
    ShowCompletionBox(ctrl, TagsToEntries(tags));
}

void wxCodeCompletionBox::DoUpdateList()
{
    bool refreshList = FilterResults();

    int curpos = m_stc->GetCurrentPos();
    if(m_entries.empty() || curpos < m_startPos || refreshList) {
        if((m_entries.empty() || refreshList) && (m_flags & kRefreshOnKeyType)) {
            // Trigger a new CC box
            wxCommandEvent event(wxEVT_MENU, XRCID("complete_word"));
            wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
        }
        DoDestroy();

    } else {
        DoDisplayTipWindow();
        Refresh();
    }
}

wxCodeCompletionBoxEntry::Ptr_t wxCodeCompletionBox::TagToEntry(TagEntryPtr tag)
{
    wxString text = tag->GetDisplayName().Trim().Trim(false);
    int imgIndex = GetImageId(tag);
    wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(text, imgIndex);
    entry->m_tag = tag;
    return entry;
}

void wxCodeCompletionBox::DoScrollDown()
{
    if(CanScrollDown()) {
        ++m_index;
        DoDisplayTipWindow();
        Refresh();
    }
}

void wxCodeCompletionBox::DoScrollUp()
{
    if(CanScrollUp()) {
        --m_index;
        DoDisplayTipWindow();
        Refresh();
    }
}

void wxCodeCompletionBox::DoDestroy() { wxCodeCompletionBoxManager::Get().DestroyCCBox(); }

void wxCodeCompletionBox::DoDrawBottomScrollButton(wxDC& dc)
{
    wxRect scrollRect = m_scrollArea;

    // Separate the scrollbar area into 2 big buttons: up and down
    m_scrollBottomRect =
        wxRect(wxPoint(scrollRect.GetTopLeft().x, scrollRect.GetTopLeft().y + scrollRect.GetHeight() / 2),
               wxSize(scrollRect.GetWidth(), scrollRect.GetHeight() / 2));
    // Draw the up arrow
    wxCoord x, y;
    x = m_scrollBottomRect.x + ((m_scrollBottomRect.GetWidth() - m_bmpDown.GetWidth()) / 2);
    y = m_scrollBottomRect.y + m_scrollBottomRect.GetHeight() - (2 * m_bmpDown.GetHeight());

    wxBitmap bmp = CanScrollDown() ? m_bmpDownEnabled : m_bmpDown;
    dc.DrawBitmap(bmp, x, y);
}

void wxCodeCompletionBox::DoDrawTopScrollButton(wxDC& dc)
{
    wxRect scrollRect = m_scrollArea;

    // Separate the scrollbar area into 2 big buttons: up and down
    m_scrollTopRect = wxRect(scrollRect.GetTopLeft(), wxSize(scrollRect.GetWidth(), scrollRect.GetHeight() / 2));

    // Draw the up arrow
    wxCoord x, y;
    x = m_scrollTopRect.x + ((m_scrollTopRect.GetWidth() - m_bmpUp.GetWidth()) / 2);
    y = m_scrollTopRect.y + m_bmpUp.GetHeight();

    wxBitmap bmp = CanScrollUp() ? m_bmpUpEnabled : m_bmpUp;
    dc.DrawBitmap(bmp, x, y);
}

bool wxCodeCompletionBox::CanScrollDown() { return ((m_index + 1) < (int)m_entries.size()); }

bool wxCodeCompletionBox::CanScrollUp() { return ((m_index - 1) >= 0); }

void wxCodeCompletionBox::DoDestroyTipWindow()
{
    if(m_tipWindow) {
        m_tipWindow->Hide();
        m_tipWindow->Destroy();
        m_tipWindow = NULL;
        m_displayedTip.Clear();
    }
}

void wxCodeCompletionBox::DoShowCompletionBox()
{
    CHECK_PTR_RET(m_stc);

    // guesstimate a line height
    wxMemoryDC dc;
    wxBitmap bmp(1, 1);
    dc.SelectObject(bmp);
    wxFont font = m_stc->StyleGetFont(0);
    dc.SetFont(font);
    wxSize textSize = dc.GetTextExtent("Tp");

    int lineHeight = textSize.y + 3; // 3 pixels margins
    wxRect rect = GetRect();
    wxSize screenSize = ::wxGetDisplaySize();

    // determine the box x position
    int wordStart = m_startPos;
    wxPoint pt = m_stc->PointFromPosition(wordStart);
    pt = m_stc->ClientToScreen(pt);
    pt.y += lineHeight;

    // Check Y axis
    if((pt.y + rect.GetHeight()) > screenSize.GetHeight()) {
        // the completion box goes out of the Y axis, move it up
        pt.y -= lineHeight;
        pt.y -= rect.GetHeight();
    }

    // Check X axis
    if((pt.x + rect.GetWidth()) > screenSize.GetWidth()) {
        // the completion box goes out of the X axis. Move it to the left
        pt.x -= ((pt.x + rect.GetWidth()) - screenSize.GetWidth());
    }
    Move(pt);
    Show();
}

void wxCodeCompletionBox::RemoveDuplicateEntries()
{
    wxStringSet_t matches;
    wxCodeCompletionBoxEntry::Vec_t uniqueList;
    for(size_t i = 0; i < m_allEntries.size(); ++i) {
        wxCodeCompletionBoxEntry::Ptr_t entry = m_allEntries.at(i);
        if(matches.count(entry->GetText()) == 0) {
            // new entry
            matches.insert(entry->GetText());
            uniqueList.push_back(entry);
        }
    }
    m_allEntries.swap(uniqueList);
}

wxBitmap wxCodeCompletionBox::GetBitmap(TagEntryPtr tag)
{
    InitializeDefaultBitmaps();
    int imgId = GetImageId(tag);
    if((imgId < 0) || (imgId >= (int)m_defaultBitmaps.size())) return wxNullBitmap;
    return m_defaultBitmaps.at(imgId);
}

void wxCodeCompletionBox::InitializeDefaultBitmaps()
{
    if(m_defaultBitmaps.empty()) {
        BitmapLoader* bmpLoader = clGetManager()->GetStdIcons();
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/class"));              // 0
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/struct"));             // 1
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/namespace"));          // 2
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_public"));      // 3
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/typedef"));            // 4
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_private"));     // 5
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_public"));      // 6
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_protected"));   // 7
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/function_private"));   // 8
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/function_public"));    // 9
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/function_protected")); // 10
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/typedef"));            // 11
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/enum"));               // 12
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/enumerator"));         // 13
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("mime/16/cpp"));              // 14
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("mime/16/h"));                // 15
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("mime/16/text"));             // 16
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/cpp_keyword"));        // 17
        m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/enum"));               // 18
    }
}

void wxCodeCompletionBox::OnMouseScroll(wxMouseEvent& event) { DoMouseScroll(event); }

void wxCodeCompletionBox::DoPgUp()
{
    int newindex = m_index - (LINES_PER_PAGE - 1);
    if(newindex < 0) {
        newindex = 0;
    }
    // only refresh if there was an actual change
    if(m_index != newindex) {
        m_index = newindex;
        DoDisplayTipWindow();
        Refresh();
    }
}

void wxCodeCompletionBox::DoPgDown()
{
    int newindex = m_index + (LINES_PER_PAGE - 1);
    if(newindex >= (int)m_entries.size()) {
        newindex = (int)m_entries.size() - 1;
    }
    // only refresh if there was an actual change
    if(m_index != newindex) {
        m_index = newindex;
        DoDisplayTipWindow();
        Refresh();
    }
}

void wxCodeCompletionBox::DoMouseScroll(wxMouseEvent& event)
{
    if(event.GetWheelRotation() < 0) {
        // down
        DoScrollDown();
    } else {
        DoScrollUp();
    }
}

wxString wxCodeCompletionBox::GetFilter()
{
    if(!m_stc) return "";
    int start = m_startPos;
    int end = m_stc->GetCurrentPos();

    return m_stc->GetTextRange(start, end);
}
