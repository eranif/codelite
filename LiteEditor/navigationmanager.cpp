#include "navigationmanager.h"

NavMgr::NavMgr()
: m_cur(wxNOT_FOUND)
{
}

NavMgr::~NavMgr()
{
	Clear();
}

NavMgr *NavMgr::Get()
{
	static NavMgr theManager;
	return &theManager;
}

void NavMgr::Clear()
{
	m_cur = wxNOT_FOUND;
	m_records.clear();
}

bool NavMgr::CanNext() const
{
	int cur = m_cur;
	cur++;
	return (cur < (int)m_records.size());
}

bool NavMgr::CanPrev() const
{
	int cur = m_cur;
	cur--;
	return (cur >= 0);
}

BrowseRecord NavMgr::Next()
{
	if(!CanNext()){
		return BrowseRecord();
	}

	m_cur++;
	return m_records.at(m_cur);
}

BrowseRecord NavMgr::Prev()
{
	if(!CanPrev()){
		return BrowseRecord();
	}
	m_cur--;
	return m_records.at(m_cur);
}

void NavMgr::Push(const BrowseRecord &rec)
{
	m_records.push_back(rec);
	m_cur = (int)m_records.size();
}
