#ifndef __svntab__
#define __svntab__

#include "wx/panel.h"

class wxScintilla;

class SvnTab : public wxPanel {
	wxScintilla *m_sci;
public:
	SvnTab(wxWindow *parent);
	virtual ~SvnTab();
	
	void Initialize();
	void AppendText(const wxString &text);
	void Clear();
};
#endif // __svntab__
