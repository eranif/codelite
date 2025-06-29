//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : memchecklistctrlerrors.h
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

#ifndef _MEMCHECKLISTCTRLERRORS_H_
#define _MEMCHECKLISTCTRLERRORS_H_

#include <wx/listctrl.h>
#include <vector>

#include "imemcheckprocessor.h"

/**
 * @class MemCheckListCtrlErrors
 * @brief wxListCtrl with wxLC_VIRTUAL need derived class to implement OnGetItemText. So this class is only wrapper to do simple thing "m_data->at(item)"
 */
class MemCheckListCtrlErrors: public wxListCtrl
{
public:
    MemCheckListCtrlErrors(wxWindow* parent,
                           wxWindowID id,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize,
                           long style = wxLC_ICON,
                           const wxValidator& validator = wxDefaultValidator,
                           const wxString& name = wxListCtrlNameStr)
        : wxListCtrl(parent, id, pos, size, style, validator, name)
    {
    }
    ~MemCheckListCtrlErrors() override = default;

    virtual wxString OnGetItemText(long item, long column) const {
        return m_data->at(item)->label;
    }

    void SetData(std::vector<MemCheckErrorPtr>* data) {
        this->m_data = data;
    }

protected:
    std::vector<MemCheckErrorPtr>* m_data;
};

#endif
