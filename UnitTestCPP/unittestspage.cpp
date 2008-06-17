#include "globals.h"
#include "unittestspage.h"
#include "imanager.h"

UnitTestsPage::UnitTestsPage(wxWindow* parent, const TestSummary& summary, IManager *mgr )
: UnitTestsBasePage( parent )
, m_summary( summary )
, m_mgr(mgr)
{
	m_progressPassed->SetMaxRange((size_t)m_summary.totalTests);
	m_progressFailed->SetMaxRange((size_t)m_summary.totalTests);
	m_progressFailed->SetFillCol(wxT("RED"));
	m_progressPassed->SetFillCol(wxT("PALE GREEN"));
	
	wxString msg;
	msg << m_summary.totalTests;
	m_staticTextTotalTests->SetLabel(msg);
	
	msg.clear();
	msg << m_summary.errorCount;
	m_staticTextFailTestsNum->SetLabel(msg);
	
	msg.clear();
	msg << m_summary.totalTests - m_summary.errorCount;
	m_staticTextSuccessTestsNum->SetLabel(msg);
	
	m_listCtrlErrors->InsertColumn(0, wxT("File"));
	m_listCtrlErrors->InsertColumn(1, wxT("Line"));
	m_listCtrlErrors->InsertColumn(2, wxT("Description"));
	
	for(size_t i=0; i<m_summary.errorLines.size(); i++){
		ErrorLineInfo info = m_summary.errorLines.at(i);
		long row = AppendListCtrlRow(m_listCtrlErrors);
		SetColumnText(m_listCtrlErrors, row, 0, info.file);
		SetColumnText(m_listCtrlErrors, row, 1, info.line);
		SetColumnText(m_listCtrlErrors, row, 2, info.description);
	}
	m_listCtrlErrors->SetColumnWidth(0, 200);
	m_listCtrlErrors->SetColumnWidth(1, 100);
	m_listCtrlErrors->SetColumnWidth(2, wxLIST_AUTOSIZE);
}

void UnitTestsPage::UpdateFailedBar(size_t amount, const wxString& msg)
{
	m_progressFailed->Update(amount, msg);
}

void UnitTestsPage::UpdatePassedBar(size_t amount, const wxString& msg)
{
	m_progressPassed->Update(amount, msg);
}

void UnitTestsPage::OnItemActivated(wxListEvent& e)
{
	wxString file = GetColumnText(m_listCtrlErrors, e.m_itemIndex, 0);
	wxString line = GetColumnText(m_listCtrlErrors, e.m_itemIndex, 1);
	
	long l;
	line.ToLong(&l);
	
	// convert the file to absolute path
	wxFileName fn(file);
	fn.MakeAbsolute();
	m_mgr->OpenFile(fn.GetFullPath(), wxEmptyString, l-1);
}
