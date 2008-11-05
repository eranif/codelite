//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : windowstack.h              
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
 #ifndef WINDOWSTACK_H
#define WINDOWSTACK_H

#include <map>
#include <vector>
#include "wx/panel.h"
#include "wx/sizer.h"

class WindowStack : public wxPanel {
    std::map<wxString, wxWindow*> m_windows;
    wxBoxSizer *m_mainSizer;
    wxWindow *m_selection;
    wxString m_selectionKey;

    void DoSelect(wxWindow *win, const wxString &key);     

public:
    WindowStack(wxWindow *parent, wxWindowID id = wxID_ANY);
    virtual ~WindowStack();
	
    void Add(wxWindow *win, const wxString &key);
  
    void Select(const wxString &key);
    void Select(wxWindow *win);
    void SelectNone();

    void Clear();
  
    wxWindow *Remove(const wxString &key);
    wxString  Remove(wxWindow *win);
    
    void Delete(const wxString &key);
    void Delete(wxWindow *win);
    
    wxWindow *Find(const wxString &key);
    wxString  Find(wxWindow *win);
    
    void GetKeys(std::vector<wxString> &keys) const;
    
    int Count() const { return m_windows.size(); }
    bool IsEmpty() const { return Count() == 0; }
    
    wxWindow *GetSelected() { return m_selection; }
    const wxString &GetSelectedKey() const { return m_selectionKey; }
};

#endif //WINDOWSTACK_H


