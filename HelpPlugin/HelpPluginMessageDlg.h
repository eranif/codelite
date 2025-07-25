//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : HelpPluginMessageDlg.h
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

#ifndef HELPPLUGINMESSAGEDLG_H
#define HELPPLUGINMESSAGEDLG_H

#include "HelpPluginUI.h"

class HelpPluginMessageDlg : public HelpPluginMessageBaseDlg
{
public:
    explicit HelpPluginMessageDlg(wxWindow* parent);
    ~HelpPluginMessageDlg() override = default;
};
#endif // HELPPLUGINMESSAGEDLG_H
