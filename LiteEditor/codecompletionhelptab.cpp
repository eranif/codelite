#include "codecompletionhelptab.h"
#include "frame.h"
#include <wx/settings.h>

BEGIN_EVENT_TABLE(CodeCompletionHelpTab, wxTextCtrl)
EVT_LEFT_DOWN(CodeCompletionHelpTab::OnLeftDown)
END_EVENT_TABLE()

CodeCompletionHelpTab::CodeCompletionHelpTab(wxWindow* parent)
	: wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2)
{
	// set the default font
	m_defaultFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	m_codeFont    = wxFont(m_defaultFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxNORMAL);

	wxFont boldFont;
	wxFont urlFont;
	SetFont(m_defaultFont);

	boldFont = m_defaultFont;
	boldFont.SetWeight(wxBOLD);

	m_titleAttr.SetFont(boldFont);
	m_codeAttr.SetFont(m_codeFont);

	urlFont.SetUnderlined(true);
	m_urlAttr.SetFont(urlFont);
	m_urlAttr.SetTextColour(wxT("BLUE"));
	SetInsertionPointEnd();
}

CodeCompletionHelpTab::~CodeCompletionHelpTab()
{
}

void CodeCompletionHelpTab::AppendText(const wxString& text)
{
	SetInsertionPointEnd();
	wxTextCtrl::AppendText( text );
}

void CodeCompletionHelpTab::Clear()
{
	wxTextCtrl::Clear();
}

void CodeCompletionHelpTab::WriteTitle(const wxString& text)
{
	int start = GetLastPosition();
	AppendText(text);
	SetStyle(start, GetLastPosition(), m_titleAttr);
}

void CodeCompletionHelpTab::WriteCode(const wxString& text)
{
	int start = GetLastPosition();
	AppendText(text);
	SetStyle(start, GetLastPosition(), m_codeAttr);
}

void CodeCompletionHelpTab::WriteFileURL(const wxString& text)
{
	int start = GetLastPosition();
	AppendText(text);
	SetStyle(start, GetLastPosition(), m_urlAttr);
}

void CodeCompletionHelpTab::OnLeftDown(wxMouseEvent& event)
{
	long pos;
	wxTextCtrlHitTestResult where = HitTest(event.GetPosition(), &pos);
	if(where == wxTE_HT_ON_TEXT) {
		// We clicked over a text, check to see if this is a file

		wxTextAttr s;
		if(GetStyle(pos, s)) {
			if(s.GetTextColour() == wxColour(wxT("BLUE"))) {
				long x,y;
				PositionToXY(pos, &x, &y);

				wxString li = this->GetLineText(y);
				wxString filename = li.BeforeLast(wxT(':'));
				wxString sNum     = li.AfterLast(wxT(':'));

				long num;
				sNum.ToLong(&num);

				Frame::Get()->GetMainBook()->OpenFile(filename, wxEmptyString, num-1);
				return;
			}
		}
	}
	event.Skip();
}
