#include "outputtabwindow.h"
#include "wx/ffile.h"
#include "output_pane.h"
#include "wx/sizer.h"
#include "wx/toolbar.h"
#include "wx/xrc/xmlres.h"
#include "macros.h"

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

OutputTabWindow::OutputTabWindow(wxWindow *parent, wxWindowID id, const wxString &name)
		: wxPanel(parent, id)
		, m_name(name)
		, m_canFocus(true)
{
	CreateGUIControl();
}

OutputTabWindow::~OutputTabWindow()
{
}

void OutputTabWindow::CreateGUIControl()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(mainSizer);
	//Create the toolbar
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_VERTICAL|wxTB_NODIVIDER);

	int id = wxNewId();
	tb->AddTool(id,
	            wxT("Clear All"),
	            wxXmlResource::Get()->LoadBitmap(wxT("document_delete")),
	            wxT("Clear All"));
	Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputTabWindow::OnClearAll ));

	id = wxNewId();
	tb->AddTool(id,
	            wxT("Word Wrap"),
	            wxXmlResource::Get()->LoadBitmap(wxT("word_wrap")),
	            wxT("Word Wrap"));
	Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputTabWindow::OnWordWrap ));

	if (m_name == OutputPane::BUILD_WIN) {
		id = wxNewId();
		tb->AddTool(id,
		            wxT("Set compiler colours..."),
		            wxXmlResource::Get()->LoadBitmap(wxT("colourise")),
		            wxT("Set compiler colours..."));
		Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputTabWindow::OnCompilerColours ));
	}

	tb->Realize();
	mainSizer->Add(tb, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

	m_sci = new wxScintilla(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	// Hide margins
	m_sci->SetLexer(wxSCI_LEX_CONTAINER);
	m_sci->StyleClearAll();

	// symbol margin
	m_sci->SetMarginType(1, wxSCI_MARGIN_SYMBOL);
	m_sci->SetMarginWidth(2, 0);
	m_sci->SetMarginWidth(1, 0);
	m_sci->SetMarginWidth(0, 0);


	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	m_sci->StyleSetFont(0, font);
	m_sci->StyleSetBackground(0, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_sci->StyleSetBackground(wxSCI_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_sci->SetReadOnly(true);

	mainSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);
	mainSizer->Layout();

	//Connect events
	//------------------------

	Connect(wxEVT_SCI_DOUBLECLICK, wxScintillaEventHandler(OutputTabWindow::OnMouseDClick), NULL, this);
	Connect(wxEVT_SCI_HOTSPOT_CLICK, wxScintillaEventHandler(OutputTabWindow::OnHotspotClicked), NULL, this);
}

void OutputTabWindow::OnSetFocus(wxFocusEvent &event)
{
	if ( m_canFocus ) {
		return;
	}

	wxWindow *prevFocusWin = event.GetWindow();
	if ( prevFocusWin ) {
		prevFocusWin->SetFocus();
	}
	event.Skip();
}

void OutputTabWindow::OnWordWrap(wxCommandEvent &e)
{
	if (m_sci->GetWrapMode() == wxSCI_WRAP_WORD) {
		m_sci->SetWrapMode(wxSCI_WRAP_NONE);
	} else {
		m_sci->SetWrapMode(wxSCI_WRAP_WORD);
	}
}

void OutputTabWindow::AppendText(const wxString &text)
{
	//----------------------------------------------
	// enable writing
	m_sci->SetReadOnly(false);

	// the next 4 lines make sure that the caret is at last line
	// and is visible
	m_sci->SetSelectionEnd(m_sci->GetLength());
	m_sci->SetSelectionStart(m_sci->GetLength());
	m_sci->SetCurrentPos(m_sci->GetLength());
	m_sci->EnsureCaretVisible();

	// add the text
	m_sci->AddText( text );

	// the next 4 lines make sure that the caret is at last line
	// and is visible
	m_sci->SetSelectionEnd(m_sci->GetLength());
	m_sci->SetSelectionStart(m_sci->GetLength());
	m_sci->SetCurrentPos(m_sci->GetLength());
	m_sci->EnsureCaretVisible();

	// enable readonly mode
	m_sci->SetReadOnly(true);
}

void OutputTabWindow::OnClearAll(wxCommandEvent &e)
{
	Clear();
}

void OutputTabWindow::Clear()
{
	m_sci->SetReadOnly(false);
	m_sci->ClearAll();
	m_sci->SetReadOnly(true);
}

void OutputTabWindow::OnCommand(wxCommandEvent &e)
{
	switch (e.GetId()) {
	case wxID_COPY:
		m_sci->Copy();
		break;
	default:
		break;
	}
}

void OutputTabWindow::OnUpdateUI(wxUpdateUIEvent &e)
{
	switch (e.GetId()) {
	case wxID_COPY:
		e.Enable( m_sci->GetSelectionStart() - m_sci->GetSelectionEnd() != 0 );
		break;
	default:
		break;
	}
	e.Skip(false);
}
