#include "notebookcustomdlg.h"
#include "custom_notebook.h"

NotebookCustomDlg::NotebookCustomDlg( wxWindow* parent, Notebook *book, size_t width )
		: NotebookCustomDlgBase( parent )
		, m_book(book)
		, m_width(width)
{
	long style = m_book->GetBookStyle();
	if(style & wxVB_TOP) {
		m_radioBox1->SetSelection(0);
	} else if(style & wxVB_BOTTOM){
		m_radioBox1->SetSelection(1);
	} else if(style & wxVB_LEFT) {
		m_radioBox1->SetSelection(2);
	} else { // wxVB_RIGHT
		m_radioBox1->SetSelection(3);
	}

	m_checkBoxFixedWidth->SetValue(m_book->GetBookStyle() & wxVB_FIXED_WIDTH);
}

void NotebookCustomDlg::OnOK(wxCommandEvent& e)
{
	int ori(wxTOP);
	int selection = m_radioBox1->GetSelection();
	switch(selection) {
	case 1: ori = wxVB_BOTTOM; break;
	case 2: ori = wxVB_LEFT;   break;
	case 3: ori = wxVB_RIGHT;  break;
	default:
	case 0: ori = wxVB_TOP;    break;
	}

	m_book->SetOrientation(ori);

	long style = m_book->GetBookStyle();
	if(m_checkBoxFixedWidth->IsChecked()) {
		if(m_width == Notebook::npos) {
			m_book->SetFixedTabWidth(120);
		}
		style |= wxVB_FIXED_WIDTH;
		m_book->SetBookStyle(style);
	} else {
		style &= ~(wxVB_FIXED_WIDTH);
		m_book->SetBookStyle(style);
	}
	EndModal(wxID_OK);
}
