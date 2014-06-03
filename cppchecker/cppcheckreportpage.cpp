#include "cppcheckreportpage.h"
#include <wx/tokenzr.h>
#include "cppchecker.h"
#include "plugin.h"
#include <wx/regex.h>
#include <wx/log.h>
#include "event_notifier.h"
#include "lexer_configuration.h"
#include "editor_config.h"

static size_t sErrorCount (0);

CppCheckReportPage::CppCheckReportPage(wxWindow* parent, IManager* mgr, CppCheckPlugin* plugin)
    : CppCheckReportBasePage( parent )
    , m_mgr(mgr)
    , m_plugin(plugin)
{
    DoInitStyle();
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(CppCheckReportPage::OnThemeChanged), NULL, this);
}

CppCheckReportPage::~CppCheckReportPage()
{
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(CppCheckReportPage::OnThemeChanged), NULL, this);
}

void CppCheckReportPage::OnClearReportUI(wxUpdateUIEvent& event)
{
    event.Enable( m_stc->GetLength() > 0 && !m_plugin->AnalysisInProgress());
}

void CppCheckReportPage::OnStopCheckingUI(wxUpdateUIEvent& event)
{
    event.Enable( m_plugin->AnalysisInProgress() );
}

void CppCheckReportPage::Clear()
{
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
    
    m_mgr->SetStatusMessage("");
    sErrorCount = 0;
}

void CppCheckReportPage::OnStopChecking(wxCommandEvent& event)
{
    m_plugin->StopAnalysis();
    m_mgr->SetStatusMessage("CppCheck Stopped");
}

void CppCheckReportPage::OnClearReport(wxCommandEvent& event)
{
    Clear();
}

void CppCheckReportPage::AppendLine(const wxString& line)
{
    wxString tmpLine (line);

    // Locate status messages:
    // 6/7 files checked 85% done
    static wxRegEx reProgress(wxT("([0-9]+)/([0-9]+)( files checked )([0-9]+%)( done)"));
    static wxRegEx reFileName(wxT("(Checking )([a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)"));

    // Locate the progress messages and update our progress bar
    wxArrayString arrLines = wxStringTokenize(tmpLine, wxT("\n\r"), wxTOKEN_STRTOK);
    for(size_t i=0; i<arrLines.GetCount(); i++) {

        if(reProgress.Matches(arrLines.Item(i))) {

            // Get the current progress
            wxString currentLine = reProgress.GetMatch(arrLines.Item(i), 1);

            long fileNo(0);
            currentLine.ToLong(&fileNo);

        }

        if(reFileName.Matches(arrLines.Item(i))) {

            // Get the file name
            wxString filename = reFileName.GetMatch(arrLines.Item(i), 2);
            m_mgr->SetStatusMessage("CppCheck: checking file " + filename);
        }
    }

    // Remove progress messages from the printed output
    reProgress.ReplaceAll(&tmpLine, wxEmptyString);
    tmpLine.Replace(wxT("\r"), wxT(""));
    tmpLine.Replace(wxT("\n\n"), wxT("\n"));

    m_stc->SetReadOnly(false);
    m_stc->AppendText(tmpLine);
    m_stc->SetReadOnly(true);

    m_stc->ScrollToLine( m_stc->GetLineCount() - 1);
}

// Lexing function
//int CppCheckReportPage::ColorLine ( int, const char *text, size_t &start, size_t &len )
//{
//    wxString txt(text, wxConvUTF8);
//
//    if(txt.StartsWith(wxT("Checking "))) {
//        return wxSTC_LEX_GCC_MAKE_ENTER;
//    }
//
//    static wxRegEx gccPattern(wxT("^([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]*)(:)([a-zA-Z ]*)"));
//
//    static int fileIndex     = 1;
////	static int lineIndex     = 3;
//    static int severityIndex = 4;
//
//    if(gccPattern.Matches(txt)) {
//        wxString severity = gccPattern.GetMatch(txt, severityIndex);
//        gccPattern.GetMatch(&start, &len, fileIndex);
//
//        sErrorCount ++;
//
//        if(severity == wxT("error")) {
//            return wxSTC_LEX_GCC_ERROR;
//        } else {
//            return wxSTC_LEX_GCC_WARNING;
//        }
//    }
//
//    // file: line: severity
//    return wxSTC_LEX_GCC_DEFAULT;
//}
//
void CppCheckReportPage::OnOpenFile(wxStyledTextEvent& e)
{
    static wxRegEx gccPattern(wxT("^([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]*)(:)([a-zA-Z ]*)"));
    static int fileIndex     = 1;
    static int lineIndex     = 3;

    wxString txt = m_stc->GetLine( m_stc->LineFromPosition(e.GetPosition()) );

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
    statusLine << _("cppcheck analysis ended. Found ") << sErrorCount << _(" possible errors");
    statusLine << wxT("=====");

    AppendLine( statusLine );
    SetMessage( _("Done") );
}

void CppCheckReportPage::SetGaugeRange(int range)
{
    wxUnusedVar( range );
}

void CppCheckReportPage::SetMessage(const wxString& msg)
{
    m_mgr->SetStatusMessage( msg );
}

void CppCheckReportPage::DoInitStyle()
{
    m_stc->SetReadOnly(true);
	LexerConfPtr config = EditorConfigST::Get()->GetLexer("text");
	if ( config ) {
		config->Apply( m_stc, true );
		m_stc->HideSelection(true);
		
	} else {
		// Initialize the output text style
		m_stc->SetLexer(wxSTC_LEX_NULL);
		m_stc->StyleClearAll();
		
		m_stc->HideSelection(true);
		
		// Use font
		for (int i=0; i<=wxSTC_STYLE_DEFAULT; i++) {
			wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
			defFont.SetFamily(wxFONTFAMILY_TELETYPE);
			m_stc->StyleSetBackground(i, DrawingUtils::GetOutputPaneBgColour());
			m_stc->StyleSetForeground(i, DrawingUtils::GetOutputPaneFgColour());
			m_stc->StyleSetFont(i, defFont);
		}
	}
}

void CppCheckReportPage::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    DoInitStyle();
}

void CppCheckReportPage::OnStyleNeeded(wxStyledTextEvent& event)
{
}
