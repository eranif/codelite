#include "quickwatchdlg.h"
#include "localvarstree.h"
#include "manager.h"

QuickWatchDlg::QuickWatchDlg( wxWindow* parent, const wxString &expression,  TreeNode<wxString, NodeData> *tree )
:
QuickWatchBaseDlg( parent )
{
	Init(expression, tree);
}

void QuickWatchDlg::OnButtonClose( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void QuickWatchDlg::Init(const wxString &expression, TreeNode<wxString, NodeData> *tree)
{
	m_localVarsTree->BuildTree(tree);
	m_textCtrl1->SetValue(expression);
}

void QuickWatchDlg::OnReEvaluate(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString expression = m_textCtrl1->GetValue();
	if(expression.IsEmpty()){
		return;
	}

	// Call the manager to update the expression
	ManagerST::Get()->DbgQuickWatch(expression);
}
