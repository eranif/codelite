#pragma once 
#include <wx/wx.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>

class Frame :
	public wxFrame
{
public:
	// Construction of MainFrame
	Frame(wxWindow* parent, const wxChar *title);
	virtual ~Frame(void);
	bool Create(wxFrame * parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style );
	wxWindow* CreatePage();

	// Menus
	wxMenu* m_fileMenu;
	wxMenu* m_editMenu;
	wxMenu* m_visualMenu;

	wxMenuBar* m_menuBar;
	wxFlatNotebookImageList m_ImageList;

#ifdef DEVELOPMENT
	wxLog *m_logTargetOld;
#endif

	// Event handlers

	// Menu
	virtual void OnInsertPage(wxCommandEvent& WXUNUSED(event));
	virtual void OnInsertBeforeGivenPage(wxCommandEvent& WXUNUSED(event));
	virtual void OnQuit(wxCommandEvent& WXUNUSED(event));
	virtual void OnDeleteAll(wxCommandEvent& WXUNUSED(event));
	virtual void OnAddPage(wxCommandEvent& WXUNUSED(event));
	virtual void OnDeletePage(wxCommandEvent& WXUNUSED(event));
	virtual void OnDeleteGivenPage(wxCommandEvent& WXUNUSED(event));
	virtual void OnSetSelection(wxCommandEvent& WXUNUSED(event));
	virtual void OnAdvanceSelectionFwd(wxCommandEvent& WXUNUSED(event));
	virtual void OnAdvanceSelectionBack(wxCommandEvent& WXUNUSED(event));
	virtual void OnShowImages(wxCommandEvent& WXUNUSED(event));
	virtual void OnVC71Style(wxCommandEvent& WXUNUSED(event));
	virtual void OnVC8Style(wxCommandEvent& WXUNUSED(event));
	virtual void OnDefaultStyle(wxCommandEvent& WXUNUSED(event));
	virtual void OnFancyStyle(wxCommandEvent& WXUNUSED(event));
	virtual void OnSelectColor(wxCommandEvent& event);
	virtual void OnSetPageImageIndex(wxCommandEvent& event);
	virtual void OnStyle(wxCommandEvent& event);
	virtual void OnDrawTabX(wxCommandEvent& event);
	virtual void OnGradientBack(wxCommandEvent& event);
	virtual void OnColorfullTabs(wxCommandEvent& event);
	virtual void OnSmartTabs(wxCommandEvent& event);
	virtual void OnDropDownArrow(wxCommandEvent& event);
	virtual void OnDropDownArrowUI(wxUpdateUIEvent & event);
	virtual void OnHideNavigationButtonsUI(wxUpdateUIEvent & event);

	virtual void OnEnableTab(wxCommandEvent& event);
	virtual void OnDisableTab(wxCommandEvent& event);
	virtual void OnEnableDrag(wxCommandEvent& event);
	virtual void OnDClickCloseTab(wxCommandEvent& event);
	virtual void OnAllowForeignDnd( wxCommandEvent & event );
	virtual void OnAllowForeignDndUI( wxUpdateUIEvent & event );

	// Notebook
	virtual void OnPageChanging(wxFlatNotebookEvent& event);
	virtual void OnPageChanged(wxFlatNotebookEvent& event);
	virtual void OnPageClosing(wxFlatNotebookEvent& event);
	virtual void OnSetAllPagesShapeAngle(wxCommandEvent& WXUNUSED(event));

private:
	long GetTabIndexFromUser(const wxString &title, const wxString &prompt) const;

	void DeleteNotebookPage (long tabIdx);
	bool InsertNotebookPage (long tabIdx);
	wxFlatNotebook *book;
	wxFlatNotebook *secondBook;
	bool m_bShowImages;
	bool m_bVCStyle;
	int newPageCounter;

	DECLARE_EVENT_TABLE()
};
