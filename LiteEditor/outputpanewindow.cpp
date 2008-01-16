#include "outputpanewindow.h"
#include "wx/wxscintilla.h"
#include "wx/xrc/xmlres.h"
#include "output_pane.h"
#include "manager.h"
#include "shell_window.h"
#include "frame.h"

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

OutputPaneWinodw::OutputPaneWinodw(wxWindow *parent, wxWindowID id, const wxString &name, int type)
: wxPanel(parent, id)
, m_name(name)
, m_type(type)
{
	CreateGUIControl();
}

OutputPaneWinodw::~OutputPaneWinodw()
{
}

void OutputPaneWinodw::CreateGUIControl()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);
	//Create the toolbar
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL);

	int id = wxNewId();
	tb->AddTool(id, 
				wxT("Clear All"), 
				wxXmlResource::Get()->LoadBitmap(wxT("document_delete")), 
				wxT("Clear All"));
	Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputPaneWinodw::OnClearAll ));

	id = wxNewId();
	tb->AddTool(id, 
				wxT("Word Wrap"), 
				wxXmlResource::Get()->LoadBitmap(wxT("word_wrap")), 
				wxT("Word Wrap"));
	Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputPaneWinodw::OnWordWrap ));
	tb->Realize(); 
	mainSizer->Add(tb, 0, wxALL|wxEXPAND, 5);
	
	//Create the scintilla page
	if(m_type == WINTYPE_SCI){
		m_window = new wxScintilla(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
		wxScintilla *sciWin = (wxScintilla *)m_window;
		// Hide margins
		sciWin->SetLexer(wxSCI_LEX_NULL);
		sciWin->StyleClearAll();
		// symbol margin
		sciWin->SetMarginType(0, wxSCI_MARGIN_SYMBOL);
		sciWin->SetMarginWidth(0, 0);//keep the symbol margin "On"
		sciWin->MarkerSetBackground(0x7, wxT("pink"));

		sciWin->SetMarginWidth(1, 0);
		sciWin->SetMarginWidth(2, 0);
		
		wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
		
		sciWin->StyleSetFont(0, font);
		sciWin->StyleSetBackground(0, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		sciWin->StyleSetBackground(wxSCI_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		sciWin->SetReadOnly(true);
		
	}else{
		//type shell
		m_window = new ShellWindow(this, ManagerST::Get());
		ManagerST::Get()->Connect(m_window->GetId(), wxEVT_SHELLWIN_LINE_ENTERED, wxCommandEventHandler(Manager::OnOutputWindow), NULL, ManagerST::Get());
		ManagerST::Get()->Connect(m_window->GetId(), wxEVT_SHELLWIN_LINE_ENTERED, wxCommandEventHandler(Manager::OnDebuggerWindow), NULL, ManagerST::Get());
	}

	mainSizer->Add(m_window, 1, wxEXPAND | wxALL, 1);
	mainSizer->Layout();
}

void OutputPaneWinodw::Clear()
{
	if(m_type == WINTYPE_SHELL){
		((ShellWindow*)m_window)->Clear();
		return;
	}

	wxScintilla *win = dynamic_cast<wxScintilla*>(m_window);
	if(win){
		win->SetReadOnly(false);
		win->ClearAll();
		win->SetReadOnly(true);
	}
}


void OutputPaneWinodw::OnClearAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Clear();
}

void OutputPaneWinodw::OnWordWrap(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxScintilla *win;
	if(m_type == WINTYPE_SCI){
		win = (wxScintilla*)m_window;
	}else{
		win = (wxScintilla*)(((ShellWindow*)m_window)->GetOutWin());
	}
	
	if(win->GetWrapMode() == wxSCI_WRAP_WORD){
		win->SetWrapMode(wxSCI_WRAP_NONE);
	}else{
		win->SetWrapMode(wxSCI_WRAP_WORD);
	}
}


void OutputPaneWinodw::AppendText(const wxString &text)
{
	if(m_type == WINTYPE_SCI){
		wxScintilla *win = dynamic_cast<wxScintilla*>(m_window);
		if( win )
		{
			// enable writing
			win->SetReadOnly(false);					

			// the next 4 lines make sure that the caret is at last line
			// and is visible
			win->SetSelectionEnd(win->GetLength());
			win->SetSelectionStart(win->GetLength());
			win->SetCurrentPos(win->GetLength());
			win->EnsureCaretVisible();

			// add the text
			win->AddText( text );						

			// the next 4 lines make sure that the caret is at last line
			// and is visible
			win->SetSelectionEnd(win->GetLength());
			win->SetSelectionStart(win->GetLength());
			win->SetCurrentPos(win->GetLength());
			win->EnsureCaretVisible();

			// enable readonly mode 
			win->SetReadOnly(true);
		}
	}else{
		ShellWindow *win = (ShellWindow*)m_window;
		win->AppendLine(text);
	}
}

