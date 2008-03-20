#ifndef __open_type_dlg__
#define __open_type_dlg__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/button.h>
#include "ctags_manager.h"
#include "map"

///////////////////////////////////////////////////////////////////////////////
/// Class OpenTypeDlg
///////////////////////////////////////////////////////////////////////////////
class wxImageList;
class wxTimer;

class OpenTypeDlg : public wxDialog 
{
protected:
	wxStaticText* m_staticText;
	wxTextCtrl* m_textTypeName;
	wxListView* m_listTypes;
	wxStaticLine* m_staticline1;
	wxButton* m_buttonOK;
	wxButton* m_button2;
	std::vector<TagEntryPtr> m_tags;
	wxString m_filter ;
	wxImageList *m_il;
	TagEntryPtr m_tag;
	std::map<wxString, TagEntryPtr> m_itemsData;
	TagsManager *m_tagsManager;
	wxTimer *m_timer;
	
protected:
	void Init();
	void OnTimer(wxTimerEvent &event);
	
	void OnCharHook(wxKeyEvent &event);
	void OnItemActivated(wxListEvent &event);
	void PopulateList();
	int GetTagImage(const wxString &kind);
	void OnOK(wxCommandEvent &event);
	void TryOpenAndEndModal();
	
public:
	OpenTypeDlg( wxWindow* parent, TagsManager *tagsMgr, int id = wxID_ANY, wxString title = wxT("Open Type"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 682, 353 ), int style = wxDEFAULT_DIALOG_STYLE);
	virtual ~OpenTypeDlg();
	TagEntryPtr GetSelectedTag() const {return m_tag;}
	DECLARE_EVENT_TABLE()
};

#endif //__open_type_dlg__
