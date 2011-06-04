#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <wx/utils.h> 

#include "GUI.h" // Base class: _LogDialog

/*! \brief Dialog for showing logs. */
class LogDialog : public _LogDialog {

public:
	LogDialog(wxWindow* parent);
	virtual ~LogDialog();

	virtual void OnCloseClick(wxCommandEvent& event);
	virtual void OnCloseUI(wxUpdateUIEvent& event);
	
	/*! \brief Enable close button. */
	void EnableClose(bool enable){ m_canClose = enable; }
	/*! \brief Clear text field */
	void Clear();
	/*! \brief Add text line */
	void AppendText(const wxString& txt);
	/*! \brief Add comment line */
	void AppendComment(const wxString& txt);
	/*! \brief Add separator. */
	void AppendSeparator();
	
protected:
	bool m_canClose;
	wxString m_text;
};

#endif // LOGDIALOG_H
