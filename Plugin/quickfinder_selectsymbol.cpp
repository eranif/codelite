#include "quickfinder_selectsymbol.h"
#include <wx/xrc/xmlres.h>
#include <wx/imaglist.h>
#include "globals.h"

static int getImageId ( const wxString &kind ) {
	if ( kind == wxT("namespace") ) return 0;
	if ( kind == wxT("class")     ) return 1;
	if ( kind == wxT("struct")    ) return 2;
	if ( kind == wxT("typedef")   ) return 3;
	if ( kind == wxT("enum")      ) return 4;
	if ( kind == wxT("union")     ) return 2;
	if ( kind == wxT("macro")     ) return 3;
	if ( kind == wxT("enumerator")) return 5;
	if ( kind == wxT("function")  ) return 6;
	if ( kind == wxT("prototype") ) return 6;
	return 1;
}

QuickfinderSelect::QuickfinderSelect( wxWindow* parent , const std::vector<TagEntryPtr> &tags)
		: SelectSymbolDlgBase( parent )
		, m_tags(tags)
{
	wxImageList *il = new wxImageList(16, 16, true);
	il->Add(wxXmlResource::Get()->LoadBitmap(_T("namespace")));//0
	il->Add(wxXmlResource::Get()->LoadBitmap(_T("class")));    //1
	il->Add(wxXmlResource::Get()->LoadBitmap(_T("struct")));   //2
	wxBitmap bmp;

	// typedef
	bmp = wxXmlResource::Get()->LoadBitmap(_T("typedef"));     //3
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	il->Add(bmp);

	bmp = wxXmlResource::Get()->LoadBitmap(_T("enum"));        //4
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	il->Add(bmp);

	bmp = wxXmlResource::Get()->LoadBitmap(wxT("enumerator")); //5
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	il->Add(bmp);

	il->Add(wxXmlResource::Get()->LoadBitmap(_T("func_public")));//6
	m_listCtrlTags->AssignImageList( il, wxIMAGE_LIST_SMALL );

	DoCreateList();
	DoPopulate  ();
}

QuickfinderSelect::~QuickfinderSelect()
{
}

void QuickfinderSelect::OnItemActivated( wxListEvent& event )
{
	long sel = m_listCtrlTags->GetFirstSelected();
	if ( sel != wxNOT_FOUND ) {
		this->selection = *((TagEntry*) m_listCtrlTags->GetItemData(sel));
		EndModal(wxID_OK);
	} else {
		EndModal(wxID_CANCEL);
	}
}

void QuickfinderSelect::DoCreateList()
{
	m_listCtrlTags->ClearAll();
	m_listCtrlTags->InsertColumn(0, wxT("Name"));
	m_listCtrlTags->InsertColumn(1, wxT("Scope"));
	m_listCtrlTags->InsertColumn(2, wxT("Line"));
	m_listCtrlTags->InsertColumn(3, wxT("File"));

	m_listCtrlTags->SetColumnWidth(0, 200);
	m_listCtrlTags->SetColumnWidth(1, 100);
	m_listCtrlTags->SetColumnWidth(2, 50 );
	m_listCtrlTags->SetColumnWidth(3, 400);
}

void QuickfinderSelect::DoPopulate()
{
	Freeze();
	for(size_t i=0; i<m_tags.size(); i++) {
		TagEntryPtr t = m_tags.at(i);
		long idx = AppendListCtrlRow(m_listCtrlTags);
		SetColumnText(m_listCtrlTags, idx, 0, t->GetName(), getImageId(t->GetKind()));
		SetColumnText(m_listCtrlTags, idx, 1, t->GetScope());
		SetColumnText(m_listCtrlTags, idx, 2, wxString::Format(wxT("%d"), t->GetLine()));
		SetColumnText(m_listCtrlTags, idx, 3, t->GetFile());
		m_listCtrlTags->SetItemPtrData(idx, (wxUIntPtr) t.Get());
	}
	m_listCtrlTags->SetColumnWidth(2, wxLIST_AUTOSIZE);
	if ( m_listCtrlTags->GetItemCount() > 0 ) {
		m_listCtrlTags->Select(0);
		m_listCtrlTags->Focus(0);
		m_listCtrlTags->SetFocus();
	}
	Thaw();
}


void QuickfinderSelect::OnButtonOK(wxCommandEvent& event)
{
	long sel = m_listCtrlTags->GetFirstSelected();
	if ( sel != wxNOT_FOUND ) {
		this->selection = *((TagEntry*) m_listCtrlTags->GetItemData(sel));
		EndModal(wxID_OK);
	} else {
		EndModal(wxID_CANCEL);
	}
}
