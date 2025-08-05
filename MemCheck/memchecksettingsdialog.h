//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : memchecksettingsdialog.h
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

/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#ifndef _MEMCHECKSETTINGSDIALOG_H_
#define _MEMCHECKSETTINGSDIALOG_H_

#include "memcheckui.h"

class MemCheckSettingsDialog: public MemCheckSettingsDialogBase
{

public:
    MemCheckSettingsDialog(wxWindow *parent,
                           MemCheckSettings *settings);
    virtual ~MemCheckSettingsDialog() = default;

protected:
    virtual void OnFilePickerValgrindOutputFileUI(wxUpdateUIEvent& event);
    
    /**
     * @brief Shows popup menu with two functions: add new supp and remove supp
     * @param event
     */
    virtual void OnSuppListRightDown(wxMouseEvent& event);

    MemCheckSettings *m_settings;

    void OnAddSupp(wxCommandEvent & event);
    void OnDelSupp(wxCommandEvent & event);

    virtual void OnValgrindOutputFileChanged(wxFileDirPickerEvent & event);
    virtual void ValgrindResetOptions(wxCommandEvent & event);
    virtual void OnOK(wxCommandEvent & event);
};
#endif // _MEMCHECKSETTINGSDIALOG_H
