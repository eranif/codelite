//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : LLDBNewBreakpointDlg.h
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

#ifndef LLDBNEWBREAKPOINTDLG_H
#define LLDBNEWBREAKPOINTDLG_H

#include "UI.h"
#include "LLDBProtocol/LLDBBreakpoint.h"

class LLDBNewBreakpointDlg : public LLDBNewBreakpointDlgBase
{
public:
    LLDBNewBreakpointDlg(wxWindow* parent);
    virtual ~LLDBNewBreakpointDlg();
    LLDBBreakpoint::Ptr_t GetBreakpoint();
    
protected:
    virtual void OnCheckFileAndLine(wxCommandEvent& event);
    virtual void OnCheckFuncName(wxCommandEvent& event);
    virtual void OnFileLineEnabledUI(wxUpdateUIEvent& event);
    virtual void OnFuncNameUI(wxUpdateUIEvent& event);
};
#endif // LLDBNEWBREAKPOINTDLG_H

