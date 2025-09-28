//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPSettersGettersDialog.h
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

#ifndef PHPSETTERSGETTERSDIALOG_H
#define PHPSETTERSGETTERSDIALOG_H

#include "PHPSetterGetterEntry.h"
#include "php_ui.h"

class IManager;
class IEditor;
class PHPSettersGettersDialog : public PHPSettersGettersDialogBase
{
    IManager* m_mgr;
    wxString m_scope;

protected:
    void DoPopulate(const PHPEntityBase::List_t& members);
    void Clear();

public:
    PHPSettersGettersDialog(wxWindow* parent, IEditor* editor, IManager* mgr);
    virtual ~PHPSettersGettersDialog();
    PHPSetterGetterEntry::Vec_t GetMembers();
    size_t GetFlags();
    const wxString& GetScope() const { return m_scope; }
};
#endif // PHPSETTERSGETTERSDIALOG_H
