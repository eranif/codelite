#include "cppcheckreportpage.h"
#include "cppchecker.h"
#include "plugin.h"
#include <wx/log.h>

const wxEventType wxEVT_CPPCHECK_ASK_STOP = wxNewEventType();

CppCheckReportPage::CppCheckReportPage(wxWindow* parent, IManager* mgr, CppCheckPlugin* plugin)
		: CppCheckReportBasePage( parent )
		, m_mgr(mgr)
		, m_plugin(plugin)
{
	m_outputText->SetReadOnly(true);
}

void CppCheckReportPage::OnClearReportUI(wxUpdateUIEvent& event)
{
	event.Enable( m_outputText->GetLength() > 0 );
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
}

void CppCheckReportPage::OnStopChecking(wxCommandEvent& event)
{
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
}
