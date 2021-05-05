//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : unittestspage.h
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

#ifndef __unittestspage__
#define __unittestspage__

#include "clWorkspaceEvent.hpp"
#include "unittestcppoutputparser.h"
#include "unittestreport.h"

class IManager;

class UnitTestsPage : public UnitTestsBasePage
{
    IManager* m_mgr;

    void OnItemActivated(wxDataViewEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& e);

public:
    /** Constructor */
    UnitTestsPage(wxWindow* parent, IManager* mgr);
    virtual ~UnitTestsPage();

    void Initialize(TestSummary* summary);
    void UpdateFailedBar(size_t amount, const wxString& msg);
    void UpdatePassedBar(size_t amount, const wxString& msg);
    void Clear();

protected:
    virtual void OnClearReport(wxCommandEvent& event);
};

#endif // __unittestspage__
