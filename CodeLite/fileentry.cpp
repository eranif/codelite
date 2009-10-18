#include "fileentry.h"

FileEntry::FileEntry()
		: m_id                   (wxNOT_FOUND)
		, m_file                 (wxEmptyString)
		, m_lastRetaggedTimestamp((int)time(NULL))
{
}

FileEntry::~FileEntry()
{
}
