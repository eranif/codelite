#include "navigationmanager.h"
#include "editor.h"
#include "manager.h"

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

BrowseRecord NavMgr::GetNextRecord()
{
	if (!CanNext()) {
		return BrowseRecord();
	}

	m_cur++;
	return m_records.at(m_cur);
}

BrowseRecord NavMgr::GetPrevRecord()
{
	if (!CanPrev()) {
		return BrowseRecord();
	}
	m_cur--;
	return m_records.at(m_cur);
}

void NavMgr::Push(const BrowseRecord &rec)
{
	m_records.insert(m_records.end(), rec);
	m_cur = (int)m_records.size();
}

void NavMgr::NavigateBackward()
{
	if (!CanPrev())
		return;

	// before jumping, save the current position
	if ( CanNext() == false ) {
		LEditor *editor = ManagerST::Get()->GetActiveEditor();
		if (editor) {
			//keep this location as well, but make sure we dont add this twice
			BrowseRecord record = editor->CreateBrowseRecord();
			BrowseRecord last = m_records.back();
			if (!(last.filename == record.filename && last.lineno == record.lineno && last.position == record.position)) {
				//different item, we can add it
				Push(record);
				//so we dont get this location again
				GetPrevRecord();
			}
		}
	}

	BrowseRecord rec = GetPrevRecord();
	ManagerST::Get()->OpenFile(rec);
}
