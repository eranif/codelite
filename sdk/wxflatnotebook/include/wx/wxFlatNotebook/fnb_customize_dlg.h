#ifndef __fnb_customize_dlg__
#define __fnb_customize_dlg__

#include <wx/statline.h>
#include <wx/button.h>
#include "wx/dialog.h"
#include "wx/panel.h"

class wxCheckBox;
class wxRadioBox;

///////////////////////////////////////////////////////////////////////////////
/// Class wxFNBCustomizeDialog
///////////////////////////////////////////////////////////////////////////////
class wxFNBCustomizeDialog : public wxDialog 
{
protected:
	wxStaticLine* m_staticline2;
	wxButton* m_close;
	long m_options;

	// Option's page members
	wxRadioBox* m_styles;
	wxRadioBox* m_tabVPosition;
	wxRadioBox* m_navigationStyle;
	wxCheckBox* m_tabBorder;
	wxCheckBox* m_hideCloseButton;
	wxCheckBox* m_mouseMiddleCloseTab;
	wxCheckBox* m_xButtonOnTab;
	wxCheckBox* m_dlbClickCloseTab;
	wxCheckBox* m_smartTabbing;
	wxCheckBox* m_allowDragAndDrop;
	wxCheckBox* m_foreignDnD;
	wxCheckBox* m_gradient;
	wxCheckBox* m_colorfulTab;

public:
	wxFNBCustomizeDialog( wxWindow* parent, long options, int id = wxID_ANY, wxString title = wxT("Customize"), wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, int style = wxDEFAULT_DIALOG_STYLE );

protected:
	void OnClose(wxCommandEvent &event);
	void OnStyle(wxCommandEvent &event);

private:
	wxPanel *CreateOptionsPage();
	void ConnectEvents();
};

#endif //__fnb_customize_dlg__

