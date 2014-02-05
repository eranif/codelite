//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cc_box.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "cc_box.h"
#include "frame.h"
#include <wx/settings.h>
#include "comment_parser.h"
#include "code_completion_manager.h"
#include "code_completion_box.h"
#include "ctags_manager.h"
#include "event_notifier.h"
#include <wx/regex.h>
#include <wx/wupdlock.h>
#include "pluginmanager.h"
#include "editor_config.h"
#include "cl_editor_tip_window.h"
#include "cl_editor.h"
#include "globals.h"
#include <wx/imaglist.h>
#include <wx/xrc/xmlres.h>
#include "entry.h"
#include "plugin.h"
#include <wx/tooltip.h>
#include <wx/tipwin.h>
#include <wx/display.h>
#include "event_notifier.h"
#include "cc_box_tip_window.h"
#include "cl_command_event.h"

#ifdef __WXGTK__
#   define TIP_TIMER 100
#else
#   define TIP_TIMER 100
#endif

const wxEventType wxCMD_EVENT_SET_EDITOR_ACTIVE = XRCID("wxCMD_EVENT_SET_EDITOR_ACTIVE");
const wxEventType wxCMD_EVENT_DISMISS_CC_BOX = ::wxNewEventType();

CCBox::CCBox(LEditor* parent, bool autoHide, bool autoInsertSingleChoice)
#if CCBOX_USE_POPUP
    : CCBoxBase(wxTheApp->GetTopWindow(), wxID_ANY, wxPoint(-1000,-1000), wxSize(1, 1))
#else
    : CCBoxBase(parent, wxID_ANY, wxPoint(10000,10000), wxSize(1, 1))
#endif
    , m_height(BOX_HEIGHT)
    , m_autoHide(autoHide)
    , m_insertSingleChoice(autoInsertSingleChoice)
    , m_owner(NULL)
    , m_startPos(wxNOT_FOUND)
    , m_editor(parent)
    , m_tipWindow(NULL)
{
    Hide();
    m_constructing = true;
    
    m_refreshListTimer = new wxTimer(this);
    Connect(m_refreshListTimer->GetId(), wxEVT_TIMER, wxTimerEventHandler(CCBox::OnRefreshList), NULL, this);
    
    m_tipTimer = new wxTimer(this);
    Connect(m_tipTimer->GetId(), wxEVT_TIMER, wxTimerEventHandler(CCBox::OnDisplayTooltip), NULL, this);
    
    // load all the CC images
    wxImageList *il = new wxImageList(16, 16, true);

    //Initialise the file bitmaps
    BitmapLoader *bmpLoader = PluginManager::Get()->GetStdIcons();
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/class")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/struct")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/namespace")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_public")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/typedef")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_private")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_public")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_protected")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_private")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_public")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_protected")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/typedef")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/enum")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/enumerator")));
    il->Add(bmpLoader->LoadBitmap(wxT("mime/16/cpp")));
    il->Add(bmpLoader->LoadBitmap(wxT("mime/16/h")));
    il->Add(bmpLoader->LoadBitmap(wxT("mime/16/text")));
    il->Add(bmpLoader->LoadBitmap(wxT("cc/16/cpp_keyword")));

    m_isTipBgDark = DrawingUtils::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

    if ( m_isTipBgDark ) {
        // the tooltip colour is dark
        m_editor->CallTipSetForegroundHighlight(wxT("YELLOW"));
    }

    // assign the image list and let the control take owner ship (i.e. delete it)
    m_listCtrl->AssignImageList(il, wxIMAGE_LIST_SMALL);
    m_listCtrl->InsertColumn(0, wxT("Name"));
    m_listCtrl->SetColumnWidth(0, m_listCtrl->GetClientSize().x - wxSystemSettings::GetMetric(wxSYS_VSCROLL_X));
    m_constructing = false;

    EventNotifier::Get()->Connect(wxEVT_TIP_BTN_CLICKED_DOWN, wxCommandEventHandler(CCBox::OnTipClickedDown), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_TIP_BTN_CLICKED_UP,   wxCommandEventHandler(CCBox::OnTipClickedUp),   NULL, this);
}

CCBox::~CCBox()
{
    EventNotifier::Get()->Disconnect(wxEVT_TIP_BTN_CLICKED_DOWN, wxCommandEventHandler(CCBox::OnTipClickedDown), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_TIP_BTN_CLICKED_UP,   wxCommandEventHandler(CCBox::OnTipClickedUp),   NULL, this);

    wxDELETE(m_refreshListTimer);
    wxDELETE(m_tipTimer);
    
    if ( m_tipWindow ) {
        m_tipWindow->Destroy();
        m_tipWindow = NULL;
    }
}

void CCBox::OnItemActivated( wxListEvent& event )
{
#if CCBOX_USE_POPUP
    m_selectedItem = event.m_itemIndex;
    InsertSelection();
    HideCCBox();
#endif
}

void CCBox::OnItemDeSelected( wxListEvent& event )
{
#ifdef __WXMAC__
    m_listCtrl->Select(event.m_itemIndex, false);
#endif
    m_selectedItem = wxNOT_FOUND;
}

void CCBox::OnItemSelected( wxListEvent& event )
{
    if(m_selectedItem == event.m_itemIndex)
        return;

    m_selectedItem = event.m_itemIndex;
    m_listCtrl->Refresh();
    PostSelectItem(m_selectedItem);
}

void CCBox::Show(const TagEntryPtrVector_t &tags, const wxString &word, bool isKeywordsList, wxEvtHandler *owner)
{
    if (tags.empty()) {
        return;
    }
    m_tags = tags;
    m_owner = owner;
    m_isKeywordsList = isKeywordsList;
    Show(word);
}

void CCBox::Display(LEditor *editor)
{
#if !CCBOX_USE_POPUP
    Move( 10000, 10000 );
#endif
    CCBoxParent::Show(false);

    m_editor = editor;
    int     point      = editor->GetCurrentPos();
    wxPoint pt         = editor->PointFromPosition(point); // Point is in editor's coordinates
    int     lineHeight = editor->GetCurrLineHeight();

    // add the line height
    pt.y += lineHeight;
    wxPoint ccPoint = pt;
    
#if CCBOX_USE_POPUP

    wxSize size = ::wxGetDisplaySize();
    // FIX BUG#3032473: "CC box position and dual screen"
    // Incase we are using multiple screens,
    // re-calculate the width
    unsigned displayCount = wxDisplay::GetCount();
    if(displayCount > 1) {
        size.x = 0;

        for(unsigned i=0; i<displayCount; i++) {
            wxDisplay display(i);
            size.x += display.GetGeometry().width;
        }
    }
    // the completion box is child of the main frame
    // Make sure that CC box remains is fully visible
    ccPoint = editor->ClientToScreen(pt);
    int diff = ccPoint.x + BOX_WIDTH - size.x;
    // Handle the X axis
    if(ccPoint.x + BOX_WIDTH > size.x) {
        ccPoint.x -= diff;
        pt.x      -= diff;
    }

    if(ccPoint.y + BOX_HEIGHT > size.y) {
        ccPoint.y -= BOX_HEIGHT;
        ccPoint.y -= lineHeight;

        pt.y -=      BOX_HEIGHT;
        pt.y -=      lineHeight;

#ifdef __WXMSW__
        // Under Windows it seems that we need another 5 pixels ...
        pt.y -= 5;
        ccPoint.y -= 5;
#endif
    }

    // adjust the X axis
    if (size.x - pt.x < BOX_WIDTH) {
        // the box is too wide to fit the screen
        if (size.x > BOX_WIDTH) {
            // the screen can contain the completion box
            pt.x = size.x - BOX_WIDTH;
        } else {
            // this will provive the maximum visible area
            pt.x = 0;
        }
    }
#else // CCBOX_USE_POPUP
    
    //-------------------------------------------------
    // Using wxPanel as our completion box
    //-------------------------------------------------
    wxSize size = editor->GetClientSize();
    
    // X-axis
    int diff = ccPoint.x + BOX_WIDTH - size.x;
    if((ccPoint.x + BOX_WIDTH) > size.x) {
        ccPoint.x -= diff;
    }
    
    // Y-axis
    if ((ccPoint.y + BOX_HEIGHT) > size.y) {
        ccPoint.y -= (BOX_HEIGHT+lineHeight);
    }
#endif
    Move( ccPoint );
    CCBoxParent::Show(true);
}

bool CCBox::SelectWord(const wxString& word)
{
    bool fullMatch;
    long item = m_listCtrl->FindMatch(word, fullMatch);
    if (item != wxNOT_FOUND) {
        // first unselect the current item
        if (m_selectedItem != wxNOT_FOUND && m_selectedItem != item) {
            m_listCtrl->Select(m_selectedItem, false);
        }

        if ( m_selectedItem != item ) {
            // We are changing selection
            m_selectedItem = item;
            SelectItem(m_selectedItem);
        }

        if(fullMatch) {
            // Incase we got a full match, insert the selection and release the completion box
            InsertSelection();
            if (m_editor) {
                m_editor->SetActive();
            }
            HideCCBox();
            return true;
        }

    } else if(m_isKeywordsList) {
        // When there is no match and the list box is of type "Keywords"
        // hide it...
        HideCCBox();
        return false;
    }

    m_refreshListTimer->Stop();
    if ( !m_isKeywordsList ) {
        m_refreshListTimer->Start(TIP_TIMER, true);
    }
    return fullMatch;
}

void CCBox::Next()
{
    if (m_selectedItem != wxNOT_FOUND) {
        if (m_selectedItem + 1 < m_listCtrl->GetItemCount()) {
            m_selectedItem++;
            // select next item
            SelectItem(m_selectedItem);
        }
    }
}

void CCBox::Previous()
{
    if (m_selectedItem != wxNOT_FOUND) {
        if (m_selectedItem - 1 >= 0) {
            m_selectedItem--;

            // select previous item
            SelectItem(m_selectedItem);
        }
    }
}

void CCBox::SelectItem(long item)
{
    if(item == m_listCtrl->GetNextSelected(-1))
        return;

    if ( item < 0 || item >= m_listCtrl->GetItemCount() )
        return;

    if (m_listCtrl->GetFirstSelected() != -1) {
        m_listCtrl->RefreshItem(m_listCtrl->GetFirstSelected());
    }
    m_listCtrl->Select(item);
    m_listCtrl->EnsureVisible(item);

    PostSelectItem(item);
}

void CCBox::PostSelectItem(long item)
{
    if( m_tipWindow ) {
        m_tipWindow->Destroy();
        m_tipWindow = NULL;
    }
    
    m_tipTimer->Stop();
    m_tipTimer->Start(200, true);
    
    //CCItemInfo tag;
    //if(m_listCtrl->GetItemTagEntry(item, tag)) {
    //    DoFormatDescriptionPage( tag );
    //}
    m_editor->SetActive();
}

void CCBox::Show(const wxString& word)
{
    wxString lastName;
    size_t i(0);
    std::vector<CCItemInfo> _tags;

    m_listCtrl->SetCursor( wxCursor(wxCURSOR_ARROW) );
    this->SetCursor( wxCursor(wxCURSOR_ARROW) );

    CCItemInfo item;

    clWindowUpdateLocker locker(m_listCtrl);

    m_listCtrl->DeleteAllItems();

    // Get the associated editor
    wxStringSet_t uniqueTags;
    if (m_tags.empty() == false) {
        _tags.reserve(m_tags.size());
        for (; i<m_tags.size(); i++) {
            TagEntryPtr tag = m_tags.at(i);
            wxString access = tag->GetAccess();

            if(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_KEEP_FUNCTION_SIGNATURE_UNFORMATTED) {
                tag->SetFlags(tag->GetFlags() | TagEntry::Tag_No_Signature_Format);
            }

            // start a new group
            
            wxString displayName = tag->GetDisplayName().Trim().Trim(false);
            if ( uniqueTags.count( displayName ) ) {
                continue;
            }
            
            uniqueTags.insert( displayName );
            
            item.Reset();
            item.displayName = displayName;
            item.imgId       = GetImageId(tag);
            item.tag         = *tag;
            item.listOfTags.push_back( *tag );
            _tags.push_back(item);
        }
    }

    if (_tags.size() == 1 && m_insertSingleChoice) {
        m_selectedItem = 0;
        DoInsertSelection(_tags.at(0).tag.GetName(), false);

        // return without calling to wxWindow::Show()
        // also, make sure we are hidden
        if ( IsShown() ) {
            HideCCBox();
        }
        if( m_tipWindow ) {
            m_tipWindow->Destroy();
            m_tipWindow = NULL;
        }

        return;
    }

    m_listCtrl->SetItems(_tags);
    m_listCtrl->SetItemCount(_tags.size());
    m_selectedItem = 0;

    bool fullMatch;
    m_selectedItem = m_listCtrl->FindMatch(word, fullMatch);
    if ( m_selectedItem == wxNOT_FOUND && GetAutoHide() ) {
        // return without calling wxWindow::Show
        return;
    }
    if (m_selectedItem == wxNOT_FOUND) {
        m_selectedItem = 0;
    }

    //m_mainPanel->GetSizer()->Fit(m_mainPanel);
    m_mainPanel->GetSizer()->Fit(this);

    if ( !CodeCompletionManager::Get().GetWordCompletionRefreshNeeded() ) {
        Display(m_editor);
    }
    SelectItem(m_selectedItem);
}

void CCBox::DoInsertSelection(const wxString& word, bool triggerTip)
{
    if (m_owner) {
        // Let the owner override the default behavior
        clCodeCompletionEvent e(wxEVT_CCBOX_SELECTION_MADE);
        e.SetWord(word);
        e.SetEventObject(m_owner);
        if(EventNotifier::Get()->ProcessEvent(e))
            return;
    }

    if ( m_editor ) {
        m_editor->CallTipCancel();
        int insertPos = m_editor->WordStartPosition(m_editor->GetCurrentPos(), true);
        int endPos    = m_editor->GetCurrentPos(); //m_editor->WordEndPosition(m_editor->GetCurrentPos(), true);

        m_editor->SetSelection(insertPos, endPos);
        m_editor->ReplaceSelection(word);

        // incase we are adding a function, add '()' at the end of the function name and place the caret in the middle
        int img_id = m_listCtrl->OnGetItemImage(m_selectedItem);
        if (img_id >= 8 && img_id <= 10) {

            // if full declaration was selected, dont do anything,
            // otherwise, append '()' to the inserted string, place the caret
            // in the middle, and trigger the function tooltip

            if (word.Find(wxT("(")) == wxNOT_FOUND && triggerTip) {

                // If the char after the insertion is '(' dont place another '()'
                int dummyPos = wxNOT_FOUND;
                wxChar charAfter = m_editor->NextChar(m_editor->GetCurrentPos(), dummyPos);
                if(charAfter != wxT('(')) {
                    // add braces
                    m_editor->InsertText(m_editor->GetCurrentPos(), wxT("()"));
                    dummyPos = wxNOT_FOUND;
                }

                int pos = dummyPos == wxNOT_FOUND ? m_editor->GetCurrentPos() : dummyPos;
                m_editor->SetSelectionStart(pos);
                m_editor->SetSelectionEnd(pos);
                m_editor->CharRight();
                m_editor->SetIndicatorCurrent(MATCH_INDICATOR);
                m_editor->IndicatorFillRange(pos, 1);
                // trigger function tip
                m_editor->CodeComplete();
                
                // select the tag to display to match the current one
                CCItemInfo itemInfo;
                if ( m_listCtrl->GetItemTagEntry(m_selectedItem, itemInfo) ) {
                    TagEntryPtr tt (new TagEntry(itemInfo.tag));
                    TagEntryPtrVector_t tags;
                    tags.push_back( tt );
                    std::vector<clTipInfo> tips;
                    clCallTip::FormatTagsToTips(tags, tips);
                    
                    // post an event to select the proper signature
                    if ( !tips.empty() ) {
                        m_editor->GetFunctionTip()->SelectSignature(tips.at(0).str);
                    }
                }
                
                wxString tipContent = m_editor->GetContext()->CallTipContent();
                int where = tipContent.Find(wxT(" : "));
                if (where != wxNOT_FOUND) {
                    tipContent = tipContent.Mid(where + 3);
                }

                if (tipContent.Trim().Trim(false) == wxT("()")) {
                    // dont place the caret in the middle of the braces,
                    // and it is OK to cancel the function calltip
                    int new_pos = m_editor->GetCurrentPos() + 1;
                    m_editor->SetCurrentPos(new_pos);
                    m_editor->SetSelectionStart(new_pos);
                    m_editor->SetSelectionEnd(new_pos);

                    // remove the current tip that we just activated.
                    // if this was the last tip, it will also make it go away
                    m_editor->GetFunctionTip()->Remove();
                }
            }
        }
    }
}

void CCBox::InsertSelection()
{
    if (m_selectedItem == wxNOT_FOUND) {
        return;
    }

    // get the selected word
    wxString word = GetColumnText(m_listCtrl, m_selectedItem, 0);
    DoInsertSelection(word.BeforeFirst('('));
}

int CCBox::GetImageId(TagEntryPtr entry)
{
    wxString kind   = entry->GetKind();
    wxString access = entry->GetAccess();
    if (kind == wxT("class"))
        return 0;

    if (kind == wxT("struct"))
        return 1;

    if (kind == wxT("namespace"))
        return 2;

    if (kind == wxT("variable"))
        return 3;

    if (kind == wxT("typedef"))
        return 4;

    if (kind == wxT("member") && access.Contains(wxT("private")))
        return 5;

    if (kind == wxT("member") && access.Contains(wxT("public")))
        return 6;

    if (kind == wxT("member") && access.Contains(wxT("protected")))
        return 7;

    //member with no access? (maybe part of namespace??)
    if (kind == wxT("member"))
        return 6;

    if ((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("private")))
        return 8;

    if ((kind == wxT("function") || kind == wxT("prototype")) && (access.Contains(wxT("public")) || access.IsEmpty()))
        return 9;

    if ((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("protected")))
        return 10;

    if (kind == wxT("macro"))
        return 11;

    if (kind == wxT("enum"))
        return 12;

    if (kind == wxT("enumerator"))
        return 13;

    if (kind == wxT("cpp_keyword"))
        return 17;

    // try the user defined images
    std::map<wxString, int>::iterator iter = m_userImages.find(kind);
    if (iter != m_userImages.end()) {
        return iter->second;
    }
    return wxNOT_FOUND;
}

void CCBox::RegisterImageForKind(const wxString& kind, const wxBitmap& bmp)
{
    wxImageList *il = m_listCtrl->GetImageList(wxIMAGE_LIST_SMALL);
    if (il && bmp.IsOk()) {
        std::map<wxString, int>::iterator iter = m_userImages.find(kind);

        if (iter == m_userImages.end()) {
            int id = il->Add(bmp);
            m_userImages[kind] = id;
        } else {
            // an entry for this kind already exist, replace the current image with new one
            il->Replace(iter->second, bmp);
        }
    }
}

void CCBox::NextPage()
{
    if (m_selectedItem != wxNOT_FOUND) {
        if (m_selectedItem + 1 < m_listCtrl->GetItemCount()) {
#ifdef __WXMAC__
// unselect current item
            m_listCtrl->Select(m_selectedItem, false);
#endif
            m_selectedItem += 10;
// select next item
            SelectItem(m_selectedItem);
        } else {
#ifdef __WXMAC__
// unselect current item
            m_listCtrl->Select(m_selectedItem, false);
#endif
            m_selectedItem = m_listCtrl->GetItemCount() - 1;
// select next item
            SelectItem(m_selectedItem);
        }
    }
}

void CCBox::PreviousPage()
{
    if (m_selectedItem != wxNOT_FOUND) {
        if (m_selectedItem - 10 >= 0) {
#ifdef __WXMAC__
// unselect current item
            m_listCtrl->Select(m_selectedItem, false);
#endif
            m_selectedItem -= 10;

// select previous item
            SelectItem(m_selectedItem);
        } else {
#ifdef __WXMAC__
// unselect current item
            m_listCtrl->Select(m_selectedItem, false);
#endif
            m_selectedItem = 0;

// select previous item
            SelectItem(m_selectedItem);
        }
    }
}

void CCBox::HideCCBox()
{
    clCodeCompletionEvent evt(wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED, GetId());
    evt.SetEventObject(this);
    EventNotifier::Get()->AddPendingEvent(evt);

    if( m_tipWindow ) {
        m_tipWindow->Destroy();
        m_tipWindow = NULL;
    }

    if(m_editor) {
        wxCommandEvent evt(wxCMD_EVENT_SET_EDITOR_ACTIVE, GetId());
        evt.SetEventObject(this);
        m_editor->GetEventHandler()->AddPendingEvent( evt );
    }
    CodeCompletionBox::Get().Hide();
}

void CCBox::DoShowTagTip()
{
    if( !m_editor ) {
        return;
    }

    if(m_currentItem.listOfTags.empty())
        return;

    if(m_currentItem.currentIndex >= (int)m_currentItem.listOfTags.size()) {
        m_currentItem.currentIndex = 0;
    }

    if(m_currentItem.currentIndex < 0) {
        m_currentItem.currentIndex = m_currentItem.listOfTags.size() - 1;
    }

    wxString prefix;
    TagEntry tag = m_currentItem.listOfTags.at(m_currentItem.currentIndex);
    size_t numOfTips = m_currentItem.listOfTags.size();

    // Send the plugins an event requesting tooltip for this tag
    bool gotAComment(false);
    if(m_owner) {
        TagEntryPtr tagPtr ( new TagEntry(tag) );
        clCodeCompletionEvent evt(wxEVT_CC_CODE_COMPLETE_TAG_COMMENT, GetId());
        evt.SetEventObject(this);
        evt.SetTagEntry( tagPtr  );
        
        if(EventNotifier::Get()->ProcessEvent(evt)) {
            prefix << evt.GetTooltip();
            gotAComment = true;
        }
    }

    if(!gotAComment) {
        if( tag.IsMethod() ) {

            if(tag.IsConstructor())
                prefix << wxT("<b>[Constructor]</b>\n");

            else if( tag.IsDestructor())
                prefix << wxT("<b>[Destructor]</b>\n");

            TagEntryPtr p(new TagEntry(tag));
            prefix << wxT("<code>") << TagsManagerST::Get()->FormatFunction(p, FunctionFormat_WithVirtual|FunctionFormat_Arg_Per_Line) << wxT("</code>\n");
            prefix.Replace(tag.GetName(), wxT("<b>") + tag.GetName() + wxT("</b>"));
        } else if( tag.IsClass() ) {

            prefix << wxT("<b>Kind:</b> ");
            prefix << wxString::Format(wxT("%s\n"), tag.GetKind().c_str() );

            if(tag.GetInheritsAsString().IsEmpty() == false) {
                prefix << wxT("<b>Inherits:</b> ");
                prefix << tag.GetInheritsAsString() << wxT("\n");
            }

        } else if(tag.IsMacro() || tag.IsTypedef() || tag.IsContainer() || tag.GetKind() == wxT("member") || tag.GetKind() == wxT("variable")) {

            prefix << wxT("<b>Kind:</b> ");
            prefix << wxString::Format(wxT("%s\n"), tag.GetKind().c_str() );

            prefix << wxT("<b>Match Pattern:</b> ");

            // Prettify the match pattern
            wxString matchPattern(tag.GetPattern());
            matchPattern.Trim().Trim(false);

            if(matchPattern.StartsWith(wxT("/^"))) {
                matchPattern.Replace(wxT("/^"), wxT(""));
            }

            if(matchPattern.EndsWith(wxT("$/"))) {
                matchPattern.Replace(wxT("$/"), wxT(""));
            }

            matchPattern.Replace(wxT("\t"), wxT(" "));
            while(matchPattern.Replace(wxT("  "), wxT(" "))) {}

            matchPattern.Trim().Trim(false);

            // BUG#3082954: limit the size of the 'match pattern' to a reasonable size (200 chars)
            matchPattern = TagsManagerST::Get()->WrapLines(matchPattern);
            matchPattern.Replace(tag.GetName(), wxT("<b>") + tag.GetName() + wxT("</b>"));
            prefix << wxT("<code>") << matchPattern << wxT("</code>\n");

        } else {
            // non valid tag entry
            return;
        }

        // Add comment section
        wxString filename (m_comments.getFilename().c_str(), wxConvUTF8);
        if(filename != tag.GetFile()) {
            m_comments.clear();
            ParseComments(tag.GetFile().mb_str(wxConvUTF8).data(), m_comments);
            m_comments.setFilename(tag.GetFile().mb_str(wxConvUTF8).data());
        }
        wxString tagComment;
        bool     foundComment(false);
        std::string comment;
        // search for comment in the current line, the line above it and 2 above it
        // use the first match we got
        for(size_t i=0; i<3; i++) {
            comment = m_comments.getCommentForLine(tag.GetLine() - i);
            if(comment.empty() == false) {
                foundComment = true;
                break;
            }
        }
        if( foundComment || !tag.GetComment().IsEmpty()) {

            wxString theComment;
            if( !tag.GetComment().IsEmpty() )
                theComment = tag.GetComment();
            else
                theComment = wxString(comment.c_str(), wxConvUTF8);

            theComment = TagsManagerST::Get()->WrapLines(theComment);
            theComment.Trim(false);
            tagComment = wxString::Format(wxT("%s\n"), theComment.c_str());
            if(prefix.IsEmpty() == false) {
                prefix.Trim().Trim(false);
                prefix << wxT("\n<hr>");
            }
            prefix << tagComment;
        }
    } // gotAComment = true

    // Update all "doxy" comments and surround them with <green> tags
    static wxRegEx reDoxyParam ("([@\\\\]{1}param)[ \\t]+([_a-z][a-z0-9_]*)?", wxRE_DEFAULT|wxRE_ICASE);
    static wxRegEx reDoxyBrief ("([@\\\\]{1}brief)[ \\t]*",                    wxRE_DEFAULT|wxRE_ICASE);
    static wxRegEx reDoxyThrow ("([@\\\\]{1}throw)[ \\t]*",                    wxRE_DEFAULT|wxRE_ICASE);
    static wxRegEx reDoxyReturn("([@\\\\]{1}return)[ \\t]*",                   wxRE_DEFAULT|wxRE_ICASE);
        
    if ( reDoxyParam.IsValid() && reDoxyParam.Matches(prefix) ) {
        reDoxyParam.ReplaceAll(&prefix, "\n<b>Parameter</b>\n<i>\\2</i>");
    }
    
    if ( reDoxyBrief.IsValid() && reDoxyBrief.Matches(prefix) ) {
        reDoxyBrief.ReplaceAll(&prefix, "");
    }
    
    if ( reDoxyThrow.IsValid() && reDoxyThrow.Matches(prefix) ) {
        reDoxyThrow.ReplaceAll(&prefix, "\n<b>Throws</b>\n");
    }
    
    if ( reDoxyReturn.IsValid() && reDoxyReturn.Matches(prefix) ) {
        reDoxyReturn.ReplaceAll(&prefix, "\n<b>Returns</b>\n");
    }
    
    if( m_tipWindow ) {
        m_tipWindow->Destroy();
        m_tipWindow = NULL;
    }

    m_startPos == wxNOT_FOUND ? m_startPos = m_editor->GetCurrentPos() : m_startPos;

    // if nothing to display skip this
    prefix.Trim().Trim(false);
    if(prefix.IsEmpty()) {
        return;
    }

    m_tipWindow = new CCBoxTipWindow(wxTheApp->GetTopWindow(), prefix, numOfTips);
#if CCBOX_USE_POPUP
    m_tipWindow->PositionRelativeTo(this, m_editor->PointFromPosition(m_editor->GetCurrentPos()), m_editor);
#else
    m_tipWindow->PositionRelativeTo(this, m_editor->PointFromPosition(m_editor->GetCurrentPos()), m_editor);
#endif

#if !CCBOX_USE_POPUP
    wxWindow *tlw = ::wxGetTopLevelParent(m_editor);
    if ( tlw ) {
        tlw->Raise();
    }
    m_editor->SetActive();
#endif
}

void CCBox::DoFormatDescriptionPage(const CCItemInfo& item)
{
    LEditor *editor = m_editor;
    if( !editor ) {
        return;
    }

    m_currentItem = item;
    if(m_currentItem.listOfTags.empty()) {
        if( m_tipWindow ) {
            m_tipWindow->Destroy();
            m_tipWindow = NULL;
        }
        return;
    }

    m_currentItem.currentIndex = 0;
    DoShowTagTip();
}

void CCBox::OnTipClickedUp(wxCommandEvent& event)
{
    event.Skip();
    m_currentItem.currentIndex--;
    DoShowTagTip();
}

void CCBox::OnTipClickedDown(wxCommandEvent& event)
{
    event.Skip();
    m_currentItem.currentIndex++;
    DoShowTagTip();
}

void CCBox::DoFilterCompletionEntries(CCItemInfo& item)
{
    std::map<wxString, TagEntry> uniqueList;

    // filter our some of the duplicate results
    // (e.g. dont show prototpe + impl as 2 entries)
    for(size_t i=0; i<item.listOfTags.size(); i++) {
        const TagEntry& t = item.listOfTags.at(i);
        const wxString& name = t.GetName();

        if(t.IsMethod()) {
            wxString signature = t.GetSignature();
            if(t.IsFunction()) {
                if(uniqueList.find(name + signature) == uniqueList.end())
                    uniqueList[name + signature] = t;

            } else {
                // override any existing item
                uniqueList[name + signature] = t;
            }
        } else {
            uniqueList[name] = t;
        }
    }

    item.listOfTags.clear();
    item.listOfTags.reserve( uniqueList.size() );
    std::map<wxString, TagEntry>::iterator iter = uniqueList.begin();
    for(; iter != uniqueList.end(); iter++ ) {
        item.listOfTags.push_back( iter->second );
    }
}
void CCBox::OnDisplayTooltip(wxTimerEvent& event)
{
    if(IsShown() == false)
        return;

    CCItemInfo tag;
    if(m_listCtrl->GetItemTagEntry(m_selectedItem, tag)) {
        DoFormatDescriptionPage( tag );
    }
}

void CCBox::OnRefreshList(wxTimerEvent& event)
{
    if(!m_isKeywordsList) {

        // clang is already slow... don't re-invoke the list
        if(m_tags.empty() == false && m_tags.at(0)->GetIsClangTag())
            return;

        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("complete_word_refresh_list"));
        event.SetEventObject(clMainFrame::Get());
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(event);
    }
}

void CCBox::OnDismiss()
{
    // our popup win was dismissed for some reason
    m_editor = NULL;
}

void CCBox::OnClose(wxCommandEvent& e)
{
    e.Skip();
    Destroy();
}

void CCBox::SortTags(std::vector<CCItemInfo>& tags, const wxString& userTyped)
{
    wxUnusedVar(tags);
    wxUnusedVar(userTyped);
}
