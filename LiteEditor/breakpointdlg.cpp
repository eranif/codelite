#include "breakpointdlg.h"
#include "debuggermanager.h"
#include "manager.h"
#include "macros.h"

BreakpointDlg::BreakpointDlg( wxWindow* parent )
:
BreakpointDlgBase( parent )
{
	Initialize();
	ConnectButton(m_buttonDelete, BreakpointDlg::OnDelete);
	ConnectButton(m_buttonDeleteAll, BreakpointDlg::OnDeleteAll);

}

void BreakpointDlg::Initialize()
{
	m_listBreakpoints->Clear();
	m_listBreakpoints->Freeze();
	
	std::vector<BreakpointInfo> bps, pbps;
	DebuggerMgr::Get().GetBreakpoints(bps);
	std::vector<BreakpointInfo>::iterator iter = bps.begin();
	for(; iter != bps.end(); iter++)
	{
		wxString entry;
		entry << iter->file << wxT("|") << iter->lineno;
		m_listBreakpoints->Append(entry);
	}
	iter = pbps.begin();
	for(; iter != pbps.end(); iter++)
	{
		wxString entry;
		entry << iter->file << wxT("|") << iter->lineno;
		m_listBreakpoints->Append(entry);
	}
	if(m_listBreakpoints->GetCount() > 0){
		m_listBreakpoints->Select(0);
	}

	m_listBreakpoints->Thaw();
}

void BreakpointDlg::OnDelete(wxCommandEvent &e)
{
	wxUnusedVar(e);
	if(m_listBreakpoints->GetCount() > 0){
		int where = m_listBreakpoints->GetSelection();
		if(where != wxNOT_FOUND){
			wxString sel = m_listBreakpoints->GetStringSelection();
			//get the file name
			wxString fileName = sel.BeforeFirst(wxT('|'));
			wxString strLine  = sel.AfterFirst(wxT('|'));
			long lineno;
			strLine.ToLong(&lineno);

			BreakpointInfo bp;
			bp.file = fileName;
			bp.lineno = lineno;
			ManagerST::Get()->DbgDeleteBreakpoint(bp);
			m_listBreakpoints->Delete((unsigned int)where);
		}
	}
}

void BreakpointDlg::OnDeleteAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
	ManagerST::Get()->DbgDeleteAllBreakpoints();
	m_listBreakpoints->Clear();
}
