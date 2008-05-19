#include "cc_box.h"
#include "globals.h"
#include <wx/imaglist.h>
#include <wx/xrc/xmlres.h>
#include "entry.h"

CCBox::CCBox( wxWindow* parent )
		:
		CCBoxBase( parent )
{
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
}

void CCBox::OnItemActivated( wxListEvent& event )
{
	// TODO: Implement OnItemActivated
}

void CCBox::OnItemDeSelected( wxListEvent& event )
{
	// TODO: Implement OnItemDeSelected
}

void CCBox::OnItemSelected( wxListEvent& event )
{
	// TODO: Implement OnItemSelected
}

void CCBox::OnKeyDown( wxListEvent& event )
{
	// TODO: Implement OnKeyDown
}

void CCBox::Initialize(const std::vector<TagEntryPtr> &tags, const wxString &word, bool showFullDecl)
{
	wxString list;
	size_t i(0);
	m_listCtrl->Freeze();
	
	//Assumption (which is always true..): the tags are sorted
	wxString lastName;
	if (tags.empty() == false) {
		for (; i<tags.size()-1; i++) {
			if (lastName != tags.at(i)->GetName()) {
				int row = AppendListCtrlRow(m_listCtrl);
				SetColumnText(m_listCtrl, row, 0, tags.at(i)->GetName(), GetImageId(*tags.at(i)));
				lastName = tags.at(i)->GetName();
			}
			if (showFullDecl) {
				//collect only declarations
				if (tags.at(i)->GetKind() == wxT("prototype")) {
					int row = AppendListCtrlRow(m_listCtrl);
					SetColumnText(m_listCtrl, row, 0, tags.at(i)->GetName()+tags.at(i)->GetSignature(), GetImageId(*tags.at(i)));
				}
			}
		}

		if (lastName != tags.at(i)->GetName()) {
			int row = AppendListCtrlRow(m_listCtrl);
			SetColumnText(m_listCtrl, row, 0, tags.at(i)->GetName(), GetImageId(*tags.at(i)));
		}
		
//		rCtrl.AutoCompSetSeparator((int)('@'));	// set the separator to be non valid language wxChar
//		rCtrl.AutoCompSetChooseSingle(true);					// If only one match, insert it automatically
//		//rCtrl.AutoCompSetIgnoreCase(false);
//		rCtrl.AutoCompSetDropRestOfWord(true);
//		rCtrl.AutoCompSetAutoHide(false);
//		rCtrl.AutoCompShow((int)word.Length(), list);
//		rCtrl.AutoCompSetFillUps(wxT("<( \t\n"));
	}
	
	m_listCtrl->Thaw();
}

int CCBox::GetImageId(const TagEntry &entry)
{
	if (entry.GetKind() == wxT("class"))
		return 1;

	if (entry.GetKind() == wxT("struct"))
		return 2;

	if (entry.GetKind() == wxT("namespace"))
		return 3;

	if (entry.GetKind() == wxT("variable"))
		return 4;

	if (entry.GetKind() == wxT("typedef"))
		return 5;

	if (entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("private")))
		return 6;

	if (entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("public")))
		return 7;

	if (entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("protected")))
		return 8;

	//member with no access? (maybe part of namespace??)
	if (entry.GetKind() == wxT("member"))
		return 7;

	if ((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && entry.GetAccess().Contains(wxT("private")))
		return 9;

	if ((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && (entry.GetAccess().Contains(wxT("public")) || entry.GetAccess().IsEmpty()))
		return 10;

	if ((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && entry.GetAccess().Contains(wxT("protected")))
		return 11;

	if (entry.GetKind() == wxT("macro"))
		return 12;

	if (entry.GetKind() == wxT("enum"))
		return 13;

	if (entry.GetKind() == wxT("enumerator"))
		return 14;

	return wxNOT_FOUND;
}
