#ifndef __cppcheckjob__
#define __cppcheckjob__

#include "job.h"
#include "serialized_object.h"

// Define the events needed by this job
extern const wxEventType wxEVT_CPPCHECKJOB_STATUS_MESSAGE;
extern const wxEventType wxEVT_CPPCHECKJOB_CHECK_COMPLETED;
extern const wxEventType wxEVT_CPPCHECKJOB_REPORT;

class IPlugin;

class CppCheckSettings : public SerializedObject
{
	bool          m_bAll;
	bool          m_bForce;
	bool          m_bStyle;
	bool          m_bUnusedFunctions;
	wxArrayString m_excludeFiles;
public:
	CppCheckSettings();

	bool All() const {
		return m_bAll;
	}
	bool Force() const {
		return m_bForce;
	}
	bool Style() const {
		return m_bStyle;
	}
	bool UnusedFunctions() const {
		return m_bUnusedFunctions;
	}

	void All(bool bAll) {
		m_bAll = bAll;
	}
	void Force(bool bForce) {
		m_bForce = bForce;
	}
	void Style(bool bStyle) {
		m_bStyle = bStyle;
	}
	void UnusedFunctions(bool bUnusedFunctions) {
		m_bUnusedFunctions = bUnusedFunctions;
	}

	void SetExcludeFiles(const wxArrayString& excludeFiles) {
		this->m_excludeFiles = excludeFiles;
	}
	const wxArrayString& GetExcludeFiles() const {
		return m_excludeFiles;
	}
	virtual void Serialize(Archive &arch);
	virtual void DeSerialize(Archive &arch);

	wxString GetOptions() const;
};

#endif // __cppcheckjob__
