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
	BrowseRecord Next();

	/**
	 * \brief return the previous place we visited at
	 * \return previous browsing record
	 */
	BrowseRecord Prev();

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

	void Clear();
};
#endif //NAVIGATIONMANAGER_H


