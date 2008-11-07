//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : dropbutton.h              
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
 #ifndef __dropbutton__
#define __dropbutton__

#include <vector>
#include "wx/panel.h"
#include "wx/bitmap.h"

class wxTabContainer;
class WindowStack;

class DropButtonBase : public wxPanel 
{
private:
    enum ButtonState {
        BTN_NONE = 0,
        BTN_PUSHED
    };

    ButtonState m_state;
    wxBitmap m_arrowDownBmp;

protected:
    virtual size_t GetItemCount() = 0;
    virtual wxString GetItem(size_t n) = 0;
    virtual bool IsItemSelected(size_t n) = 0;

public:
    DropButtonBase(wxWindow *parent);
    ~DropButtonBase();
    
    DECLARE_EVENT_TABLE()
    void OnLeftDown(wxMouseEvent &e);
    void OnPaint(wxPaintEvent &e);
    
    virtual void OnMenuSelection(wxCommandEvent &e) = 0;
};

class DropButton : public DropButtonBase
{
private:
    wxTabContainer *m_tabContainer;

protected:
    size_t GetItemCount();
    wxString GetItem(size_t n);
    bool IsItemSelected(size_t n);

public:
    DropButton(wxWindow *parent, wxTabContainer *tabContainer);
    ~DropButton();
    
    void OnMenuSelection(wxCommandEvent &e);
};

class StackButton : public DropButtonBase
{
private:
    std::vector<wxString> m_windowKeys;
    WindowStack *m_windowStack;

protected:
    size_t GetItemCount();
    wxString GetItem(size_t n);
    bool IsItemSelected(size_t n);

public:
    StackButton(wxWindow *parent, WindowStack *windowStack);
    ~StackButton();
    
    void SetWindowStack(WindowStack *windowStack);
    
    void OnMenuSelection(wxCommandEvent &e);
};


#endif // __dropbutton__
