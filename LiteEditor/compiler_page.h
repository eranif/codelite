///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __compiler_page__
#define __compiler_page__

#include <wx/wx.h>

#include <wx/listctrl.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CompilerPage
///////////////////////////////////////////////////////////////////////////////
class CompilerPage : public wxScrolledWindow 
{
	protected:
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textErrorPattern;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textErrorFileIndex;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textErrorLineNumber;
		wxStaticText* m_staticText51;
		wxTextCtrl* m_textWarnPattern;
		wxStaticText* m_staticText61;
		wxTextCtrl* m_textWarnFileIndex;
		wxStaticText* m_staticText71;
		wxTextCtrl* m_textWarnLineNumber;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textCompilerName;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_textLinkerName;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_textSOLinker;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textArchiveTool;
		wxStaticText* m_staticText14;
		wxTextCtrl* m_textResourceCmp;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textObjectExtension;
		wxStaticText* m_staticText141;
		wxTextCtrl* m_textCtrlGlobalIncludePath;
		wxStaticText* m_staticText16;
		wxTextCtrl* m_textCtrlGlobalLibPath;
		wxStaticText* m_staticText8;
		wxListCtrl* m_listSwitches;


	wxString m_selSwitchName ;
	wxString m_selSwitchValue;
	wxString m_cmpname;

	// Virtual event handlers, overide them in your derived class
	virtual void OnItemActivated( wxListEvent& event );
	virtual void OnItemSelected( wxListEvent& event );

	void EditSwitch();
	void InitSwitches();
	void CustomInitialize();
	void ConnectEvents();	
	void AddSwitch(const wxString &name, const wxString &value, bool choose);
	

public:
	CompilerPage( wxWindow* parent, wxString name, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 732,409 ), int style = wxTAB_TRAVERSAL );
	void Save();

};

#endif //__compiler_page__
