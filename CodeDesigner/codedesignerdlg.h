#ifndef CODEDESIGNERDLG_H
#define CODEDESIGNERDLG_H

#include "codedesignersettingsbasedlg.h" // Base class: CodeDesignerBaseDlg

class CodeDesignerDlg : public CodeDesignerBaseDlg {

public:
	CodeDesignerDlg(wxWindow *parent);
	virtual ~CodeDesignerDlg();
	
	void SetPath(const wxString& Path) {this->m_Path = Path;}
	void SetPort(const wxString& Port) {this->m_Port = Port;}
	const wxString& GetPath() const {return m_Path;}
	const wxString& GetPort() const {return m_Port;}

protected:
	wxString m_Path;
	wxString m_Port;

	virtual void OnButtonBrowse(wxCommandEvent& event);
	virtual void OnButtonCancel(wxCommandEvent& event);
	virtual void OnButtonOK(wxCommandEvent& event);
	virtual void OnInit(wxInitDialogEvent& event);
};

#endif // CODEDESIGNERDLG_H
