#ifndef CLFUNCTIONTIPWINDOW_H
#define CLFUNCTIONTIPWINDOW_H

#include <wx/panel.h> // Base class: wxPanel
#include <wx/bitmap.h>
#include <vector>
#include "cl_calltip.h"

class clEditorTipWindow : public wxPanel {
public:
	struct TipInfo {
		clCallTipPtr tip;
		int          highlightIndex;
	};
	
protected:
	std::vector<TipInfo> m_tips;
	int                  m_highlighIndex;
	wxString             m_tipText;
	wxPoint              m_point;
	int                  m_lineHeight;
	
protected:
	wxSize DoGetTipSize();
	int    DoGetTextLen(const wxString &txt);
	void   DoAdjustPosition();
	void   DoLayoutTip();
	
public:
	clEditorTipWindow(wxWindow *parent);
	virtual ~clEditorTipWindow();
	
	// API
	///////////////////////////////////////////////////
	void         Add(clCallTipPtr tip);
	void         Remove();
	clCallTipPtr GetTip();
	bool         IsEmpty();
	void         Clear();
	bool         IsActive();
	void         Activate(wxPoint pt, int lineHeight);
	void         Deactivate();
	void         SelectNext (int argIdxToHilight);
	void         SelectPrev (int argIdxToHilight);
	void         Highlight  (int argIdxToHilight);
	wxString     GetText();
	
	DECLARE_EVENT_TABLE()
	void OnPaint  (wxPaintEvent &e);
	void OnEraseBg(wxEraseEvent &e);
	void OnLeftDown(wxMouseEvent &e);
};

#endif // CLFUNCTIONTIPWINDOW_H
