#ifndef SUBVERSIONLOCALPROPERTIES_H
#define SUBVERSIONLOCALPROPERTIES_H

#include <wx/string.h>
#include <map>

typedef std::map<wxString, wxString>    SimpleTable;
typedef std::map<wxString, SimpleTable> GroupTable;

class SubversionLocalProperties
{
	wxString   m_url;
	GroupTable m_values;

public:
	static wxString BUG_TRACKER_URL;
	static wxString BUG_TRACKER_MESSAGE;

protected:
	wxString GetConfigFile();
	void     ReadProperties();
	void     WriteProperties();

public:
	SubversionLocalProperties(const wxString &url);
	virtual ~SubversionLocalProperties();

	wxString ReadProperty (const wxString &propName);
	void     WriteProperty(const wxString &name, const wxString &val);
};

#endif // SUBVERSIONLOCALPROPERTIES_H
