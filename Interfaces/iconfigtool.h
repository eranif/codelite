#ifndef ICONFIGTOOL_H
#define ICONFIGTOOL_H

#include "wx/string.h"
#include "serialized_object.h"

//------------------------------------------------------------------
// The configuration tool interface
//------------------------------------------------------------------
class IConfigTool {
public:
	IConfigTool(){}
	virtual ~IConfigTool(){}
	
	virtual bool ReadObject(const wxString &name, SerializedObject *obj) = 0;
	virtual bool WriteObject(const wxString &name, SerializedObject *obj) = 0;
};

#endif //ICONFIGTOOL_H
