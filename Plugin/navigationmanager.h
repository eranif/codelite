//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : navigationmanager.h
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
#ifndef NAVIGATIONMANAGER_H
#define NAVIGATIONMANAGER_H

#include "browse_record.h"
#include "clWorkspaceEvent.hpp"
#include "codelite_exports.h"
#include "ieditor.h"
#include "imanager.h"

#include <stack>

/**
 * @class NavMgr
 * @brief a manager class that remembers navigation position in a queue
 * @author Eran
 * @date 08/10/07
 */
class WXDLLIMPEXP_SDK NavMgr : public wxEvtHandler
{
    std::stack<BrowseRecord> m_nexts;
    std::stack<BrowseRecord> m_prevs;
    BrowseRecord m_currentLocation;

private:
    NavMgr();
    virtual ~NavMgr();

    void OnWorkspaceClosed(clWorkspaceEvent& e);

public:
    static NavMgr* Get();

    /**
     * @brief check if a browse record is actually a valid jump location (source or target)
     */
    bool ValidLocation(const BrowseRecord& rec) const;

    /**
     * @brief store the current jump origin -> target
     */
    void StoreCurrentLocation(const BrowseRecord& origin, const BrowseRecord& target);

    /**
     * @brief return true if manager has more next items
     * @return
     */
    bool CanNext() const;

    /**
     * @brief return true if manager has more previous items
     * @return
     */
    bool CanPrev() const;

    bool NavigateBackward(IManager* mgr);
    bool NavigateForward(IManager* mgr);
    void Clear();
};
#endif // NAVIGATIONMANAGER_H
