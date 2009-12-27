#include <wx/tokenzr.h>
#include "svnblameeditor.h"
#include "drawingutils.h"
#include "svnblamedialog.h"
#include "wx/wxscintilla.h"
#include <map>

SvnBlameDialog::SvnBlameDialog(wxWindow* window, const wxString &content)
		: wxDialog(window, wxID_ANY, wxString(wxT("Svn Blame")), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX |wxMINIMIZE_BOX)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	m_textCtrl = new SvnBlameEditor(this);
	GetSizer()->Add(m_textCtrl, 1, wxEXPAND|wxALL);

	m_textCtrl->SetText(content);
}

SvnBlameDialog::~SvnBlameDialog()
{
}
