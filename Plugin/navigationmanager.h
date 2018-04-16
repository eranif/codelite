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
#include "codelite_exports.h"
#include "ieditor.h"
#include "imanager.h"
#include "vector"

/**
 * @class NavMgr
 * @brief a manager class that remembers navigation position in a queue
 * @author Eran
 * @date 08/10/07
 */
class WXDLLIMPEXP_SDK NavMgr
{
    std::vector<BrowseRecord> m_jumps;
    size_t m_cur;

private:
    NavMgr();
    virtual ~NavMgr();

public:
    static NavMgr* Get();

    /**
     * @brief check if a browse record is actually a valid jump location (source or target)
     */
    bool ValidLocation(const BrowseRecord& rec) const;

    /**
     * @brief add new jump record to the manager, this new record become the new top of the list
     * @param from the starting point of the jump
     * @param to the ending point of the jump
     */
    void AddJump(const BrowseRecord& from, const BrowseRecord& to);

    /**
     * @brief return the next place to visit (destination)
     * @return next browsing record
     */
    BrowseRecord GetNext();

    /**
     * @brief return the previous place we visited (source)
     * @return previous browsing record
     */
    BrowseRecord GetPrev();

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
