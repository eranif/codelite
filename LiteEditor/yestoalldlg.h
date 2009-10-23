#ifndef __yestoalldlg__
#define __yestoalldlg__

/**
    \class YesToAllDlg
	A generic dialog with Yes/No buttons and an 'Apply to all' checkbox.
	Set the dialog's message in the ctor, or call SetMessage() later.
	Customise the checkbox text with SetCheckboxText().
	EndModal returns one of ID_YES ID_NO or ID_CANCEL
*/

#include "yestoallbasedlg.h"

class YesToAllDlg : public YesToAllBaseDlg 
{
	protected:
		virtual void OnNoClicked( wxCommandEvent& WXUNUSED(event) ) { EndModal(wxID_NO); }
		virtual void OnYesClicked( wxCommandEvent& WXUNUSED(event) ) { EndModal(wxID_YES); }

	public:
		YesToAllDlg( wxWindow* parent, const wxString& message = wxT("This space unintentionally left blank"), const bool setcheck = false,
						const wxString& title = wxT("Are you sure?"), wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxSize( wxDefaultSize ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		void SetMessage(const wxString& message);
		void SetCheckboxText(const wxString& text);
		bool GetIsChecked() const { return m_checkBoxAll->IsChecked(); }
	
};

#endif //__yestoalldlg__
