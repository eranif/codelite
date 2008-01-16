///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 28 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __quickoutlinedlg__
#define __quickoutlinedlg__

#include <wx/wx.h>

#include <wx/statline.h>
#include <wx/treectrl.h>
#include "wx/timer.h"

class CppSymbolTree;

///////////////////////////////////////////////////////////////////////////////
/// Class QuickOutlineDlg
///////////////////////////////////////////////////////////////////////////////
class QuickOutlineDlg : public wxDialog 
{
	wxString m_fileName;
	
protected:
	wxTextCtrl* m_textFilter;
	wxStaticLine* m_staticline1;
	CppSymbolTree* m_treeOutline;
	
	DECLARE_EVENT_TABLE()
	void OnCharHook(wxKeyEvent &e);
	void OnTextEntered(wxCommandEvent &e);
 
public:
	QuickOutlineDlg( wxWindow* parent, const wxString &fileName, int id = wxID_ANY, wxString title = wxEmptyString, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 371,386 ), int style = wxBORDER_SIMPLE);
	virtual ~QuickOutlineDlg();
};

#endif //__quickoutlinedlg__
