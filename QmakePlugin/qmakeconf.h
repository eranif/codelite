#ifndef __qmakeconf__
#define __qmakeconf__

#include <wx/string.h>
#include <wx/fileconf.h>

class QmakeConf : public wxFileConfig {
public:
	QmakeConf(const wxString &confPath);
	virtual ~QmakeConf();

	wxArrayString GetAllConfigurations();
};
#endif // __qmakeconf__
