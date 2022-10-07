//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : codeformatterdlg.h
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
#ifndef __CODEFORMATTERDLG__
#define __CODEFORMATTERDLG__

#include "CodeFormatterManager.hpp"
#include "FormatterPage.hpp"
#include "codeformatterdlgbase.h"

class IManager;
class CodeFormatter;

class CodeFormatterDlg : public CodeFormatterBaseDlg
{
    bool m_isDirty = false;
    CodeFormatterManager& m_formatter_manager;
    FormatterPage* m_page = nullptr;

protected:
    void OnSelectFileTypes(wxCommandEvent& event);
    void OnRevert(wxCommandEvent& event);
    void OnSelectionChanged(wxDataViewEvent& event);
    void OnSelectionChanging(wxDataViewEvent& event);
    void OnOK(wxCommandEvent& e);
    void InitDialog();

public:
    CodeFormatterDlg(wxWindow* parent, CodeFormatterManager& mgr);
    virtual ~CodeFormatterDlg();
};

#endif // __CODEFORMATTERDLG__
