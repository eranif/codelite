#ifndef __confformbuilder__
#define __confformbuilder__

#include "serialized_object.h"

class ConfFormBuilder : public SerializedObject
{

	wxString m_fbPath;
	wxString m_command;

public:
	ConfFormBuilder();
	virtual ~ConfFormBuilder();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);


	void SetCommand(const wxString& command) {
		this->m_command = command;
	}
	void SetFbPath(const wxString& fbPath) {
		this->m_fbPath = fbPath;
	}
	const wxString& GetCommand() const {
		return m_command;
	}
	const wxString& GetFbPath() const {
		return m_fbPath;
	}
};
#endif // __confformbuilder__
