#ifndef __configurationtoolbase__
#define __configurationtoolbase__

#include "wx/xml/xml.h"

class SerializedObject;

class ConfigurationToolBase {
	wxXmlDocument m_doc;
	wxString m_fileName;
	
public:
	ConfigurationToolBase();
	virtual ~ConfigurationToolBase();
	
	bool Load(const wxString &filename);
	bool WriteObject(const wxString &name, SerializedObject *obj);
	bool ReadObject(const wxString &name, SerializedObject *obj);
	
	virtual wxString GetRootName() = 0;
};
#endif // __configurationtoolbase__

