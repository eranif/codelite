#include "filehistory.h"

FileHistory::FileHistory()
{
}

FileHistory::~FileHistory()
{
}

void FileHistory::GetFiles(wxArrayString &files)
{
	for(size_t i=0; i<m_fileHistoryN; i++)
	{
		files.Add(m_fileHistory[i]);
	}
}
