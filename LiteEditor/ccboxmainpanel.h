#ifndef CCBOXMAINPANEL_H
#define CCBOXMAINPANEL_H

#include "precompiled_header.h"
#include <wx/panel.h>
#include <wx/window.h>

class CCBoxMainPanel : public wxPanel
{
public:
	CCBoxMainPanel(wxWindow *win);
	virtual ~CCBoxMainPanel();

	DECLARE_EVENT_TABLE();
	void OnPaint(wxPaintEvent &e);
	void OnErasebg(wxEraseEvent &e);
};

#endif // CCBOXMAINPANEL_H
