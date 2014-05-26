#ifndef __cppcheckjob__
#define __cppcheckjob__

#include "job.h"
#include "serialized_object.h"
#include "project.h"

// Define the events needed by this job
extern const wxEventType wxEVT_CPPCHECKJOB_STATUS_MESSAGE;
extern const wxEventType wxEVT_CPPCHECKJOB_CHECK_COMPLETED;
extern const wxEventType wxEVT_CPPCHECKJOB_REPORT;

class IPlugin;
class wxCheckListBox;

class CppCheckSettings : public SerializedObject
{
	typedef std::map<wxString, wxString> StrStrMap;

	bool          m_Style;
	bool          m_Performance;
	bool          m_Portability;
	bool          m_UnusedFunctions;
	bool          m_MissingIncludes;
	bool          m_Information;
	bool          m_PosixStandards;
	bool          m_C99Standards;
	bool          m_Cpp11Standards;
	bool          m_Force;
	wxArrayString m_excludeFiles;
	StrStrMap     m_SuppressedWarnings0;     // The items unchecked in the checklistbox
	StrStrMap     m_SuppressedWarnings1;     // The checked ones
	StrStrMap     m_SuppressedWarningsOrig0; // Ditto, containing the original values
	StrStrMap     m_SuppressedWarningsOrig1;
	bool          m_saveSuppressedWarnings;
	wxArrayString m_definitions;
	wxArrayString m_undefines;

public:
	CppCheckSettings();

	bool GetStyle() const {
		return m_Style;
	}
	bool GetPerformance() const {
		return m_Performance;
	}
	bool GetPortability() const {
		return m_Portability;
	}
	bool GetUnusedFunctions() const {
		return m_UnusedFunctions;
	}
	bool GetMissingIncludes() const {
		return m_MissingIncludes;
	}
	bool GetInformation() const {
		return m_Information;
	}
	bool GetPosixStandards() const {
		return m_PosixStandards;
	}
	bool GetC99Standards() const {
		return m_C99Standards;
	}
	bool GetCpp11Standards() const {
		return m_Cpp11Standards;
	}
	bool GetForce() const {
		return m_Force;
	}
	const wxArrayString& GetExcludeFiles() const {
		return m_excludeFiles;
	}
	const StrStrMap* GetSuppressedWarningsStrings0() const {
		return &m_SuppressedWarnings0;
	}
	const StrStrMap* GetSuppressedWarningsStrings1() const {
		return &m_SuppressedWarnings1;
	}
	const wxArrayString& GetDefinitions() const {
		return m_definitions;
	}
	const wxArrayString& GetUndefines() const {
		return m_undefines;
	}

	void SetStyle(bool Style)                     { m_Style = Style; }
	void SetPerformance(bool Performance)         { m_Performance = Performance; }
	void SetPortability(bool Portability)         { m_Portability = Portability; }
	void SetUnusedFunctions(bool UnusedFunctions) { m_UnusedFunctions = UnusedFunctions; }
	void SetMissingIncludes(bool MissingIncludes) { m_MissingIncludes = MissingIncludes; }
	void SetInformation(bool Information)         { m_Information = Information; }
	void SetPosixStandards(bool PosixStandards)   { m_PosixStandards = PosixStandards; }
	void SetC99Standards(bool C99Standards)       { m_C99Standards = C99Standards; }
	void SetCpp11Standards(bool Cpp11Standards)   { m_Cpp11Standards = Cpp11Standards; }
	void SetForce(bool Force)                     { m_Force = Force; }
	void SetExcludeFiles(const wxArrayString& excludeFiles) {
		m_excludeFiles = excludeFiles;
	}
	void AddSuppressedWarning(const wxString& key, const wxString& label, bool checked);
	void RemoveSuppressedWarning(const wxString& key);
	void SetSuppressedWarnings(wxCheckListBox* clb, const wxArrayString& keys);
	void SetSaveSuppressedWarnings(bool save) { m_saveSuppressedWarnings = save; }
	void SetDefaultSuppressedWarnings();
	void SetDefinitions(const wxArrayString& definitions) {
		m_definitions = definitions;
	}
	void SetUndefines(const wxArrayString& undefines) {
		m_undefines = undefines;
	}

	virtual void Serialize(Archive& arch);
	virtual void DeSerialize(Archive& arch);

	wxString GetOptions() const;
    void LoadProjectSpecificSettings(ProjectPtr proj);
};

#endif // __cppcheckjob__
