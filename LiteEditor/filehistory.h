#ifndef FILE_HISTORY_H
#define FILE_HISTORY_H

#include "wx/docview.h"

class FileHistory : public wxFileHistory
{
public:
	FileHistory();
	virtual ~FileHistory();
	
	void GetFiles(wxArrayString &files);
};
#endif //FILE_HISTORY_H
