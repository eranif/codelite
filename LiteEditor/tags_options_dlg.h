///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __tags_options_dlg__
#define __tags_options_dlg__

#include <wx/wx.h>

#include <wx/panel.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h> 
#include <wx/statline.h>
#include <wx/button.h>
#include "serialized_object.h"
#include "wx/filename.h"
#include "tags_options_data.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class TagsOptionsDlg
///////////////////////////////////////////////////////////////////////////////
class TagsOptionsDlg : public wxDialog 
{
private:

protected:
	wxFlatNotebook* m_mainBook;
	wxPanel* m_generalPage;
	wxCheckBox* m_checkParseComments;
	wxCheckBox* m_checkDisplayComments;
	wxCheckBox* m_checkDisplayTypeInfo;
	wxCheckBox* m_checkDisplayFunctionTip;
	wxCheckBox* m_checkLoadLastDB;
	wxCheckBox* m_checkLoadToMemory;
	wxPanel* m_ctagsPage;
	wxStaticText* m_staticText1;
	wxTextCtrl* m_textPrep;
	wxButton* m_buttonAdd;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_textFileSpec;
	wxStaticText* m_staticText5;
	wxComboBox* m_comboBoxLang;
	wxCheckBox* m_checkFilesWithoutExt;
	wxStaticLine* m_staticline1;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;
	TagsOptionsData m_data;
	wxCheckBox* m_checkColourLocalFuncAndVars;
	
protected:
	void SetFlag(CodeCompletionOpts flag, bool set);

	void InitValues();
	void CopyData();
	void OnButtonOK(wxCommandEvent &event);
	void OnButtonAdd(wxCommandEvent &event);

public:
	TagsOptionsDlg( wxWindow* parent, 
					const TagsOptionsData& data,					
				    int id = wxID_ANY, 
				    wxString title = wxT("Tags Options"), 
				    wxPoint pos = wxDefaultPosition, 
				    wxSize size = wxSize(469, 362), 
				    int style = wxDEFAULT_DIALOG_STYLE);

	TagsOptionsData &GetData() {return m_data;}
};

#endif //__tags_options_dlg__
