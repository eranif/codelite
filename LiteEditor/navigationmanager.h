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

#include "vector"
#include "browse_record.h"

/**
 * \class NavMgr
 * \brief a manager class that remembers navigation position in a queue
 * \author Eran
 * \date 08/10/07
 */
class NavMgr
{
	std::vector<BrowseRecord> m_records;
	int m_cur;

private:
	NavMgr();
	virtual ~NavMgr();
	
public:
	static NavMgr *Get();
	
	/**
	 * \brief add new record to the manager, this new record will be added in top of our 
	 * list 
	 * \param rec
	 */
	void Push(const BrowseRecord &rec);
	
	/**
	 * \brief return the next place to visit
	 * \return next browsing record
	 */
	BrowseRecord GetNextRecord();

	/**
	 * \brief return the previous place we visited at
	 * \return previous browsing record
	 */
	BrowseRecord GetPrevRecord();

	/**
	 * \brief return true if manager has more next items
	 * \return 
	 */
	bool CanNext() const;
	
	/**
	 * \brief return true if manager has more previous items
	 * \return 
	 */
	bool CanPrev() const;

	void NavigateBackward();
	void Clear();
};
#endif //NAVIGATIONMANAGER_H


