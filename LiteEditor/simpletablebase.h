///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __simpletablebase__
#define __simpletablebase__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listctrl.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define ID_TOOLNEW 2500
#define ID_TOOLDELETE 2501
#define ID_TOOLDELETEALL 2502

///////////////////////////////////////////////////////////////////////////////
/// Class SimpleTableBase
///////////////////////////////////////////////////////////////////////////////
class SimpleTableBase : public wxPanel 
{
	private:
	
	protected:
		wxToolBar* m_toolBar2;
		wxListCtrl* m_listTable;
		wxStaticText* m_staticTextDisplayText;
		wxChoice* m_choiceDisplayFormat;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnNewWatch( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnNewWatchUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnDeleteWatch( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDeleteWatchUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnDeleteAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDeleteAllUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnListEditLabelBegin( wxListEvent& event ){ event.Skip(); }
		virtual void OnListEditLabelEnd( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemDeSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemRightClick( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnListKeyDown( wxListEvent& event ){ event.Skip(); }
		virtual void OnDisplayFormat( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		SimpleTableBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~SimpleTableBase();
	
};

#endif //__simpletablebase__
