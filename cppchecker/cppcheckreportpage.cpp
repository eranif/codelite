#include "cppcheckreportpage.h"
#include "drawingutils.h"
#include "cppchecker.h"
#include "plugin.h"
#include <wx/log.h>

const wxEventType wxEVT_CPPCHECK_ASK_STOP = wxNewEventType();

CppCheckReportPage::CppCheckReportPage(wxWindow* parent, IManager* mgr, CppCheckPlugin* plugin)
: CppCheckReportBasePage( parent )
, m_mgr(mgr)
, m_plugin(plugin)
{
	m_listCtrlReport->InsertColumn(0, wxT("ID"));
	m_listCtrlReport->InsertColumn(1, wxT("Line"));
	m_listCtrlReport->InsertColumn(2, wxT("Severity"));
	m_listCtrlReport->InsertColumn(3, wxT("Description"));

	m_listCtrlReport->SetColumnWidth(0, 100);
	m_listCtrlReport->SetColumnWidth(1, 100);
	m_listCtrlReport->SetColumnWidth(2, 150);
	m_listCtrlReport->SetColumnWidth(3, 400);

	m_filelist->SetReportPage( this );
#ifdef __WXMSW__
	m_progress->SetFillCol(DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 5 ));
#else
	m_progress->SetFillCol(DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 3 ));
#endif
}

/**
 * Manage selection of file : show errors from the selected file
 */
void CppCheckReportPage::FileSelected( const wxString &filename )
{
	CppCheckResults::const_iterator itFileRes = m_results.find(filename);
	if (itFileRes == m_results.end()) {
		wxLogError(_("CppChecker error : file %s not found !"), filename.c_str());
		return;
	}

	Freeze();

	m_listCtrlReport->DeleteAllItems();
	const CppCheckFileResults& fileRes = itFileRes->second;
	for (CppCheckFileResults::const_iterator itRes = fileRes.begin(); itRes != fileRes.end(); ++itRes) {
		const CppCheckResult& res = *itRes;
		long idx = m_listCtrlReport->InsertItem(m_listCtrlReport->GetItemCount(), res.id);
		m_listCtrlReport->SetItem(idx, 1, wxString::Format(wxT("%d"), res.lineno));
		m_listCtrlReport->SetItem(idx, 2, res.severity);
		m_listCtrlReport->SetItem(idx, 3, res.msg);
		m_listCtrlReport->SetItemPtrData(idx, (wxUIntPtr)(&res));
	}

	Thaw();
}

/**
 *
 */
void CppCheckReportPage::OnListCtrlItemActivated( wxListEvent& event )
{
	const CppCheckResult* res = (const CppCheckResult*) event.GetItem().GetData();
	if (res) {
		m_mgr->OpenFile(res->filename, wxEmptyString, res->lineno - 1);
	}
}

/**
 * Clear actual report
 */
void CppCheckReportPage::OnClearReport( wxCommandEvent& event )
{
	DoClearReport();
}

/**
 *
 */
void CppCheckReportPage::OnStopChecking( wxCommandEvent& event )
{
	SetStatus(_("Stopping current analysis..."));
	m_plugin->StopAnalysis();
	SetStatus(_("Ready"));
}

/**
 * Add results to the actual ones
 */
void CppCheckReportPage::AddResults(const CppCheckResults* results)
{
	if (!results) {
		return;
	}

	// Append results to existing ones
	for (CppCheckResults::const_iterator itFileRes = results->begin(); itFileRes != results->end(); ++ itFileRes) {
		m_results[itFileRes->first].insert(m_results[itFileRes->first].end(), itFileRes->second.begin(), itFileRes->second.end());
	}

	for (CppCheckResults::const_iterator itFileRes = results->begin(); itFileRes != results->end(); ++ itFileRes) {
		m_filelist->AddFile( itFileRes->first );
	}

}

void CppCheckReportPage::SetStatus(const wxString& status)
{
	m_progress->Update(m_plugin->GetProgress(), status);
}

void CppCheckReportPage::OnClearReportUI(wxUpdateUIEvent& event)
{
	event.Enable( m_filelist->GetLength() > 0 );
}

void CppCheckReportPage::OnStopCheckingUI(wxUpdateUIEvent& event)
{
	event.Enable( m_plugin->AnalysisInProgress() );
}

void CppCheckReportPage::Clear()
{
	DoClearReport();
}

void CppCheckReportPage::DoClearReport()
{
	m_results.clear();
	m_filelist->Clear();
	m_listCtrlReport->DeleteAllItems();
	m_progress->Clear();
}


void CppCheckReportPage::OnSkipFile(wxCommandEvent& event)
{
	wxUnusedVar( event  );
	m_plugin->SkipCurrentFile();
}

void CppCheckReportPage::OnSkipFileUI(wxUpdateUIEvent& event)
{
	event.Enable( m_plugin->AnalysisInProgress() );
}

