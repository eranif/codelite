#ifndef CODEDESIGNERITEMDLG_H
#define CODEDESIGNERITEMDLG_H

#include "codedesigneritembasedlg.h" // Base class: CodeDesignerItemBaseDlg
class IManager;

enum CDItemType
{
	cdtStateChart,
	cdtHierarchicalStateChart,
	cdtClassDiagram
};

struct CDItemInfo
{
	bool m_UseMain;
	bool m_GenerateCode;
	wxString m_PackageName;
	wxString m_DiagramName;
	wxString m_TreeFolderName;
	wxString m_Language;
	CDItemType m_Type;
};

class CodeDesignerItemDlg : public CodeDesignerItemBaseDlg
{
public:
	CodeDesignerItemDlg(wxWindow *parent, IManager *mgr, bool allowmain);
	virtual ~CodeDesignerItemDlg();
	
	void SetCDInfo(const CDItemInfo& CDInfo) {this->m_CDInfo = CDInfo;}
	const CDItemInfo& GetCDInfo() const {return m_CDInfo;}

protected:
	CDItemInfo m_CDInfo;
	bool m_AllowMain;
	IManager *m_mgr;
	
	virtual void OnCancel(wxCommandEvent& event);
	virtual void OnInit(wxInitDialogEvent& event);
	virtual void OnOk(wxCommandEvent& event);
	virtual void OnUpdateMain(wxUpdateUIEvent& event);
	virtual void OnBrowse(wxCommandEvent& event);

};

#endif // CODEDESIGNERITEMDLG_H
