#ifndef SVNBLAMEDIALOG_H
#define SVNBLAMEDIALOG_H

#include <wx/dialog.h>

class SvnBlameEditor;

class SvnBlameDialog : public wxDialog {
public:
	SvnBlameEditor *m_textCtrl;

public:
	SvnBlameDialog(wxWindow *window, const wxString &content);
	virtual ~SvnBlameDialog();
};

#endif // SVNBLAMEDIALOG_H
