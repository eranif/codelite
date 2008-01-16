///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __new_item_dlg__
#define __new_item_dlg__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/statline.h>
#include "wx/filename.h"
#include "wx/imaglist.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewItemDlg
///////////////////////////////////////////////////////////////////////////////
class NewItemDlg : public wxDialog 
{
private:

protected:
	wxListCtrl* m_fileType;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_location;
	wxButton* m_browseBtn;
	wxStaticText* m_staticText6;
	wxTextCtrl* m_fileName;
	wxStaticLine* m_staticline1;
	wxButton* m_okButton;
	wxButton* m_cancel;
	wxString m_fileTypeValue;
	wxFileName m_newFileName;
	wxString m_cwd;

public:
	NewItemDlg( wxWindow* parent, wxString cwd, int id = wxID_ANY, wxString title = wxEmptyString, wxPoint pos = wxDefaultPosition, wxSize size = wxSize(500, 350), int style = wxDEFAULT_DIALOG_STYLE );
	const wxFileName &GetFileName() const { return m_newFileName; }

protected:
	//------------------------------------
	// Event handlers
	//------------------------------------
	DECLARE_EVENT_TABLE();
	virtual void OnClick(wxCommandEvent &event);
	virtual void OnListItemSelected(wxListEvent &event);
	virtual void OnCharHook(wxKeyEvent &event);
	
private:
	void ConnectEvents();
	bool Validate(wxString &errMsg);
	void DoCreateFile();
};

#endif //__new_item_dlg__
