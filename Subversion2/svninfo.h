#ifndef SVNINFO_H
#define SVNINFO_H

#include <wx/string.h>

class SvnInfo
{
public:
	wxString m_url;
	wxString m_sourceUrl;
	wxString m_revision;
	wxString m_author;
	wxString m_date;

public:
	SvnInfo() {}
	~SvnInfo() {}
};

#endif // SVNINFO_H
