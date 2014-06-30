//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : LLDBSettingDialog.h
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

#ifndef LLDBSETTINGDIALOG_H
#define LLDBSETTINGDIALOG_H
#include "UI.h"

class LLDBSettingDialog : public LLDBSettingDialogBase
{
    bool m_modified;
public:
    LLDBSettingDialog(wxWindow* parent);
    virtual ~LLDBSettingDialog();
    void Save();
    
protected:
    virtual void OnApply(wxCommandEvent& event);
    virtual void OnAdvancedValueChanged(wxPropertyGridEvent& event);
    virtual void OnGeneralValueChanged(wxPropertyGridEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // LLDBSETTINGDIALOG_H
