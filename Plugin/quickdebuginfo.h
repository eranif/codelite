#ifndef __quickdebuginfo__
#define __quickdebuginfo__

#include <wx/arrstr.h>
#include <wx/string.h>
#include "serialized_object.h"
class QuickDebugInfo : public SerializedObject
{

	wxString m_exePath;
	wxString m_wd;
	wxString m_arguments;
	wxArrayString m_startCmds;
	int m_selectedDbg;

public:
	QuickDebugInfo();
	~QuickDebugInfo();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);

	void SetArguments(const wxString& arguments) {
		this->m_arguments = arguments;
	}
	void SetExePath(const wxString& exePath) {
		this->m_exePath = exePath;
	}
	void SetSelectedDbg(const int& selectedDbg) {
		this->m_selectedDbg = selectedDbg;
	}
	void SetStartCmds(const wxArrayString& startCmds) {
		this->m_startCmds = startCmds;
	}
	void SetWd(const wxString& wd) {
		this->m_wd = wd;
	}

	const wxString& GetArguments() const {
		return m_arguments;
	}
	const wxString& GetExePath() const {
		return m_exePath;
	}
	const int& GetSelectedDbg() const {
		return m_selectedDbg;
	}
	const wxArrayString& GetStartCmds() const {
		return m_startCmds;
	}
	const wxString& GetWd() const {
		return m_wd;
	}
};
#endif // __quickdebuginfo__
