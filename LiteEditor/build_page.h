///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __build_page__
#define __build_page__

#include <wx/wx.h>

#include <wx/choicebk.h>
#include "wx/panel.h"
#include "filepicker.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BuildPage
///////////////////////////////////////////////////////////////////////////////
class BuildPage : public wxPanel 
{
private:

protected:
	wxStaticText* m_staticText;
	wxChoicebook* m_bookBuildSystems;
	

	void CustomInit();
	wxPanel *CreateBuildSystemPage(const wxString &name);

public:
	BuildPage( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxTAB_TRAVERSAL );
	void Save();
};

class BuildSystemPage : public wxPanel {
	wxStaticText* m_staticText17;
	FilePicker* m_filePicker;
	wxStaticText* m_staticText18;
	wxTextCtrl* m_textBuildToolOptions;
	wxString m_name;
public:
	BuildSystemPage(wxWindow *parent, wxString name);
	virtual ~BuildSystemPage(){}

	void Save();
};
#endif //__build_page__
