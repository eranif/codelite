//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : outputtabwindow.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
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
    bool m_outputScrolls;
	
protected:
	virtual void CreateGUIControl();
	//Event handlers
    virtual void OnOutputScrolls(wxCommandEvent &e);
	virtual void OnClearAll(wxCommandEvent &e);
	virtual void OnWordWrap(wxCommandEvent &e);
	virtual void OnMouseDClick(wxScintillaEvent &event){event.Skip();}
	virtual void OnHotspotClicked(wxScintillaEvent &event){event.Skip();}
	virtual void OnStyleNeeded(wxScintillaEvent &event){event.Skip();}
	virtual void OnCompilerColours(wxCommandEvent &event){event.Skip();};
	virtual bool AcceptsFocus() const {return m_canFocus;}
	
public:
	OutputTabWindow(wxWindow *parent, wxWindowID id, const wxString &name);
	virtual ~OutputTabWindow();
	
	void CanFocus(bool can) { m_canFocus = can; }
	wxWindow *GetInternalWindow() {return m_sci;}
	
	virtual const wxString &GetCaption() const {return m_name;}
	virtual void AppendText(const wxString &text);
	virtual void Clear();
	
	virtual void OnCommand(wxCommandEvent &e);
	virtual void OnUpdateUI(wxUpdateUIEvent &e);
};
#endif // __outputtabwindow__

