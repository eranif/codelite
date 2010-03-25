#ifndef CLREGISTRY_H
#define CLREGISTRY_H

#include <wx/string.h>
#include <wx/ffile.h>
#include <map>

class clRegistry {
	static wxString              m_filename;
	wxFFile                      m_fp;
	std::map<wxString, wxString> m_entries;

public:
	static void SetFilename(const wxString &filename);

public:
	clRegistry();
	virtual ~clRegistry();

	bool Read (const wxString &key, wxString &val);
};

#endif // CLREGISTRY_H
