#ifndef __outputtabwindow__
#define __outputtabwindow__

#include "wx/panel.h"
#include "wx/wxscintilla.h"

class OutputTabWindow : public wxPanel 
{
protected:
	wxString m_name;
	wxScintilla *m_sci;
	bool m_canFocus;
	
protected:
	virtual void CreateGUIControl();
	//Event handlers
	virtual void OnClearAll(wxCommandEvent &e);
	virtual void OnWordWrap(wxCommandEvent &e);
	virtual void OnSetFocus(wxFocusEvent &event);
	virtual void OnMouseDClick(wxScintillaEvent &event){event.Skip();}
	virtual void OnStyleNeeded(wxScintillaEvent &event){event.Skip();}
	virtual void OnCompilerColours(wxCommandEvent &event){event.Skip();};
	
public:
	OutputTabWindow(wxWindow *parent, wxWindowID id, const wxString &name);
	virtual ~OutputTabWindow();
	
	void CanFocus(bool can) { m_canFocus = can; }
	
	virtual const wxString &GetCaption() const {return m_name;}
	virtual void AppendText(const wxString &text);
	virtual void Clear();
};
#endif // __outputtabwindow__

