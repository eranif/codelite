#include "wxCodeCompletionBox.h"

#include "ColoursAndFontsManager.h"
#include "CxxTemplateFunction.h"
#include "StringUtils.h"
#include "bitmap_loader.h"
#include "cc_box_tip_window.h"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"
#include "wxCodeCompletionBoxManager.h"

#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/display.h>
#include <wx/font.h>
#include <wx/stc/stc.h>

wxCodeCompletionBox::BmpVec_t wxCodeCompletionBox::m_defaultBitmaps;
thread_local bool strip_html_tags = false;

wxCodeCompletionBox::wxCodeCompletionBox(wxWindow* parent, wxEvtHandler* eventObject, size_t flags)
    : wxCodeCompletionBoxBase(parent)
    , m_stc(NULL)
    , m_startPos(wxNOT_FOUND)
    , m_eventObject(eventObject)
    , m_tipWindow(NULL)
    , m_flags(flags)
{
    MSWSetWindowDarkTheme(this);
    // Use the active editor's font (if any)
    wxColour bgColour;
    wxColour textColour;
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if(!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("text");
    }
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        bgColour = editor->GetCtrl()->StyleGetBackground(0);
        textColour = editor->GetCtrl()->StyleGetForeground(0);
    } else {
        bgColour = clSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    }

    // bool isDark = DrawingUtils::IsDark(bgColour);
    clColours colours;
    colours.InitFromColour(bgColour);
    colours.SetSelItemBgColour(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    colours.SetSelItemTextColour(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    colours.SetSelItemBgColourNoFocus(colours.GetSelItemBgColour());
    colours.SetSelItemTextColourNoFocus(colours.GetSelItemTextColour());

    SetBackgroundColour(colours.GetBorderColour());
    m_mainPanel->SetBackgroundColour(colours.GetBorderColour());
    m_ccFont = ColoursAndFontsManager::Get().GetFixedFont(true);

    // m_ccFont.SetPointSize(m_ccFont.GetPointSize() - clGetSize(1, this));
    m_list->SetNativeTheme(false);
    m_list->SetRendererType(eRendererType::RENDERER_DIRECT2D);
    m_list->SetColours(colours);
    m_list->SetDefaultFont(m_ccFont);

    // Calculate a suitable completion dialog width
    {
        wxClientDC dc(this);
        dc.SetFont(m_ccFont);

        wxSize textSize = dc.GetTextExtent(wxString('X', 70));
        SetSizeHints(textSize.GetWidth(), (textSize.GetHeight() * 10));
        SetSize(textSize.GetWidth(), (textSize.GetHeight() * 10));
    }

    // Set the default bitmap list
    BitmapLoader* bmpLoader = clGetManager()->GetStdIcons();
    m_bitmaps.push_back(bmpLoader->LoadBitmap("class")); // 0
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindClass, m_bitmaps.size() - 1 });
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindConstructor, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("struct")); // 1
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindStruct, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("namespace")); // 2
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindModule, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("member_public")); // 3
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindVariable, m_bitmaps.size() - 1 });
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindField, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("typedef")); // 4
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindReference, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("member_private")); // 5
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindVariable, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("member_public")); // 6
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindVariable, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("member_protected")); // 7
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindVariable, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("function_private")); // 8
    m_bitmaps.push_back(bmpLoader->LoadBitmap("function_public"));  // 9
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindFunction, m_bitmaps.size() - 1 });
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindMethod, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("function_protected")); // 10

    m_bitmaps.push_back(bmpLoader->LoadBitmap("macro")); // 11
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindText, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("enum")); // 12
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindEnum, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("enumerator")); // 13
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindValue, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("mime-cpp"));    // 14
    m_bitmaps.push_back(bmpLoader->LoadBitmap("mime-h"));      // 15
    m_bitmaps.push_back(bmpLoader->LoadBitmap("mime-text"));   // 16
    m_bitmaps.push_back(bmpLoader->LoadBitmap("cpp_keyword")); // 17
    m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindKeyword, m_bitmaps.size() - 1 });

    m_bitmaps.push_back(bmpLoader->LoadBitmap("enum")); // 18

    InitializeDefaultBitmaps();
    m_list->SetBitmaps(&m_bitmaps);

    m_bmpDown = wxXmlResource::Get()->LoadBitmap("cc-box-down");
    m_bmpDownEnabled = m_bmpDown.ConvertToDisabled();

    m_bmpUp = wxXmlResource::Get()->LoadBitmap("cc-box-up");
    m_bmpUpEnabled = m_bmpUp.ConvertToDisabled();
    m_list->SetSortFunction(nullptr);
}

wxCodeCompletionBox::~wxCodeCompletionBox() { DoDestroyTipWindow(); }

void wxCodeCompletionBox::Reset(wxEvtHandler* eventObject, size_t flags)
{
    m_eventObject = eventObject;
    m_flags = flags;
    DoDestroyTipWindow();
    m_allEntries.clear();
    m_startPos = wxNOT_FOUND;
    m_stc = nullptr;
    m_entries.clear();
    m_list->DeleteAllItems();
}

void wxCodeCompletionBox::ShowCompletionBox(wxStyledTextCtrl* ctrl, const wxCodeCompletionBoxEntry::Vec_t& entries)
{
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
    size_t startsWithCount = 0;
    size_t containsCount = 0;
    size_t exactMatchCount = 0;
    FilterResults(true, startsWithCount, containsCount, exactMatchCount);
    wxUnusedVar(containsCount);

    // If we got a single match - insert it
    if(m_entries.size() == 1 && (m_flags & kInsertSingleMatch)) {
        wxString entryText = m_entries[0]->GetText().BeforeFirst('(');
        if(startsWithCount == 1 && entryText.CmpNoCase(GetFilter()) == 0) {
            DoDestroy();
            return;
        }
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
    if(m_list->GetItemCount() == 0) {
        DoDestroyTipWindow();
    } else {
        wxDataViewItem item = m_list->GetSelection();
        CHECK_ITEM_RET(item);

        size_t index = static_cast<size_t>(m_list->GetItemData(item));

        wxString docComment = m_entries.at(index)->GetComment();
        docComment.Trim().Trim(false);

        if(docComment.IsEmpty()) {
            // No tip to display
            DoDestroyTipWindow();

        } else if(!docComment.IsEmpty() && docComment != m_displayedTip) {
            // destroy old tip window
            DoDestroyTipWindow();

            // keep the old tip
            m_displayedTip = docComment;

            // Construct a new tip window and display the tip
            m_tipWindow = new CCBoxTipWindow(GetParent(), docComment, strip_html_tags);
            m_tipWindow->PositionRelativeTo(this, m_stc->PointFromPosition(m_stc->GetCurrentPos()));

            // restore focus to the editor
            m_stc->CallAfter(&wxStyledTextCtrl::SetFocus);
        }
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

bool wxCodeCompletionBox::FilterResults(bool updateEntries, size_t& startsWithCount, size_t& containsCount,
                                        size_t& exactMatchCount)
{
    containsCount = 0;
    startsWithCount = 0;
    wxString word = GetFilter();
    if(word.IsEmpty()) {
        if(updateEntries) {
            m_entries = m_allEntries;
        }
        return false;
    }

    if(updateEntries) {
        m_entries.clear();
    }
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

    startsWithCount = startsWith.size() + startsWithI.size() + exactMatches.size() + exactMatchesI.size();
    containsCount = startsWithCount + contains.size() + containsI.size();
    exactMatchCount = exactMatches.size();

    // Merge the results
    if(updateEntries) {
        m_entries.insert(m_entries.end(), exactMatches.begin(), exactMatches.end());
        m_entries.insert(m_entries.end(), exactMatchesI.begin(), exactMatchesI.end());
        m_entries.insert(m_entries.end(), startsWith.begin(), startsWith.end());
        m_entries.insert(m_entries.end(), startsWithI.begin(), startsWithI.end());
        m_entries.insert(m_entries.end(), contains.begin(), contains.end());
        m_entries.insert(m_entries.end(), containsI.begin(), containsI.end());
    }
    return exactMatches.empty() && exactMatchesI.empty() && startsWith.empty() && startsWithI.empty();
}

void wxCodeCompletionBox::InsertSelection(wxCodeCompletionBoxEntry::Ptr_t entry)
{

    if(m_stc) {
        wxCodeCompletionBoxEntry::Ptr_t match = entry;
        if(match == nullptr) {
            wxDataViewItem item = m_list->GetSelection();
            CHECK_PTR_RET(item);
            size_t index = static_cast<size_t>(m_list->GetItemData(item));

            match = m_entries[index];
        }

        // Let the owner override the default behavior
        clCodeCompletionEvent e(wxEVT_CCBOX_SELECTION_MADE);
        e.SetWord(match->GetInsertText());
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
            wxCodeCompletionBoxManager::Get().CallAfter(&wxCodeCompletionBoxManager::InsertSelection, match, true);
        }
    }
}

wxCodeCompletionBoxEntry::Vec_t wxCodeCompletionBox::TagsToEntries(const TagEntryPtrVector_t& tags)
{
    wxCodeCompletionBoxEntry::Vec_t entries;
    for(size_t i = 0; i < tags.size(); ++i) {
        TagEntryPtr tag = tags[i];
        wxCodeCompletionBoxEntry::Ptr_t entry = TagToEntry(tag);
        entries.push_back(entry);
    }
    return entries;
}

int wxCodeCompletionBox::GetImageId(TagEntryPtr entry)
{
    wxString kind = entry->GetKind();
    wxString access = entry->GetAccess();
    if(kind == wxT("class"))
        return 0;
    if(kind == wxT("struct"))
        return 1;
    if(kind == wxT("namespace"))
        return 2;
    if(kind == wxT("variable"))
        return 3;
    if(kind == wxT("typedef"))
        return 4;
    if(kind == wxT("member") && access.Contains(wxT("private")))
        return 5;
    if(kind == wxT("member") && access.Contains(wxT("public")))
        return 6;
    if(kind == wxT("member") && access.Contains(wxT("protected")))
        return 7;
    // member with no access? (maybe part of namespace??)
    if(kind == wxT("member"))
        return 6;
    if((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("private")))
        return 8;
    if((kind == wxT("function") || kind == wxT("prototype")) && (access.Contains(wxT("public")) || access.IsEmpty()))
        return 9;
    if((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("protected")))
        return 10;
    if(kind == wxT("macro"))
        return 11;
    if(kind == wxT("enum"))
        return 12;
    if(kind == wxT("enumerator"))
        return 13;
    if(kind == wxT("cpp_keyword"))
        return 17;
    if(kind == "cenum")
        return 18;
    return wxNOT_FOUND;
}

void wxCodeCompletionBox::ShowCompletionBox(wxStyledTextCtrl* ctrl, const TagEntryPtrVector_t& tags)
{
    ShowCompletionBox(ctrl, TagsToEntries(tags));
}

void wxCodeCompletionBox::DoUpdateList()
{
    size_t startsWithCount = 0;
    size_t containsCount = 0;
    size_t exactMatchCount = 0;

    bool refreshList = FilterResults(true, startsWithCount, containsCount, exactMatchCount);
    wxUnusedVar(containsCount);
    wxUnusedVar(refreshList);

    // If there a single entry exact match hide the cc box
    if(m_entries.size() == 1) {
        wxString entryText = m_entries[0]->GetText().BeforeFirst('(');
        if(entryText.CmpNoCase(GetFilter()) == 0) {
            CallAfter(&wxCodeCompletionBox::DoDestroy);
            return;
        }
    }

    // int curpos = m_stc->GetCurrentPos();
    if(!GetFilter().empty() && (m_entries.empty() && !m_allEntries.empty())) {
        // the CC might not reported all possible matches
        // (we have a limit to the number of matches we display)
        // trigger another CC action
        wxCommandEvent event(wxEVT_MENU, XRCID("complete_word"));
        wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
        DoDestroy();
    } else {
        DoDisplayTipWindow();
        DoPopulateList();
    }

    if(exactMatchCount == 0) {
        wxCommandEvent event(wxEVT_MENU, XRCID("complete_word"));
        wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
    }
}

wxCodeCompletionBoxEntry::Ptr_t wxCodeCompletionBox::TagToEntry(TagEntryPtr tag)
{
    wxString text = tag->GetDisplayName().Trim().Trim(false);
    int imgIndex = GetImageId(tag);
    wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(text, imgIndex);
    entry->m_tag = tag;
    entry->SetInsertText(text.BeforeFirst('('));
    entry->SetIsFunction(tag->IsMethod());
    entry->SetIsTemplateFunction(tag->IsTemplateFunction());

    wxString sig = tag->GetSignature();
    sig = sig.AfterFirst('(').BeforeLast(')');
    entry->SetSignature(sig);
    return entry;
}

void wxCodeCompletionBox::DoDestroy() { wxCodeCompletionBoxManager::Get().DestroyCCBox(); }

void wxCodeCompletionBox::DoDestroyTipWindow()
{
    if(m_tipWindow) {
        m_tipWindow->Hide();
        m_tipWindow->Destroy();
        m_tipWindow = NULL;
        m_displayedTip.Clear();
    }
}

void wxCodeCompletionBox::StcLeftDown(wxMouseEvent& event)
{
    event.Skip();
    DoDestroy();
}

void wxCodeCompletionBox::StcKeyDown(wxKeyEvent& event)
{
    switch(event.GetKeyCode()) {
    case WXK_UP:
        m_list->LineUp();
        break;
    case WXK_DOWN:
        m_list->LineDown();
        break;
    case WXK_PAGEDOWN: {
        m_list->PageDown();
        break;
    }
    case WXK_PAGEUP: {
        m_list->PageUp();
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
    default: {
        int modifier_key = event.GetModifiers();
        wxChar ch = event.GetUnicodeKey();
        if (modifier_key == wxMOD_CONTROL && ch == 'U') {
            m_list->PageUp();
        } else if (modifier_key == wxMOD_CONTROL && ch == 'D') {
            m_list->PageDown();
        } else if (modifier_key == wxMOD_CONTROL && (ch == 'J' || ch == 'N')) {
            m_list->LineDown();
        }  else if (modifier_key == wxMOD_CONTROL && (ch == 'K' || ch == 'P')) {
            m_list->LineUp();
        } else {
            event.Skip();
        }
        break;
    }
    }
}

void wxCodeCompletionBox::DoShowCompletionBox()
{
    CHECK_PTR_RET(m_stc);
    DoPopulateList();

    int lineHeight = m_stc->TextHeight(m_stc->GetCurrentLine());
    wxRect rect = GetRect();

    // determine the box x position
    int wordStart = m_startPos;
    wxPoint pt = m_stc->PointFromPosition(wordStart);
    pt = m_stc->ClientToScreen(pt);
    pt.y += lineHeight;

    wxRect screenSize = clGetDisplaySize();
    int displayIndex = wxDisplay::GetFromPoint(pt);
    if(displayIndex != wxNOT_FOUND) {
        screenSize = wxDisplay(displayIndex).GetGeometry();
    }

    // Check Y axis
    if((pt.y + rect.GetHeight()) > screenSize.GetHeight()) {
        // the completion box goes out of the Y axis, move it up
        pt.y -= lineHeight;
        pt.y -= rect.GetHeight();
    }

    // Check X axis
    if((pt.x + rect.GetWidth()) > (screenSize.GetX() + screenSize.GetWidth())) {
        // the completion box goes out of the X axis. Move it to the left
        pt.x = (screenSize.GetX() + screenSize.GetWidth()) - rect.GetWidth();
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
    if((imgId < 0) || (imgId >= (int)m_defaultBitmaps.size()))
        return wxNullBitmap;
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

wxString wxCodeCompletionBox::GetFilter()
{
    if(!m_stc)
        return "";
    int start = m_startPos;
    int end = m_stc->GetCurrentPos();

    return m_stc->GetTextRange(start, end);
}

void wxCodeCompletionBox::ShowCompletionBox(wxStyledTextCtrl* ctrl, const LSP::CompletionItem::Vec_t& completions)
{
    ShowCompletionBox(ctrl, LSPCompletionsToEntries(completions));
}

wxCodeCompletionBoxEntry::Vec_t
wxCodeCompletionBox::LSPCompletionsToEntries(const LSP::CompletionItem::Vec_t& completions)
{
    wxCodeCompletionBoxEntry::Vec_t entries;
    for(size_t i = 0; i < completions.size(); ++i) {
        LSP::CompletionItem::Ptr_t completion = completions[i];
        wxString text = completion->GetLabel();
        int imgIndex = GetImageId(completion);
        wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(text, imgIndex);

        wxString comment;
        if(!completion->GetDetail().IsEmpty()) {
            wxString detail = completion->GetDetail();
            StringUtils::DisableMarkdownStyling(detail);
            comment << detail;
        }
        if(!completion->GetDocumentation().GetValue().IsEmpty()) {
            if(!comment.IsEmpty()) {
                comment << "\n---\n";
            }
            comment << completion->GetDocumentation().GetValue();
        }

        // if 'insertText' is provided, use it instead of the label
        wxString insertText;
        insertText = completion->GetInsertText().IsEmpty() ? completion->GetLabel() : completion->GetInsertText();
        if(completion->HasTextEdit()) {
            // According to the spec: if textEdit exists, we ignore 'insertText'
            insertText = completion->GetTextEdit()->GetNewText();
        }

        static wxRegEx reTemplateFunction("([$_a-zA-Z0-9\\:]+)[ \t]*<.*?>\\(", wxRE_ADVANCED);
        entry->SetInsertText(insertText);
        wxChar ch = text.empty() ? 0 : text[0];
        // Handle special cases
        if(reTemplateFunction.IsValid() && reTemplateFunction.Matches(text)) {
            entry->SetInsertText(reTemplateFunction.GetMatch(text, 1) + "<|>");
            entry->SetIsSnippet(true);
        } else if(text.StartsWith("include <")) {
            // include statement
            entry->SetInsertText("include <|");
            entry->SetIsSnippet(true);
        } else if(text.StartsWith("include \"")) {
            entry->SetInsertText("include \"|");
            entry->SetIsSnippet(true);
        } else if(ch == L'•') {
            // this completion entry triggers an #include insertion
            entry->SetTriggerInclude(!completion->GetAdditionalText().empty());
            if(entry->IsTriggerInclude()) {
                const auto& v = completion->GetAdditionalText();
                comment = v[0]->GetNewText();
                comment.Trim().Trim(false);
                if(comment.Contains("\"")) {
                    comment = comment.AfterFirst('"');
                    comment.Prepend("\"");
                } else if(comment.Contains("<")) {
                    comment = comment.AfterFirst('<');
                    comment.Prepend("<");
                }
            }
        }

        entry->SetImgIndex(imgIndex);
        entry->SetComment(comment);
        entry->SetIsFunction(completion->GetKind() == LSP::CompletionItem::kKindConstructor ||
                             completion->GetKind() == LSP::CompletionItem::kKindFunction ||
                             completion->GetKind() == LSP::CompletionItem::kKindMethod);
        entry->SetIsTemplateFunction(completion->GetLabel().Contains("<") && completion->GetLabel().Contains(">"));
        if(entry->IsFunction()) {
            // extract the function signature from the label
            wxString label = completion->GetLabel();
            wxString signature = label.AfterFirst('(');
            signature = signature.BeforeLast(')');
            signature.Trim().Trim(false);
            entry->SetSignature(signature);
        }
        entries.push_back(entry);
    }
    return entries;
}

int wxCodeCompletionBox::GetImageId(LSP::CompletionItem::Ptr_t entry) const
{
    if(m_lspCompletionItemImageIndexMap.count(entry->GetKind())) {
        return m_lspCompletionItemImageIndexMap.find(entry->GetKind())->second;
    }
    return m_lspCompletionItemImageIndexMap.find(LSP::CompletionItem::kKindText)->second;
}

void wxCodeCompletionBox::DoPopulateList()
{
    // Fill the control with the entries
    m_list->DeleteAllItems();

    m_list->Begin();
    wxVector<wxVariant> cols;
    for(size_t i = 0; i < m_entries.size(); ++i) {
        wxCodeCompletionBoxEntry::Ptr_t cc_item = m_entries[i];
        cols.clear();
        cols.push_back(::MakeBitmapIndexText(cc_item->GetText(), cc_item->GetImgIndex()));
        m_list->AppendItem(cols, (wxUIntPtr)i);
    }
    m_list->Commit();
    // Select the first item
    if(m_list->GetItemCount()) {
        m_list->Select(m_list->RowToItem(0));
    }
}

void wxCodeCompletionBox::OnSelectionActivated(wxDataViewEvent& event)
{
    event.Skip();
    InsertSelection();
    CallAfter(&wxCodeCompletionBox::DoDestroy);
}

void wxCodeCompletionBox::OnSelectionChanged(wxDataViewEvent& event)
{
    event.Skip();
    CallAfter(&wxCodeCompletionBox::DoDisplayTipWindow);
}

void wxCodeCompletionBox::SetStripHtmlTags(bool strip) { strip_html_tags = strip; }
