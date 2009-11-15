#include "compilerpatternspage.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "globals.h"
#include "windowattrmanager.h"

CompilerPatternsPage::CompilerPatternsPage( wxWindow* parent, const wxString &cmpname )
		: CompilerPatternsBase( parent )
		, m_cmpname(cmpname)
{
	m_listErrPatterns->InsertColumn(0,  _("Pattern"));
	m_listErrPatterns->InsertColumn(1,  _("File name index"));
	m_listErrPatterns->InsertColumn(2,  _("Line number index"));

	m_listWarnPatterns->InsertColumn(0,  _("Pattern"));
	m_listWarnPatterns->InsertColumn(1,  _("File name index"));
	m_listWarnPatterns->InsertColumn(2,  _("Line number index"));

	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);

	const Compiler::CmpListInfoPattern& errPatterns = cmp->GetErrPatterns();
	Compiler::CmpListInfoPattern::const_iterator itPattern;
	for (itPattern = errPatterns.begin(); itPattern != errPatterns.end(); ++itPattern) {
		long item = AppendListCtrlRow(m_listErrPatterns);
		SetColumnText(m_listErrPatterns, item, 0,  itPattern->pattern);
		SetColumnText(m_listErrPatterns, item, 1, itPattern->fileNameIndex);
		SetColumnText(m_listErrPatterns, item, 2, itPattern->lineNumberIndex);
	}

	const Compiler::CmpListInfoPattern& warnPatterns = cmp->GetWarnPatterns();
	for (itPattern = warnPatterns.begin(); itPattern != warnPatterns.end(); ++itPattern) {
		long item = AppendListCtrlRow(m_listWarnPatterns);
		SetColumnText(m_listWarnPatterns, item, 0,  itPattern->pattern);
		SetColumnText(m_listWarnPatterns, item, 1, itPattern->fileNameIndex);
		SetColumnText(m_listWarnPatterns, item, 2, itPattern->lineNumberIndex);
	}

	m_listWarnPatterns->SetColumnWidth(0, 200);
	m_listWarnPatterns->SetColumnWidth(1, 50);
	m_listWarnPatterns->SetColumnWidth(2, 50);

	m_listErrPatterns->SetColumnWidth(0, 200);
	m_listErrPatterns->SetColumnWidth(1, 50);
	m_listErrPatterns->SetColumnWidth(2, 50);
}

void CompilerPatternsPage::Save(CompilerPtr cmp)
{
	Compiler::CmpListInfoPattern errPatterns;
	for (int i = 0; i < m_listErrPatterns->GetItemCount(); ++i) {
		Compiler::CmpInfoPattern infoPattern;
		infoPattern.pattern         = GetColumnText(m_listErrPatterns, i, 0);
		infoPattern.fileNameIndex   = GetColumnText(m_listErrPatterns, i, 1);
		infoPattern.lineNumberIndex = GetColumnText(m_listErrPatterns, i, 2);
		errPatterns.push_back(infoPattern);
	}
	cmp->SetErrPatterns(errPatterns);

	Compiler::CmpListInfoPattern warnPatterns;
	for (int i = 0; i < m_listWarnPatterns->GetItemCount(); ++i) {
		Compiler::CmpInfoPattern infoPattern;
		infoPattern.pattern         = GetColumnText(m_listWarnPatterns, i, 0);
		infoPattern.fileNameIndex   = GetColumnText(m_listWarnPatterns, i, 1);
		infoPattern.lineNumberIndex = GetColumnText(m_listWarnPatterns, i, 2);
		warnPatterns.push_back(infoPattern);
	}
	cmp->SetWarnPatterns(warnPatterns);
}

void CompilerPatternsPage::OnBtnAddErrPattern( wxCommandEvent& event )
{
	CompilerPatternDlg dlg(this, _("Add compiler error pattern"));
	if (dlg.ShowModal() == wxID_OK) {
		long item = AppendListCtrlRow( m_listErrPatterns );
		SetColumnText(m_listErrPatterns, item, 0, dlg.m_pattern);
		SetColumnText(m_listErrPatterns, item, 1, dlg.m_fileIdx);
		SetColumnText(m_listErrPatterns, item, 2, dlg.m_lineIdx);
	}
}

void CompilerPatternsPage::OnBtnDelErrPattern( wxCommandEvent& event )
{
	int sel =  m_listErrPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (sel == wxNOT_FOUND) {
		return;
	}
	m_listErrPatterns->DeleteItem(sel);
}

void CompilerPatternsPage::OnBtnUpdateErrPattern( wxCommandEvent& event )
{
	int sel = m_listErrPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (sel == wxNOT_FOUND) {
		return;
	}
	DoUpdateErrPattern(sel);
}

void CompilerPatternsPage::OnErrItemActivated( wxListEvent& event )
{
	wxLogMessage(wxT("CompilerPatternsPage::OnErrItemActivated(%d)"), event.GetIndex());
	DoUpdateErrPattern(event.GetIndex());
}

void CompilerPatternsPage::DoUpdateErrPattern(long item)
{
	wxString pattern = GetColumnText(m_listErrPatterns, item, 0);
	wxString fileIdx = GetColumnText(m_listErrPatterns, item, 1);
	wxString lineIdx = GetColumnText(m_listErrPatterns, item, 2);
	CompilerPatternDlg dlg(this, _("Update compiler error pattern"));
	dlg.SetPattern(pattern, lineIdx, fileIdx);
	if (dlg.ShowModal() == wxID_OK) {
		SetColumnText(m_listErrPatterns, item, 0,  dlg.m_pattern);
		SetColumnText(m_listErrPatterns, item, 1, dlg.m_fileIdx);
		SetColumnText(m_listErrPatterns, item, 2, dlg.m_lineIdx);
	}
}

void CompilerPatternsPage::OnBtnAddWarnPattern( wxCommandEvent& event )
{
	CompilerPatternDlg dlg(this, _("Add compiler warning pattern"));
	if (dlg.ShowModal() == wxID_OK) {
		long item = AppendListCtrlRow( m_listWarnPatterns );
		SetColumnText(m_listWarnPatterns, item, 0, dlg.m_pattern);
		SetColumnText(m_listWarnPatterns, item, 1, dlg.m_fileIdx);
		SetColumnText(m_listWarnPatterns, item, 2, dlg.m_lineIdx);
	}
}

void CompilerPatternsPage::OnBtnDelWarnPattern( wxCommandEvent& event )
{
	int sel = m_listWarnPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (sel == wxNOT_FOUND) {
		return;
	}
	m_listWarnPatterns->DeleteItem(sel);
}

void CompilerPatternsPage::OnBtnUpdateWarnPattern( wxCommandEvent& event )
{
	int sel = m_listWarnPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (sel == wxNOT_FOUND) {
		return;
	}
	DoUpdateWarnPattern(sel);
}

void CompilerPatternsPage::OnWarnItemActivated( wxListEvent& event )
{
	DoUpdateWarnPattern(event.GetIndex());
}

void CompilerPatternsPage::DoUpdateWarnPattern(long item)
{
	wxString pattern = GetColumnText(m_listWarnPatterns, item, 0);
	wxString fileIdx = GetColumnText(m_listWarnPatterns, item, 1);
	wxString lineIdx = GetColumnText(m_listWarnPatterns, item, 2);
	CompilerPatternDlg dlg(this, _("Update compiler warning pattern"));
	dlg.SetPattern(pattern, lineIdx, fileIdx);
	if (dlg.ShowModal() == wxID_OK) {
		SetColumnText(m_listWarnPatterns, item, 0,  dlg.m_pattern);
		SetColumnText(m_listWarnPatterns, item, 1, dlg.m_fileIdx);
		SetColumnText(m_listWarnPatterns, item, 2, dlg.m_lineIdx);
	}
}

void CompilerPatternsPage::OnErrorPatternSelectedUI(wxUpdateUIEvent& event)
{
	int sel = m_listErrPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	event.Enable(sel != wxNOT_FOUND);
}

void CompilerPatternsPage::OnWarningPatternSelectedUI(wxUpdateUIEvent& event)
{
	int sel = m_listWarnPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	event.Enable(sel != wxNOT_FOUND);
}

CompilerPatternDlg::CompilerPatternDlg(wxWindow* parent, const wxString& title)
: CompilerPatternDlgBase(parent, wxID_ANY, title)
{
	WindowAttrManager::Load(this, wxT("CompilerPatternDlg"), NULL);
}

CompilerPatternDlg::~CompilerPatternDlg()
{
	WindowAttrManager::Save(this, wxT("CompilerPatternDlg"), NULL);
}

void CompilerPatternDlg::SetPattern(const wxString& pattern, const wxString& lineIdx, const wxString& fileIdx)
{
	m_pattern = pattern;
	m_lineIdx = lineIdx;
	m_fileIdx = fileIdx;
	TransferDataToWindow();
}

void CompilerPatternDlg::OnSubmit( wxCommandEvent& event )
{
	if (TransferDataFromWindow()) {
		if( m_pattern.Trim().IsEmpty() || m_lineIdx.Trim().IsEmpty() || m_fileIdx.Trim().IsEmpty() ) {
			wxMessageBox(_("Please fill all the fields"), wxT("CodeLite"), wxOK|wxICON_INFORMATION, this);
			return;
		}
		EndModal(wxID_OK);
	}
}

