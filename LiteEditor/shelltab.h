//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : shelltab.h              
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
#ifndef SHELLTAB_H
#define SHELLTAB_H

#include "outputtabwindow.h"

class AsyncExeCmd;
class QuickFindBar;


class ShellTab : public OutputTabWindow
{
protected:
    wxSizer      *m_inputSizer;
    wxComboBox   *m_input;
    QuickFindBar *m_findBar;
    AsyncExeCmd  *m_cmd;
    
    static  void InitStyle    (wxScintilla *sci);
    
    virtual bool DoSendInput  (const wxString &line);
    
    virtual void OnProcStarted(wxCommandEvent  &e);
    virtual void OnProcOutput (wxCommandEvent  &e);
    virtual void OnProcError  (wxCommandEvent  &e);
    virtual void OnProcEnded  (wxCommandEvent  &e);
    
    virtual void OnShowInput  (wxCommandEvent  &e);
    virtual void OnShowSearch (wxCommandEvent  &e);
    virtual void OnSendInput  (wxCommandEvent  &e);
    virtual void OnStopProc   (wxCommandEvent  &e);
    virtual void OnKeyDown    (wxKeyEvent      &e);
    virtual void OnUpdateUI   (wxUpdateUIEvent &e);
    
    DECLARE_EVENT_TABLE()
    
public:
	ShellTab(wxWindow *parent, wxWindowID id, const wxString &name);
	~ShellTab();
};
 

class DebugTab : public ShellTab
{
protected:
    bool DoSendInput(const wxString  &line);
    void OnStopProc (wxCommandEvent  &e);
    void OnUpdateUI (wxUpdateUIEvent &e);

public:
	DebugTab(wxWindow *parent, wxWindowID id, const wxString &name);
	~DebugTab();

    void AppendLine(const wxString &line);
};

#endif //SHELLTAB_H



