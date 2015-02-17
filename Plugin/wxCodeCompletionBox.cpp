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

#define MAX_NUM_LINES 8
#define Y_SPACER 2
#define BOX_WIDTH (400 + 8)

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

wxCodeCompletionBox::wxCodeCompletionBox(wxWindow* parent, wxEvtHandler* eventObject, size_t flags)
    : wxCodeCompletionBoxBase(parent)
    , m_index(0)
    , m_stc(NULL)
    , m_startPos(wxNOT_FOUND)
    , m_eventObject(eventObject)
    , m_tipWindow(NULL)
    , m_flags(flags)
{
    m_ccFont = wxFont(wxFontInfo(DEFAULT_FONT_SIZE).Family(wxFONTFAMILY_TELETYPE).FaceName(DEFAULT_FACE_NAME));
    SetCursor(wxCURSOR_HAND);

    // Calculate the size of the box
    int singleLineHeight = GetSingleLineHeight();
    int boxHeight = singleLineHeight * MAX_NUM_LINES;
    int boxWidth = BOX_WIDTH; // 100 pixels
    wxSize boxSize = wxSize(boxWidth, boxHeight);
    wxRect rect(boxSize);

    // Set the default bitmap list
    BitmapLoader* bmpLoader = ::clGetManager()->GetStdIcons();

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

    // Increase the size by 2 pixel for each dimension
    rect.Inflate(2, 2);
    SetSize(rect);
    m_canvas->SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_canvas->Bind(wxEVT_LEFT_DOWN, &wxCodeCompletionBox::OnLeftDClick, this);
    m_canvas->Bind(wxEVT_LEFT_DCLICK, &wxCodeCompletionBox::OnLeftDClick, this);

    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSING, &wxCodeCompletionBox::OnDismissBox, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &wxCodeCompletionBox::OnDismissBox, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CLOSING, &wxCodeCompletionBox::OnDismissBox, this);
    wxTheApp->Bind(wxEVT_ACTIVATE_APP, &wxCodeCompletionBox::OnAppActivate, this);
}

wxCodeCompletionBox::~wxCodeCompletionBox()
{
    m_canvas->Unbind(wxEVT_LEFT_DOWN, &wxCodeCompletionBox::OnLeftDClick, this);
    m_canvas->Unbind(wxEVT_LEFT_DCLICK, &wxCodeCompletionBox::OnLeftDClick, this);
    if(m_stc) {
        m_stc->Unbind(wxEVT_STC_MODIFIED, &wxCodeCompletionBox::OnStcModified, this);
        m_stc->Unbind(wxEVT_STC_CHARADDED, &wxCodeCompletionBox::OnStcCharAdded, this);
        m_stc->Unbind(wxEVT_KEY_DOWN, &wxCodeCompletionBox::OnStcKey, this);
        m_stc->Unbind(wxEVT_LEFT_DOWN, &wxCodeCompletionBox::OnStcLeftDown, this);
    }

    if(m_tipWindow) {
        m_tipWindow->Hide();
        m_tipWindow->Destroy();
        m_tipWindow = NULL;
    }
    wxTheApp->Unbind(wxEVT_ACTIVATE_APP, &wxCodeCompletionBox::OnAppActivate, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &wxCodeCompletionBox::OnDismissBox, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CLOSING, &wxCodeCompletionBox::OnDismissBox, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSING, &wxCodeCompletionBox::OnDismissBox, this);
    wxCodeCompletionBoxManager::Get().WindowDestroyed();
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
    // Draw the entire box with single solid colour
    dc.SetBrush(lightBorder);
    dc.SetPen(lightBorder);
    dc.DrawRectangle(rect);

    // Shrink the rectangle by 2 to provide a 2 pixle
    // border
    rect.Deflate(2, 2);
    dc.SetBrush(bgColour);
    dc.SetPen(bgColour);
    dc.DrawRectangle(rect);

    // We want to have a scrollbar alike. We do this by reducing the size of the
    // items area by 5 pixels
    rect.SetWidth(rect.GetWidth() - 8);
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
        dc.SetClippingRegion(itemRect);

        // Truncate the text to fit the screen
        wxString choppedText;
        DrawingUtils::TruncateText(entry->GetText(), itemRect.GetWidth(), dc, choppedText);

        dc.DrawText(choppedText, x, y);
        dc.DestroyClippingRegion();
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

    // Keep the start position
    m_startPos = m_stc->WordStartPosition(m_stc->GetCurrentPos(), true);

    // If we got a single match - insert it
    if((m_entries.size() == 1) && (m_flags & kInsertSingleMatch)) {
        // single match
        HideAndInsertSelection();
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
        m_stc->Bind(wxEVT_STC_MODIFIED, &wxCodeCompletionBox::OnStcModified, this);
        m_stc->Bind(wxEVT_STC_CHARADDED, &wxCodeCompletionBox::OnStcCharAdded, this);
        m_stc->Bind(wxEVT_KEY_DOWN, &wxCodeCompletionBox::OnStcKey, this);
        m_stc->Bind(wxEVT_LEFT_DOWN, &wxCodeCompletionBox::OnStcLeftDown, this);

        // Set the focus back to the completion control
        m_stc->CallAfter(&wxStyledTextCtrl::SetFocus);
    }

    // Display the help window
    DoDisplayTipWindow();
}

void wxCodeCompletionBox::DoDisplayTipWindow()
{
    // Display the tooltip
    if(m_tipWindow) {
        m_tipWindow->Hide();
        m_tipWindow->Destroy();
        m_tipWindow = NULL;
    }

    if(m_index >= 0 && m_index < (int)m_entries.size()) {
        wxString docComment = m_entries.at(m_index)->GetComment();
        docComment.Trim().Trim(false);
        if(docComment.IsEmpty() && m_entries.at(m_index)->m_tag) {
            // Format the comment on demand if the origin was a tag entry
            docComment = m_entries.at(m_index)->m_tag->FormatComment();
        }

        if(!docComment.IsEmpty()) {
            m_tipWindow = new CCBoxTipWindow(GetParent(), docComment, 1, false);
            m_tipWindow->PositionRelativeTo(this, m_stc->PointFromPosition(m_stc->GetCurrentPos()));
            m_stc->CallAfter(&wxStyledTextCtrl::SetFocus);
        }
    }
}

void wxCodeCompletionBox::OnStcKey(wxKeyEvent& event)
{
    switch(event.GetKeyCode()) {
    case WXK_UP:
        if((m_index - 1) >= 0) {
            --m_index;
            DoDisplayTipWindow();
            Refresh();
        }
        break;
    case WXK_DOWN:
        if((m_index + 1) < (int)m_entries.size()) {
            ++m_index;
            DoDisplayTipWindow();
            Refresh();
        }
        break;
    case WXK_ESCAPE:
    case WXK_LEFT:
    case WXK_RIGHT:
    case WXK_HOME:
    case WXK_END:
    case WXK_DELETE:
    case WXK_NUMPAD_DELETE:
        Hide();
        Destroy();
        break;
    case WXK_TAB:
    case WXK_RETURN:
    case WXK_NUMPAD_ENTER:
        // Insert the selection
        HideAndInsertSelection();
        Destroy();
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
    int end = m_stc->GetCurrentPos();

    wxString word = m_stc->GetTextRange(start, end); // the current word
    if(word.IsEmpty()) {
        m_entries = m_allEntries;
        return;
    }

    m_entries.clear();
    wxString lcFilter = word.Lower();
    // Smart sorting:
    // We preare the list of matches in the following order:
    // Exact matches (case sensitive)
    // Exact matches (case in-sensitive)
    // Starts with
    // Contains
    wxCodeCompletionBoxEntry::Vec_t exactMatches, exactMatchesI, startsWith, contains;
    for(size_t i = 0; i < m_allEntries.size(); ++i) {
        wxString entryText = m_allEntries.at(i)->GetText().BeforeFirst('(');
        entryText.Trim().Trim(false);
        wxString lcEntryText = entryText.Lower();

        // Exact match:
        if(word == entryText) {
            exactMatches.push_back(m_allEntries.at(i));
        } else if(lcEntryText == lcFilter) {
            exactMatchesI.push_back(m_allEntries.at(i));
        } else if(lcEntryText.StartsWith(lcFilter)) {
            startsWith.push_back(m_allEntries.at(i));
        } else if(lcEntryText.Contains(lcFilter)) {
            contains.push_back(m_allEntries.at(i));
        }
    }

    // Merge the results
    m_entries.insert(m_entries.end(), exactMatches.begin(), exactMatches.end());
    m_entries.insert(m_entries.end(), exactMatchesI.begin(), exactMatchesI.end());
    m_entries.insert(m_entries.end(), startsWith.begin(), startsWith.end());
    m_entries.insert(m_entries.end(), contains.begin(), contains.end());
    m_index = 0;
}

void wxCodeCompletionBox::HideAndInsertSelection()
{
    if((m_index >= 0 && m_index < (int)m_entries.size()) && m_stc) {
        wxCodeCompletionBoxEntry::Ptr_t match = m_entries.at(m_index);

        // Let the owner override the default behavior
        clCodeCompletionEvent e(wxEVT_CCBOX_SELECTION_MADE);
        e.SetWord(match->GetText());
        e.SetEventObject(m_eventObject);
        if(EventNotifier::Get()->ProcessEvent(e)) {
            // A plugin handled the insertion, hide and return
            Hide();
            return;
        }

        // Default behviour: remove the partial text from teh editor and replace it
        // with the selection
        int start = m_stc->WordStartPosition(m_stc->GetCurrentPos(), true);
        int end = m_stc->WordEndPosition(m_stc->GetCurrentPos(), true);
        m_stc->SetSelection(start, end);

        wxString entryText = match->GetText();
        if(entryText.Find("(") != wxNOT_FOUND) {
            // a function like
            wxString textToInsert = entryText.BeforeFirst('(');
            
            // Build the function signature
            wxString funcSig = entryText.AfterFirst('(');
            funcSig = funcSig.BeforeLast(')');
            funcSig.Trim().Trim(false);
                        
            CL_DEBUG("Inserting selection: %s", textToInsert);
            CL_DEBUG("Signature is: %s", funcSig);
            
            textToInsert << "()";
            m_stc->CallAfter(&wxStyledTextCtrl::ReplaceSelection, textToInsert);
            if(!funcSig.IsEmpty()) {
                // Place the caret between the parenthesis
                int caretPos = start + textToInsert.Len() - 1;
                m_stc->SetCurrentPos(caretPos);
                m_stc->SetSelection(caretPos, caretPos);

                // trigger a code complete for function calltip.
                // We do this by simply mimicing the user action of going to the menubar:
                // Edit->Display Function Calltip
                wxCommandEvent event(wxEVT_MENU, XRCID("function_call_tip"));
                wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
            }
        } else {
            m_stc->CallAfter(&wxStyledTextCtrl::ReplaceSelection, entryText);
        }
        Hide();
    }
}

void wxCodeCompletionBox::OnStcLeftDown(wxMouseEvent& event)
{
    event.Skip();
    Hide(); // Hide before we destroy, it reduces the flicker on screen
    Destroy();
}

void wxCodeCompletionBox::OnLeftDClick(wxMouseEvent& event)
{
    for(size_t i = 0; i < m_entries.size(); ++i) {
        if(m_entries.at(i)->m_itemRect.Contains(event.GetPosition())) {
            m_index = i;
            HideAndInsertSelection();
            Destroy();
            return;
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
    return wxNOT_FOUND;
}

void wxCodeCompletionBox::ShowCompletionBox(wxStyledTextCtrl* ctrl, const TagEntryPtrVector_t& tags)
{
    ShowCompletionBox(ctrl, TagsToEntries(tags));
}

void wxCodeCompletionBox::DoUpdateList()
{
    FilterResults();

    int curpos = m_stc->GetCurrentPos();
    if(m_entries.empty() || curpos < m_startPos) {
        Hide();
        Destroy();
    } else {
        DoDisplayTipWindow();
        Refresh();
    }
}

void wxCodeCompletionBox::OnStcCharAdded(wxStyledTextEvent& event)
{
    event.Skip();
    int keychar = m_stc->GetCharAt(m_stc->PositionBefore(m_stc->GetCurrentPos()));
    if(((keychar >= 65) && (keychar <= 90)) ||  // A-Z
       ((keychar >= 97) && (keychar <= 122)) || // a-z
       ((keychar >= 48) && (keychar <= 57)) ||  // 0-9
       (keychar == 95))                         // Underscore
    {
        DoUpdateList();
    } else {
        Hide();
        Destroy();
    }
}

void wxCodeCompletionBox::OnStcModified(wxStyledTextEvent& event)
{
    event.Skip();
    DoUpdateList();
}

void wxCodeCompletionBox::OnAppActivate(wxActivateEvent& event)
{
    event.Skip();
    // Application lost / got focus - reset the tooltip
    Hide();
    Destroy();
}

void wxCodeCompletionBox::OnDismissBox(wxCommandEvent& event)
{
    event.Skip();
    Hide();
    Destroy();
}

wxCodeCompletionBoxEntry::Ptr_t wxCodeCompletionBox::TagToEntry(TagEntryPtr tag)
{
    wxString text = tag->GetDisplayName().Trim().Trim(false);
    int imgIndex = GetImageId(tag);
    wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(text, imgIndex);
    return entry;
}
