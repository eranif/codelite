#ifndef DIRKEEPER_H
#define DIRKEEPER_H

#include "wx/string.h"

class DirKeeper
{
	wxString m_curDir;
public:
	DirKeeper(){
		m_curDir = wxGetCwd();
	}

	virtual ~DirKeeper(){
		wxSetWorkingDirectory(m_curDir);
	}
};

#endif //DIRKEEPER_H
