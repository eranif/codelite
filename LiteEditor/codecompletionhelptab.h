#ifndef CODECOMPLETIONHELPTAB_H
#define CODECOMPLETIONHELPTAB_H

#include <wx/textctrl.h> // Base class: wxTextCtrl

class CodeCompletionHelpTab : public wxTextCtrl {

	wxFont     m_defaultFont;
	wxFont     m_codeFont;
	wxTextAttr m_titleAttr;
	wxTextAttr m_codeAttr;
	wxTextAttr m_urlAttr;

public:
	CodeCompletionHelpTab(wxWindow* parent);
	virtual ~CodeCompletionHelpTab();

	void WriteFileURL(const wxString &text);
	void WriteTitle  (const wxString &text);
	void WriteCode   (const wxString &text);
	void AppendText  (const wxString &text);
	void Clear();

	DECLARE_EVENT_TABLE();
	void OnLeftDown(wxMouseEvent &event);
};

#endif // CODECOMPLETIONHELPTAB_H
