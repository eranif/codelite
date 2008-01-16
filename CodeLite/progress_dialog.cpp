#include "progress_dialog.h"

///////////////////////////////////////////////////////////////////////////

ProgressDialog::ProgressDialog(const wxString &title, const wxString &message, int maximum, wxWindow *parent) 
: wxProgressDialog(title, message, maximum, parent,  wxPD_APP_MODAL |	wxPD_SMOOTH | wxPD_AUTO_HIDE)
{
	SetSize(400, 250);
	Layout();
}

ProgressDialog::~ProgressDialog()
{
}
