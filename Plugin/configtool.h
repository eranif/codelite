#ifndef CONFIGTOOL_H
#define CONFIGTOOL_H

#include "wx/xml/xml.h"

class SerializedObject;

/**
 * \class ConfigTool 
 * \brief this class provides a simple class that allows writing serialized classes to disk
 * \author Eran
 * \date 11/18/07
 */
class ConfigTool 
{
	wxXmlDocument m_doc;
	wxString m_fileName;
public:
	ConfigTool();
	~ConfigTool();
	
	bool Load(const wxString &filename);
	bool WriteObject(const wxString &name, SerializedObject *obj);
	bool ReadObject(const wxString &name, SerializedObject *obj);
};

#endif //CONFIGTOOL_H

