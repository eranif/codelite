#ifndef EXTDBDATA_H
#define EXTDBDATA_H

#include <wx/arrstr.h>

struct ExtDbData {
	wxString dbName;
	wxString rootPath;
	wxArrayString includeDirs;
	bool attachDb;
	wxString fileMasking;
};

#endif //EXTDBDATA_H
