#include "compilerpatternspage.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "globals.h"
#include "windowattrmanager.h"

CompilerPatternsPage::CompilerPatternsPage( wxWindow* parent, const wxString &cmpname )
		: CompilerPatternsBase( parent )
		, m_cmpname(cmpname)
{
	m_listErrPatterns->InsertColumn(COL_PATTERN,  _("Pattern"));
	m_listErrPatterns->InsertColumn(COL_LINE_IDX, _("Line number index"));
	m_listErrPatterns->InsertColumn(COL_FILE_IDX, _("File name index"));

	m_listWarnPatterns->InsertColumn(COL_PATTERN,  _("Pattern"));
	m_listWarnPatterns->InsertColumn(COL_LINE_IDX, _("Line number index"));
	m_listWarnPatterns->InsertColumn(COL_FILE_IDX, _("File name index"));

	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);

	const Compiler::CmpListInfoPattern& errPatterns = cmp->GetErrPatterns();
	Compiler::CmpListInfoPattern::const_iterator itPattern;
	for (itPattern = errPatterns.begin(); itPattern != errPatterns.end(); ++itPattern) {
		long item = AppendListCtrlRow(m_listErrPatterns);
		SetColumnText(m_listErrPatterns, item, COL_PATTERN,  itPattern->pattern);
		SetColumnText(m_listErrPatterns, item, COL_LINE_IDX, itPattern->lineNumberIndex);
		SetColumnText(m_listErrPatterns, item, COL_FILE_IDX, itPattern->fileNameIndex);
	}

	const Compiler::CmpListInfoPattern& warnPatterns = cmp->GetWarnPatterns();
	for (itPattern = warnPatterns.begin(); itPattern != warnPatterns.end(); ++itPattern) {
		long item = AppendListCtrlRow(m_listWarnPatterns);
		SetColumnText(m_listWarnPatterns, item, COL_PATTERN,  itPattern->pattern);
		SetColumnText(m_listWarnPatterns, item, COL_LINE_IDX, itPattern->lineNumberIndex);
		SetColumnText(m_listWarnPatterns, item, COL_FILE_IDX, itPattern->fileNameIndex);
	}

	m_listWarnPatterns->SetColumnWidth(COL_PATTERN , 200);
	m_listWarnPatterns->SetColumnWidth(COL_LINE_IDX, 50);
	m_listWarnPatterns->SetColumnWidth(COL_FILE_IDX, 50);

	m_listErrPatterns->SetColumnWidth(COL_PATTERN , 200);
	m_listErrPatterns->SetColumnWidth(COL_LINE_IDX, 50);
	m_listErrPatterns->SetColumnWidth(COL_FILE_IDX, 50);
}

void CompilerPatternsPage::Save(CompilerPtr cmp)
{
	Compiler::CmpListInfoPattern errPatterns;
	for (int i = 0; i < m_listErrPatterns->GetItemCount(); ++i) {
		Compiler::CmpInfoPattern infoPattern;
		infoPattern.pattern         = GetColumnText(m_listErrPatterns, i, COL_PATTERN);
		infoPattern.lineNumberIndex = GetColumnText(m_listErrPatterns, i, COL_LINE_IDX);
		infoPattern.fileNameIndex   = GetColumnText(m_listErrPatterns, i, COL_FILE_IDX);
		errPatterns.push_back(infoPattern);
	}
	cmp->SetErrPatterns(errPatterns);

	Compiler::CmpListInfoPattern warnPatterns;
	for (int i = 0; i < m_listWarnPatterns->GetItemCount(); ++i) {
		Compiler::CmpInfoPattern infoPattern;
		infoPattern.pattern         = GetColumnText(m_listWarnPatterns, i, COL_PATTERN);
		infoPattern.lineNumberIndex = GetColumnText(m_listWarnPatterns, i, COL_LINE_IDX);
		infoPattern.fileNameIndex   = GetColumnText(m_listWarnPatterns, i, COL_FILE_IDX);
		warnPatterns.push_back(infoPattern);
	}
	cmp->SetWarnPatterns(warnPatterns);
}

void CompilerPatternsPage::OnBtnAddErrPattern( wxCommandEvent& event )
{
	CompilerPatternDlg dlg(this, _("Add compiler error pattern"));
	if (dlg.ShowModal() == wxID_OK) {
		long item = m_listErrPatterns->InsertItem(m_listErrPatterns->GetItemCount(), dlg.m_pattern);
		m_listErrPatterns->SetItem(item, COL_LINE_IDX, dlg.m_lineIdx);
		m_listErrPatterns->SetItem(item, COL_FILE_IDX, dlg.m_fileIdx);
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
	wxString pattern = GetColumnText(m_listErrPatterns, item, COL_PATTERN);
	wxString fileIdx = GetColumnText(m_listErrPatterns, item, COL_FILE_IDX);
	wxString lineIdx = GetColumnText(m_listErrPatterns, item, COL_LINE_IDX);
	CompilerPatternDlg dlg(this, _("Update compiler error pattern"));
	dlg.SetPattern(pattern, lineIdx, fileIdx);
	if (dlg.ShowModal() == wxID_OK) {
		SetColumnText(m_listErrPatterns, item, COL_PATTERN,  dlg.m_pattern);
		SetColumnText(m_listErrPatterns, item, COL_FILE_IDX, dlg.m_fileIdx);
		SetColumnText(m_listErrPatterns, item, COL_LINE_IDX, dlg.m_lineIdx);
	}
}

void CompilerPatternsPage::OnBtnAddWarnPattern( wxCommandEvent& event )
{
	CompilerPatternDlg dlg(this, _("Add compiler warning pattern"));
	if (dlg.ShowModal() == wxID_OK) {
		long item = m_listWarnPatterns->InsertItem(m_listWarnPatterns->GetItemCount(), dlg.m_pattern);
		m_listWarnPatterns->SetItem(item, COL_LINE_IDX, dlg.m_lineIdx);
		m_listWarnPatterns->SetItem(item, COL_FILE_IDX, dlg.m_fileIdx);
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
	wxString pattern = GetColumnText(m_listWarnPatterns, item, COL_PATTERN);
	wxString fileIdx = GetColumnText(m_listWarnPatterns, item, COL_FILE_IDX);
	wxString lineIdx = GetColumnText(m_listWarnPatterns, item, COL_LINE_IDX);
	CompilerPatternDlg dlg(this, _("Update compiler warning pattern"));
	dlg.SetPattern(pattern, lineIdx, fileIdx);
	if (dlg.ShowModal() == wxID_OK) {
		SetColumnText(m_listWarnPatterns, item, COL_PATTERN,  dlg.m_pattern);
		SetColumnText(m_listWarnPatterns, item, COL_FILE_IDX, dlg.m_fileIdx);
		SetColumnText(m_listWarnPatterns, item, COL_LINE_IDX, dlg.m_lineIdx);
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

