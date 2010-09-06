#include "outputviewcontrolbar.h"
#include "cl_aui_tb_are.h"
#include "globals.h"
#include <wx/bmpbuttn.h>
#include <wx/wupdlock.h>
#include <wx/imaglist.h>
#include <wx/button.h>
#include <wx/frame.h>
#include "editor_config.h"
#include <wx/app.h>
#include "plugin.h"
#include <wx/image.h>
#include <wx/menu.h>
#include <wx/tokenzr.h>
#include <wx/choicebk.h>
#include <wx/sizer.h>
#include "drawingutils.h"
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/log.h>
#include <wx/aui/framemanager.h>
#include <wx/toolbar.h>

OutputViewControlBar::OutputViewControlBar(wxWindow* win, OutputPaneBook *book, wxAuiManager *aui, wxWindowID id)
		: wxPanel          (win, id, wxDefaultPosition, wxSize(-1, -1))
		, m_aui            (aui)
		, m_book           (book)
		, m_buttons        (NULL)
		, m_buildInProgress(false)
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer( mainSizer );

#if !OP_USE_AUI_TOOLBAR
	m_buttons = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_FLAT|wxTB_NODIVIDER);
#else
	m_buttons = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT);
	m_buttons->SetArtProvider(new CLMainAuiTBArt());
#endif

	if ( m_book ) {
		m_book->Connect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, wxChoicebookEventHandler(OutputViewControlBar::OnPageChanged), NULL, this);
	}
	m_buttons->Connect(wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(OutputViewControlBar::OnButtonClicked), NULL, this);
	if ( m_aui ) {
		m_aui->Connect(wxEVT_AUI_RENDER, wxAuiManagerEventHandler(OutputViewControlBar::OnRender), NULL, this);
	}
	wxTheApp->Connect ( wxEVT_EDITOR_CLICKED         , wxCommandEventHandler ( OutputViewControlBar::OnEditorFocus          ), NULL, this );
	mainSizer->Add(m_buttons, 1, wxEXPAND);
	mainSizer->Layout();
}

OutputViewControlBar::~OutputViewControlBar()
{
}

void OutputViewControlBar::AddButton(const wxString& text, const wxBitmap& bmp, bool selected)
{
#if !OP_USE_AUI_TOOLBAR
	m_tools.push_back( m_buttons->AddTool(wxID_ANY, text, bmp, text, wxITEM_CHECK) );
#else
	m_buttons->AddTool(wxXmlResource::GetXRCID(text.c_str()), text, bmp, text, wxITEM_CHECK);
#endif
}

void OutputViewControlBar::OnButtonClicked(wxCommandEvent& event)
{
	event.Skip();
	wxString label;
#if !OP_USE_AUI_TOOLBAR
	wxToolBarToolBase *item =  m_buttons->FindById(event.GetId());
	if(item) {
		label = item->GetLabel();
	}

	if(label.IsEmpty())
		return;

	wxWindowUpdateLocker locker( wxTheApp->GetTopWindow() );
	DoMarkActive( label );
	if(event.IsChecked())
		DoTogglePane(false);
	else
		DoTogglePane(true);

	bool checked = event.IsChecked();
#else

	wxAuiToolBarItem *item = m_buttons->FindTool(event.GetId());
	if(item) {
		label = item->GetLabel();
	}

	if(label.IsEmpty())
		return;

	wxWindowUpdateLocker locker( wxTheApp->GetTopWindow() );
	DoMarkActive( label );

	bool checked = m_buttons->GetToolToggled(event.GetId());
	if(checked)
		DoTogglePane(false);
	else
		DoTogglePane(true);
#endif

	// Uncheck all the buttons except for the selected one
	DoSetButtonState(checked ? event.GetId() : wxNOT_FOUND);
}

void OutputViewControlBar::DoTogglePane(bool hide)
{
	static wxString saved_dock_info;
	if ( m_book && m_aui ) {
		wxAuiPaneInfo &pane_info = m_aui->GetPane(wxT("Output View"));
		wxString dock_info ( wxString::Format(wxT("dock_size(%d,%d,%d)"), pane_info.dock_direction, pane_info.dock_layer, pane_info.dock_row) );
		if ( hide ) {
			if ( pane_info.IsShown() ) {
				wxTheApp->GetTopWindow()->Freeze();

				DoFindDockInfo(m_aui->SavePerspective(), dock_info, saved_dock_info);
				pane_info.Hide();

				m_aui->Update();

				wxTheApp->GetTopWindow()->Thaw();
			}


		} else {
			// Show it
			if ( pane_info.IsShown() == false ) {
				wxTheApp->GetTopWindow()->Freeze();
				if ( saved_dock_info.IsEmpty() ) {
					pane_info.Show();
					m_aui->Update();
				} else {
					wxString auiPerspective = m_aui->SavePerspective();
					if ( auiPerspective.Find(dock_info) == wxNOT_FOUND ) {
						// the dock_info does not exist
						auiPerspective << saved_dock_info << wxT("|");
						m_aui->LoadPerspective(auiPerspective, false);
						pane_info.Show();
						m_aui->Update();
					} else {
						pane_info.Show();
						m_aui->Update();
					}
				}
				wxTheApp->GetTopWindow()->Thaw();
			}
		}
	}

#if wxVERSION_NUMBER >= 2900
	// This is needed in >=wxGTK-2.9, otherwise output pane doesn't fully expand, or on closing the auinotebook doesn't occupy its space
	wxTheApp->GetTopWindow()->SendSizeEvent(wxSEND_EVENT_POST);
#endif
}

void OutputViewControlBar::OnRender(wxAuiManagerEvent& event)
{
	if ( m_aui && m_aui->GetPane(wxT("Output View")).IsShown() == false ) {
		DoMarkActive( wxEmptyString );
		DoSetButtonState(wxNOT_FOUND);

	} else if ( m_aui ) {
		DoMarkActive ( m_book->GetPageText( m_book->GetSelection() ) );
		DoSetButtonState( m_book->GetPageText( m_book->GetSelection() ) );
	}

	event.Skip();
}

void OutputViewControlBar::DoMarkActive(const wxString& name)
{
	if ( m_book && name.IsEmpty() == false ) {
		for (size_t i=0; i<m_book->GetPageCount(); i++) {
			if ( m_book->GetPageText(i) == name ) {
				m_book->SetSelection(i);
				break;
			}
		}
	}
}

void OutputViewControlBar::AddAllButtons()
{

	if ( m_book ) {
		for (size_t i=0; i<m_book->GetPageCount(); i++) {

			wxString text = m_book->GetPageText(i);
			wxBitmap bmp  = m_book->GetBitmap(i);

			AddButton(text, bmp, (size_t)m_book->GetSelection() == i);
		}
		m_buttons->Realize();
	}

}

void OutputViewControlBar::OnPageChanged(wxChoicebookEvent& event)
{
	int cursel = m_book->GetSelection();
	if ( cursel != wxNOT_FOUND ) {
		wxString selectedPageText = m_book->GetPageText(cursel);
		DoMarkActive( selectedPageText );

		DoSetButtonState( selectedPageText );
	}

	event.Skip();
}

bool OutputViewControlBar::DoFindDockInfo(const wxString &saved_perspective, const wxString &dock_name, wxString &dock_info)
{
	// search for the 'Output View' perspective
	wxArrayString panes = wxStringTokenize(saved_perspective, wxT("|"), wxTOKEN_STRTOK);
	for (size_t i=0; i<panes.GetCount(); i++) {
		if ( panes.Item(i).StartsWith(dock_name) ) {
			dock_info = panes.Item(i);
			return true;
		}
	}
	return false;
}

void OutputViewControlBar::OnEditorFocus(wxCommandEvent& event)
{
	event.Skip();

	if (EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {

		// Optionally don't hide the Debug pane: it's irritating during a debug session, you click to set a breakpoint...
		if (EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfDebug()) {
			// Find if it's 'Debug' that's visible
			int cursel = m_book->GetSelection();
			if ( cursel != wxNOT_FOUND && m_book->GetPageText(cursel) == wxT("Debug") ) {
				return;
			}
		}

		// avoid auto-hiding when build is in progress
		if(m_buildInProgress) {
			return;
		}

		// and hide the output view
		DoTogglePane(true);
	}
}

void OutputViewControlBar::DoSetButtonState(int btnId)
{
#if !OP_USE_AUI_TOOLBAR
	for(size_t i=0; i<m_tools.size(); i++) {
		if(m_tools.at(i)->GetId() == btnId) {
			m_buttons->ToggleTool(m_tools.at(i)->GetId(), true);

		} else {
			m_buttons->ToggleTool(m_tools.at(i)->GetId(), false);

		}
	}
#else

	for(size_t i=0; i<m_buttons->GetToolCount(); i++) {
		wxAuiToolBarItem *item = m_buttons->FindToolByIndex(i);
		if(item && item->GetId() != wxNOT_FOUND) {
			m_buttons->ToggleTool(item->GetId(), false);
		}
	}
	if(btnId != wxNOT_FOUND)
		m_buttons->ToggleTool(btnId, true);

	m_buttons->Refresh();
#endif
}

void OutputViewControlBar::DoSetButtonState(const wxString& label)
{
#if !OP_USE_AUI_TOOLBAR
	for(size_t i=0; i<m_tools.size(); i++) {
		if(m_tools.at(i)->GetLabel() == label) {
			DoSetButtonState( m_tools.at(i)->GetId() );
			return;

		}
	}
#else
	for(size_t i=0; i<m_buttons->GetToolCount(); i++) {
		wxAuiToolBarItem *item = m_buttons->FindToolByIndex(i);
		if(item && item->GetLabel() == label) {
			DoSetButtonState( item->GetId() );
			return;
		}
	}
#endif
	DoSetButtonState(wxNOT_FOUND);
}

void OutputViewControlBar::OnBuildEnded(wxCommandEvent& event)
{
	m_buildInProgress = false;
	event.Skip();
}

void OutputViewControlBar::OnBuildStarted(wxCommandEvent& event)
{
	m_buildInProgress = true;
	event.Skip();
}

