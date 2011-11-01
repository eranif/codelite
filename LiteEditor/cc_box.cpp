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
#include "ctags_manager.h"
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

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

const wxEventType wxCMD_EVENT_SET_EDITOR_ACTIVE = XRCID("wxCMD_EVENT_SET_EDITOR_ACTIVE");

CCBox::CCBox(LEditor* parent, bool autoHide, bool autoInsertSingleChoice)
#if CC_USES_POPUPWIN
	: CCBoxBase(wxTheApp->GetTopWindow(), wxID_ANY, wxDefaultPosition, wxSize(0, 0))
#else
	: CCBoxBase(parent, wxID_ANY, wxDefaultPosition, wxSize(0, 0))
#endif
	, m_showFullDecl(false)
	, m_height(BOX_HEIGHT)
	, m_autoHide(autoHide)
	, m_insertSingleChoice(autoInsertSingleChoice)
	, m_owner(NULL)
	, m_hideExtInfoPane(true)
	, m_startPos(wxNOT_FOUND)
	, m_editor(parent)
	, m_timer(NULL)
	, m_needRepopulateTagList(false)
{
#ifdef __WXMAC__
	Hide();
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
#endif
	m_constructing = true;
	HideCCBox();
	MSWSetNativeTheme(m_listCtrl);

	m_timer = new wxTimer(this);
	Connect(m_timer->GetId(),  wxEVT_TIMER, wxTimerEventHandler(CCBox::OnDisplayTooltip),  NULL, this);

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

	// assign the image list and let the control take owner ship (i.e. delete it)
	m_listCtrl->AssignImageList(il, wxIMAGE_LIST_SMALL);
	m_listCtrl->InsertColumn(0, wxT("Name"));
	m_listCtrl->SetColumnWidth(0, 345);

	m_isTipBgDark = DrawingUtils::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

	if ( !m_isTipBgDark ) {
		GetEditor()->m_isTipBgDark = false;

	} else {
		// the tooltip colour is dark
		GetEditor()->CallTipSetForegroundHighlight(wxT("YELLOW"));
		GetEditor()->m_isTipBgDark = true;
	}

	m_listCtrl->SetFocus();
	// return the focus to scintilla
	GetEditor()->SetActive();
	m_constructing = false;

	GetParent()->Connect(wxEVT_SCI_CALLTIP_CLICK, wxScintillaEventHandler(CCBox::OnTipClicked), NULL, this);
}

CCBox::~CCBox()
{
	EditorConfigST::Get()->SaveLongValue(wxT("CC_Show_Item_Commetns"), LEditor::m_ccShowItemsComments  ? 1 : 0);
	EditorConfigST::Get()->SaveLongValue(wxT("CC_Show_All_Members"),   LEditor::m_ccShowPrivateMembers ? 1 : 0);
	Disconnect(m_timer->GetId(),  wxEVT_TIMER, wxTimerEventHandler(CCBox::OnDisplayTooltip),  NULL, this);
	delete m_timer;
	m_needRepopulateTagList = false;
}

void CCBox::OnItemActivated( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
	InsertSelection();
	HideCCBox();
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

	m_timer->Stop();
	m_timer->Start(100, true);

	LEditor *editor = GetEditor();
	if( editor ) {
		editor->CallTipCancel();
	}

}

void CCBox::RefreshList(const std::vector<TagEntryPtr>& tags, const wxString& word)
{
	m_tags = tags;
	m_timer->Stop();
	Show(word);
}

void CCBox::Show(const std::vector<TagEntryPtr> &tags, const wxString &word, bool showFullDecl, bool isKeywordsList, wxEvtHandler *owner)
{
	if (tags.empty()) {
		return;
	}

	//m_height = BOX_HEIGHT;
	m_tags = tags;
	m_showFullDecl = showFullDecl;
	m_owner = owner;
	m_isKeywordsList = isKeywordsList;
	Show(word);
}

void CCBox::Adjust()
{
	LEditor *editor = GetEditor();

	int     point      = editor->GetCurrentPos();
	wxPoint pt         = editor->PointFromPosition(point); // Point is in editor's coordinates
	int     lineHeight = editor->GetCurrLineHeight();

	// add the line height
	pt.y += lineHeight;

#if CC_USES_POPUPWIN

	wxSize  size = ::wxGetDisplaySize();

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
	wxPoint ccPoint = pt;
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

	// Not needed under wx29
#if  defined(__WXMSW__) && (wxVERSION_NUMBER < 2900)
	ccPoint = GetParent()->ScreenToClient(ccPoint);
#endif

	Move(ccPoint);
	editor->m_ccPoint = pt;
	editor->m_ccPoint.x += 2;

#else
	wxSize  size       = GetParent()->GetClientSize();
	int diff = size.y - pt.y;
	m_height = BOX_HEIGHT;

	if (diff < BOX_HEIGHT) {
		pt.y -= BOX_HEIGHT;
		pt.y -= lineHeight;

		if (pt.y < 0) {
			// the completion box is out of screen, resotre original size
			pt.y += BOX_HEIGHT;
			pt.y += lineHeight;
			m_height = diff;
		}
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
	editor->m_ccPoint = pt;
	Move(pt);
#endif
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

		m_selectedItem = item;
		SelectItem(m_selectedItem);

		if(fullMatch) {
			// Incase we got a full match, insert the selection and release the completion box
			InsertSelection();

			LEditor *editor = GetEditor();
			if (editor) {
				editor->SetActive();
			}
		}
	}

	m_needRepopulateTagList = true;
	m_timer->Stop();
	m_timer->Start(100, true);
	return fullMatch;
}

void CCBox::Next()
{
	if (m_selectedItem != wxNOT_FOUND) {
		if (m_selectedItem + 1 < m_listCtrl->GetItemCount()) {
#ifdef __WXMAC__
			// unselect current item
			m_listCtrl->Select(m_selectedItem, false);
#endif
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
#ifdef __WXMAC__
			// unselect current item
			m_listCtrl->Select(m_selectedItem, false);
#endif
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

	m_listCtrl->Select(item);
	m_listCtrl->EnsureVisible(item);
	m_timer->Stop();
	m_timer->Start(100, true);

	LEditor *editor = GetEditor();
	if( editor ) {
		editor->CallTipCancel();
	}
}

void CCBox::Show(const wxString& word)
{
#ifdef __WXMSW__
	ULONG_PTR style = GetClassLongPtr(this->GetHandle(), GCL_STYLE);
	style |= CS_DROPSHADOW;
	SetClassLongPtr(GetHandle(), GCL_STYLE, style);
#endif

	wxString lastName;
	size_t i(0);
	std::vector<CCItemInfo> _tags;

	m_listCtrl->SetCursor( wxCursor(wxCURSOR_ARROW) );
	m_toolBar1->SetCursor( wxCursor(wxCURSOR_ARROW) );
	this->SetCursor( wxCursor(wxCURSOR_ARROW) );

	long checkIt (0);
	if(LEditor::m_ccInitialized == false) {
		if(EditorConfigST::Get()->GetLongValue(wxT("CC_Show_All_Members"), checkIt)) {
			LEditor::m_ccShowPrivateMembers = (bool)checkIt;
		}

		if(EditorConfigST::Get()->GetLongValue(wxT("CC_Show_Item_Commetns"), checkIt)) {
			LEditor::m_ccShowItemsComments = (bool)checkIt;
		}
		LEditor::m_ccInitialized = true;
	}

	m_toolBar1->ToggleTool(TOOL_SHOW_PRIVATE_MEMBERS, LEditor::m_ccShowPrivateMembers);
	m_toolBar1->ToggleTool(TOOL_SHOW_ITEM_COMMENTS,   LEditor::m_ccShowItemsComments);

	CCItemInfo item;
	clWindowUpdateLocker locker(m_listCtrl);
	m_listCtrl->DeleteAllItems();

	// Get the associated editor
	LEditor *editor = GetEditor();
	wxString scopeName = editor->GetContext()->GetCurrentScopeName();
	if (m_tags.empty() == false) {
		_tags.reserve(m_tags.size());
		for (; i<m_tags.size(); i++) {

			const TagEntryPtr& tag = m_tags.at(i);
			wxString access = tag->GetAccess();
			bool        isVisible = access == wxT("private") || access == wxT("protected");
			bool        isInScope = (editor && (tag->GetParent() == scopeName));
			bool        collectIt = (!isVisible && !LEditor::m_ccShowPrivateMembers) || (LEditor::m_ccShowPrivateMembers) || (isInScope);

			if(collectIt) {

				if (item.displayName != tag->GetName()) {// Starting a new group or it is first time
					if( item.IsOk() ) {
						// we got a group of tags stored in 'item' add it
						// to the _tags before we continue
						DoFilterCompletionEntries(item);
						_tags.push_back(item);
					}

					// start a new group
					item.Reset();

					item.displayName =  tag->GetName();
					item.imgId       = GetImageId(tag);
					item.tag         = *tag;
					item.listOfTags.push_back( *tag );

				} else {
					item.listOfTags.push_back( *tag );
				}
			}
		}
	}

	if(item.IsOk()) {
		DoFilterCompletionEntries(item);
		_tags.push_back(item);
	}

	if (_tags.size() == 1 && m_insertSingleChoice) {
		m_selectedItem = 0;
		DoInsertSelection(_tags.at(0).displayName, false);

		// return without calling to wxWindow::Show()
		// also, make sure we are hidden
		if ( IsShown() ) {
			HideCCBox();
		}
		DoHideCCHelpTab();
		return;
	}

	m_listCtrl->SetItems(_tags);
	m_listCtrl->SetItemCount(_tags.size());
	m_selectedItem = 0;

	bool fullMatch;
	m_selectedItem = m_listCtrl->FindMatch(word, fullMatch);
	if ( m_selectedItem == wxNOT_FOUND && GetAutoHide() ) {
		// return without calling wxWindow::Show
		DoHideCCHelpTab();
		return;
	}
	if (m_selectedItem == wxNOT_FOUND) {
		m_selectedItem = 0;
	}

#if !CC_USES_POPUPWIN
	SetSize(BOX_WIDTH, m_height);
#endif

#if CC_USES_POPUPWIN
	m_mainPanel->GetSizer()->Fit(m_mainPanel);
	m_mainPanel->GetSizer()->Fit(this);


#ifdef __WXGTK__
	if(wxPopupWindow::IsShown() == false)
		wxPopupWindow::Show();

	clMainFrame::Get()->Raise();
	GetEditor()->SetFocus();
	GetEditor()->SetSCIFocus(true);
#else // Windows
	if(wxPopupTransientWindow::IsShown() == false)
		wxPopupTransientWindow::Show();
#endif

#else
	if(wxWindow::IsShown() == false)
		wxWindow::Show();
#endif
	SelectItem(m_selectedItem);
}

void CCBox::DoInsertSelection(const wxString& word, bool triggerTip)
{
	if (m_owner) {
		// Let the owner override the default behavior
		wxCommandEvent e(wxEVT_CCBOX_SELECTION_MADE);
		e.SetClientData( (void*)&word );
		if(m_owner->ProcessEvent(e))
			return;
	}

	LEditor *editor = GetEditor();
	int insertPos = editor->WordStartPosition(editor->GetCurrentPos(), true);

	editor->SetSelection(insertPos, editor->GetCurrentPos());
	editor->ReplaceSelection(word);

	// incase we are adding a function, add '()' at the end of the function name and place the caret in the middle
	int img_id = m_listCtrl->OnGetItemImage(m_selectedItem);
	if (img_id >= 8 && img_id <= 10) {

		// if full declaration was selected, dont do anything,
		// otherwise, append '()' to the inserted string, place the caret
		// in the middle, and trigger the function tooltip

		if (word.Find(wxT("(")) == wxNOT_FOUND && triggerTip) {

			// If the char after the insertion is '(' dont place another '()'
			int dummyPos = wxNOT_FOUND;
			wxChar charAfter = editor->NextChar(editor->GetCurrentPos(), dummyPos);
			if(charAfter != wxT('(')) {
				// add braces
				editor->InsertText(editor->GetCurrentPos(), wxT("()"));
				dummyPos = wxNOT_FOUND;
			}

			int pos = dummyPos == wxNOT_FOUND ? editor->GetCurrentPos() : dummyPos;
			editor->SetSelectionStart(pos);
			editor->SetSelectionEnd(pos);
			editor->CharRight();
			editor->SetIndicatorCurrent(MATCH_INDICATOR);
			editor->IndicatorFillRange(pos, 1);
			// trigger function tip
			editor->CodeComplete();

			wxString tipContent = editor->GetContext()->CallTipContent();
			int where = tipContent.Find(wxT(" : "));
			if (where != wxNOT_FOUND) {
				tipContent = tipContent.Mid(where + 3);
			}

			if (tipContent.Trim().Trim(false) == wxT("()")) {
				// dont place the caret in the middle of the braces,
				// and it is OK to cancel the function calltip
				int new_pos = editor->GetCurrentPos() + 1;
				editor->SetCurrentPos(new_pos);
				editor->SetSelectionStart(new_pos);
				editor->SetSelectionEnd(new_pos);

				// remove the current tip that we just activated.
				// if this was the last tip, it will also make it go away
				editor->GetFunctionTip()->Remove();
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
	DoInsertSelection(word);
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
	if( IsShown() ) {
#ifdef __WXMSW__
		ULONG_PTR style = GetClassLongPtr(this->GetHandle(), GCL_STYLE);
		style &= ~CS_DROPSHADOW;
		SetClassLongPtr(GetHandle(), GCL_STYLE, style);
#endif
		Hide();
		DoHideCCHelpTab();
		if( !m_constructing ) {
			bool members_checked, comments_checked;
#if USE_AUI_TOOLBAR
			members_checked = m_toolBar1->GetToolToggled(TOOL_SHOW_PRIVATE_MEMBERS);
			comments_checked = m_toolBar1->GetToolToggled(TOOL_SHOW_ITEM_COMMENTS);
#else
			members_checked = m_toolBar1->GetToolState(TOOL_SHOW_PRIVATE_MEMBERS);
			comments_checked = m_toolBar1->GetToolState(TOOL_SHOW_ITEM_COMMENTS);
#endif
			LEditor::m_ccShowPrivateMembers = members_checked ? 1 : 0;
			LEditor::m_ccShowItemsComments  = comments_checked ? 1 : 0;
		}
	}

	if(m_owner) {
		wxCommandEvent evt(wxEVT_CMD_CODE_COMPLETE_BOX_DISMISSED, GetId());
		evt.SetEventObject(this);
		m_owner->AddPendingEvent(evt);
	}

	if(GetEditor()) {
		wxCommandEvent evt(wxCMD_EVENT_SET_EDITOR_ACTIVE, GetId());
		evt.SetEventObject(this);
		GetEditor()->GetEventHandler()->AddPendingEvent( evt );
	}
}

void CCBox::OnShowPublicItems(wxCommandEvent& event)
{
	wxUnusedVar(event);
	HideCCBox();
}

void CCBox::DoShowTagTip()
{
	LEditor *editor = GetEditor();
	if( !editor ) {
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
	if(m_currentItem.listOfTags.size() > 1) {
		prefix << wxT("  \002 ") << m_currentItem.currentIndex+1 << wxT(" of ") << m_currentItem.listOfTags.size() << wxT("\001 ");
		prefix << wxT("\n@@LINE@@\n");
	}

	// Send the plugins an event requesting tooltip for this tag
	bool gotAComment(false);
	if(m_owner) {
		wxCommandEvent evt(wxEVT_CMD_CODE_COMPLETE_TAG_COMMENT, GetId());
		evt.SetEventObject(this);
		evt.SetClientData(tag.GetUserData());
		if(m_owner->ProcessEvent(evt)) {
			prefix << evt.GetString();
			gotAComment = true;
		}
	}

	if(!gotAComment) {
		if( tag.IsMethod() ) {

			if(tag.IsConstructor())
				prefix << wxT("[Constructor]\n");

			else if( tag.IsDestructor())
				prefix << wxT("[Destructor]\n");

			TagEntryPtr p(new TagEntry(tag));
			prefix << TagsManagerST::Get()->FormatFunction(p, FunctionFormat_WithVirtual|FunctionFormat_Arg_Per_Line) << wxT("\n");

		} else if( tag.IsClass() ) {

			prefix << wxT("Kind: ");
			prefix << wxString::Format(wxT("%s\n"), tag.GetKind().c_str() );

			if(tag.GetInheritsAsString().IsEmpty() == false) {
				prefix << wxT("Inherits: ");
				prefix << tag.GetInheritsAsString() << wxT("\n");
			}

		} else if(tag.IsMacro() || tag.IsTypedef() || tag.IsContainer() || tag.GetKind() == wxT("member") || tag.GetKind() == wxT("variable")) {

			prefix << wxT("Kind : ");
			prefix << wxString::Format(wxT("%s\n"), tag.GetKind().c_str() );

			prefix << wxT("Match Pattern: ");

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
			prefix << matchPattern << wxT("\n");

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
		if( foundComment ) {
			wxString theComment(comment.c_str(), wxConvUTF8);
			theComment = TagsManagerST::Get()->WrapLines(theComment);
			theComment.Trim(false);
			tagComment = wxString::Format(wxT("%s\n"), theComment.c_str());
			if(prefix.IsEmpty() == false) {
				prefix.Trim().Trim(false);
				prefix << wxT("\n\n@@LINE@@\n");
			}
			prefix << tagComment;
		}
	} // gotAComment = true

	editor->CallTipCancel();
	m_startPos == wxNOT_FOUND ? m_startPos = editor->GetCurrentPos() : m_startPos;

	// if nothing to display skip this
	prefix.Trim().Trim(false);
	if(prefix.IsEmpty()) {
		return;
	}

	editor->CallTipShowExt( m_startPos, prefix);
	int hightlightFrom = prefix.Find(tag.GetName());
	if(hightlightFrom != wxNOT_FOUND) {
		int highlightLen = tag.GetName().Length();
		editor->CallTipSetHighlight(hightlightFrom, hightlightFrom + highlightLen);
	}
}

void CCBox::DoFormatDescriptionPage(const CCItemInfo& item)
{
	LEditor *editor = GetEditor();
	if( !editor ) {
		return;
	}

	if(LEditor::m_ccShowItemsComments == false) {
		editor->CallTipCancel();
		return;
	}

	m_currentItem = item;
	if(m_currentItem.listOfTags.empty()) {
		editor->CallTipCancel();
		return;
	}

	m_currentItem.currentIndex = 0;
	DoShowTagTip();
}

void CCBox::EnableExtInfoPane()
{
	m_hideExtInfoPane = false;
}

void CCBox::DoHideCCHelpTab()
{
	m_hideExtInfoPane = true;
	m_startPos = wxNOT_FOUND;
	m_currentItem.Reset();
	if(m_timer) {
		m_timer->Stop();
	}
	LEditor *editor = GetEditor();
	m_needRepopulateTagList = false;
	if(editor)
		editor->CallTipCancel();
}

void CCBox::OnShowComments(wxCommandEvent& event)
{
	LEditor::m_ccShowItemsComments = event.IsChecked();
	if( LEditor::m_ccShowItemsComments == false ) {
		DoFormatDescriptionPage( CCItemInfo() );
	} else {
		CCItemInfo tag;
		if(m_listCtrl->GetItemTagEntry(m_selectedItem, tag)) {
			DoFormatDescriptionPage( tag );
		}
	}
	event.Skip();
}

void CCBox::OnTipClicked(wxScintillaEvent& event)
{
	event.Skip();
	if(event.GetPosition() == 1) {       // Up
		m_currentItem.currentIndex++;
		DoShowTagTip();
	} else if(event.GetPosition() == 2) {// down
		m_currentItem.currentIndex--;
		DoShowTagTip();
	}
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

LEditor* CCBox::GetEditor()
{
#if !CC_USES_POPUPWIN
	if(m_editor == NULL)
		m_editor = dynamic_cast<LEditor*>( GetParent() );
#endif
	return m_editor;
}


void CCBox::OnKeyDown(wxListEvent& event)
{
	event.Skip();
}

void CCBox::OnDisplayTooltip(wxTimerEvent& event)
{
	if(IsShown() == false)
		return;

	CCItemInfo tag;
	if(m_listCtrl->GetItemTagEntry(m_selectedItem, tag)) {
		DoFormatDescriptionPage( tag );
	}

	if(m_needRepopulateTagList && !m_isKeywordsList) {
		m_needRepopulateTagList = false;
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("complete_word_refresh_list"));
		event.SetEventObject(clMainFrame::Get());
		clMainFrame::Get()->GetEventHandler()->AddPendingEvent(event);
	}
}
