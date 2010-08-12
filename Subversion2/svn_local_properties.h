#ifndef SUBVERSIONLOCALPROPERTIES_H
#define SUBVERSIONLOCALPROPERTIES_H

#include <wx/string.h>
#include <map>

typedef std::map<wxString, wxString>    SimpleTable;
typedef std::map<wxString, SimpleTable> GroupTable;

class SubversionLocalProperties
{
	wxString   m_path;
	GroupTable m_values;

protected:
	wxString GetConfigFile();
	void     ReadProperties();

public:
	SubversionLocalProperties(const wxString &directory);
	virtual ~SubversionLocalProperties();

	wxString ReadProperty (const wxString &propName);
	void     WriteProperty(const wxString &name, const wxString &val);
};

#endif // SUBVERSIONLOCALPROPERTIES_H
