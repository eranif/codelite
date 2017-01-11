//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : evalpane.h
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

#ifndef EVALPANE_H
#define EVALPANE_H
#include "php_ui.h"
#include "xdebugevent.h"

class EvalPane : public EvalPaneBase
{
public:
    EvalPane(wxWindow* parent);
    virtual ~EvalPane();
protected:
    virtual void OnSendXDebugCommand(wxCommandEvent& event);
    virtual void OnSendXDebugCommandUI(wxUpdateUIEvent& event);
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnSend(wxCommandEvent& event);
    virtual void OnSendUI(wxUpdateUIEvent& event);
    void OnExpressionEvaluate(XDebugEvent &e);
    void OnDBGPCommandEvaluated(XDebugEvent &e);
    void OnSettingsChanged(wxCommandEvent& event);

};
#endif // EVALPANE_H
