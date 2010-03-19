#ifndef __auicontrolbar__
#define __auicontrolbar__

#include <wx/panel.h>
#include <wx/window.h>
#include <wx/bitmap.h>
#include <vector>
#include <wx/choicebk.h>
#include <wx/pen.h>
#include <wx/sizer.h>
#include <wx/aui/auibar.h>
#include <wx/toolbar.h>
#include <wx/aui/framemanager.h>
#include "output_pane_book.h"

#if defined (__WXMSW__)||defined(__WXMAC__)
#    define OP_USE_AUI_TOOLBAR 1
#else
#    define OP_USE_AUI_TOOLBAR 0
#endif

//--------------------------------------------------------
class OutputViewControlBar : public wxPanel
{
	wxAuiManager *                  m_aui;
	OutputPaneBook*                 m_book;

#if !OP_USE_AUI_TOOLBAR
	wxToolBar*                      m_buttons;
#else
	wxAuiToolBar*                   m_buttons;
#endif

	std::vector<wxToolBarToolBase*> m_tools;

protected:
	void DoSetButtonState(int btnId);
	void DoSetButtonState(const wxString &label);

public:
	OutputViewControlBar(wxWindow *win, OutputPaneBook *book, wxAuiManager *aui, wxWindowID id);

	virtual ~OutputViewControlBar();

	void AddButton        (const wxString &text, const wxBitmap & bmp, bool selected);
	void AddAllButtons    ();

	void OnButtonClicked        (wxCommandEvent      &event);
	void OnPageChanged          (wxChoicebookEvent   &event);
	void OnRender               (wxAuiManagerEvent   &event);
	void OnEditorFocus          (wxCommandEvent      &event);

protected:
	void        DoTogglePane     (bool hide = true);
	wxWindow *  DoFindButton     (const wxString &name);
	void        DoMarkActive     (const wxString &name);
	bool        DoFindDockInfo   (const wxString &saved_perspective, const wxString &dock_name, wxString &dock_info);
};

#endif // __auicontrolbar__

