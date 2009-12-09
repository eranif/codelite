#ifndef SVNSETTINGSDATA_H
#define SVNSETTINGSDATA_H

#include "serialized_object.h" // Base class

class SvnSettingsData : public SerializedObject
{
	wxString m_executable;

public:
	SvnSettingsData()
			: m_executable(wxT("svn")) {
	}

	virtual ~SvnSettingsData() {
	}

public:
	virtual void DeSerialize(Archive &arch) {
		arch.Read(wxT("m_executable"), m_executable);
	}

	virtual void Serialize(Archive &arch) {
		arch.Read(wxT("m_executable"), m_executable);
	}
	void SetExecutable(const wxString& executable) {
		this->m_executable = executable;
	}
	const wxString& GetExecutable() const {
		return m_executable;
	}
};

#endif // SVNSETTINGSDATA_H
