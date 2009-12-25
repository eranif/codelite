#ifndef SVNBLAMEDIALOG_H
#define SVNBLAMEDIALOG_H

#include <wx/dialog.h>

class wxScintilla;
class SvnBlameDialog : public wxDialog {
public:
	wxString     m_content;
	wxScintilla *m_textCtrl;

protected:
	void SetText();

public:
	SvnBlameDialog(wxWindow *window, const wxString &content);
	virtual ~SvnBlameDialog();

};

#endif // SVNBLAMEDIALOG_H
