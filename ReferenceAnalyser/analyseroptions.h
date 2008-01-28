#ifndef ANALYSEROPTIONS_H
#define ANALYSEROPTIONS_H

#include "serialized_object.h"

class AnalyserOptions : public SerializedObject {
	wxString m_containerName;
	wxString m_containerOperator;
	
public:
	AnalyserOptions();
	virtual ~AnalyserOptions();

	void DeSerialize(Archive &arch);
	void Serialize(Archive &arch);
	
	const wxString& getContainerName() { return m_containerName; }	
	const wxString& getContainerOperator() { return m_containerOperator; }
	
	void setContainerName(const wxString& value) { m_containerName = value; }
	void setContainerOperator(const wxString& value) { m_containerOperator = value; }
};

#endif //ANALYSEROPTIONS_H
