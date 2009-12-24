#include "cppcheckreportpage.h"
#include "globals.h"
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
	std::vector<CppCheckResult> *results = m_results.GetResultsForFile(filename);
	if (results->empty()) {
		wxLogError(_("CppChecker error : file %s not found !"), filename.c_str());
		return;
	}

	Freeze();

	ClearListCtrl();

	for (size_t i=0; i<results->size(); i++) {
		CppCheckResult res = results->at(i);
		long idx = AppendListCtrlRow(m_listCtrlReport);
		SetColumnText(m_listCtrlReport, idx, 0, res.id);
		SetColumnText(m_listCtrlReport, idx, 1, wxString::Format(wxT("%d"), res.lineno));
		SetColumnText(m_listCtrlReport, idx, 2, res.severity);
		SetColumnText(m_listCtrlReport, idx, 3, res.msg);
		m_listCtrlReport->SetItemPtrData(idx, (wxUIntPtr)(new CppCheckResult(res)));
	}

	Thaw();
}

/**
 *
 */
void CppCheckReportPage::OnListCtrlItemActivated( wxListEvent& event )
{
	int item = event.m_itemIndex;
	if(item == wxNOT_FOUND)
		return;

	const CppCheckResult* res = (const CppCheckResult*) m_listCtrlReport->GetItemData(item);
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
void CppCheckReportPage::AddResults(const wxString &xmlOutput)
{
	m_results.AddResultsForFile(xmlOutput);

	wxArrayString files = m_results.GetFiles();
	for(size_t i=0; i<files.GetCount(); i++)
		m_filelist->AddFile( files.Item(i) );
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
	m_results.ClearAll();
	m_filelist->Clear();
	ClearListCtrl();
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

void CppCheckReportPage::ClearListCtrl()
{
	for(size_t i=0; i<m_listCtrlReport->GetItemCount(); i++) {
		CppCheckResult* data = (CppCheckResult*)m_listCtrlReport->GetItemData(i);
		if(data) {
			delete data;
		}
	}
	m_listCtrlReport->DeleteAllItems();
}
