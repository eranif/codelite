#include "cppcheckreportpage.h"
#include "cppchecker.h"
#include "plugin.h"
#include <wx/regex.h>
#include <wx/log.h>

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

// from sdk/wxscintilla/src/scintilla/src/LexGCC.cxx:
extern void SetCppCheckColourFunction ( int ( *colorfunc ) ( int, const char*, size_t&, size_t& ) );
static size_t sErrorCount (0);

CppCheckReportPage::CppCheckReportPage(wxWindow* parent, IManager* mgr, CppCheckPlugin* plugin)
		: CppCheckReportBasePage( parent )
		, m_mgr(mgr)
		, m_plugin(plugin)
{
	m_outputText->SetReadOnly(true);

	SetCppCheckColourFunction ( ColorLine );

	// Initialize the output text style
	m_outputText->SetLexer(wxSCI_LEX_CPPCHECK);
	m_outputText->StyleClearAll();

	// Use font
	wxFont defFont = wxSystemSettings::GetFont ( wxSYS_DEFAULT_GUI_FONT );

#ifdef __WXMAC__
	wxFont font ( 12, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL );
	wxFont bold ( 12, wxFONTFAMILY_TELETYPE, wxNORMAL, wxFONTWEIGHT_BOLD );
#else
	wxFont font ( defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL );
	wxFont bold ( defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxFONTWEIGHT_BOLD );
#endif

	for (int i=0; i<=wxSCI_STYLE_DEFAULT; i++) {
		m_outputText->StyleSetBackground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		m_outputText->StyleSetForeground(i, *wxBLACK);
	}

	m_outputText->StyleSetForeground ( wxSCI_LEX_GCC_MAKE_ENTER, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) );
	m_outputText->StyleSetBackground ( wxSCI_LEX_GCC_MAKE_ENTER, wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	m_outputText->StyleSetForeground ( wxSCI_LEX_GCC_MAKE_LEAVING, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) );
	m_outputText->StyleSetBackground ( wxSCI_LEX_GCC_MAKE_LEAVING, wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	m_outputText->StyleSetForeground ( wxSCI_LEX_GCC_OUTPUT, wxT("BLACK") );
	m_outputText->StyleSetBackground ( wxSCI_LEX_GCC_OUTPUT, wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW ) );

	m_outputText->StyleSetForeground ( wxSCI_LEX_GCC_BUILDING, wxT("BLACK") );
	m_outputText->StyleSetBackground ( wxSCI_LEX_GCC_BUILDING, wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW ) );

	m_outputText->StyleSetForeground ( wxSCI_LEX_GCC_WARNING, wxT("BROWN") );
	m_outputText->StyleSetBackground ( wxSCI_LEX_GCC_WARNING, wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	m_outputText->StyleSetForeground ( wxSCI_LEX_GCC_ERROR, wxT("RED") );
	m_outputText->StyleSetBackground ( wxSCI_LEX_GCC_ERROR, wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	m_outputText->StyleSetForeground ( wxSCI_LEX_GCC_FILE_LINK, wxT ( "BLUE" ) );


	m_outputText->StyleSetFont ( wxSCI_LEX_GCC_DEFAULT,      font );
	m_outputText->StyleSetFont ( wxSCI_LEX_GCC_OUTPUT,       font );
	m_outputText->StyleSetFont ( wxSCI_LEX_GCC_BUILDING,     bold );
	m_outputText->StyleSetFont ( wxSCI_LEX_GCC_FILE_LINK,    font );
	m_outputText->StyleSetFont ( wxSCI_LEX_GCC_WARNING,      bold );
	m_outputText->StyleSetFont ( wxSCI_LEX_GCC_ERROR,        bold );
	m_outputText->StyleSetFont ( wxSCI_LEX_GCC_MAKE_ENTER,   font );
	m_outputText->StyleSetFont ( wxSCI_LEX_GCC_MAKE_LEAVING, font );

	m_outputText->StyleSetHotSpot(wxSCI_LEX_GCC_WARNING, true);
	m_outputText->StyleSetHotSpot(wxSCI_LEX_GCC_ERROR, true);

	m_outputText->Colourise ( 0, m_outputText->GetLength() );


	// Connect events
	m_outputText->Connect(wxEVT_SCI_HOTSPOT_CLICK, wxScintillaEventHandler(CppCheckReportPage::OnOpenFile), NULL, this);
}

CppCheckReportPage::~CppCheckReportPage()
{
	m_outputText->Disconnect(wxEVT_SCI_HOTSPOT_CLICK, wxScintillaEventHandler(CppCheckReportPage::OnOpenFile), NULL, this);
}

void CppCheckReportPage::OnClearReportUI(wxUpdateUIEvent& event)
{
	event.Enable( m_outputText->GetLength() > 0 && !m_plugin->AnalysisInProgress());
}

void CppCheckReportPage::OnStopCheckingUI(wxUpdateUIEvent& event)
{
	event.Enable( m_plugin->AnalysisInProgress() );
}

void CppCheckReportPage::Clear()
{
	m_outputText->SetReadOnly(false);
	m_outputText->ClearAll();
	m_outputText->SetReadOnly(true);
	sErrorCount = 0;
}

void CppCheckReportPage::OnStopChecking(wxCommandEvent& event)
{
	m_plugin->StopAnalysis();
}

void CppCheckReportPage::OnClearReport(wxCommandEvent& event)
{
	Clear();
}

void CppCheckReportPage::AppendLine(const wxString& line)
{
	m_outputText->SetReadOnly(false);
	m_outputText->AppendText(line);
	m_outputText->SetReadOnly(true);

	m_outputText->ScrollToLine( m_outputText->GetLineCount() - 1);
}

// Lexing function
int CppCheckReportPage::ColorLine ( int, const char *text, size_t &start, size_t &len )
{
	wxString txt(text, wxConvUTF8);

	if(txt.StartsWith(wxT("Checking "))) {
		return wxSCI_LEX_GCC_MAKE_ENTER;
	}

	static wxRegEx gccPattern(wxT("^([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]*)(:)([a-zA-Z ]*)"));

	static int fileIndex     = 1;
	static int lineIndex     = 3;
	static int severityIndex = 4;

	if(gccPattern.Matches(txt)) {
		wxString severity = gccPattern.GetMatch(txt, severityIndex);
		gccPattern.GetMatch(&start, &len, fileIndex);

		sErrorCount ++;

		if(severity == wxT("error")) {
			return wxSCI_LEX_GCC_ERROR;
		} else {
			return wxSCI_LEX_GCC_WARNING;
		}
	}

	// file: line: severity
	return wxSCI_LEX_GCC_DEFAULT;
}

void CppCheckReportPage::OnOpenFile(wxScintillaEvent& e)
{
	static wxRegEx gccPattern(wxT("^([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]*)(:)([a-zA-Z ]*)"));
	static int fileIndex     = 1;
	static int lineIndex     = 3;

	wxString txt = m_outputText->GetLine( m_outputText->LineFromPosition(e.GetPosition()) );

	if(gccPattern.Matches(txt)) {
		wxString file       = gccPattern.GetMatch(txt, fileIndex);
		wxString lineNumber = gccPattern.GetMatch(txt, lineIndex);

		if(file.IsEmpty() == false) {
			long n (0);
			lineNumber.ToLong(&n);

			// Zero based line number
			if(n) n--;

			m_mgr->OpenFile(file, wxEmptyString, n);
		}
	}
}

size_t CppCheckReportPage::GetErrorCount() const
{
	return sErrorCount;
}

void CppCheckReportPage::PrintStatusMessage()
{
	wxString statusLine;

	statusLine << wxT("===== ");
	statusLine << wxT("cppcheck analysis ended. Found ") << sErrorCount << wxT(" possible errors ");
	statusLine << wxT("=====");

	AppendLine( statusLine );
}
