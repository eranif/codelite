#include <wx/tokenzr.h>
#include "drawingutils.h"
#include "svnblamedialog.h"
#include "wx/wxscintilla.h"
#include <map>

SvnBlameDialog::SvnBlameDialog(wxWindow* window, const wxString &content)
		: wxDialog(window, wxID_ANY, wxString(wxT("Svn Blame")), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX |wxMINIMIZE_BOX)
		, m_content(content)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	m_textCtrl = new wxScintilla(this);
	GetSizer()->Add(m_textCtrl, 1, wxEXPAND|wxALL);

	// Initialize some styles
	m_textCtrl->SetLexer(wxSCI_LEX_NULL);
	m_textCtrl->StyleClearAll();
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	for (int i=0; i<=wxSCI_STYLE_DEFAULT; i++) {
		m_textCtrl->StyleSetBackground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		m_textCtrl->StyleSetForeground(i, wxT("BLACK"));
		m_textCtrl->StyleSetFont(i, font);
	}

	m_textCtrl->SetMarginType (0, wxSCI_MARGIN_RTEXT);
	m_textCtrl->SetMarginWidth(0, 200);
	m_textCtrl->SetMarginWidth(1, 0);
	m_textCtrl->SetMarginWidth(2, 0);
	m_textCtrl->SetMarginWidth(3, 0);
	m_textCtrl->SetMarginWidth(4, 0);

	SetText();
}

SvnBlameDialog::~SvnBlameDialog()
{
}

void SvnBlameDialog::SetText()
{

	m_textCtrl->StyleSetBackground(1, DrawingUtils::LightColour(wxT("GREEN"),  7.0));
	m_textCtrl->StyleSetBackground(2, DrawingUtils::LightColour(wxT("BLUE"),   7.0));
	m_textCtrl->StyleSetBackground(3, DrawingUtils::LightColour(wxT("ORANGE"), 7.0));
	m_textCtrl->StyleSetBackground(4, DrawingUtils::LightColour(wxT("YELLOW"), 7.0));
	m_textCtrl->StyleSetBackground(5, DrawingUtils::LightColour(wxT("PURPLE"), 7.0));

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	int xx, yy;
	int marginWidth(100);

	std::map<wxString, int> authorsColorsMap;
	wxArrayString lines = wxStringTokenize(m_content, wxT("\n"), wxTOKEN_RET_DELIMS);
	for(size_t i=0; i<lines.GetCount(); i++) {
		wxString revision;
		wxString author;
		wxString text;

		if(lines.Item(i).IsEmpty())
			continue;

		wxString line = lines.Item(i);
		line.Trim(false);

		revision = line.BeforeFirst(wxT(' '));
		revision.Trim().Trim(false);
		line = line.AfterFirst(wxT(' '));
		line.Trim(false);

		author = line.BeforeFirst(wxT(' '));
		author.Trim().Trim(false);

		int style;
		std::map<wxString, int>::iterator iter = authorsColorsMap.find(author);
		if(iter != authorsColorsMap.end()) {
			// Create new random color and use it
			style = (*iter).second;
		} else {
			style = (time(NULL) % 5) + 1;
			authorsColorsMap[author] = style;
		}

		line = line.AfterFirst(wxT(' '));

		wxString marginText = wxString::Format(wxT("%08s: %s"), revision.c_str(), author.c_str());
		wxWindow::GetTextExtent(marginText, &xx, &yy, NULL, NULL, &font);

		marginWidth = wxMax(marginWidth, xx);
		m_textCtrl->AppendText(line);
		m_textCtrl->MarginSetText((int)i, marginText);
		m_textCtrl->MarginSetStyle((int)i, style);
	}
	m_textCtrl->SetMarginWidth(0, marginWidth);
	m_textCtrl->SetReadOnly(true);
}
