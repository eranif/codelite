#include "cc_box.h"
#include "cl_editor.h"
#include "globals.h"
#include <wx/imaglist.h>
#include <wx/xrc/xmlres.h>
#include "entry.h"

#define BOX_HEIGHT 200
#define BOX_WIDTH  300

CCBox::CCBox( LEditor* parent)
		:
		CCBoxBase(parent, wxID_ANY, wxDefaultPosition, wxSize(0, 0))
		, m_showFullDecl(false)
		, m_height(BOX_HEIGHT)
{
	Hide();

	// load all the CC images
	wxImageList *il = new wxImageList(16, 16, true);

	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("class")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("struct")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("namespace")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_public")));

	wxBitmap m_tpyedefBmp = wxXmlResource::Get()->LoadBitmap(wxT("typedef"));
	m_tpyedefBmp.SetMask(new wxMask(m_tpyedefBmp, wxColor(0, 128, 128)));
	il->Add(m_tpyedefBmp);

	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_private")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_public")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_protected")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_private")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_public")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_protected")));
	wxBitmap m_macroBmp = wxXmlResource::Get()->LoadBitmap(wxT("typedef"));
	m_macroBmp.SetMask(new wxMask(m_macroBmp, wxColor(0, 128, 128)));
	il->Add(m_macroBmp);

	wxBitmap m_enumBmp = wxXmlResource::Get()->LoadBitmap(wxT("enum"));
	m_enumBmp.SetMask(new wxMask(m_enumBmp, wxColor(0, 128, 128)));
	il->Add(m_enumBmp);

	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("enumerator")));

	//Initialise the file bitmaps
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("page_white_cplusplus")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("page_white_h")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("page_white_text")));

	// assign the image list and let the control take owner ship (i.e. delete it)
	m_listCtrl->AssignImageList(il, wxIMAGE_LIST_SMALL);
	m_listCtrl->InsertColumn(0, wxT("Name"));
	
	m_listCtrl->SetFocus();
	// return the focus to scintilla
	parent->SetActive();
}

void CCBox::OnItemActivated( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
	InsertSelection();
	Hide();
}

void CCBox::OnItemDeSelected( wxListEvent& event )
{
	m_selectedItem = wxNOT_FOUND;
}

void CCBox::OnItemSelected( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
}

void CCBox::Show(const std::vector<TagEntryPtr> &tags, const wxString &word, bool showFullDecl)
{
	if (tags.empty()) {
		return;
	}

	//m_height = BOX_HEIGHT;
	m_tags = tags;
	m_showFullDecl = showFullDecl;
	Show(word);
}

void CCBox::Adjust()
{
	LEditor *parent = (LEditor*)GetParent();

	int point = parent->GetCurrentPos();
	wxPoint pt = parent->PointFromPosition(point);
	
	// add the line height
	int hh = parent->GetCurrLineHeight();
	pt.y += hh;

	wxSize size = parent->GetClientSize();
	int diff = size.y - pt.y;
	m_height = BOX_HEIGHT;
	if (diff < BOX_HEIGHT) {
		pt.y -= BOX_HEIGHT;
		pt.y -= hh;

		if (pt.y < 0) {
			// the completion box is out of screen, resotre original size
			pt.y += BOX_HEIGHT;
			pt.y += hh;
			m_height = diff;
		}
	}
	Move(pt);
}

void CCBox::SelectWord(const wxString& word)
{
	long item = m_listCtrl->FindMatch(word);
	if (item != wxNOT_FOUND) {
		m_selectedItem = item;
		SelectItem(m_selectedItem);
	}
}

void CCBox::Next()
{
	if (m_selectedItem != wxNOT_FOUND) {
		if (m_selectedItem + 1 < m_listCtrl->GetItemCount()) {
			m_selectedItem++;
			SelectItem(m_selectedItem);
		}
	}
}

void CCBox::Previous()
{
	if (m_selectedItem != wxNOT_FOUND) {
		if (m_selectedItem - 1 >= 0) {
			m_selectedItem--;
			SelectItem(m_selectedItem);
		}
	}
}

void CCBox::SelectItem(long item)
{
	m_listCtrl->SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	m_listCtrl->SetItemState(item, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
	m_listCtrl->EnsureVisible(item);
}

void CCBox::Show(const wxString& word)
{
	wxString lastName;
	size_t i(0);
	std::vector<CCItemInfo> _tags;

	CCItemInfo item;
	m_listCtrl->DeleteAllItems();

	if (m_tags.empty() == false) {
		for (; i<m_tags.size(); i++) {
			TagEntryPtr tag = m_tags.at(i);
			if (lastName != m_tags.at(i)->GetName()) {

				item.displayName =  tag->GetName();
				item.imgId = GetImageId(*m_tags.at(i));
				_tags.push_back(item);

				lastName = tag->GetName();
			}

			if (m_showFullDecl) {
				//collect only declarations
				if (m_tags.at(i)->GetKind() == wxT("prototype")) {
					item.displayName =  tag->GetName()+tag->GetSignature();
					item.imgId = GetImageId(*m_tags.at(i));
					_tags.push_back(item);

				}
			}
		}
	}

	if (_tags.size() == 1) {
		// only one match, insert it
		LEditor *editor = (LEditor*)GetParent();
		int insertPos = editor->WordStartPosition(editor->GetCurrentPos(), true);
		
		editor->SetSelection(insertPos, editor->GetCurrentPos());
		
		CCItemInfo info = _tags.at(0);
		editor->ReplaceSelection(info.displayName);
		
		// return without calling to wxWindow::Show()
		return;
	}

	m_listCtrl->SetColumnWidth(0, BOX_WIDTH);
	m_listCtrl->SetItems(_tags);
	m_listCtrl->SetItemCount(_tags.size());

	m_selectedItem = 0;

	m_selectedItem = m_listCtrl->FindMatch(word);
	if (m_selectedItem == wxNOT_FOUND) {
		m_selectedItem = 0;
	}

	SetSize(BOX_WIDTH, m_height);
	GetSizer()->Layout();
	wxWindow::Show();
	
	SelectItem(m_selectedItem);
}

void CCBox::InsertSelection()
{
	if (m_selectedItem == wxNOT_FOUND) {
		return;
	}

	// get the selected word
	wxString word = GetColumnText(m_listCtrl, m_selectedItem, 0);

	LEditor *editor = (LEditor*)GetParent();
	int insertPos = editor->WordStartPosition(editor->GetCurrentPos(), true);

	editor->SetSelection(insertPos, editor->GetCurrentPos());
	editor->ReplaceSelection(word);

}

int CCBox::GetImageId(const TagEntry &entry)
{
	if (entry.GetKind() == wxT("class"))
		return 0;

	if (entry.GetKind() == wxT("struct"))
		return 1;

	if (entry.GetKind() == wxT("namespace"))
		return 2;

	if (entry.GetKind() == wxT("variable"))
		return 3;

	if (entry.GetKind() == wxT("typedef"))
		return 4;

	if (entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("private")))
		return 5;

	if (entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("public")))
		return 6;

	if (entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("protected")))
		return 7;

	//member with no access? (maybe part of namespace??)
	if (entry.GetKind() == wxT("member"))
		return 6;

	if ((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && entry.GetAccess().Contains(wxT("private")))
		return 8;

	if ((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && (entry.GetAccess().Contains(wxT("public")) || entry.GetAccess().IsEmpty()))
		return 9;

	if ((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && entry.GetAccess().Contains(wxT("protected")))
		return 10;

	if (entry.GetKind() == wxT("macro"))
		return 11;

	if (entry.GetKind() == wxT("enum"))
		return 12;

	if (entry.GetKind() == wxT("enumerator"))
		return 13;

	return wxNOT_FOUND;
}
